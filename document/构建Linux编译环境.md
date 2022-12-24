# 构建Linux环境

对于初步接触嵌入式Linux平台开发的使用者，建议还是使用开发板或者芯片厂商提供的编译脚本和环境。如果进一步深入，希望了解这个环境时如何构建的，这篇文章则从安装Ubuntu,到完成编译系统构建，以及执行具体项目的全过程，也会包含遇到的问题和解决办法，当然每个版本遇到的问题可能都不一样，这就需要根据自己的经验去了解.

## 1.安装Linu系统

### (1)选择安装环境和系统版本，并安装

安装环境可以选择虚拟机模式和直接系统安装，不过因为我习惯windows下开发，所以使用虚拟机安装，直接系统安装可用UltraISO制作系统盘安装，不过这里平台选择如下.  
1.VMvare 15.5.1  
2.Ubuntu16.04 LTS  
选择>新建虚拟机>典型, 选择安装文件*.iso，直接下一步安装，等待完成即可.  

### (2)修改镜像源，完善构建环境

Ubuntu默认使用的镜像源再国外，直接下载访问比较慢，我一般更新为国内源，我所使用过的不错的有如下.  
清华镜像源: https://mirrors.tuna.tsinghua.edu.cn/help/ubuntu/  
按照页面说明选择对应版本，替换/etc/apt/sources.list下文件，然后执行.  

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
将[gcc安装目录]/usr/lib64/libstdc++.so.6.0.x先复制到/usr/lib/  x86_64-linux-gnu/中, 执行如下命令安装.

```bash
cd /usr/lib/x86_64-linux-gnu/
ln -sf libstdc++.so.6.0.28 libstdc++.so.6
```

## 2.搭建嵌入式Linux的编译平台

### (1)创建项目目录

因为个人习惯问题，我喜欢创建目录如下.

* /home/[自定义]/download/   存放下载的软件
* /home/[自定义]/install/    存放编译后make install指定目录或软件的安装目录
* /home/[自定义]/aplication/ 存放个人的项目，例如本项目就放在这个目录下的git中

### (2)构建交叉编译环境

对于嵌入式linux，第一步就是下载交叉编译工具，具体如下.
编译工具:arm-linux-gnueabihf-gcc/arm-linux-gnueabihf-g++
下载目录:

```bash
https://releases.linaro.org/components/toolchain/binaries/7.5-2019.12/arm-linux-gnueabihf/ 
```

下载linux平台下软件

```bash
gcc-linaro-7.5.0-2019.12-i686_arm-linux-gnueabihf.tar.xz
```

解压到上面的install目录下.
对于普通用户，可在/etc/profile中通过export命令添加, 需要重启后生效或者执行source /etc/profile在当前环境下生效.

```bash
export PATH=$PATH:/home/[自定义]/install/[gcc]/bin
```

对于ROOT用户，可直接修改/etc/environment,添加到里面指定的PATH路径即可.
如果在环境中输入arm-linux-，按tab能扩展出支持arm的编译工具，此时即表示添加成功.

## 3.嵌入式Linux支持环境编译

### (1)uboot的编译

说起来, uboot的编译一般流程为下载官方更新的分支，裁剪修改设备树，驱动配置符合使用硬件板的需求，然后执行脚本编译，不过这部分在后续文档中整理，这里演示用开发板提供的uboot直接编译.
将uboot复制到linux平台，执行如下命令

```bash
tar -xvf uboot-imx-2016.03-2.1.0-g9bd38ef-v1.0.tar.bz2
make distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- mx6ull_14x14_evk_emmc_defconfig
make V=1 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j12
```

如果编译中显示缺少文件，基本上是上面提到的部分库未安装，找到安装即可.
编译目录下的**uboot.bin**即用到的boot文件.

### (2)linux内核编译

linux的编译也类似, 这里也直接用开发板提供的内核.

```bash
tar -xvf linux-imx-4.1.15-2.1.0-g49efdaa-v1.0.tar.bz2
vim arch/arm/boot/dts/Makefile
```

在CONFIG_SOC_IMX6ULL添加设备树，如我需要的是imx6ull-14x14-emmc-4.3-800x480-c.dtb, 另外因为开发内核需求，设备树也要更新,将"kernal_mod/dts/imx6ull-14x14-evk.dts"复制到"arch/arm/boot/dts/"下覆盖.

```bash
make distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- imx_v7_defconfig
make V=1 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- all -j16
```

则在生成目录下的
arch/arm/boot/zImage
arch/arm/boot/dts/imx6ull-14x14-emmc-4.3-800x480-c.dtb
这两个文件即是编译需要用到的文件.

