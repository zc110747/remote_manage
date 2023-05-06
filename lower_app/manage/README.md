# remote_manage

## version v0.0.0.4
1.logger接口添加，基于asio实现web服务,支持远程logger<br/>
2.逐步采用C++接口替代linux系统原生接口，进行现代C++功能改造
3.增加基于事件的硬件处理模块并封装，为后期多线程控制硬件提供接口<br/>

## version v0.0.0.3
1.SPI和LED的驱动不能够共存问题解决，修改LED驱动为设备树模式<br/>
2.I2C传感器的上下位机处理添加完成，测试功能正常<br/>
3.Driver模块整体修改，符合单例模式<br/>

## version v0.0.0.2
1.增加关于线程同步的处理, 基于posix消息队列和软件定时器timer, 去除sleep操作<br/>
2.增加文件的传输机制，包含指令和数据传输，测试UDP和TCP文件传输满足要求<br/>
3.SPI驱动添加lower和upper的处理<br/>
4.线程同步支持Posix Mq和FIFO两种实现, 以应对WSL中不支持Mq的运行环境<br/>
5.RTC驱动添加lower和upper的处理, 增加基于系统time的替代方案

## version v0.0.0.1
1.完成UART的串口通讯实现, 建立一套局域网通讯的协议规范<br/>
2.配合Beep，Led实现初步的远程控制<br/>
3.整合协议层，实现独立的接口应用<br/>
4.扩展实现基于Socket(TCP)的通讯机制<br/>
5.扩展实现基于Socket(UDP)的通讯机制<br/>
6.添加基于JSON的启动状态配置信息处理<br/>
7.完成协议通讯中CRC校验的实际实现<br/>
8.添加对LED和BEEP状态位的更新,解决数据处理中的bug<br/>

## bug issue
1. 使用posix mq需要内核支持消息队列，错误处理Function no<br/>
让内核支持posix消息队列<br/>
```bash
make menuconfig
 General setup<br/>--->
<br/><br/><br/> [ ] POSIX Message Queues<br/>-> [*] POSIX Message Queues 
```
2. insmod:disagrees about version of symbol module_layout<br/>
内核版本和模块版本不一致<br/>
重新编译内核后，需要重新编译模块<br/>
下位机的具体实现，目前功能:<br/>
1.LED，BEEP，复位的远程控制<br/>
2.UART的远程协议控制，支持不同硬件的扩展(参考Uart实现)<br/>
3.Socket(TCP)的远程协议控制<br/>
4.基于json格式的初始化配置<br/>

代码结构:<br/>
driver/<br/><br/> 硬件驱动的实现<br/>
include/<br/><br/>头文件路径<br/>
lib/<br/><br/><br/><br/>应用编写需要的外部支持库<br/>
source/<br/><br/> 应用代码实现<br/>
template/<br/> 代码结构基础模板<br/> 