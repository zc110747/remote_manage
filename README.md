# remote_manage
用于运行在imx6ull平台的远程管理方案  
目的:  
整合嵌入式Linux开发需要的技术，从应用开发的角度实现综合性的项目。  

设计思路和文档:  
https://www.cnblogs.com/zc110747/p/12747213.html  

具体实现功能需求  
1.对嵌入式Linux中字符型驱动设备，I2C, SPI，设备树等进行运用  
2.支持配置管理，允许通过文件管理设备启动状态  
3.对Linux应用层接口，多线程/多进程知识学习和应用  
4.串口的远程管理，包含自定义协议的知识处理  
5.socket(UDP, TCP)的局域网远程管理  
6.支持基于QT技术的界面化设备管理  
  
项目结构  
demo/       快速体验下位机应用的代码  
document/   文档资料说明  
kernal_mod/ 内核驱动模块实现  
lower_app/  嵌入式Linux平台中C++代码实现  
server/     嵌入式Linux平台的服务器支持node环境和前端网页   
test/       用于测试的代码  
upper_app/  上位机代码基于QT控制实现  

硬件适配和兼容性:  
下位机驱动适配平台(kernal_mod):正点原子I.MX6U-ALPHA 开发板  
下位机应用层适配平台(lower_app):兼容嵌入式Linux平台，仅修改设备文件名即可支持  
上位机兼容性(upper_app):仅测试window10平台， QT5.12.9工具，不保证其它本版本系统的兼容性  

编译环境  
下位机交叉编译工具:    
arm-linux-gnueabihf-gcc编译内核模块  
arm-linux-gnueabihf-g++编译应用层实现  
版本v4.9.3  
上位机编译工具: QT5.12.8  
目前Makefile中路径为绝对路径，实际编译需要修改。  

本项目是我学习嵌入式Linux开发，结合自身目前的产品开发经验，形成的综合性项目，不一定适用于实际产品，但其中的思路在嵌入式Linux产品开发中是共通的，希望对每个看过的人有帮助。
