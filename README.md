# 基于嵌入式Linux的局域网和云平台管理应用

## 项目说明
用于管理

## 项目框架

![image](doc/image/mainFrame.png)

## 快速启动项目
下载项目后到本地后，执行预处理脚本构建完整的应用平台。
```bash
#download the program
git clone https://github.com/zc110747/remote_manage.

#enter the directory
cd remote_mange/

#prepare for the environment
./preBuildEnvironment.sh
```
重新开启命令行，如果加载如下所示，表示已经成功安装，项目需要在非root权限下执行，root权限无法加载。
```bash
Loading CDE Plugin...
-------------------------------------------------------------------------
Load Plugin Success!
Update the Plugin by filepath /home/[root]/.bashrc.
Root Path:/home/program/application/remote_manage
Load the Env Data...
Update Environment Data Success!
Can use command 'SysHelpCommand' for more helps.
Current Platform is Embed-Linux, Remote IPAdress is 192.168.1.24.        
Current Firmware Version is 1.0.0.7.
Update the Alias Command...
Update the Alias Command Success!
-------------------------------------------------------------------------

#进入当前项目目录
cda

#下载第三方应用, 编译当前平台的库文件
SysPullAll
```
备注:<br />
Current Platform is Embed-Linux表示的是编译后的固件执行平台，分别代表使用g++和arm-linux-gnueabihf-g++编译.<br />
Remote IPAdress指示用于通过ssh上传编译好固件的地址.<br />
3.在使用该平台前，需要搭建好支持uboot，kernal，rootfs编译的嵌入式Linux系统，并构建平台运行执行的环境(包含设备树，node执行环境)，后续会用网盘分享我个人编译好的一套系统，如果自己希望构建，参考document/linux summy notes/3.嵌入式Linux编译.md.<br />
4.对于以构建好能够执行的嵌入式Linux平台(阿尔法开发板为例)，使用如下命令可进行编译和上传文件包.<br/>
```bash
SysBulidApplication
SysPushFirmware
```
即可完成项目的编译。<br/>
另外可通过SysHelpCommand查询支持的命令, SysSetPlatformEmbedLinux和SysSetPlatformLinux切换编译不同平台的应用文件，目前支持命令如下所示.<br/>
```bash
SysPullAll
    Pull all thirdpart application for build.
SysBuildApplication
    Build Application, Package Firmware.
SysBuildKernel
    Build the Kernal, if exist kernal directory.
SysSetPlatformEmbedLinux
    Set Current Platform to Embed-Linux Complier.
SysSetPlatformLinux
    Set Current Platform to Linux Complier.
SysSetRemoteIpAddress
    example:SysSetRemoteIpAddress 192.168.0.1.
    Set the remote ipaddress for command <SysPushApplication>
SysSetApplicationVer
    example:SysSetRemoteIpAddress 1.0.0.1.
    Set the verison of the firmware when build
SysBuildApplication
    Build the application, copy to nfs directory, also can push to embed-linux device.
SysPushApplication
    Push firmware to remote embed-linux device.
SysCleanApplication
    Clean the app firmware for next.
SysBuildKernel
    Build the linux kernal, copy to tftp directory, if failed, execute SysCleanKernel first.
SysCleanKernel
    Clean the kernal compiler data for next.
SysBuildUboot
    Build the uboot, copy to tftp directory, if failed, execute SysCleanUboot first.
SysCleanUboot
    Clean the uboot compiler data for next.
SysBuildRootfs
    build the rootfs, copy to nfs directory, if failed, execut SysCleanRootfs first.
SysCleanRootfs
    Clean the rootfs compiler data for next.
SysPackageFirmware
    Package the uboot, kernal, rootfs and application, can use tools download.
SysHelpCommand
    Show the help command.
```
如果编译报错，可参考*document/构建Linux编译环境.md*目录下说明.<br/>
1.上述命令支持需要在指定的目录里仿真对应的文件，如果不存在会打印对应的目录，需要将对应的文件放置在指定目录中。<br/>
2.如果编译失败，应该是g++版本过低，中说明如何更新arm-linux-gnueabihf-g++版本，目前测试通过使用的是7.5.0版本.<br/>
3.在编译完成后，通过ssh将打包后文件传送到嵌入式平台，并上传到/tmp目录下，执行SysPushFirmware命令即可实现Code更新.<br/>
4.嵌入式linux平台需要支持node服务器，可参考server/README.md构建，另外需要支持环回接口即127.0.0.1本地连接, 需要rcS文件添加如下端口.<br/>

