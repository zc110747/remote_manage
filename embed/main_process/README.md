# remote_manage
版本更新内容和计划详细见document/系统文件说明.xlsx中的Schedular目录.

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