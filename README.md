# remote_manage
基于嵌入式Linux的局域网和云平台管理方案开发  

设计思路和文档:  
https://www.cnblogs.com/zc110747/p/12747213.html  

具体实现功能需求  
1.对嵌入式Linux中字符型驱动设备，I2C, SPI等进行访问,支持设备树  
2.支持配置管理，允许通过文件管理设备启动状态(配置文件使用JSON格式)  
3.对Linux应用层API接口和多线程的应用,支持线程同步  
4.串口的远程管理，基于自定义协议  
5.socket(UDP, TCP)的局域网远程管理，兼容自定义协议  
6.支持基于QT技术的上位机界面化设备管理  
7.通讯的安全机制处理  
8.支持云平台方案开发  
9.支持与云平台方案对接的手机端系统开发  
 
项目框架  
![image](https://github.com/zc110747/remote_manage/blob/master/document/Image/system.png)  

项目结构  
demo/           编译好可执行的平台代码  
document/       设计文档资料说明  
kernal_mod/     内核驱动模块实现  
lower_app/      嵌入式Linux平台下位机代码实现  
---node_server/    基于node的服务器实现, 用于浏览器的局域网访问  
---qt_gui/         基于QT界面的下位机实现  
---system/         涉及UDP，TCP和UART的局域网通讯实现，同时外设的访问和管理  
support/        用于支持平台代运行的库  
test_app/       用于实现单个功能的代码，后续添加到lower_app和upper_app下  
upper_app/      上位机的代码实现  
---qt_manage/      基于QT实现的上位机管理界面    

硬件适配和兼容性:  
涉及内核取代模块部分因与平台和硬件绑定，只支持imx6ull内核，使用正点原子阿尔法开发板  
下位机只支持Linux平台，通过修改编译选项和宏定义支持Linux和嵌入式Linux平台，涉及硬件的部分在非嵌入式Linu平台会执行失败  
上位机基于windows平台开发，后续扩展支持其它平台  

编译环境  
下位机软件交叉编译工具 
内核模块使用编译工具:arm-linux-gnueabihf-gcc  
下位机应用层编编译工具和实现环境:  
qt_gui和system使用arm-linux-gnueabihf-g++或g++  
node_server使用node作为运行环境，执行语言为js    
上位机编译工具  
QT5.12.8  

本项目是结合自身职业开发经历，总结经验实现的嵌入式Linux综合性项目，在设计上会考虑实际产品开发中的各种细节问题，对于下位机的代码开发中，也会尽量考虑按照实际量产项目的标准开发和调试软件实现，作为入门的项目会过于复杂，但是基本包含到大部分嵌入式开发所需要的思想，目前进一步深入开发。  
