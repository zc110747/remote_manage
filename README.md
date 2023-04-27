# 基于嵌入式Linux的局域网和云平台管理应用

## 项目框架

![image](https://github.com/zc110747/remote_manage/document/image/firmware.jpg)

## 如何编译执行项目

通过git下载项目后，进入remote_manage目录下.  
1.修改environment/.bashrc下的配置文件.  
主要修改项如下:  
```bash
export ENV_PATH_ROOT="/mnt/d/user_project/git/remote_manage" 
export ENV_KERNEL_DIR="/home/center/application/project/linux" 
export EMBED_DEVICE_IPADDRESS="192.168.2.99"
```
2.将environment/.bashrc复制到系统根目录.  
```bash
cp environment/.bashrc /home/freedom
```
然后重新Ctrl+ALT+T打开新窗口，如果显示如下，表示脚本加载成功.  
```bash
Loading CDE Plugin...
-------------------------------------------------------------------------
Load Plugin Success!
Can use command 'SysHelpCommand' for more helps.
Update the Plugin by filepath /home/[root]/.bashrc.
Root Path:/mnt/d/user_project/git/remote_manage
Load the Env Data...
Update Environment Data Success!
Current Platform is Embed-Linux, Remote IPAdress is 192.168.1.24.
Current Firmware Version is 1.0.0.9.
Update the Alias Command...
Update the Alias Command Success!
-------------------------------------------------------------------------
```
3.使用命令进行编译和上传firmware.  
```bash
SysBulidApplication
SysPushFirmware
```
即可完成项目的编译。  
如果编译报错，可参考*document/构建Linux编译环境.md*目录下说明  
1.如果编译失败，应该是g++版本过低，中说明如何更新arm-linux-gnueabihf-g++版本，目前测试通过使用的是7.5.0版本.  
2.在编译完成后，通过ssh将打包后文件传送到嵌入式平台，并上传到/tmp目录下，执行SysPushFirmware命令即可实现Code更新.  
3.嵌入式linux平台需要支持node服务器，可参考server/README.md构建，另外需要支持环回接口即127.0.0.1本地连接, 需要rcS文件添加如下端口.  

```bash
ifconfig lo up
ifconfig lo netmask 255.255.255.0
```

## 项目结构

Build/              编译结构基本框架  
document/       	设计文档资料说明  
kernael_mod/     	内核驱动模块  
lower_app/          嵌入式Linux设备应用实现  
    Executables     生成可执行文件目录  
    manage/         嵌入式数据管理进程(数据处理核心进程)  
    gui/            嵌入式图形界面开发  
    server/         web服务器和web网页  
support/            用于支持应用执行的lib库或者环境  
package/            编译完成后的打包目录
upper_app/          PC客户端应用实现  
    manage/         用于访问嵌入式设备的桌面客户端(暂定QT)  
    loger_tool/     用于支持logger打印的网络调试工具(C#)  

## 设计文档

参考文档(见document目录下说明), 初步设计包含:  

嵌入式Linux端设计  

1. 嵌入式驱动设备的访问，包含字符型设备，I2C, SPI，串口等访问，包含设备树支持  
2. 支持配置管理，允许通过文件管理设备启动状态(配置文件使用JSON格式)  
3. 基于linux系统API的线程创建，管理，线程间通讯支持  
4. 本地端的通讯处理(Serial, Socket)等，基于自定义协议进行通讯(特殊指令的安全机制)  
5. 内部状态读取更新显示，远端和本地的设置修改  
6. 支持本地和网络的logger打印接口(基于asio设计)，调试等级显示可调  
7. 访问外部设备的模块接口(基于CAN或者串口的轮询控制接口)  

PC应用端设计  

1. 访问Linux端接口，获取内部数据  
2. 操作Linux端设备外设  
3. 控制访问linux管理的远端信息  
4. 其它功能组件  

版本更新说明见demo/version_releas.md  

## 硬件适配和兼容性

涉及内核取代模块部分因与平台和硬件绑定，只支持imx6ull内核，使用正点原子阿尔法开发板  
下位机只支持Linux平台，通过修改编译选项和宏定义支持Linux和嵌入式Linux平台，涉及硬件的部分在非嵌入式Linu平台会执行失败  
上位机基于windows平台开发，后续扩展支持其它平台  

## 编译环境

嵌入式软件交叉编译工具  
    内核模块使用编译工具 - arm-linux-gnueabihf-gcc  
    manage，gui编译工具 - arm-linux-gnueabihf-g++  
    server使用node作为运行环境  
    网页使用vue方案  
上位机编译工具  
    QT(the newest stable version)  
logger显示工具  
    visual studio(the newest stable version)  