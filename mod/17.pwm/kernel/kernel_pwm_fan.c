
#include <linux/hwmon.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/regulator/consumer.h>
#include <linux/sysfs.h>
#include <linux/thermal.h>
#include <linux/timer.h>

/*
usr_fan0 {
    compatible = "rmk,pwm-fan";
    #cooling-cells = <2>;
    pwms = <&pwm7 0 10000 0>;
    cooling-levels = <0 102 170 230 255>;
};
*/

#define MAX_PWM 255

enum pwm_fan_enable_mode {
    pwm_off_reg_off,
    pwm_disable_reg_enable,
    pwm_enable_reg_enable,
    pwm_disable_reg_disable,
};

struct pwm_fan_ctx {
    struct device *dev;

    struct mutex lock;
    struct pwm_device *pwm;
    struct pwm_state pwm_state;
    struct regulator *reg_en;
    enum pwm_fan_enable_mode enable_mode;
    bool regulator_enabled;
    bool enabled;

    unsigned int pwm_value;
    unsigned int pwm_fan_state;
    unsigned int pwm_fan_max_state;
    unsigned int *pwm_fan_cooling_levels;
    struct thermal_cooling_device *cdev;

    struct hwmon_chip_info info;
    struct hwmon_channel_info fan_channel;
};

static int pwm_fan_power_on(struct pwm_fan_ctx *ctx);
static int pwm_fan_power_off(struct pwm_fan_ctx *ctx);
static void pwm_fan_enable_mode_2_state(int enable_mode,
                    struct pwm_state *state,
                    bool *enable_regulator);

static int pwm_fan_switch_power(struct pwm_fan_ctx *ctx, bool on)
{
    int ret = 0;

    if (!ctx->reg_en)
        return ret;

    if (!ctx->regulator_enabled && on) {
        ret = regulator_enable(ctx->reg_en);
        if (ret == 0)
            ctx->regulator_enabled = true;
    } else if (ctx->regulator_enabled && !on) {
        ret = regulator_disable(ctx->reg_en);
        if (ret == 0)
            ctx->regulator_enabled = false;
    }
    return ret;
}

static int pwm_fan_power_on(struct pwm_fan_ctx *ctx)
{
    struct pwm_state *state = &ctx->pwm_state;
    int ret;

    if (ctx->enabled)
        return 0;

    ret = pwm_fan_switch_power(ctx, true);
    if (ret < 0) {
        dev_err(ctx->dev, "failed to enable power supply\n");
        return ret;
    }

    state->enabled = true;
    ret = pwm_apply_state(ctx->pwm, state);
    if (ret) {
        dev_err(ctx->dev, "failed to enable PWM\n");
        goto disable_regulator;
    }

    ctx->enabled = true;

    return 0;

disable_regulator:
    pwm_fan_switch_power(ctx, false);
    return ret;
}

static int pwm_fan_power_off(struct pwm_fan_ctx *ctx)
{
    struct pwm_state *state = &ctx->pwm_state;
    bool enable_regulator = false;
    int ret;

    if (!ctx->enabled)
        return 0;

    pwm_fan_enable_mode_2_state(ctx->enable_mode,
                    state,
                    &enable_regulator);

    state->enabled = false;
    state->duty_cycle = 0;
    ret = pwm_apply_state(ctx->pwm, state);
    if (ret) {
        dev_err(ctx->dev, "failed to disable PWM\n");
        return ret;
    }

    pwm_fan_switch_power(ctx, enable_regulator);

    ctx->enabled = false;

    return 0;
}

static int  __set_pwm(struct pwm_fan_ctx *ctx, unsigned long pwm)
{
    struct pwm_state *state = &ctx->pwm_state;
    unsigned long period;
    int ret = 0;

    if (pwm > 0) {
        if (ctx->enable_mode == pwm_off_reg_off)
            /* pwm-fan hard disabled */
            return 0;

        period = state->period;
        state->duty_cycle = DIV_ROUND_UP(pwm * (period - 1), MAX_PWM);
        ret = pwm_apply_state(ctx->pwm, state);
        if (ret)
            return ret;
        ret = pwm_fan_power_on(ctx);
    } else {
        ret = pwm_fan_power_off(ctx);
    }
    if (!ret)
        ctx->pwm_value = pwm;

    return ret;
}