编译错误分析:
[1].编译中出现重复变量yylloc
scripts/dtc/dtc-parser.tab.o:(.bss+0x50): multiple definition of `yylloc'
scripts/dtc/dtc-lexer.lex.o:(.bss+0x0): first defined here
将scripts/dtc/dtc-lexer.lex.c_shipped中的yylloc变量声明如下:

```c
extern YYLTYPE yylloc;
```

重新执行上诉脚本.

## 4.常用工具安装和编译

本小节主要列出常用的工具指导，如远程访问的ssh，用于程序开发的vscode，当然也提供用于支持嵌入式Linux平台的工具编译如sqlite，node, mtd-utils,不过这些都是编译和积累的技术，可以先不需要了解，用到当作查找工具即可.

### (1)安装和编译远程访问工具openssh

#### ubuntu安装ssh服务器并远端访问

执行如下指令安装

```bash
sudo apt-get install openssh-server
sudo service ssh restart
```

可通过

```bash
ssh [用户名]@[ipAddress]
```

如ssh freedom@192.168.0.99登录，不过默认不支持密码登录，需要在服务器/etc/ssh/sshd_config中，打开密码登录选：**PasswordAuthentication yes**
此外，还要指定连接的加密算法，我一般使用的如下所示:

```bash
Ciphers aes128-cbc
MACs  hmac-md5
KexAlgorithms diffie-hellman-group1-sha1
```

分别放在服务端的/etc/ssh/sshd_config和客户端的/etc/ssh/ssh_config中即可.
当然对于windows系统，添加在C:\ProgramData\ssh\ssh_config中.
此外也可以通过scp命令传文件到系统中，这样可以提高效率，执行命令如下

```bash
scp -r [file] [用户名]@[ipAddress]:[目录]
```

例如 scp -r test.txt freedom@192.168.0.99:/tmp/, 另外目录需要在当前权限下可写才可以.
如果远端的ssh服务器发生变化与本地保存不一致，可通过

```bash
ssh-keygen -f "/root/.ssh/known_hosts" -R [ipaddress]  
```

清除本地保存的密钥.

上诉主要讲述了如何在linux平台使用openssh, 不过对于嵌入式linux平台，就需要自己编译openssh，这里给出说明.

#### 编译嵌入式环境下的ssh服务

[1].下载zlib, openssl, openssh, 将所有文件下载并解压到/home/[自定义]/download目录下，这里目录选择center.

```bash
wget http://www.zlib.net/fossils/zlib-1.2.13.tar.gz
wget https://www.openssl.org/source/openssl-3.0.7.tar.gz
wget https://mirrors.tuna.tsinghua.edu.cn/OpenBSD/OpenSSH/portable/openssh-9.1p1.tar.gz
tar -xvf zlib-1.2.13.tar.gz
tar -xvf openssl-3.0.7.tar.gz
tar -xvf openssh-9.1p1.tar.gz
```

[2].编译zlib库.

```bash
cd zlib-1.2.13/
export CHOST=arm-linux-gnueabihf
./configure --prefix=/home/center/download/openssh-9.1p1/zlib
make & make install
```

[3].编译openssl.

```bash
cd openssl-3.0.7/
./config --cross-compile-prefix=arm-linux-gnueabihf- no-asm linux-aarch64 --prefix=/home/center/download/openssh-9.1p1/openssl
make & make install
```

[4].编译openssh.

```bash
./configure --host=arm-linux-gnueabihf --with-libs --with-zlib=/home/center/download/openssh-9.1p1/zlib --with-ssl-dir=/home/center/download/openssh-9.1p1/openssl --disable-etc-default-login 
```

编译完成后，目录下的scp sftp ssh sshd ssh-add ssh-agent ssh-keygen ssh-keyscan文件，即是编译后的执行文件.

[5].上传文件到嵌入式linux平台，并创建运行环境
创建以下目录(存在则不需要创建)

```bash
mkdir /usr/local/bin
mkdir /usr/local/etc
mkdir /usr/libexec
mkdir /var/run
mkdir /var/empty
```

将scp, sftp, ssh, sshd, ssh-agent, ssh-keygen, ssh-keyscan拷贝到/usr/local/bin目录.
将sftp-server, ssh-keysign拷贝到/usr/libexec目录.
将moduli, ssh_config, sshd_config拷贝到/usr/local/etc目录.
在嵌入式平台生成ssh对应密钥

```bash
cd /usr/bin/etc
/usr/local/bin/ssh-keygen -t rsa -f ssh_host_rsa_key -N ""
/usr/local/bin/ssh-keygen -t dsa -f ssh_host_dsa_key -N ""
/usr/local/bin/ssh-keygen -t ecdsa -f ssh_host_ecdsa_key -N ""
/usr/local/bin/ssh-keygen -t dsa -f ssh_host_ed25519_key -N ""
```

生成完成后，在/etc/passwd中添加sshd用户支持

```bash
sshd:x:115:65534::/var/run/sshd:/usr/sbin/nologin
```

当然也要和上面桌面端一样，在/usr/local/etc/sshd_config添加支持的加密算法，然后执行.

```bash
/usr/local/bin/sshd
```

出错处理
1.显示缺少libz.so.1,需要将上面zlib安装目录下的/home/center/download/openssh-9.1p1/zlib/lib/中的库复制到嵌入式文件系统的/usr/lib下
2.出现加密算法不匹配
需要在桌面端/etc/ssh/ssh_config和嵌入式linux平台/usr/local/etc/sshd_config中添加如下字段

```bash
Ciphers aes128-cbc
MACs  hmac-md5
KexAlgorithms diffie-hellman-group1-sha1
```

### (2)编译和安装最新的g++

首先安装最新的gcc版本，可以在https://mirrors.tuna.tsinghua.edu.cn/gnu/gcc/ 目录下找寻下载，如现在采用最新的为12.2.0, 进入下载目录，使用wget即可下载.

```bash
wget https://mirrors.tuna.tsinghua.edu.cn/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz
tar -xvf gcc-12.2.0.tar.gz
cd gcc-12.2.0
```

理论上要执行./contrib/download_prerequisites直接安装编译环境，不过国内节点速度会很慢，建议先通过vim contrib/download_prerequisites，在起始处有需要的版本,如12.2.0需要如下.
gmp='gmp-6.2.1.tar.bz2'
mpfr='mpfr-4.1.0.tar.bz2'
mpc='mpc-1.2.1.tar.gz'
isl='isl-0.24.tar.bz2'
然后就可以使用命令在目录下下载.

```bash
wget https://mirrors.tuna.tsinghua.edu.cn/gnu/gmp/gmp-6.2.1.tar.bz2
wget https://mirrors.tuna.tsinghua.edu.cn/gnu/mpfr/mpfr-4.1.0.tar.bz2
wget https://mirrors.tuna.tsinghua.edu.cn/gnu/mpc/mpc-1.2.1.tar.gz
wget https://gcc.gnu.org/pub/gcc/infrastructure/isl-0.24.tar.bz2
./contrib/download_prerequisites
./configure --enable-checking=release --enable-languages=c,c++ --disable-multilib --prefix=/home/center/install/gcc12/
make -j4
make install
```

如果编译出现了'stage1-bubble' 清除make distclean, 重新执行make即可
编译完成后，需要将编译完成的gcc和g++文件链接到系统目录/usr/bin/下，执行如下

```bash
GCC_INSTALL_PATH=/home/center/install/gcc12
echo $GCC_INSTALL_PATH
sudo ln -sf $GCC_INSTALL_PATH/bin/g++ /usr/bin/g++
sudo ln -sf $GCC_INSTALL_PATH/bin/gcc /usr/bin/gcc
sudo ln -sf $GCC_INSTALL_PATH/bin/gcc-ranlib /usr/bin/gcc-ranlib
sudo ln -sf $GCC_INSTALL_PATH/bin//gcc-ar /usr/bin/gcc-ar
sudo ln -sf $GCC_INSTALL_PATH/bin/gcc-nm /usr/bin/gcc-nm
sudo ln -sf $GCC_INSTALL_PATH/bin/gcov /usr/bin/gcov
sudo ln -sf $GCC_INSTALL_PATH/lib64/libstdc++.so.6.0.30 /usr/lib/x86_64-linux-gnu/libstdc++.so.6
```

可通过gcc -v或者g++ -v查询版本是否替换成功.

### (3)安装vscode作为linux上的开发工具

vscode的下载地址为https://code.visualstudio.com/#alt-downloads, 选择Ubuntu版本，先在官网上点击下载，然后复制链接，将前面的路径替换成国内源vscode.cdn.azure.cn, 如果不清楚可以使用如下命令在linux上下载

```bash
wget https://vscode.cdn.azure.cn/stable/1ad8d514439d5077d2b0b7ee64d2ce82a9308e5a/code_1.74.1-1671015296_amd64.deb
sudo dpkg -i code_1.74.1-1671015296_amd64.deb
```

安装完成后，输入code即可在linux上打开code，不过我喜欢添加vscode作为打开的link.

```bash
sudo ln -sf code /usr/bin/vscode
```

之后用vscode即可使用软件.
