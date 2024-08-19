#!/bin/bash
#
# Copyright (c) 2024 zc Co., Ltd
#
# SPDX-License-Identifier: GPL-2.0
#

########################################### global variable Defined #############################################
PATH_PWD="$(pwd)"
UBOOT_PATH="${PATH_PWD}/u-boot"
KERNEL_PATH="${PATH_PWD}/kernel"
PACKAGE_PATH="${PATH_PWD}/package"
ROOTFS_PATH="${PATH_PWD}/rootfs"

GCC_VERSION="gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu"
CROSS_COMPILE="aarch64-none-linux-gnu-"

#trust information
ARM_TRUST="arm-trusted-firmware"
ATF_PLAT="sun50i_h616"
ARM_TRUST_PATH="${PATH_PWD}/${ARM_TRUST}"

#u-boot information
UBOOT_CONFIG="walnutpi_1b_defconfig"
UBOOT_BIN_NAME="u-boot-sunxi-with-spl.bin"

#kernel information
CHIP_ARCH="arm64"
CHIP_NAME="H616"
LINUX_CONFIG="walnutpi1b_defconfig"
THREAD_COUNT=3

FILE_CROSS_COMPILE="${PATH_PWD}/toolchain/${GCC_VERSION}/bin/${CROSS_COMPILE}"

#rootfs information
DEBAIN_SIZE=4096
ROOTFS_SHELL_PATH="${ROOTFS_PATH}/shell"
ROOFTS_DEBAIN_PATH="${ROOTFS_PATH}/debain"

#########################################################################################################

function help()
{
	echo
	echo "Usage:"
	echo "	./make.sh [sub-command]"
	echo
	echo "	 - sub-command:  elf*|loader|trust|uboot|--spl|--tpl|itb|map|sym|<addr>"
	echo "	 - ini:          ini file to pack trust/loader"
	echo
	echo "Output:"
	echo "	 When board built okay, there are output images in current directory"
	echo
	echo "Example:"
	echo
	echo "1. Build:"
	echo "	./make.sh                          --- build with exist .config"
	echo
	echo "2. Pack:"
    echo "	./make.sh trust                    --- pack trust.img"
	echo "	./make.sh uboot                    --- pack uboot.img"
	echo "	./make.sh --spl                    --- pack loader with u-boot-spl.bin"
	echo "	./make.sh --tpl                    --- pack loader with u-boot-tpl.bin"
	echo "	./make.sh --tpl --spl              --- pack loader with u-boot-tpl.bin and u-boot-spl.bin"
	echo
}

exit_if_last_error() {
    if [[ $? -ne 0 ]]; then
        echo "上一条命令执行失败，脚本将退出。"
        exit 1
    fi
}

#trust firmware address
#git clone 
compile_trust_firmware() 
{
    echo "====== trusted firmware compile ======"

    cd "${ARM_TRUST_PATH}" || return
    make PLAT="${ATF_PLAT}"  DEBUG=1 bl31 CROSS_COMPILE=${FILE_CROSS_COMPILE}

    exit_if_last_error

    echo "======  trusted firmware compile success! ======"
}

compile_u_boot()
{
    cd "${UBOOT_PATH}" || return

    echo "====== u-boot compile ======"

    mkimage -C none -A arm -T script -d boot.cmd boot.scr
    mv boot.scr "${PACKAGE_PATH}"/
    cp config.txt "${PACKAGE_PATH}/"

    make ${UBOOT_CONFIG}

    make BL31=${ARM_TRUST_PATH}/build/$ATF_PLAT/debug/bl31.bin CROSS_COMPILE=$FILE_CROSS_COMPILE

    echo "====== u-boot compile success! ======"

    cp -rv "${UBOOT_BIN_NAME}" "${PACKAGE_PATH}"/
}

compile_kernel()
{
    cd "${KERNEL_PATH}" || return

    make "${LINUX_CONFIG}" CROSS_COMPILE="${FILE_CROSS_COMPILE}" ARCH="${CHIP_ARCH}"
    make -j"${THREAD_COUNT}" CROSS_COMPILE="${FILE_CROSS_COMPILE}" ARCH="${CHIP_ARCH}"

    exit_if_last_error
    
    echo "kernel compile success!"

    #install zimage 
    cp "${KERNEL_PATH}/arch/${CHIP_ARCH}/boot/Image" "${PACKAGE_PATH}/"
    make modules_install INSTALL_MOD_PATH="${PACKAGE_PATH}" ARCH=${CHIP_ARCH}
    make dtbs_install INSTALL_DTBS_PATH="${PACKAGE_PATH}" ARCH=${CHIP_ARCH}
}

compile_debain()
{
    echo "====== start create debain ======"

    cd "${PACKAGE_PATH}" || return

    if [ ! -f debain.img ]; then
        dd if=/dev/zero of=debain.img bs=1M count=${DEBAIN_SIZE}
        mkfs.ext4 debain.img
    fi

    sudo mount -o loop "${PACKAGE_PATH}"/debain.img  "${ROOFTS_DEBAIN_PATH}"/

    cd "${ROOTFS_SHELL_PATH}" || return

    chmod 777 install-debain.sh && ./install-debain.sh "${ROOFTS_DEBAIN_PATH}"

    echo "====== debain build finished, success! ======"

    sudo umount "${ROOFTS_DEBAIN_PATH}"/
}

compile_clean()
{
    if [ -d ${PACKAGE_PATH} ]; then
        rm -rf ${PACKAGE_PATH}/*
    fi

    make clean -C ${KERNEL_PATH}
    make clean -C ${UBOOT_PATH}
    make clean -C ${ARM_TRUST_PATH}

    if [ -d ${ROOTFS_PATH}/buildroot ]; then
        make clean -C ${ROOTFS_PATH}/buildroot
    fi
}

function process_args()
{
    case $1 in
        *help|--h|-h)
            help
            exit 0
            ;;
        trust)
            compile_trust_firmware
            exit 0
            ;;
        uboot|u-boot|u-boot/)
            compile_trust_firmware
            compile_u_boot
            exit 0
            ;;
        kernel|kernel/)
            compile_kernel
            exit 0
            ;;
        debain)
            compile_debain
            exit 0
            ;;
        clean)
            compile_clean
            exit 0
            ;;
    esac
}

process_args $*
