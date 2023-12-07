#/bin/sh
##############################################################################
#这个脚本用于构建支持项目环境应用的系统
#包含自动安装编译工具(GCC), uboot, kernel, rootfs到指定地址，安装完成后即可配合env
#中定义的系统指令进行编译环境的构建，整个系统的编译工具，uboot，kernel和rootfs都已经
#指定。
#GCC:gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf
#uboot:uboot-imx-lf_v2022.04
#kernel:linux-imx-lf-6.1.y
#rootfs:busybox-1.36.1
################################################################################

#备用下载文件目录
DOWNLOAD_PATH=/mnt/d/download
GCC_TOOLCHAIN=gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf
OLD_GCC_TOOLCHAIN=gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf
UBOOT_FILE=uboot-imx-lf_v2022.04
KERNEL_FILE=linux-imx-lf-6.1.y
ROOTFS_FILE=busybox-1.36.1

#全局的环境目录创建
GLOBAL_PROGRAM_PATH=/home/program
GLOBAL_PROGRAM_THIRDPARTS=${GLOBAL_PROGRAM_PATH}/thirdparts
GLOBAL_PROGRAM_INSTALL=${GLOBAL_PROGRAM_PATH}/install
GLOBAL_PROGRAM_SUPPORT=${GLOBAL_PROGRAM_PATH}/support
GLOBAL_PROGRAM_DOWNLOAD=${GLOBAL_PROGRAM_PATH}/download
GLOBAL_PROGRAM_BUILD=${GLOBAL_PROGRAM_PATH}/build
GLOBAL_PROGRAM_APPLICATION=${GLOBAL_PROGRAM_PATH}/application

CURRENT_PATH=$(pwd)

#定义全局color
readonly defText="$(tput sgr0)"
readonly redText="$(tput setaf 1)"
readonly greenText="$(tput setaf 2)"

#创建编译环境
if [ ! -d "$GLOBAL_PROGRAM_PATH" ]; then
    
    echo "start create the environment."
    sudo mkdir $GLOBAL_PROGRAM_PATH
    sudo chmod 777 $GLOBAL_PROGRAM_PATH

    #构建系统需要的环境信息
    mkdir -p ${GLOBAL_PROGRAM_DOWNLOAD}/tmp
    mkdir -p ${GLOBAL_PROGRAM_THIRDPARTS}
    mkdir -p ${GLOBAL_PROGRAM_INSTALL}/arm
    mkdir -p ${GLOBAL_PROGRAM_INSTALL}/i386
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/compiler
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/old_compiler
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/uboot
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/kernel
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/rootfs 
    mkdir -p ${GLOBAL_PROGRAM_PATH}/build/nfs_root
    mkdir -p ${GLOBAL_PROGRAM_PATH}/build/tftp_root

    sudo chmod -Rv 777 ${GLOBAL_PROGRAM_PATH}/
else
    echo "Directory ${greenText}${GLOBAL_PROGRAM_PATH}${defText} exist, Check Ok!"
fi

echo "now put file used into the environment."
echo "  ${greenText}${GLOBAL_PROGRAM_DOWNLOAD}/${defText}           :下载的各类软件，安装包或者编译的库源码"
echo "  ${greenText}${GLOBAL_PROGRAM_THIRDPARTS}/${defText}         :第三方库解压后存放目录，用于编译"
echo "  ${greenText}${GLOBAL_PROGRAM_INSTALL}/${defText}            :用于安装库的目录"
echo "  ${greenText}${GLOBAL_PROGRAM_SUPPORT}/compiler/${defText}   :交叉编译工具, 将arm-gcc解压后内部文件复制到此目录下"
echo "  ${greenText}${GLOBAL_PROGRAM_SUPPORT}/uboot/${defText}      :uboot目录, 用于编译uboot的源码"
echo "  ${greenText}${GLOBAL_PROGRAM_SUPPORT}/kernel/${defText}     :kernel目录, 用于编译内核的源码"
echo "  ${greenText}${GLOBAL_PROGRAM_SUPPORT}/rootfs/${defText}     :文件系统目录, 用于编译文件系统的源码(busybox), 如debain或android个人很难从源码编译,直接获取编译好的文件系统"
echo "  ${greenText}${GLOBAL_PROGRAM_SUPPORT}/build/${defText}      :编译后系统文件存储目录"
echo "未复制则后续编译时会显示找不到文件或者直接编译失败"

