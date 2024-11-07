#!/bin/bash
#
# Copyright (c) 2024 zc Co., Ltd
#
# SPDX-License-Identifier: GPL-2.0
#

########################################### global variable Defined #############################################
PATH_PWD="$(pwd)"

CROSS_COMPILE="arm-none-linux-gnueabihf-"
FILE_CROSS_COMPILE="${PATH_PWD}/toolchain/bin/${CROSS_COMPILE}"
THREAD_COUNT=3

#kernel information
CHIP_ARCH="arm"
LINUX_CONFIG="imx_rmk_v7_defconfig"
LINUX_QEMU_CONFIG="imx_qemu_defconfig"
ZIMAGE_FILE=zImage
DTB_EMMC_FILE=imx6ull-14x14-emmc-4.3-800x480-c.dtb
DTB_NAND_FILE=imx6ull-14x14-nand-4.3-800x480-c.dtb
QEMU_DTB_FILE=imx6ull-qemu.dtb

KERNEL_ZIMAGE="${SUPPORT_ENV_KERNEL_DIR}/arch/arm/boot/${ZIMAGE_FILE}"
KERNEL_DTB="${SUPPORT_ENV_KERNEL_DIR}/arch/arm/boot/dts/${DTB_EMMC_FILE}"
KERNEL_NAND_DTB="${SUPPORT_ENV_KERNEL_DIR}/arch/arm/boot/dts/${DTB_NAND_FILE}"
KERNEL_QEMU_DTB="${SUPPORT_ENV_KERNEL_DIR}/arch/arm/boot/dts/${QEMU_DTB_FILE}"

#rootfs information
DEBIAN_SIZE=2048

UBOOT_PATH="${PATH_PWD}/u-boot"
KERNEL_PATH="${PATH_PWD}/kernel"
ROOTFS_PATH="${PATH_PWD}/rootfs"
PACKAGE_PATH="${PATH_PWD}/package"

ROOTFS_SHELL_PATH="${ROOTFS_PATH}/shell"
ROOFTS_DEBIAN_PATH="${ROOTFS_PATH}/debian"
ROOTFS_IMG_PATH="${ROOTFS_PATH}/img"
###############################################################################################################

function help()
{
    echo
    echo "Usage:"
    echo "  ./make.sh [sub-command] [parameter]"
    echo
    echo "  - sub-command:  elf*|loader|uboot|--spl|--tpl|itb|map|sym|<addr>"
    echo "  - parameter:    parameter for build"
    echo
    echo "Output:"
    echo "  When board built okay, there are output images in current directory"
    echo
    echo "Example:"
    echo
    echo "1. Build:"
    echo "  ./make.sh                          --- build with exist .config, for all"
    echo
    echo "2. Pack:"
    echo "  ./make.sh uboot                    --- pack uboot.img, same as emmc"
    echo "  ./make.sh uboot emmc               --- pack uboot.img with emmc env"   
    echo "  ./make.sh uboot sd                 --- pack uboot.img with sdcard env"
    echo "  ./make.sh uboot nand               --- pack uboot.img with nand env"
    echo "  ./make.sh kernel                   --- pack linux kernel"
    echo "  ./make.sh kernel normal            --- pack linux kernel normal mode"
    echo "  ./make.sh kernel qemu              --- pack linux kernel qemu mode"    
    echo "  ./make.sh rootfs                   --- pack rootfs, same as buildroot"
    echo "  ./make.sh rootfs buildroot         --- pack rootfs buildroot"
    echo "  ./make.sh rootfs debian            --- pack rootfs debian"
    echo "  ./make.sh rootfs ubuntu            --- pack rootfs ubuntu"
}

exit_if_last_error() {
    if [[ $? -ne 0 ]]; then
        echo "上一条命令执行失败，脚本将退出。"
        exit 1
    fi
}

