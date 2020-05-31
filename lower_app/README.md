# remote_manage

下位机的具体实现，目前功能:  
1.LED，BEEP，复位的远程控制  
2.UART的远程协议控制，支持不同硬件的扩展(参考Uart实现)  
3.Socket(TCP)的远程协议控制  
4.基于json格式的初始化配置  

代码结构:  
driver/ 硬件驱动的实现  
include/ 头文件路径
lib/ 应用编写需要的外部支持库
source/ 应用代码实现
template/ 代码结构模板