#按照系统编译支持的library库
if [ x$1 != x ] && [ $1 == "all" ]; then
    echo "install library or tools for the build environment." 
    sudo apt-get install vim lib32z1 cmake net-tools unzip -y
    sudo apt-get install bc lzop libffi-dev libssl-dev lzop git -y
    sudo apt-get install libncurses5-dev u-boot-tools openssh-server -y 
    sudo apt-get install dos2unix gzip libtool flex -y
    sudo apt-get install build-essential manpages-dev make bison -y
    sudo apt-get install software-properties-common binutils gcc-multilib -y
    sudo apt-get install libc6-dev-i386 net-tools -y
    sudo apt-get install lsb-core lib32stdc++6 -y
fi

#下载arm-none-linux-gnueabihf-交叉编译器到compiler目录
function support_new_compiler()
{
    LOCAL_GCC_TOOLCHAIN=${DOWNLOAD_PATH}/${GCC_TOOLCHAIN}.tar.xz
    GCC_SOURCE_ADDR=https://mirrors.tuna.tsinghua.edu.cn/armbian-releases/_toolchain/${GCC_TOOLCHAIN}.tar.xz

    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/compiler/bin ]; then
        cd $GLOBAL_PROGRAM_DOWNLOAD/tmp/

        if [ -f ${LOCAL_GCC_TOOLCHAIN} ]; then
            echo "copy gcc from local address:${LOCAL_GCC_TOOLCHAIN}"
            cp ${LOCAL_GCC_TOOLCHAIN} ./
        else
            echo "wget from address:${GCC_SOURCE_ADDR}"
            wget ${GCC_SOURCE_ADDR}
        fi

        tar -xvf ${GCC_TOOLCHAIN}.tar.xz
        cp -rf ${GCC_TOOLCHAIN}/* ${GLOBAL_PROGRAM_SUPPORT}/compiler/
        rm -rf ${GCC_TOOLCHAIN}/
        mv ${GCC_TOOLCHAIN}.tar.xz ../
    else
        echo "Compiler ${greenText}${GCC_TOOLCHAIN}${defText} Check Ok!"
    fi
}
support_new_compiler

#下载arm-linux-gnueabihf-交叉编译器到old_compiler目录
function support_old_compiler()
{
    LOCAL_OLD_GCC_TOOLCHAIN=${DOWNLOAD_PATH}/${OLD_GCC_TOOLCHAIN}.tar.xz
    OLD_GCC_SOURCE_ADDR=https://mirrors.tuna.tsinghua.edu.cn/armbian-releases/_toolchain/${OLD_GCC_TOOLCHAIN}.tar.xz

    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/old_compiler/bin ]; then
        cd ${GLOBAL_PROGRAM_DOWNLOAD}/tmp/

        #compiler download exist, tar and add.
        if [ -f ${LOCAL_OLD_GCC_TOOLCHAIN} ]; then
            echo "copy gcc from local address:${LOCAL_OLD_GCC_TOOLCHAIN}"
            cp ${LOCAL_OLD_GCC_TOOLCHAIN} ./
        else
            echo "wget from address:$OLD_GCC_SOURCE_ADDR"
            wget ${OLD_GCC_SOURCE_ADDR}
        fi

        tar -xvf ${OLD_GCC_TOOLCHAIN}.tar.xz
        cp -rf ${OLD_GCC_TOOLCHAIN}/* ${GLOBAL_PROGRAM_SUPPORT}/old_compiler/
        rm -rf ${OLD_GCC_TOOLCHAIN}/
        mv ${OLD_GCC_TOOLCHAIN}.tar.xz ../
    else
        echo "Old Compiler ${greenText}$OLD_GCC_TOOLCHAIN${defText} Check Ok!"
    fi
}
support_old_compiler

#进行uboot环境的构建
function uboot_check()
{
    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/uboot/include ]; then 
        cd ${GLOBAL_PROGRAM_DOWNLOAD}/tmp/

        LOCAL_UBOOT=${DOWNLOAD_PATH}/${UBOOT_FILE}.zip
        if [ -f ${LOCAL_UBOOT} ]; then
            echo "copy uboot from local address:${LOCAL_UBOOT}"
            cp ${LOCAL_UBOOT} ./

            unzip ${LOCAL_UBOOT}
            cp -rf ${UBOOT_FILE}/* ${GLOBAL_PROGRAM_SUPPORT}/uboot/
            rm -rf ${UBOOT_FILE}/
            mv ${UBOOT_FILE}.zip ../
        else
            echo "${redText}uboot not install, copy to directory:${GLOBAL_PROGRAM_SUPPORT}/uboot/${defText}"
        fi
    else
            echo "uboot already install, directory:${greenText}${GLOBAL_PROGRAM_SUPPORT}/uboot/${defText}"
    fi
}
uboot_check

#进行kernel环境的构建
function kernel_check()
{
    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/kernel/include ]; then 
        cd ${GLOBAL_PROGRAM_DOWNLOAD}/tmp/

        LOCAL_KERNEL=${DOWNLOAD_PATH}/${KERNEL_FILE}.zip
        if [ -f ${LOCAL_KERNEL} ]; then
            echo "copy kernel from local address:${LOCAL_KERNEL}"
            cp ${LOCAL_KERNEL} ./

            unzip ${LOCAL_KERNEL}
            cp -rf ${KERNEL_FILE}/* ${GLOBAL_PROGRAM_SUPPORT}/kernel/
            rm -rf ${KERNEL_FILE}/
            mv ${KERNEL_FILE}.zip ../
        else
            echo "${redText}kernel not install, copy to directory:${GLOBAL_PROGRAM_SUPPORT}/kernel/${defText}"
        fi
    else
        echo "kernel already install, directory:${greenText}${GLOBAL_PROGRAM_SUPPORT}/kernel/${defText}"
    fi
}
kernel_check

#检查rootfs文件系统
function rootfs_check()
{
    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/rootfs/arch ]; then
        cd ${GLOBAL_PROGRAM_DOWNLOAD}/tmp/

        LOCAL_ROOTFS=${DOWNLOAD_PATH}/${ROOTFS_FILE}.tar.bz2
        if [ -f ${LOCAL_ROOTFS} ]; then
            echo "copy rootfs from local address:${LOCAL_ROOTFS}"
            cp ${LOCAL_ROOTFS} ./

            tar -xvf  ${LOCAL_ROOTFS}
            cp -rf ${ROOTFS_FILE}/* ${GLOBAL_PROGRAM_SUPPORT}/rootfs/
            rm -rf ${ROOTFS_FILE}/
            mv ${ROOTFS_FILE}.tar.bz2 ../
        else
            echo "${redText}rootfs not install, copy to directory:${GLOBAL_PROGRAM_SUPPORT}/rootfs/${defText}"
        fi
    else
        echo "rootfs already install, directory:${greenText}${GLOBAL_PROGRAM_SUPPORT}/rootfs/${defText}"
    fi
}
rootfs_check

#拷贝bashrc到根目录
function copy_bashrc()
{
    cd
    BASHRC_TEMPLATE=$CURRENT_PATH/env/.bashrc_template
    BASHRC_TMP=$CURRENT_PATH/env/.bashrc_tmp
    BASHRC=$CURRENT_PATH/env/.bashrc 

    if [ -f $BASHRC ]; then
        rm -rf $BASHRC
    fi

    if [ -f ".bashrc" ] && [ -f $CURRENT_PATH/env/.bashrc_template ]; then
        echo "copy user bashrc to the linux system."
        echo $CURRENT_PATH
        
        echo "export ENV_PATH_ROOT=$CURRENT_PATH">>$BASHRC_TMP
        cat $BASHRC_TMP $BASHRC_TEMPLATE>>$BASHRC
        
        rm -f $BASHRC_TMP
        mv .bashrc .bashrc.temp
        mv $BASHRC .bashrc
    else
        echo "bashrc not exist, just $CURRENT_PATH/env/.bashrc in environment to Directory $(pwd)"
    fi
    cd $CURRENT_PATH
}
copy_bashrc
