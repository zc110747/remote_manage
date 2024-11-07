# 文件系统构建方法

本节目录如下.

- [文件系统综述](#overview)
- [基于busybox构建文件系统](#busybox_create)
- [基于buildroot构建文件系统](#buildroot_create)
- [基于debian构建文件系统](#debian_create)
- [基于Ubuntu Core构建文件系统](#ubuntu_create)
- [支持QT的Linux文件系统构建](#linux_qt_support)

## overview

Linux文件系统是用于管理和存储的一系列的文件的集合，按照功能上包含如下内容。

- 系统和文件管理有关的程序(如ls，cat)
- 存储设备信息的文件，以及系统运行相关的设备文件
- 支持软件和脚本运行的环境和库

对于文件系统的讲解，内容都是从busybox，buildroot上如何构建文件系统(复杂的可能是yocto或debian)。可回想起来，有问题一直在困扰我，为什么使用这些工具能够构建可用的系统，它们实现的原理是什么？在理解这个问题以前，无论是构建文件系统，还是交叉编译软件，我都只能去查找教程或者搜索别人是否遇到相同的问题，然后按照对应的方法尝试，直到系统了总结这个问题，才开始有了主动思考，分析并解决问题的能力。
从本质上讲，文件系统是被内核访问的，由文件目录和文件构成的树结构的集合体。它设计上是为内核提供可以访问的外部接口，而Linux中"一切皆文件的思想"，又将这些接口以文件的形式展示。简单的说，rootfs也可以说由文件和目录构成类似集合的树结构文件系统.

1. 创建包含系统工作需要的目录。
2. 必须的配置文件和服务，以及权限管理机制。
3. 运行的必要环境变量和程序脚本。
4. 支持上述程序或脚本运行的库。
5. 用户安装的扩展系统功能的程序。

理解了上述这些，就可以进一步去掌握文件系统各目录的功能，具体如下。

- bin/      **必须**，存放系统执行的必备文件，用于系统快速访问，如bash，cp，mkdir等。
- dev/      **必须**，包含了与硬件设备相关的特殊文件。这些文件是Linux内核提供的接口，用于与硬件设备、驱动程序以及其他内核功能进行通信。
- etc/      **必须**，系统配置文件目录，用于管理系统启动配置项和服务项，如启动脚本，系统服务，网络配置
  - etc/fstab 定义文件系统的静态信息的文件。它包含了系统中硬盘分区、挂载点、文件系统类型、挂载选项等信息，用于指导系统如何挂载文件系统.
  - etc/hosts 包含了本机或其他主机的IP地址和对应的主机名，以及主机名的别名.
  - etc/passwd 包含用户信息的文件，如用户名，权限以及启动后执行的脚本命令.
  - etc/shadow etc/passwd的影子口令文件，用户密码信息存放在此处，root权限可读
  - etc/init.d/rc* 启动、或改变运行集执行的脚本文件
- lib/      **必须**，用于存放系统所需的共享库文件的目录。这些库文件包含了程序运行时所需要的函数和数据，以便程序能够正确地执行。
- media/    可选，用于挂载可移动存储设备的目录，提供方便，统一的挂载点。
- mnt/      可选，mnt目录是一个挂载点，用于临时挂载文件系统。它通常用于挂载外部存储设备或网络文件系统，以便用户可以轻松地访问和操作这些文件系统
- opt/      可选，用于存放独立的第三方软件包，这些软件包通常不是操作系统的核心组件，也不会与系统自带的软件产生冲突。
- proc/     **必须**，以文件系统的方式为访问系统内核数据的操作提供接口，使得用户和应用程序可以通过/proc来查看有关系统硬件、当前正在运行进程的信息，甚至可以更改其中某些文件来改变内核的运行状态。
- root/     **必须**，root用户的配置目录，存储root用户的个人文件和配置信息。
- run/      可选，是一个临时文件目录，用于存储系统启动以来的信息，重启后，系统会自动删除相关信息。
- sbin/     **必须**，主要放置了一些系统管理的必备程序，主要放置了一些系统管理的必备程序。
- sys/      **必须**，虚拟文件系统目录，也称为sysfs，它提供了对内核对象和设备树的访问。
- tmp/      **必须**，临时文件存储目录，其作用是为应用程序和用户提供公共的临时文件存储空间。
- usr/      **必须**，用户安装的软件和文件，以及系统默认的软件和文件，包含了系统的核心文件和应用程序。
- var/      可选，包含了在系统运行过程中会改变的文件，这些文件通常包括日志文件、临时文件和缓存文件。

对于文件系统的构建，就是创建上述目录，并在对应目录下写入相应的文件，库或者程序，而上面提到的构建过程都是实现这个目录，并添加必要文件和程序的过程，理解了文件系统的本质，那么构建系统和交叉编译的问题就有另外的思路去理解。

构建系统的步骤可以分为如下:

1. 创建系统启动的目录(包含上面的**必须**目录)。
2. 复制写入系统启动必须的动态库，放置在lib目录下。(busybox需要自己从编译工具导入，其它都自动导入)
3. 创建系统执行的必要配置和服务，这个在etc目录下。
4. 安装系统启动的必要命令工具，如bash(sh), cp, ls等

至此，一个基于命令行的，可以远程访问的最小Linux文件系统构建完成，当然这样构建的系统只支持基本的Linux功能，如文件处理，网络功能，如果想扩展功能，就需要去找源码单独编译，然后将动态库和程序安装到系统中，前面的交叉编译就是讲述了这个过程，基于busybox的系统构建就是上述过程，busybox编译完成仅提供必要的命令工具，目录，动态库和配置文件都需要手动安装，当然这种方式会十分麻烦，而且容易出错。

基于busybox手动安装过多的问题，就有人发起项目，通过脚本工具链，以busybox为基础，在集合Linux平台常用工具，配合界面化的管理脚本，实现一次命令直接生成最终的文件系统，这就是buildroot项目，这就简化了系统创建和交叉编译安装软件的流程。

## busybox_create

第一步，busybox的下载和编译

```shell
#下载busybox, 解压并进入
wget https://busybox.net/downloads/busybox-1.36.1.tar.bz2
tar -xvf busybox-1.36.1.tar.bz2
cd busybox-1.36.1/

#修改配置文件，修改编译工具
make menuconfig ARCH=arm

#修改交叉编译工具
menu>Settings>Build Options>Cross Compiler prefix，增加arm-none-linux-gnueabihf-

#关闭后保存，开启编译
make -j4 ARCH=arm
make install

#编译好的文件保存在_install目录下，默认为
bin/
linuxrc 
sbin/
usr/
```

如果编译结束后，出现如下编译结果。

```shell
Trying libraries: crypt m resolv rt
Library crypt is not needed, excluding it
Library m is needed, can't exclude it (yet)
Library resolv is needed, can't exclude it (yet)
Final link with: m resolv
```

这并不是报错，而是如果编译部分扩展的文件libbusybox.so等之前，需要提前剔除编译工具不支持的库或者链接，否则则会发生编译错误，无法继续，详细可以看script/trylink中脚本。至此，完成第一阶段工作，编译busybox系统。

第二步，就是创建目录来符合Linux对文件系统的需求，并添加系统启动的必要文件

```shell
#创建系统文件
mkdir dev etc lib proc root sys tmp run

#创建系统启动必须的文件
#busybox传统的SysV init系统, 因此启动中优先调用/etc/inittab文件。

#inittab启动信息
vim etc/inittab

#用户密码和启动执行应用
vim etc/passwd

#文件系统信息
vim etc/fstab

#系统启动时执行的脚本
vim etc/init.d/rcS
```

详细信息查看file/ch02/ch02-05/rootfs中对应路径下文件。

第三步，安装支持文件系统运行的动态库，前面提到，linux系统文件运行除了特殊编译全静态链接的，大部分都需要系统的C库支持，这部分直接复制编译工具下的lib目录即可，命令如下。

```shell
#复制c++ lib
sudo cp -ravf $GCC_LIB/lib/*so* lib/

#复制c lib到
sudo cp -ravf $GCC_LIB/libc/lib/*so* $NFS_PATH/lib/
```

至此，我们完成了构建最小文件系统的全部工作。如果后续需要扩展功能，就交叉编译相应的工具，将可执行文件和库放入对应的目录，就可以正常的被系统所访问，不过对于下载工具或者Linux系统，可能需要打包成tar文件或者img包，这里也提供打包成img文件的方法。

```shell
export root_name=arm-busybox
export root_size=1024

#创建打包文件*.img
dd if=/dev/zero of=${root_name}.img bs=1M count=${root_size}
mkfs.ext4 ${root_name}.img

#导入rootfs信息到img
#原理就是将img文件以硬盘的形式挂载到系统中，然后将上述编译好的文件系统复制到其中，再移除硬盘，此时
sudo mount -o loop ${root_name}.img arm/  
sudo cp -arv rootfs/ arm/
sudo umount arm
```

打包好的硬盘img文件通过qemu-img文件管理

```shell
#查看虚拟镜像的信息
qemu-img info *.img

#调整硬盘容量
qemu-img resize *.img [size]
```

可以看到，通过编译busybox，创建文件系统，安装系统服务再通过打包动作，我们可以获得最小文件系统的虚拟硬盘文件，可以看到步骤还是挺繁琐的，很容易出错，另外如果像扩展其它应用支持，还需要参考**ch01-04.linux_cross_compiler**进行交叉编译和移植，这个就十分复杂了，有需求自然有解决方法，有开发者将上述编译需要的环境，库和软件进行打包，构成了自动编译工具，可以做到开箱即用，这就是buildroot/yocto, 因为这两种类似，这里选择buildroot进行讲解。

## buildroot_create

buildroot就是以busybox为核心，支持编译工具，库，可执行文件和服务的，用于Linux文件系统构建的自动化脚本工具，当然对于buildroot新版本，也增加了对uboot和kernel的支持，不过这里不进行添加，uboot和kernel下载和编译时间太长，而且也更重要，再前两节进行单独讲解，这里主要说明对于文件系统的支持，先讲解下目录中跟编译和构建息息相关的功能。

- configs/ 包含默认的配置文件，当然我们定义的配置文件也可以放置再此目录中
- dl/ 编译中需要支持的库，在编译中会下载到此目录，在国内可能下载困难，可以自己下载放置到此目录中
- ouput/ 构建好的文件系统存储目录

下载buildroot安装包, 并解压。

```shell
#下载buildroot
wget https://buildroot.org/downloads/buildroot-2024.02.1.tar.gz
tar -xvf buildroot-2024.02.1.tar.gz
cd buildroot-2024.02.1/
```

buildroot也支持和uboot，linux一样使用默认配置文件，这里定义为configs/imx6ull_user_defconfig, 内容如下：

```shell
# architecture
BR2_arm=y
BR2_cortex_a7=y
BR2_ARM_FPU_NEON_VFPV4=y

#
# Toolchain
#
BR2_TOOLCHAIN_EXTERNAL=y
BR2_TOOLCHAIN_EXTERNAL_CUSTOM=y
BR2_TOOLCHAIN_EXTERNAL_PREINSTALLED=y
#BR2_TOOLCHAIN_EXTERNAL_PATH="/home/freedom/sdk/arm/support/compiler"
BR2_PACKAGE_PROVIDES_TOOLCHAIN_EXTERNAL="toolchain-external-custom"
BR2_TOOLCHAIN_EXTERNAL_PREFIX="arm-none-linux-gnueabihf"
BR2_TOOLCHAIN_EXTERNAL_CUSTOM_PREFIX="arm-none-linux-gnueabihf"
BR2_TOOLCHAIN_EXTERNAL_GCC_11=y
BR2_TOOLCHAIN_EXTERNAL_HEADERS_4_20=y
BR2_TOOLCHAIN_EXTERNAL_CUSTOM_GLIBC=y
BR2_TOOLCHAIN_EXTERNAL_CXX=y
BR2_TOOLCHAIN_EXTERNAL_FORTRAN=y
BR2_TOOLCHAIN_EXTERNAL_OPENMP=y
BR2_TOOLCHAIN_EXTERNAL_INET_RPC=n
BR2_TARGET_ROOTFS_EXT2_SIZE="1G"
BR2_KERNEL_MIRROR="https://mirrors.kernel.org/pub"


# system
BR2_TARGET_GENERIC_GETTY_PORT="ttymxc0"

# filesystem / image
BR2_TARGET_ROOTFS_EXT2=y
BR2_TARGET_ROOTFS_EXT2_4=y

# System configuration
BR2_TARGET_GENERIC_HOSTNAME="freedom"
BR2_TARGET_GENERIC_ISSUE="Welcome to freedom"
BR2_TARGET_GENERIC_ROOT_PASSWD="root"
```

将上述文件放置在buildroot/configs/目录下，执行编译脚本。

```shell

#生成.config文件
make menuconfig imx6ull_user_defconfig

#开始编译
make -j6
```

这一步要等待一段时间，因为会自动下载一些软件，如果位于国外，会比较慢，可以自己看命令下载放置在dl目录。等待编译完成后，output/images目录下即为编译好的虚拟硬盘文件，直接使用即可，可以直接用mount命令挂载查看。

```shell

#创建目录，并挂载文件系统
mkdir output/images/rootfs/
cd output/images
sudo mount -o loop rootfs.ext4 rootfs/
```

此时，就可以在rootfs中查看构建好的文件系统, 如果需要增加软件或者库支持，在buildroot配置项中增加相应工具即可。

## debian_create

基于buildroot我们已经可以快速轻松的构建可用的文件系统，不过这并不是没有缺陷，这里有两点：

1. 增加新的软件和库需要重新构建文件系统
2. 需要的软件buildroot不支持，仍然需要交叉编译

熟悉使用Linux系统的了解，提供dpkg和apt的两种安装软件的方式，可以本地安装和从服务器下载系统支持的文件，对于嵌入式来说，当然也支持，我们可以基于国内的镜像站构建可用的debian系统，具体脚本如下。

```shell
OPT_OS_VER=bookworm

sudo apt-get install debootstrap debian-archive-keyring qemu-user-static -y

run_as_client() {
    $@ > /dev/null 2>&1
}

#挂载系统目录
mount_chroot()
{
    sudo mount -t proc chproc "${NFS_PATH}"/proc
    sudo mount -t sysfs chsys "${NFS_PATH}"/sys
    sudo mount -t devtmpfs chdev "${NFS_PATH}"/dev || sudo mount --bind /dev "${NFS_PATH}"/dev
    sudo mount -t devpts chpts "${NFS_PATH}"/dev/pts
}

#移除系统目录
umount_chroot()
{
    while grep -Eq "${NFS_PATH}.*(dev|proc|sys)" /proc/mounts
    do
        sudo umount -l --recursive "${NFS_PATH}"/dev >/dev/null 2>&1
        sudo umount -l "${NFS_PATH}"/proc >/dev/null 2>&1
        sudo umount -l "${NFS_PATH}"/sys >/dev/null 2>&1
        sleep 5
    done
}

#使用debootstap完成从镜像源下载文件
if [ ! -d ${NFS_PATH}/bin/ ]; then
    sudo debootstrap --foreign --verbose  --arch=${CHIP_ARCH} ${OPT_OS_VER} ${NFS_PATH}  http://mirrors.tuna.tsinghua.edu.cn/debian/
fi

#安装qemu-*-static
sudo chmod -Rv 777 ${NFS_PATH}/usr/
cp /usr/bin/qemu-${qemu_arch}-static ${NFS_PATH}/usr/bin/
chmod +x ${NFS_PATH}/usr/bin/qemu-${qemu_arch}-static

#基于debootstrap完成后续安装
cd ${NFS_PATH}
mount_chroot
LC_ALL=C LANGUAGE=C LANG=C sudo chroot ${NFS_PATH} /debootstrap/debootstrap --second-stage --verbose
LC_ALL=C LANGUAGE=C LANG=C sudo chroot ${NFS_PATH} apt-get install vim libatomic1 -y
umount_chroot
```

通过上述步骤就可以构建完成基于debian的系统，后续可通过apt和debian安装后续的文件，功能上更丰富，不过占有资源也更多，需要根据自身硬件平台选择使用。

## ubuntu_create

基于官方Ubuntu构建支持i.MX6Ull的文件系统。

Ubuntu的地址: <http://cdimage.ubuntu.com/ubuntu-base/releases/22.04/release/>

```shell
#安装qemu虚拟机
sudo apt-get install qemu-user-static

##需要系统版本和安装版本一致，否则可能无法执行
#1.下载Ubuntu-Base
对于arm下载:
wget http://cdimage.ubuntu.com/ubuntu-base/releases/22.04/release/ubuntu-base-22.04.2-base-armhf.tar.gz

对于aarch64下载:
wget https://cdimage.ubuntu.com/ubuntu-base/releases/22.04/release/ubuntu-base-22.04.2-base-arm64.tar.gz

#2.提取文件系统
tar -xvf ubuntu-base-22.04.2-base-armhf.tar.gz

#3.修改镜像源和dns服务器，地址
vim etc/apt/sources.list
更新内容见网址： https://mirrors.tuna.tsinghua.edu.cn/help/ubuntu-ports/

#修改dns服务器(必须删除后添加，原为链接，修改无效)
rm -rf etc/resolv.conf

#写入命令
cat << EOF > etc/resolv.conf
nameserver 223.5.5.5
nameserver 8.8.8.8
nameserver 8.8.4.4
EOF

#通过chroot进入配置模式, 进行源更新
workdir=$(pwd)
sudo mount -t proc chproc "${workdir}"/proc
sudo mount -t sysfs chsys "${workdir}"/sys
sudo mount -t devtmpfs chdev "${workdir}"/dev || sudo mount --bind /dev "${workdir}"/dev
sudo mount -t devpts chpts "${workdir}"/dev/pts
LC_ALL=C LANGUAGE=C LANG=C sudo chroot "${workdir}"

#######################################################
#此步骤后进入沙盒模式(root权限)，执行环境配置
#######################################################
#进行系统更新支持
chmod 777 /tmp
apt-get update
apt-get install ca-certificates --reinstall -y
apt-get update
apt-get install libterm-readkey-perl -y
apt-get upgrade

#安装ubuntu的必要软件
unminimize
apt-get install sudo vim systemd kmod -y
ln -sf /usr/bin/systemd /sbin/init
apt-get install dialog apt-utils
apt-get install net-tools

#增加用户，并添加到sudo中
adduser freedom
...

##freedom用户支持sudo命令
visudo
freedom ALL=(ALL:ALL) ALL

##更新密码
passwd root
......
```

接下来就是管理Ubuntu启动时的服务，Ubuntu服务默认使用systemctrl命令管理，包含如下。

```shell
#复制串口ttymxc0启动服务，所有步骤完成
cd /etc/systemd/system/getty.target.wants

#[ TIME ] Timed out waiting for device dev-ttymxc0.device. 解决方案
cp -d getty@tty1.service getty@ttymxc0.service

#创建个人服务，用于启动网络
cat << EOF > /etc/systemd/system/startservice.service

[Unit]
Description=My Service
After=network.target

[Service]
ExecStart=/etc/init.d/start.sh
WorkingDirectory=/tmp/

[Install]
WantedBy=default.target
EOF

#创建启动脚本, 起始第一行必须指定执行环境
cat << EOF > /etc/init.d/start.sh
#!/bin/bash

echo "start network service"
ifconfig eth0 192.168.2.99 netmask 255.255.255.0
route add default gw 192.168.2.1
ifconfig eth0 up
EOF

#开启服务
systemctl enable startservice.service
```

接下来可以移除环境，打包即可。

```shell
#11.移除连接，进行打包
exit
sudo umount -l --recursive "${workdir}"/dev >/dev/null 2>&1
sudo umount -l "${workdir}"/proc >/dev/null 2>&1
sudo umount -l "${workdir}"/sys >/dev/null 2>&1

tar -vcjf rootfs.tar.bz2 *
```

此时打包的rootfs.tar.bz2就是基于Ubuntu Core构建的系统，后续可以执行其它升级功能。

## linux_qt_support

对于Qt环境的支持，理论上并不属于直接构建文件系统，而是文件系统中的一个程序的编译和移植。不过Qt因为编译调试复杂，因此将Qt系统编译也作为单独的系统构建部分说明。对于Qt环境的支持，包含如下所示。

1. [tslib触摸屏库和应用的编译安装](#tslib_cross_compiler)
2. [Qt库和应用的编译安装](#qt_cross_compiler)
3. [Qt桌面端开发编译环境支持](#qt_design_support)

### tslib_cross_compiler

```shell
#下载tslib地址
wget https://github.com/libts/tslib/releases/download/1.23/tslib-1.23.tar.bz2

tar -xvf tslib-1.23.tar.bz2
cd tslib-1.23/

./configure --host=arm-none-linux-gnueabihf ac_cv_func_malloc_0_nonnull=yes -prefix="${SUPPORT_ENV_INSTALL}"

make && make install
```

### qt_cross_compiler

```shell
#下载tslib地址
wget https://mirrors.tuna.tsinghua.edu.cn/qt/archive/qt/5.15/5.15.13/single/qt-everywhere-opensource-src-5.15.13.tar.xz

tar -xvf qt-everywhere-opensource-src-5.15.13.tar.xz
cd  qt-everywhere-src-5.15.13/qtbase/

#修改编译工具和flags
vim mkspecs/linux-arm-gnueabi-g++/qmake.conf

#####################################################################
#mkspecs/linux-arm-gnueabi-g++/qmake.conf
#
# qmake configuration for building with arm-linux-gnueabi-g++
#

MAKEFILE_GENERATOR      = UNIX
CONFIG                 += incremental
QMAKE_INCREMENTAL_STYLE = sublib

include(../common/linux.conf)
include(../common/gcc-base-unix.conf)
include(../common/g++-unix.conf)

QT_QPA_DEFAULT_PLATFORM = linuxfb
QMAKE_CFLAGS += -O2 -march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=hard
QMAKE_CXXFLAGS += -O2 -march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=hard

# modifications to g++.conf
QMAKE_CC                = arm-none-linux-gnueabihf-gcc
QMAKE_CXX               = arm-none-linux-gnueabihf-g++
QMAKE_LINK              = arm-none-linux-gnueabihf-g++
QMAKE_LINK_SHLIB        = arm-none-linux-gnueabihf-g++

# modifications to linux.conf
QMAKE_AR                = arm-none-linux-gnueabihf-ar cqs
QMAKE_OBJCOPY           = arm-none-linux-gnueabihf-objcopy
QMAKE_NM                = arm-none-linux-gnueabihf-nm -P
QMAKE_STRIP             = arm-none-linux-gnueabihf-strip
load(qt_config)
#########################################################################

#添加编译脚本
cd  ../
vim autogen.sh

#####################################################################
#autogen.sh
#
# 自动编译脚本
#
./configure -prefix ${SUPPORT_ENV_INSTALL} \
-opensource \
-confirm-license \
-release \
-strip \
-shared \
-xplatform linux-arm-gnueabi-g++ \
-optimized-qmake \
-c++std c++14 \
--rpath=no \
-pch \
-skip qt3d \
-skip qtactiveqt \
-skip qtandroidextras \
-skip qtcanvas3d \
-skip qtconnectivity \
-skip qtdatavis3d \
-skip qtdoc \
-skip qtgamepad \
-skip qtlocation \
-skip qtmacextras \
-skip qtnetworkauth \
-skip qtpurchasing \
-skip qtremoteobjects \
-skip qtscript \
-skip qtscxml \
-skip qtsensors \
-skip qtspeech \
-skip qtsvg \
-skip qttools \
-skip qttranslations \
-skip qtwayland \
-skip qtwebengine \
-skip qtwebview \
-skip qtwinextras \
-skip qtx11extras \
-skip qtxmlpatterns \
-make libs \
-make examples \
-nomake tools -nomake tests \
-gui \
-widgets \
-dbus-runtime \
--glib=no \
--iconv=no \
--pcre=qt \
--zlib=qt \
-no-openssl \
--freetype=qt \
--harfbuzz=qt \
-no-opengl \
-linuxfb \
--xcb=no \
-tslib \
--libpng=qt \
--libjpeg=qt \
--sqlite=qt \
-plugin-sql-sqlite \
-I"${SUPPORT_ENV_INSTALL}"/include \
-L"${SUPPORT_ENV_INSTALL}"/lib \
-recheck-all

#start build
gmake -j3

#start install
gmake install
##############################################################

#执行编译脚本
chmod 777 autogen.sh && ./autogen.sh
```

增加环境变量

```shell
export TSLIB_ROOT=""
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb0
export TSLIB_TSDEVICE=/dev/input/event1
export TSLIB_CONFFILE=$TSLIB_ROOT/etc/ts.conf
export TSLIB_PLUGINDIR=$TSLIB_ROOT/lib/ts
export TSLIB_CALIBFILE=/etc/pointercal
export LD_PRELOAD=$TSLIB_ROOT/lib/libts.so

export QT_ROOT=""
export QT_QPA_GENERIC_PLUGINS=tslib:/dev/input/event1
export QT_QPA_FONTDIR=/usr/share/fonts
export QT_QPA_PLATFORM_PLUGIN_PATH=$QT_ROOT/plugins
export QT_QPA_PLATFORM=linuxfb:tty=/dev/fb0
export QT_PLUGIN_PATH=$QT_ROOT/plugins
export LD_LIBRARY_PATH=$QT_ROOT/lib:$QT_ROOT/plugins/platforms
export QML2_IMPORT_PATH=$QT_ROOT/qml
export QT_QPA_FB_TSLIB=1

export XDG_RUNTIME_DIR="/tmp/runtime-root"
```

### qt_design_support

## next_chapter

[返回目录](./SUMMARY.md)

直接开始下一章节说明: [qemu虚拟机环境构建](./ch02-09.qemu.md)
