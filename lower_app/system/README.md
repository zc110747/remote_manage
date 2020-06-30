# remote_manage

常见错误整理:
1. 使用posix mq需要内核支持消息队列，错误处理Function no 
让内核支持posix消息队列
```bash
make menuconfig
 General setup  --->
       [ ] POSIX Message Queues  -> [*] POSIX Message Queues 
```
2. insmod:disagrees about version of symbol module_layout
内核版本和模块版本不一致
重新编译内核后，需要重新编译模块

下位机的具体实现，目前功能:  
1.LED，BEEP，复位的远程控制  
2.UART的远程协议控制，支持不同硬件的扩展(参考Uart实现)  
3.Socket(TCP)的远程协议控制  
4.基于json格式的初始化配置  

版本信息和更新计划:  
## version v0.0.1  
完成UART的串口通讯实现, 建立一套局域网通讯的协议规范  
配合Beep，Led实现初步的远程控制  

## version v0.0.2  
整合协议层，实现独立的接口应用  
扩展实现基于Socket(TCP)的通讯机制  

## version v0.03  
扩展实现基于Socket(UDP)的通讯机制  
通讯异常修复  

## version v0.04  
添加基于JSON的启动状态配置信息处理  
完成协议通讯中CRC校验的实际实现  
添加对LED和BEEP状态位的更新,解决数据处理中的bug  

## version 0.0.5 20200617
增加关于线程同步的处理, 基于posix消息队列和软件定时器timer, 去除sleep操作  
降低CPU占用率，提高响应效率  
增加文件的传输机制，包含指令和数据传输，测试UDP和TCP文件传输满足要求  

## version 0.0.6 20200628
SPI驱动添加lower和upper的处理  
线程同步支持Posix Mq和FIFO两种实现, 以应对WSL中不支持Mq的运行环境  
RTC驱动添加lower和upper的处理, 增加基于系统time的替代方案

下一步计划:
单元测试的模块添加 
文件的断点重传功能
TCP对于数据传输处理增加线程池/epoll替代方案的实现
提供数据库sqlite而非共享内存的替换方案  
通讯数据的安全加密openssl

代码结构:  
driver/     硬件驱动的实现  
include/    头文件路径
lib/        应用编写需要的外部支持库
source/     应用代码实现
template/   代码结构基础模板  

编译方法:  
修改Makefile内的注释:
PC端平台选择    
```bash
#HOST = arm-linux-gnueabihf-
#CPU = arm-
HOST=
CPU=x86-
```
嵌入式Linux平台选择
```bash
HOST = arm-linux-gnueabihf-
CPU = arm-
#HOST=
#CPU=x86-
```
HOST根据具体的嵌入式Linux平台选择, 目前提供的jsoncpp库只支持PC端和imx6ull, 使用其它平台需要修改重新编译   JsonCPP的库。  