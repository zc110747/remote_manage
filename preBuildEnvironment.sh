#!/bin/bash
##############################################################################
#这个脚本用于构建支持项目环境的sdk
#系统位于当前目录sdk下
#包含自动安装编译工具(GCC), uboot, kernel, rootfs到指定地址，安装完成后即可配合env
#中定义的系统指令进行编译环境的构建，整个系统的编译工具，uboot，kernel和rootfs都已经
#指定。
#GCC:gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf
#uboot:uboot-imx-lf_v2022.04
#kernel:linux-imx-lf-6.1.y
#rootfs:busybox-1.36.1
################################################################################

################################################################################
#   download                    #下载文件的目录
#       - tmp                   #缓存空间
#   nfs                         #文件系统img挂载目录,远程访问也使用此目录
#   tftp                        #u-boot,kernel编译后存放目录
#   arm
#       - install               #系统运行需要的支持环境
#       - u-boot                #u-boot系统源码
#       - kernel                #linux系统源码
#       - package               #系统打包的目录(文件系统img存放再此目录)
#       - toolchain             #交叉编译工具
#       - rootfs                #文件系统目录
#           - buildroot         #buildroot文件系统源码
#           - debain            #debain系统构建目录
#           - qt                #qt系统编译目录
#           - ubuntu            #ubuntu系统构建目录
#           - shell             #构建脚本 
#################################################################################

#备用下载文件目录

LOCAL_PATH=$(pwd)/..
GLOBAL_PROGRAM_PATH="${LOCAL_PATH}"/sdk
GLOBAL_PROGRAM_DOWNLOAD="${GLOBAL_PROGRAM_PATH}"/download
GLOBAL_PROGRAM_NFS="${GLOBAL_PROGRAM_PATH}"/nfs
GLOBAL_PROGRAM_TFTP="${GLOBAL_PROGRAM_PATH}"/tftp
GLOBAL_PROGRAM_ARM_PATH="${GLOBAL_PROGRAM_PATH}"/arm
GLOBAL_PROGRAM_AARCH64_PATH="${GLOBAL_PROGRAM_PATH}"/aarch64
DOWNLOAD_PATH=${GLOBAL_PROGRAM_DOWNLOAD}
IMX_PLATFORM_PATH="${LOCAL_PATH}"/remote_manage/platform/I.MX6ULL
RUN_PATH=$(pwd)

#定义全局color
readonly defText="$(tput sgr0)"
readonly redText="$(tput setaf 1)"
readonly greenText="$(tput setaf 2)"

echo "now put file used into the environment."
echo "  ${greenText}${GLOBAL_PROGRAM_DOWNLOAD}/${defText}                           :下载的各类软件，安装包或者编译的库源码"
echo "  ${greenText}${GLOBAL_PROGRAM_NFS}/${defText}                                :文件系统img挂载目录,远程访问也使用此目录"
echo "  ${greenText}${GLOBAL_PROGRAM_TFTP}/${defText}                               :u-boot,kernel编译后存放目录"
echo "  ${greenText}${GLOBAL_PROGRAM_PATH}/[core]/install/${defText}                :用于安装库的目录"
echo "  ${greenText}${GLOBAL_PROGRAM_PATH}/[core]/u-boot/${defText}                 :u-boot系统源码"
echo "  ${greenText}${GLOBAL_PROGRAM_PATH}/[core]/kernel/${defText}                 :linux系统源码"
echo "  ${greenText}${GLOBAL_PROGRAM_PATH}/[core]/package/${defText}                :系统打包的目录(文件系统img存放再此目录)"
echo "  ${greenText}${GLOBAL_PROGRAM_PATH}/[core]/toolchain/${defText}              :交叉编译工具"
echo "  ${greenText}${GLOBAL_PROGRAM_PATH}/[core]/rootfs/buildroot/${defText}       :buildroot文件系统目录(support qt)"
echo "  ${greenText}${GLOBAL_PROGRAM_PATH}/[core]/rootfs/debain/${defText}          :debain文件系统目录"
echo "  ${greenText}${GLOBAL_PROGRAM_PATH}/[core]/rootfs/ubuntu/${defText}          :ubuntu文件系统目录"
echo "  ${greenText}${GLOBAL_PROGRAM_PATH}/[core]/rootfs/shell/${defText}           :shell文件系统目录"
echo "未复制则后续编译时会显示找不到文件或者直接编译失败"

#按照系统编译支持的library库
if [ x"$1" != x ] && [ "$1" == "all" ]; then
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