static int set_pwm(struct pwm_fan_ctx *ctx, unsigned long pwm)
{
    int ret;

    mutex_lock(&ctx->lock);
    ret = __set_pwm(ctx, pwm);
    mutex_unlock(&ctx->lock);

    return ret;
}

static void pwm_fan_update_state(struct pwm_fan_ctx *ctx, unsigned long pwm)
{
    int i;

    for (i = 0; i < ctx->pwm_fan_max_state; ++i)
        if (pwm < ctx->pwm_fan_cooling_levels[i + 1])
            break;

    ctx->pwm_fan_state = i;
}

static int pwm_fan_update_enable(struct pwm_fan_ctx *ctx, long val)
{
    int ret = 0;
    int old_val;

    mutex_lock(&ctx->lock);

    if (ctx->enable_mode == val)
        goto out;

    old_val = ctx->enable_mode;
    ctx->enable_mode = val;

    if (val == 0) {
        /* Disable pwm-fan unconditionally */
        if (ctx->enabled)
            ret = __set_pwm(ctx, 0);
        else
            ret = pwm_fan_switch_power(ctx, false);
        if (ret)
            ctx->enable_mode = old_val;
        pwm_fan_update_state(ctx, 0);
    } else {
        /*
         * Change PWM and/or regulator state if currently disabled
         * Nothing to do if currently enabled
         */
        if (!ctx->enabled) {
            struct pwm_state *state = &ctx->pwm_state;
            bool enable_regulator = false;

            state->duty_cycle = 0;
            pwm_fan_enable_mode_2_state(val,
                            state,
                            &enable_regulator);

            pwm_apply_state(ctx->pwm, state);
            pwm_fan_switch_power(ctx, enable_regulator);
            pwm_fan_update_state(ctx, 0);
        }
    }
out:
    mutex_unlock(&ctx->lock);

    return ret;
}

static int pwm_fan_write(struct device *dev, enum hwmon_sensor_types type,
             u32 attr, int channel, long val)
{
    struct pwm_fan_ctx *ctx = dev_get_drvdata(dev);
    int ret;

    switch (attr) {
    case hwmon_pwm_input:
        if (val < 0 || val > MAX_PWM)
            return -EINVAL;
        ret = set_pwm(ctx, val);
        if (ret)
            return ret;
        pwm_fan_update_state(ctx, val);
        break;
    case hwmon_pwm_enable:
        if (val < 0 || val > 3)
            ret = -EINVAL;
        else
            ret = pwm_fan_update_enable(ctx, val);

        return ret;
    default:
        return -EOPNOTSUPP;
    }

    return 0;
}

static int pwm_fan_read(struct device *dev, enum hwmon_sensor_types type,
            u32 attr, int channel, long *val)
{
    struct pwm_fan_ctx *ctx = dev_get_drvdata(dev);

    switch (type) {
    case hwmon_pwm:
        switch (attr) {
        case hwmon_pwm_input:
            *val = ctx->pwm_value;
            return 0;
        case hwmon_pwm_enable:
            *val = ctx->enable_mode;
            return 0;
        }
        return -EOPNOTSUPP;
    default:
        return -ENOTSUPP;
    }
}

static umode_t pwm_fan_is_visible(const void *data,
                  enum hwmon_sensor_types type,
                  u32 attr, int channel)
{
    switch (type) {
    case hwmon_pwm:
        return 0644;

    case hwmon_fan:
        return 0444;

    default:
        return 0;
    }
}

static const struct hwmon_ops pwm_fan_hwmon_ops = {
    .is_visible = pwm_fan_is_visible,
    .read = pwm_fan_read,
    .write = pwm_fan_write,
};

