# 构建Linux环境

对于初步接触嵌入式Linux平台开发的使用者，建议还是使用开发板或者芯片厂商提供的编译脚本和环境。如果进一步深入，希望了解这个环境时如何构建的，这篇文章则从安装Ubuntu,到完成编译系统构建，以及执行具体项目的全过程，也会包含遇到的问题和解决办法，当然每个版本遇到的问题可能都不一样，这就需要根据自己的经验去了解。

## 1.安装Linu系统

### 第一步:选择安装环境和系统版本，并安装

安装环境可以选择虚拟机模式和直接系统安装，不过因为我习惯windows下开发，所以使用虚拟机安装，直接系统安装可用UltraISO制作系统盘安装，不过这里平台选择如下。
1.VMvare 15.5.1
2.Ubuntu16.04 LTS
选择>新建虚拟机>典型, 选择安装文件*.iso，直接下一步安装，等待完成即可。

### 第二步：修改镜像源，完善构建环境

Ubuntu默认使用的镜像源再国外，直接下载访问比较慢，我一般更新为国内源，我所使用过的不错的有如下.
清华镜像源: https://mirrors.tuna.tsinghua.edu.cn/help/ubuntu/
按照页面说明选择对应版本，替换/etc/apt/sources.list下文件，然后执行
```bash
sudo apt-get update
sudo apt-get upgrade
```
如果出现Ign0:xxxxxxx的访问错误，这是表示访问失败，将文本的所有https替换成http即可.  
构建完成后，需要按照一些编译支持的脚本或者库，下面提供了一些我在编译驱动，内核还有编译一些第三方时遇到后增加的软件，可酌情安装.
```bash
sudo apt-get install bc lzop libffi-dev libssl-dev lzop git
sudo apt-get install libncurses5-dev u-boot-tools openssh-server 
sudo apt-get install dos2unix gzip libtool flex
sudo apt-get install build-essential manpages-dev make bison
sudo apt-get install  software-properties-common binutils gcc-multilib
```
如此，基本完成了Linux平台基础环境的构建.

## 2.搭建嵌入式Linux的编译平台
因为个人习惯问题，我喜欢创建目录如下.  
/home/[自定义]/download/   存放下载的软件  
/home/[自定义]/install/    存放编译后make install指定目录或软件的安装目录  
/home/[自定义]/aplication/ 存放个人的项目，例如本项目就放在这个目录下的git中  
对于嵌入式linux，第一步就是下载交叉编译工具，具体如下.  
编译工具:arm-linux-gnueabihf-gcc/arm-linux-gnueabihf-g++  
下载目录: