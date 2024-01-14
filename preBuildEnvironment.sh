#/bin/sh
##############################################################################
#这个脚本用于构建支持项目环境应用的系统
#系统位于当前目录sdk下
#包含自动安装编译工具(GCC), uboot, kernel, rootfs到指定地址，安装完成后即可配合env
#中定义的系统指令进行编译环境的构建，整个系统的编译工具，uboot，kernel和rootfs都已经
#指定。
#GCC:gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf
#uboot:uboot-imx-lf_v2022.04
#kernel:linux-imx-lf-6.1.y
#rootfs:busybox-1.36.1
################################################################################

#备用下载文件目录
DOWNLOAD_PATH=/home/program/download
AARCH_GCC_TOOLCHAIN=gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu
GCC_TOOLCHAIN=gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf
OLD_GCC_TOOLCHAIN=gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf
UBOOT_FILE=uboot-imx-lf_v2022.04
KERNEL_FILE=linux-imx-lf-6.1.y
ROOTFS_FILE=busybox-1.36.1

#全局的环境目录创建
LOCAL_PATH=$(pwd)
GLOBAL_PROGRAM_PATH=${LOCAL_PATH}/sdk

#保存下载的文件
GLOBAL_PROGRAM_DOWNLOAD=${GLOBAL_PROGRAM_PATH}/download
GLOBAL_PROGRAM_BUILD=${GLOBAL_PROGRAM_PATH}/build
GLOBAL_PROGRAM_INSTALL=${GLOBAL_PROGRAM_PATH}/install
GLOBAL_PROGRAM_SUPPORT=${GLOBAL_PROGRAM_PATH}/support
GLOBAL_PROGRAM_IMG=${GLOBAL_PROGRAM_PATH}/img

#定义全局color
readonly defText="$(tput sgr0)"
readonly redText="$(tput setaf 1)"
readonly greenText="$(tput setaf 2)"

#创建编译环境
if [ ! -d "$GLOBAL_PROGRAM_BUILD" ]; then
    
    echo "start create the environment."
    sudo mkdir $GLOBAL_PROGRAM_PATH
    sudo chmod 777 $GLOBAL_PROGRAM_PATH

    #安装img文件
    mkdir -p ${GLOBAL_PROGRAM_IMG}/

    #构建系统需要的环境信息 
    #下载目录，存放下载的第三方库
    mkdir -p ${GLOBAL_PROGRAM_DOWNLOAD}/tmp
    
    #存放编译后的安装目录，特别时交叉编译环境
    mkdir -p ${GLOBAL_PROGRAM_INSTALL}/arm
    mkdir -p ${GLOBAL_PROGRAM_INSTALL}/aarch64

    #存放编译支持的目录，如编译工具，脚本，文件系统
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/arm/compiler/
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/arm/old_compiler/
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/arm/uboot/
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/arm/kernel/
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/arm/rootfs/ 

    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/aarch64/compiler/
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/aarch64/uboot/
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/aarch64/kernel/
    mkdir -p ${GLOBAL_PROGRAM_SUPPORT}/aarch64/rootfs/ 

    #存放编译好的nfs和tftp目录
    mkdir -p ${GLOBAL_PROGRAM_BUILD}/img/
    mkdir -p ${GLOBAL_PROGRAM_BUILD}/nfs_root/arm   
    mkdir -p ${GLOBAL_PROGRAM_BUILD}/nfs_root/aarch64
    mkdir -p ${GLOBAL_PROGRAM_BUILD}/tftp_root/arm/
    mkdir -p ${GLOBAL_PROGRAM_BUILD}/tftp_root/aarch64/

    sudo chmod -Rv 777 ${GLOBAL_PROGRAM_PATH}/
else
    echo "Directory ${greenText}${GLOBAL_PROGRAM_PATH}${defText} exist, Check Ok!"
fi

echo "now put file used into the environment."
echo "  ${greenText}${GLOBAL_PROGRAM_DOWNLOAD}/${defText}           :下载的各类软件，安装包或者编译的库源码"
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

function support_aarch64_compiler()
{
    LOCAL_GCC_TOOLCHAIN=${DOWNLOAD_PATH}/${AARCH_GCC_TOOLCHAIN}.tar.xz
    GCC_SOURCE_ADDR=https://mirrors.tuna.tsinghua.edu.cn/armbian-releases/_toolchain/${AARCH_GCC_TOOLCHAIN}.tar.xz

    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/aarch64/compiler/bin ]; then
        cd ${GLOBAL_PROGRAM_DOWNLOAD}/tmp/

        if [ -f ${LOCAL_GCC_TOOLCHAIN} ]; then
            echo "copy gcc from local address:${LOCAL_GCC_TOOLCHAIN}"
            cp ${LOCAL_GCC_TOOLCHAIN} ./
        else
            echo "wget from address:${GCC_SOURCE_ADDR}"
            wget ${GCC_SOURCE_ADDR}
        fi

        tar -xvf ${AARCH_GCC_TOOLCHAIN}.tar.xz
        cp -rf ${AARCH_GCC_TOOLCHAIN}/* ${GLOBAL_PROGRAM_SUPPORT}/aarch64/compiler/
        rm -rf ${AARCH_GCC_TOOLCHAIN}/
        mv ${AARCH_GCC_TOOLCHAIN}.tar.xz ../
    else
        echo "Compiler ${greenText}${AARCH_GCC_TOOLCHAIN}${defText} Check Ok!"
    fi
}
support_aarch64_compiler

#下载arm-none-linux-gnueabihf-交叉编译器到compiler目录
function support_new_compiler()
{
    LOCAL_GCC_TOOLCHAIN=${DOWNLOAD_PATH}/${GCC_TOOLCHAIN}.tar.xz
    GCC_SOURCE_ADDR=https://mirrors.tuna.tsinghua.edu.cn/armbian-releases/_toolchain/${GCC_TOOLCHAIN}.tar.xz

    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/arm/compiler/bin ]; then
        cd $GLOBAL_PROGRAM_DOWNLOAD/tmp/

        if [ -f ${LOCAL_GCC_TOOLCHAIN} ]; then
            echo "copy gcc from local address:${LOCAL_GCC_TOOLCHAIN}"
            cp ${LOCAL_GCC_TOOLCHAIN} ./
        else
            echo "wget from address:${GCC_SOURCE_ADDR}"
            wget ${GCC_SOURCE_ADDR}
        fi

        tar -xvf ${GCC_TOOLCHAIN}.tar.xz
        cp -rf ${GCC_TOOLCHAIN}/* ${GLOBAL_PROGRAM_SUPPORT}/arm/compiler/
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

    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/arm/old_compiler/bin ]; then
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
        cp -rf ${OLD_GCC_TOOLCHAIN}/* ${GLOBAL_PROGRAM_SUPPORT}/arm/old_compiler/
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
    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/arm/uboot/include ]; then 
        cd ${GLOBAL_PROGRAM_DOWNLOAD}/tmp/

        LOCAL_UBOOT=${DOWNLOAD_PATH}/${UBOOT_FILE}.zip
        if [ -f ${LOCAL_UBOOT} ]; then
            echo "copy uboot from local address:${LOCAL_UBOOT}"
            cp ${LOCAL_UBOOT} ./

            unzip ${LOCAL_UBOOT}
            cp -rf ${UBOOT_FILE}/* ${GLOBAL_PROGRAM_SUPPORT}/arm/uboot/
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
    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/arm/kernel/include ]; then 
        cd ${GLOBAL_PROGRAM_DOWNLOAD}/tmp/

        LOCAL_KERNEL=${DOWNLOAD_PATH}/${KERNEL_FILE}.zip
        if [ -f ${LOCAL_KERNEL} ]; then
            echo "copy kernel from local address:${LOCAL_KERNEL}"
            cp ${LOCAL_KERNEL} ./

            unzip ${LOCAL_KERNEL}
            cp -rf ${KERNEL_FILE}/* ${GLOBAL_PROGRAM_SUPPORT}/arm/kernel/
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
    if [ ! -d ${GLOBAL_PROGRAM_SUPPORT}/arm/rootfs/arch ]; then
        cd ${GLOBAL_PROGRAM_DOWNLOAD}/tmp/

        LOCAL_ROOTFS=${DOWNLOAD_PATH}/${ROOTFS_FILE}.tar.bz2
        if [ -f ${LOCAL_ROOTFS} ]; then
            echo "copy rootfs from local address:${LOCAL_ROOTFS}"
            cp ${LOCAL_ROOTFS} ./

            tar -xvf  ${LOCAL_ROOTFS}
            cp -rf ${ROOTFS_FILE}/* ${GLOBAL_PROGRAM_SUPPORT}/arm/rootfs/
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
    BASHRC_TEMPLATE=$LOCAL_PATH/env/.bashrc_template
    BASHRC_TMP=$LOCAL_PATH/env/.bashrc_tmp
    BASHRC=$LOCAL_PATH/env/.bashrc 

    if [ -f $BASHRC ]; then
        rm -rf $BASHRC
    fi

    if [ -f ".bashrc" ] && [ -f $LOCAL_PATH/env/.bashrc_template ]; then
        echo "copy user bashrc to the linux system."
        echo $LOCAL_PATH
        
        echo "export ENV_PATH_ROOT=$LOCAL_PATH">>$BASHRC_TMP
        cat $BASHRC_TMP $BASHRC_TEMPLATE>>$BASHRC
        
        rm -f $BASHRC_TMP
        mv .bashrc .bashrc.temp
        mv $BASHRC .bashrc
        
    else
        echo "bashrc not exist, just $LOCAL_PATH/env/.bashrc in environment to Directory $(pwd)"
    fi
    cd $LOCAL_PATH
}
copy_bashrc