if [ ! -d "${GLOBAL_PROGRAM_PATH}" ]; then
    echo "***start create the environment.***"
    sudo mkdir -m 777 -p "${GLOBAL_PROGRAM_PATH}"

    mkdir -m 755 -p "${GLOBAL_PROGRAM_ARM_PATH}"
    mkdir -m 755 -p "${GLOBAL_PROGRAM_AARCH64_PATH}"

    #构建系统需要的环境信息 
    #下载目录，存放下载的第三方库
    mkdir -m 755 -p "${GLOBAL_PROGRAM_DOWNLOAD}"/tmp
    mkdir -m 755 -p "${GLOBAL_PROGRAM_PATH}"/tftp
    mkdir -m 755 -p "${GLOBAL_PROGRAM_PATH}"/nfs 
fi

function creat_arm_sdk()
{
    gcc_toolchain=gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf
    uboot_file=uboot-imx-lf_v2022.04
    kernel_file=linux-imx-lf-6.1.y

    #support image
    global_program_img=${GLOBAL_PROGRAM_ARM_PATH}/img
    mkdir -m 755 -p "${global_program_img}"

    #support package
    global_program_package=${GLOBAL_PROGRAM_ARM_PATH}/package
    mkdir -m 755 -p "${global_program_package}"

    #support compiler
    global_program_toolchain=${GLOBAL_PROGRAM_ARM_PATH}/toolchain
    mkdir -m 755 -p "${global_program_toolchain}"
    local_gcc_toolchain="${DOWNLOAD_PATH}/${gcc_toolchain}.tar.xz"
    gcc_source_webfile=https://mirrors.tuna.tsinghua.edu.cn/armbian-releases/_toolchain/${gcc_toolchain}.tar.xz

    if [ ! -d "${global_program_toolchain}/bin" ]; then
        cd "${GLOBAL_PROGRAM_DOWNLOAD}/tmp/" || return

        if [ -f ${local_gcc_toolchain} ]; then
            echo "copy gcc from local address:${local_gcc_toolchain}"
            cp ${local_gcc_toolchain} ./
        else
            echo "wget from address:${gcc_source_webfile}"
            wget ${gcc_source_webfile}
        fi

        tar -xvf "${gcc_toolchain}.tar.xz"
        cp -rf "${gcc_toolchain}"/* "${global_program_toolchain}/"
        rm -rf "${gcc_toolchain:?}/"
        mv ${gcc_toolchain}.tar.xz ../
    else
        echo "Compiler ${greenText}${gcc_toolchain}${defText} Check Ok!"
    fi

    #uboot install
    global_program_uboot=${GLOBAL_PROGRAM_ARM_PATH}/u-boot
    mkdir -m 755 -p "${global_program_uboot}"

    if [ ! -d "${global_program_uboot}/include" ]; then 
        cd "${GLOBAL_PROGRAM_DOWNLOAD}/tmp/" || return

        local_uboot="${DOWNLOAD_PATH}/${uboot_file}.zip"
        if [ -f "${local_uboot}" ]; then
            echo "copy uboot from local address:${local_uboot}"
            cp ${local_uboot} ./

            unzip ${local_uboot}
            cp -rf "${uboot_file}"/* "${global_program_uboot}/"
            rm -rf ${uboot_file:?}/
            mv ${uboot_file}.zip ../
        else
            echo "${redText}uboot not install, copy to directory:${global_program_uboot}/${defText}"
        fi
    else
            echo "uboot already install, directory:${greenText}${global_program_uboot}/${defText}"
    fi

    #kernel install
    global_program_kernel=${GLOBAL_PROGRAM_ARM_PATH}/kernel
    mkdir -m 755 -p "${global_program_kernel}"

    if [ ! -d "${global_program_kernel}/include" ]; then 
        cd "${GLOBAL_PROGRAM_DOWNLOAD}/tmp/" || return

        LOCAL_KERNEL=${DOWNLOAD_PATH}/${kernel_file}.zip
        if [ -f ${LOCAL_KERNEL} ]; then
            echo "copy kernel from local address:${LOCAL_KERNEL}"
            cp ${LOCAL_KERNEL} ./

            unzip ${LOCAL_KERNEL}
            cp -rf "${kernel_file}"/* "${global_program_kernel}/"
            rm -rf "${kernel_file:?}/"
            mv ${kernel_file}.zip ../
        else
            echo "${redText}kernel not install, copy to directory:${global_program_kernel}/${defText}"
        fi
    else
        echo "kernel already install, directory:${greenText}${global_program_kernel}/${defText}"
    fi

    #rootfs install
    global_program_rootfs=${GLOBAL_PROGRAM_ARM_PATH}/rootfs
    mkdir -m 755 -p "${global_program_rootfs}/buildroot"
    mkdir -m 755 -p "${global_program_rootfs}/debain"
    mkdir -m 755 -p "${global_program_rootfs}/qt"
    mkdir -m 755 -p "${global_program_rootfs}/shell"
    mkdir -m 755 -p "${global_program_rootfs}/ubuntu"

    chmod 777 "${IMX_PLATFORM_PATH}/platform-shell/make.sh"
    cp "${IMX_PLATFORM_PATH}/platform-shell/make.sh" "${GLOBAL_PROGRAM_ARM_PATH}/"

    chmod 777 "${IMX_PLATFORM_PATH}/platform-shell/install-debain.sh"
    cp "${IMX_PLATFORM_PATH}/platform-shell/install-debain.sh" "${global_program_rootfs}/shell/"
}

function creat_aarch64_sdk()
{
    AARCH_gcc_toolchain=gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu

    global_program_build="${GLOBAL_PROGRAM_AARCH64_PATH}"/build
    mkdir -m 755 -p "${global_program_build}"/nfs_root 
    mkdir -m 755 -p "${global_program_build}"/tftp_root

    global_program_img=${GLOBAL_PROGRAM_AARCH64_PATH}/img
    mkdir -m 755 -p "${global_program_img}"

    global_program_install=${GLOBAL_PROGRAM_AARCH64_PATH}/install
    mkdir -m 755 -p "${global_program_install}"

    global_program_support=${GLOBAL_PROGRAM_AARCH64_PATH}/support
    mkdir -m 755 -p "${global_program_support}"/compiler/
    mkdir -m 755 -p "${global_program_support}"/uboot/
    mkdir -m 755 -p "${global_program_support}"/kernel/
    mkdir -m 755 -p "${global_program_support}"/rs_buildroot/ 
    mkdir -m 755 -p "${global_program_support}"/rs_debain/
    mkdir -m 755 -p "${global_program_support}"/rs_ubuntu/ 

    local_gcc_toolchain="${DOWNLOAD_PATH}/${AARCH_gcc_toolchain}".tar.xz
    gcc_source_webfile=https://mirrors.tuna.tsinghua.edu.cn/armbian-releases/_toolchain/"${AARCH_gcc_toolchain}".tar.xz

    if [ ! -d "${global_program_support}"/compiler/bin ]; then
        cd "${GLOBAL_PROGRAM_DOWNLOAD}/tmp/" || return

        if [ -f "${local_gcc_toolchain}" ]; then
            echo "copy gcc from local address:${local_gcc_toolchain}"
            cp "${local_gcc_toolchain}" ./
        else
            echo "wget from address:${gcc_source_webfile}"
            wget ${gcc_source_webfile}
        fi

        tar -xvf "${AARCH_gcc_toolchain}.tar.xz"
        cp -rf "${AARCH_gcc_toolchain}"/* "${global_program_support}"/compiler/
        #rm -rf "${AARCH_gcc_toolchain:?}/"
        mv "${AARCH_gcc_toolchain}.tar.xz" ../
    else
        echo "Compiler ${greenText}${AARCH_gcc_toolchain}${defText} Check Ok!"
    fi
}

#拷贝bashrc到根目录
function copy_bashrc()
{
    cd  || return

    BASHRC_TEMPLATE=${RUN_PATH}/env/.bashrc_template
    BASHRC_TMP=${RUN_PATH}/env/.bashrc_tmp
    BASHRC=${RUN_PATH}/env/.bashrc 

    if [ -f "${BASHRC}" ]; then
        rm -rf "${BASHRC:?}"
    fi

    if [ -f ".bashrc" ] && [ -f "${RUN_PATH}/env/.bashrc_template" ]; then
        echo "copy user bashrc to the linux system."
        echo "${RUN_PATH}"
        
        echo "export ENV_PATH_ROOT=${RUN_PATH}">>"${BASHRC_TMP}"
        cat "${BASHRC_TMP}" "${BASHRC_TEMPLATE}">>"${BASHRC}"
        
        rm -f "${BASHRC_TMP:?}"
        mv .bashrc .bashrc.temp
        mv "${BASHRC}" .bashrc
        
    else
        echo "bashrc not exist, just ${RUN_PATH}/env/.bashrc in environment to Directory $(pwd)"
    fi
    cd "${RUN_PATH}" || exit
}

function install_all_sdk()
{
    creat_arm_sdk

    #creat_aarch64_sdk

    copy_bashrc
}
install_all_sdk
