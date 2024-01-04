
# 版本发布信息

## plans - [FM Version 1.0.0.9]

[embed]
1.增加各模块的logger调试接口

- !gui [command] [data]
- !locd [command] [data]
- !mp  [command] [data]
- !lowd  [command] [data]

2.node_server使用mqtt接口实现，基于json进行数据交互。
3.main_process增加对当前设备状态信息的获取,并转换成json数据。

[web]
1.基于vue进行界面重构，生成新的网页实现

[Driver]
1.增加对IIO驱动的处理(ADC)

## [FM Version 1.0.0.8]

[ALL]
1.扩展环境构建命令，支持debain文件系统，交叉编译第三方库。
2.项目结构更新，对于模块，应用，系统，远端进行分类管理。

[embed]
1.将系统拆分，包含调试，gui, 硬件设备管理，下位机硬件管理和主控模块
2.支持mqtt应用，移植mqtt客户端支持
3.增加fmt库的支持

## [FM Version 1.0.0.7]

[ALL]
1.项目结构更新，增加Linux扩展命令用于编译

[Application]
1.应用代码重新整理，进行注释
2.移除udp协议，对于tcp和uart通讯进行重构，修改为异步实现
3.协议模块重构，基于fifo解析处理

## [FM Version 1.0.0.6]

NA

## [FM Version 1.0.0.5]

NA

## [FM Version 1.0.0.4]

1.logger接口添加，基于asio实现web服务,支持远程logger
2.逐步采用C++接口替代linux系统原生接口，进行现代C++功能改造
3.增加基于事件的硬件处理模块并封装，为后期多线程控制硬件提供接口

## [FM Version 1.0.0.3]

1.SPI和LED的驱动不能够共存问题解决，修改LED驱动为设备树模式
2.I2C传感器的上下位机处理添加完成，测试功能正常
3.Driver模块整体修改，符合单例模式

## [FM Version 1.0.0.2]

1.增加关于线程同步的处理, 基于posix消息队列和软件定时器timer, 去除sleep操作
2.增加文件的传输机制，包含指令和数据传输，测试UDP和TCP文件传输满足要求
3.SPI驱动添加lower和upper的处理
4.线程同步支持Posix Mq和FIFO两种实现, 以应对WSL中不支持Mq的运行环境
5.RTC驱动添加lower和upper的处理, 增加基于系统time的替代方案

## [FM Version 1.0.0.1]

1.完成UART的串口通讯实现, 建立一套局域网通讯的协议规范
2.配合Beep，Led实现初步的远程控制
3.整合协议层，实现独立的接口应用
4.扩展实现基于Socket(TCP)的通讯机制
5.扩展实现基于Socket(UDP)的通讯机制
6.添加基于JSON的启动状态配置信息处理
7.完成协议通讯中CRC校验的实际实现
8.添加对LED和BEEP状态位的更新,解决数据处理中的bug