```bash
ifconfig lo up
ifconfig lo netmask 255.255.255.0
```

## 项目结构

Build/              编译文件的基础结构，后面编译文件依赖此结构<br/>
document/           设计文档资料说明<br/>
enviroment/         用于构建编译环境的脚本<br/>
kernal_mod/         内核驱动模块实现<br/>
lower_app/          嵌入式Linux设备应用实现<br/>
&emsp;&emsp;Executables     生成可执行文件目录<br/>
&emsp;&emsp;manage/         嵌入式数据管理进程(数据处理核心进程)<br/>
&emsp;&emsp;gui/            嵌入式图形界面开发<br/>
&emsp;&emsp;server/         web服务器和web网页<br/>
package/            生成应用的打包文件<br/>
thirdparts/         支持平台运行的库，程序等<br/>
package/            编译完成后的打包目录<br/>
upper_app/          PC客户端应用实现<br/>
&emsp;&emsp;manage_tool/         用于访问嵌入式设备的桌面客户端(暂定QT)<br/>
&emsp;&emsp;loger_tool/     用于支持logger打印的网络调试工具(C#)<br/>

## 设计文档

参考文档(见document目录下说明), 初步设计包含:<br/>

嵌入式Linux端设计<br/>

1. 嵌入式驱动设备的访问，包含字符型设备，I2C, SPI，串口等访问，包含设备树支持<br/>
2. 支持配置管理，允许通过文件管理设备启动状态(配置文件使用JSON格式)<br/>
3. 基于linux系统API的线程创建，管理，线程间通讯支持<br/>
4. 本地端的通讯处理(Serial, Socket)等，基于自定义协议进行通讯(特殊指令的安全机制)<br/>
5. 内部状态读取更新显示，远端和本地的设置修改<br/>
6. 支持本地和网络的logger打印接口(基于asio设计)，调试等级显示可调<br/>
7. 访问外部设备的模块接口(基于CAN或者串口的轮询控制接口)<br/>

PC应用端设计<br/>

1. 访问Linux端接口，获取内部数据<br/>
2. 操作Linux端设备外设<br/>
3. 控制访问linux管理的远端信息<br/>
4. 其它功能组件<br/>

版本更新说明见demo/version_releas.md<br/>

## 硬件适配和兼容性

整个项目由上位机(windows平台和web平台), 主控设备(嵌入式linux平台)和其它设备平台(STM32单片机)组成.<br/>
1.主控设备基于正点原子阿尔法开发板实现，使用imx6ull内核.<br/>
2.windows平台主要提供对于开发板的远程管理，基于QT设计，用于本地的软件访问和管理.<br/>
3.web平台基于vue开发，主要用于本地的web访问和管理.<br/>
4.其它设备平台基于STM32单片机设计.<br/>

## 编译环境

嵌入式软件交叉编译工具<br/>
&emsp;&emsp;内核模块使用编译工具 - arm-linux-gnueabihf-gcc<br/>
&emsp;&emsp;manage，gui编译工具 - arm-linux-gnueabihf-g++<br/>
&emsp;&emsp;server使用node作为运行环境<br/>
&emsp;&emsp;网页使用vue方案<br/>
上位机编译工具<br/>
&emsp;&emsp;QT(the newest stable version)<br/>
logger显示工具<br/>
&emsp;&emsp;visual studio(the newest stable version)<br/>