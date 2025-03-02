# LED驱动说明

LED驱动包含设备树定义，字符设备创建和硬件访问接口三部分组成。

## 设备树声明

```c
usr_led {
    compatible = "rmk,usr-led";
    pinctrl-0 = <&pinctrl_gpio_led>;
    led-gpio = <&gpio1 3 GPIO_ACTIVE_LOW>;
    status = "okay";
};

pinctrl_gpio_led: gpio-leds {
    fsl,pins = <
        MX6UL_PAD_GPIO1_IO03__GPIO1_IO03    0x17059
    >;
};
```

## 字符设备创建

```c
static int led_device_create(struct led_data *chip)
{
    int ret;
    int major, minor;
    struct platform_device *pdev;

    major = DEFAULT_MAJOR;
    minor = DEFAULT_MINOR;
    pdev = chip->pdev;

    if (major){
        chip->dev_id = MKDEV(major, minor);
        ret = register_chrdev_region(chip->dev_id, 1, DEVICE_NAME);
    } else {
        ret = alloc_chrdev_region(&chip->dev_id, 0, 1, DEVICE_NAME);
    }

    if (ret < 0){
        dev_err(&pdev->dev, "id alloc failed!\n");
        goto exit;
    }
    
    cdev_init(&chip->cdev, &led_fops);
    chip->cdev.owner = THIS_MODULE;
    ret = cdev_add(&chip->cdev, chip->dev_id, 1);
    if (ret){
        dev_err(&pdev->dev, "cdev add failed:%d!\n", ret);
        goto exit_cdev_add;
    }

    chip->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(chip->class)) {
        dev_err(&pdev->dev, "class create failed!\n");
        ret = PTR_ERR(chip->class);
        goto exit_class_create;
    }

    chip->device = device_create(chip->class, NULL, chip->dev_id, NULL, DEVICE_NAME);
    if (IS_ERR(chip->device)) {
        dev_err(&pdev->dev, "device create failed!\r\n");
        ret = PTR_ERR(chip->device);
        goto exit_device_create;
    }

    dev_info(&pdev->dev, "device create success!\r\n");
    return 0;

exit_device_create:
    class_destroy(chip->class);
exit_class_create:
    cdev_del(&chip->cdev);
exit_cdev_add:
    unregister_chrdev_region(chip->dev_id, 1);
exit:
    return ret;
}
```

### 设备硬件访问

```c
static int led_hardware_init(struct led_data *chip)
{
    int ret;
    struct platform_device *pdev = chip->pdev;
    struct device_node *led_nd = pdev->dev.of_node;

    chip->gpio = of_get_named_gpio(led_nd, "led-gpio", 0);
    if (chip->gpio < 0){
        dev_err(&pdev->dev, "find gpio in dts failed!\n");
        return -EINVAL;
    }
    ret = devm_gpio_request(&pdev->dev, chip->gpio, "led");
    if (ret < 0){
        dev_err(&pdev->dev, "request gpio failed!\n");
        return -EINVAL;   
    }

    gpio_direction_output(chip->gpio, 1);
    led_hardware_set(chip, LED_OFF);

    dev_info(&pdev->dev, "hardware init finished, %s num %d", led_nd->name, chip->gpio);
    return 0;
}
```
