# logger_tool

logger是管理系统调试的功能模块，主要包含功能调试信息打印和命令行管理两部分工作内容。

1. 调试信息打印，主要接收其它设备的调试信息，然后输出到打印窗口。打印窗口主要有界面窗口和远程的socket客户端窗口。
2. 命令行管理，用于接收命令行的数据，转发给对应的模块处理。

## logger_printf

logger打印模式主要流程为：

设备产生调试信息 => 写入发送logger发送fifo(LOGGER_RX_FIFO) => logger工具读取fifo中数据 => 将数据根据当前连接接口发送(本地/远端socket客户端)

```shell

[day hour:minute:second][deivce][logger_level]:information data
example:
[0000 00:00:34][main_process][2]:global new information.

```

注意: **发送logger信息长度要小于PIPE stack, 默认为512字节，保证fifo的原子性，避免数据冲突**。

## logger_command

logger命令行模式，主要用于指令管理，对于每个模块使用不同的接口。

```shell
#提交到GUI的命令
!gui [command]

#提交到local_device的命令
!loc_dev [command]

#提交到lower_device的命令
!low_dev [command]

#提交到main_process的命令
!mp [command]
```

对于每个设备的支持的命令格式如下。

