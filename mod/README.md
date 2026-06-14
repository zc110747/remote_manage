# 驱动应用和测试

## led

设备文件/dev/led操作。

```shell
# 设备文件 /dev/led
# 通过设备文件读取
hexdump -C -n 1 /dev/led

# 写入设备文件
echo 0 > /dev/led
echo 1 > /dev/led
```

引脚配置pinctrl修改。

```shell
# 配置文件
/sys/devices/platform/20c406c.usr_led/pinctrl_config

# 读取寄存器
cat /sys/devices/platform/20c406c.usr_led/pinctrl_config

# 写入寄存器
echo 0 > /sys/devices/platform/20c406c.usr_led/pinctrl_config
echo 1 > /sys/devices/platform/20c406c.usr_led/pinctrl_config

# 读取存储区域命令
devmem 0x020E02F4 32 0x10B0
devmem 0x020E02F4
```

## loopled

```shell

```

## rtc

```shell
# 从rtc读取数据
hwclock -r -f /dev/rtc1             # 从rtc中读取时间并显示

# 从rtc读取时间并设置到系统中
hwclock -s -f /dev/rtc1             # 从rtc中读取时间到系统date中
date                                # 查看当前的系统时间

# 将系统时间设置到rtc1中
date -s '2024-03-29 22:40:00'       # 设置系统date时间
hwclock -w -f /dev/rtc1             # 将date时间同步到系统rtc中
```