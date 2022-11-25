# 基于嵌入式Linux的局域网和云平台管理应用

## 项目框架

![image](https://github.com/zc110747/remote_manage/blob/master/document/Image/firmware.jpg)

## 项目结构

demo/           	测试代码    
document/       	设计文档资料说明    
kernael_mod/     	内核驱动模块  
lower_app/              嵌入式Linux设备应用实现  
	-manage/     	主工作流应用，驱动模块处理，logger实现，外设和其它设备接口访问  
	-gui/        	下位机图形界面，支持状态显示和基本操作(QT)  
	-server/        支持桌面访问得web服务器(node)  
support/        	用于支持应用执行的lib库或者环境  
upper_app/              PC客户端应用实现  
	-manage/        用于访问嵌入式设备的桌面客户端(暂定QT)  

## 设计文档

参考文档(见document目录下说明), 初步设计包含:

嵌入式Linux端设计

1. 嵌入式驱动设备的访问，包含字符型设备，I2C, SPI，串口等访问，包含设备树支持
2. 支持配置管理，允许通过文件管理设备启动状态(配置文件使用JSON格式)
3. 基于linux系统API的线程创建，管理，线程间通讯支持
4. 本地端的通讯处理(Serial, Socket)等，基于自定义协议进行通讯(特殊指令的安全机制)
5. 内部状态读取更新显示，远端和本地的设置修改
6. 支持本地和网络的logger打印接口，调试等级显示可调
7. 访问外部设备的模块接口

PC应用端设计

1. 访问Linux端接口，获取内部数据
2. 操作Linux端设备外设
3. 控制访问linux管理的远端信息
4. 其它功能组件

## 硬件适配和兼容性

涉及内核取代模块部分因与平台和硬件绑定，只支持imx6ull内核，使用正点原子阿尔法开发板  
下位机只支持Linux平台，通过修改编译选项和宏定义支持Linux和嵌入式Linux平台，涉及硬件的部分在非嵌入式Linu平台会执行失败  
上位机基于windows平台开发，后续扩展支持其它平台  

## 编译环境

嵌入式软件交叉编译工具
	内核模块使用编译工具 - arm-linux-gnueabihf-gcc  
	manage，gui编译工具 - arm-linux-gnueabihf-g++  
	server使用node作为运行环境  
	网页则使用前端技术
上位机编译工具
	QT(the newest stable version)