compile_u_boot()
{
    config_file=mx6ull_14x14_rmk_emmc_defconfig

    if [ "$1" == "sd" ]; then
        config_file=mx6ull_14x14_rmk_sd_defconfig
    elif [ "$1" == "nand" ];then
        config_file=mx6ull_14x14_rmk_nand_defconfig
    fi

    cd "${UBOOT_PATH}" || return

    echo "====== start u-boot build, Config file:$config_file ======"

    #create boot.scr
    mkimage -C none -A arm -T script -d boot.cmd boot.scr
    mv boot.scr "${PACKAGE_PATH}"/

    #build u-boot
    make "${config_file}" 
    make V=1 ARCH=arm CROSS_COMPILE="${FILE_CROSS_COMPILE}"

    exit_if_last_error

    cp u-boot-dtb.imx "${PACKAGE_PATH}"/

    echo "====== u-boot build finished, success! ======"
}

compile_kernel()
{
    cd "${KERNEL_PATH}" || return

    linux_config_file="${LINUX_CONFIG}"

    if [ "$1" == "qemu" ]; then
        linux_config_file="${LINUX_QEMU_CONFIG}"
    fi

    echo "====== start kernel, Config file:$linux_config_file ======"

    make "${linux_config_file}" CROSS_COMPILE="${FILE_CROSS_COMPILE}" ARCH="${CHIP_ARCH}"
    make -j"${THREAD_COUNT}" CROSS_COMPILE="${FILE_CROSS_COMPILE}" ARCH="${CHIP_ARCH}"

    exit_if_last_error
    
    cp -avf "${KERNEL_ZIMAGE}" "${PACKAGE_PATH}"/
    cp -avf "${KERNEL_DTB}" "${PACKAGE_PATH}"/
    cp -avf "${KERNEL_NAND_DTB}" "${PACKAGE_PATH}"/
    cp -avf "${KERNEL_QEMU_DTB}" "${PACKAGE_PATH}"/

    echo "====== kernel build finished, success! ======"
}

compile_buildroot()
{
    echo "====== start create buildroot ======"

    export BR2_TOOLCHAIN_EXTERNAL_PATH="${PATH_PWD}/toolchain"
    make imx6ullrmk_defconfig
    make -j"${CPU_CORE}"

    exit_if_last_error

    echo "====== buildroot build finished, success! ======"

    cp -fv output/images/rootfs.ext2 "${PACKAGE_PATH}"/buildroot.img
}

compile_debian()
{
    echo "====== start create debian ======"

    cd "${PACKAGE_PATH}" || return

    if [ ! -f debian.img ]; then
        dd if=/dev/zero of=debian.img bs=1M count=${DEBIAN_SIZE}
        mkfs.ext4 debian.img
    fi

    sudo mount -o loop "${PACKAGE_PATH}"/debian.img  "${ROOFTS_DEBIAN_PATH}"/

    cd "${ROOTFS_SHELL_PATH}" || return

    chmod 777 install-debian.sh && ./install-debian.sh "${ROOFTS_DEBIAN_PATH}"

    echo "====== debian build finished, success! ======"

    sudo umount "${ROOFTS_DEBIAN_PATH}"/
}

compile_rootfs()
{
    rootfs=buildroot

    if [ "$1" == "debian" ]; then
        rootfs=debian
    elif [ "$1" == "ubuntu" ];then
        rootfs=ubuntu
    fi

    ROOTFS_FILE_PATH="${ROOTFS_PATH}/${rootfs}"

    cd "${ROOTFS_FILE_PATH}" || return
    
    if [ "${rootfs}" == "buildroot" ]; then
        compile_buildroot
    elif [ "${rootfs}" == "debian" ]; then     
        compile_debian
    fi
}

clean_all()
{
    if [ -d ${PACKAGE_PATH} ]; then
        rm -rf ${PACKAGE_PATH}/*
    fi

    make clean -C ${KERNEL_PATH}
    make clean -C ${UBOOT_PATH}
    make clean -C ${ROOTFS_PATH}/buildroot
}

function process_args()
{
    case $1 in
        *help|--h|-h)
            help
            exit 0
            ;;
        uboot | u-boot | u-boot/)
            compile_u_boot "$2"
            exit 0
            ;;
        kernel | kernel/)
            compile_kernel "$2"
            exit 0
            ;;
        rootfs| rootfs/)
            compile_rootfs "$2"
            exit 0
            ;;
        clean)
            clean_all 
            exit 0
            ;;
        esac
}

process_args $*
