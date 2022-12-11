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
sudo apt-get install vim
sudo apt-get install bc lzop libffi-dev libssl-dev lzop git
sudo apt-get install libncurses5-dev u-boot-tools openssh-server 
sudo apt-get install dos2unix gzip libtool flex
sudo apt-get install build-essential manpages-dev make bison
sudo apt-get install  software-properties-common binutils gcc-multilib
```

如此，基本完成了Linux平台基础环境的构建.
如果gcc升级，也需要将linux平台的共享库libstdc++.so.6升级，具体路径如下.
将[gcc安装目录]/usr/lib64/libstdc++.so.6.0.x先复制到/usr/lib/x86_64-linux-gnu/中, 执行如下命令安装.

```bash
cd /usr/lib/x86_64-linux-gnu/
ln -sf libstdc++.so.6.0.28 libstdc++.so.6
```

## 2.搭建嵌入式Linux的编译平台

因为个人习惯问题，我喜欢创建目录如下.

* /home/[自定义]/download/   存放下载的软件
* /home/[自定义]/install/    存放编译后make install指定目录或软件的安装目录
* /home/[自定义]/aplication/ 存放个人的项目，例如本项目就放在这个目录下的git中
  对于嵌入式linux，第一步就是下载交叉编译工具，具体如下.
  编译工具:arm-linux-gnueabihf-gcc/arm-linux-gnueabihf-g++
  下载目录:https://releases.linaro.org/components/toolchain/binaries/7.5-2019.12/arm-linux-gnueabihf/
  下载linux平台下软件gcc-linaro-7.5.0-2019.12-i686_arm-linux-gnueabihf.tar.xz，解压到上面的install目录下，对于普通用户，可在/etc/profile中通过export命令添加, 需要重启后生效，或者执行source /etc/profile在当前环境下生效.

```bash
export PATH=$PATH:/home/[自定义]/install/[gcc]/bin
```

对于ROOT用户，可直接修改/etc/environment,添加到里面指定的PATH路径即可.如果在环境中输入arm-linux-，按tab能扩展出支持arm的编译工具，此时即表示添加成功.

## 3.常用工具安装和编译(可跳过)

本小节主要列出常用的工具指导，如远程访问的ssh，用于程序开发的vscode，当然也提供用于支持嵌入式Linux平台的工具编译如sqlite，node, mtd-utils,不过这些都是编译和积累的技术，可以先不需要了解，用到当作查找工具即可.

### 安装和编译远程访问工具openssh
执行如下指令安装
```bash
sudo apt-get install openssh-server
sudo service ssh restart
```
可通过
```bash
ssh [用户名]@[ipAddress]
```
如ssh freedom@192.168.0.99登录，不过默认不支持密码登录，需要在服务器
/etc/ssh/sshd_config中，打开密码登录选项
**PasswordAuthentication yes**
此外，还要指定连接的加密算法，我一般使用的如下所示:
```bash
Ciphers aes128-cbc
MACs  hmac-md5
KexAlgorithms diffie-hellman-group1-sha1
```
分别放在服务端的/etc/ssh/sshd_config和客户端的/etc/ssh/ssh_config中末尾即可，当然对于windows系统，添加在C:\ProgramData\ssh\ssh_config中.  
此外也可以通过scp命令传文件到系统中,这样可以提高效率，执行命令如下  
```bash
scp -r [file] [用户名]@[ipAddress]:[目录]
```
例如 scp -r test.txt freedom@192.168.0.99:/tmp/, 另外目录需要在当前权限下可写才可以.  
上诉主要讲述了如何在linux平台使用openssh, 不过对于嵌入式linux平台，就需要自己编译openssh，这里给出说明。