static int pwm_fan_get_max_state(struct thermal_cooling_device *cdev,
                 unsigned long *state)
{
    struct pwm_fan_ctx *ctx = cdev->devdata;

    if (!ctx)
        return -EINVAL;

    *state = ctx->pwm_fan_max_state;

    return 0;
}

static int pwm_fan_get_cur_state(struct thermal_cooling_device *cdev,
                 unsigned long *state)
{
    struct pwm_fan_ctx *ctx = cdev->devdata;

    if (!ctx)
        return -EINVAL;

    *state = ctx->pwm_fan_state;

    return 0;
}

static int
pwm_fan_set_cur_state(struct thermal_cooling_device *cdev, unsigned long state)
{
    struct pwm_fan_ctx *ctx = cdev->devdata;
    int ret;

    if (!ctx || (state > ctx->pwm_fan_max_state))
        return -EINVAL;

    if (state == ctx->pwm_fan_state)
        return 0;

    ret = set_pwm(ctx, ctx->pwm_fan_cooling_levels[state]);
    if (ret) {
        dev_err(&cdev->device, "Cannot set pwm!\n");
        return ret;
    }

    ctx->pwm_fan_state = state;

    return ret;
}

static const struct thermal_cooling_device_ops pwm_fan_cooling_ops = {
    .get_max_state = pwm_fan_get_max_state,
    .get_cur_state = pwm_fan_get_cur_state,
    .set_cur_state = pwm_fan_set_cur_state,
};

static void pwm_fan_enable_mode_2_state(int enable_mode,
                    struct pwm_state *state,
                    bool *enable_regulator)
{
    switch (enable_mode) {
    case pwm_disable_reg_enable:
        /* disable pwm, keep regulator enabled */
        state->enabled = false;
        *enable_regulator = true;
        break;
    case pwm_enable_reg_enable:
        /* keep pwm and regulator enabled */
        state->enabled = true;
        *enable_regulator = true;
        break;
    case pwm_off_reg_off:
    case pwm_disable_reg_disable:
        /* disable pwm and regulator */
        state->enabled = false;
        *enable_regulator = false;
    }
}

static void pwm_fan_cleanup(void *__ctx)
{
    struct pwm_fan_ctx *ctx = __ctx;

    /* Switch off everything */
    ctx->enable_mode = pwm_disable_reg_disable;
    pwm_fan_power_off(ctx);
}

static int pwm_fan_of_get_cooling_data(struct device *dev,
                       struct pwm_fan_ctx *ctx)
{
    struct device_node *np = dev->of_node;
    int num, i, ret;

    if (!of_find_property(np, "cooling-levels", NULL))
        return 0;

    ret = of_property_count_u32_elems(np, "cooling-levels");
    if (ret <= 0) {
        dev_err(dev, "Wrong data!\n");
        return ret ? : -EINVAL;
    }

    num = ret;
    ctx->pwm_fan_cooling_levels = devm_kcalloc(dev, num, sizeof(u32),
                           GFP_KERNEL);
    if (!ctx->pwm_fan_cooling_levels)
        return -ENOMEM;

    ret = of_property_read_u32_array(np, "cooling-levels",
                     ctx->pwm_fan_cooling_levels, num);
    if (ret) {
        dev_err(dev, "Property 'cooling-levels' cannot be read!\n");
        return ret;
    }

    for (i = 0; i < num; i++) {
        if (ctx->pwm_fan_cooling_levels[i] > MAX_PWM) {
            dev_err(dev, "PWM fan state[%d]:%d > %d\n", i,
                ctx->pwm_fan_cooling_levels[i], MAX_PWM);
            return -EINVAL;
        }
    }

    ctx->pwm_fan_max_state = num - 1;

    return 0;
}

static int pwm_fan_probe(struct platform_device *pdev)
{
    struct thermal_cooling_device *cdev;
    struct device *dev = &pdev->dev;
    struct pwm_fan_ctx *ctx;
    struct device *hwmon;
    int ret;
    const struct hwmon_channel_info **channels;
    int channel_count = 1;    /* We always have a PWM channel. */

    ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
    if (!ctx)
        return -ENOMEM;

    mutex_init(&ctx->lock);

    ctx->dev = &pdev->dev;
    ctx->pwm = devm_pwm_get(dev, NULL);
    if (IS_ERR(ctx->pwm))
        return dev_err_probe(dev, PTR_ERR(ctx->pwm), "Could not get PWM\n");

    platform_set_drvdata(pdev, ctx);

    ctx->reg_en = devm_regulator_get_optional(dev, "fan");
    if (IS_ERR(ctx->reg_en)) {
        if (PTR_ERR(ctx->reg_en) != -ENODEV)
            return PTR_ERR(ctx->reg_en);

        ctx->reg_en = NULL;
    }

    pwm_init_state(ctx->pwm, &ctx->pwm_state);

    /*
     * set_pwm assumes that MAX_PWM * (period - 1) fits into an unsigned
     * long. Check this here to prevent the fan running at a too low
     * frequency.
     */
    if (ctx->pwm_state.period > ULONG_MAX / MAX_PWM + 1) {
        dev_err(dev, "Configured period too big\n");
        return -EINVAL;
    }

    ctx->enable_mode = pwm_disable_reg_enable;

    /*
     * Set duty cycle to maximum allowed and enable PWM output as well as
     * the regulator. In case of error nothing is changed
     */
    ret = set_pwm(ctx, MAX_PWM);
    if (ret) {
        dev_err(dev, "Failed to configure PWM: %d\n", ret);
        return ret;
    }
    ret = devm_add_action_or_reset(dev, pwm_fan_cleanup, ctx);
    if (ret)
        return ret;

    channels = devm_kcalloc(dev, channel_count + 1,
                sizeof(struct hwmon_channel_info *), GFP_KERNEL);
    if (!channels)
        return -ENOMEM;

    channels[0] = HWMON_CHANNEL_INFO(pwm, HWMON_PWM_INPUT | HWMON_PWM_ENABLE);

    ctx->info.ops = &pwm_fan_hwmon_ops;
    ctx->info.info = channels;

    hwmon = devm_hwmon_device_register_with_info(dev, "pwmfan",
                             ctx, &ctx->info, NULL);
    if (IS_ERR(hwmon)) {
        dev_err(dev, "Failed to register hwmon device\n");
        return PTR_ERR(hwmon);
    }

    ret = pwm_fan_of_get_cooling_data(dev, ctx);
    if (ret)
        return ret;

    ctx->pwm_fan_state = ctx->pwm_fan_max_state;
    if (IS_ENABLED(CONFIG_THERMAL)) {
        cdev = devm_thermal_of_cooling_device_register(dev,
            dev->of_node, "pwm-fan", ctx, &pwm_fan_cooling_ops);
        if (IS_ERR(cdev)) {
            ret = PTR_ERR(cdev);
            dev_err(dev,
                "Failed to register pwm-fan as cooling device: %d\n",
                ret);
            return ret;
        }
        ctx->cdev = cdev;
    }

    return 0;
}

static const struct of_device_id of_pwm_fan_match[] = {
    { .compatible = "rmk,pwm-fan", },
    {},
};
MODULE_DEVICE_TABLE(of, of_pwm_fan_match);

static struct platform_driver pwm_fan_driver = {
    .probe        = pwm_fan_probe,
    .driver    = {
        .name        = "pwm-fan",
        .of_match_table    = of_pwm_fan_match,
    },
};

module_platform_driver(pwm_fan_driver);

MODULE_AUTHOR("zc <1107473010@qq.com>");
MODULE_ALIAS("platform:pwm-fan");
MODULE_DESCRIPTION("PWM FAN driver");
MODULE_LICENSE("GPL");