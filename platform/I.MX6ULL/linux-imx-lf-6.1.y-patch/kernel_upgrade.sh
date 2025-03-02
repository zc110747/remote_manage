
PLATFORM_BOOT=$(pwd)

#SRC_DIR=${SUPPORT_ENV_KERNEL_DIR}
#DST_DIR=${PLATFORM_BOOT}
SRC_DIR=${PLATFORM_BOOT}
DST_DIR=${SUPPORT_ENV_KERNEL_DIR}

#dts
cp -rv ${SRC_DIR}/arch/arm/boot/dts/* ${DST_DIR}/arch/arm/boot/dts/

#config
cp -rv ${SRC_DIR}/arch/arm/configs/* ${DST_DIR}/arch/arm/configs/

#firmware
if [ ! -d ${DST_DIR}/firmware ]; then
    mkdir ${DST_DIR}/firmware
fi
cp -rv ${SRC_DIR}/firmware/* ${DST_DIR}/firmware/

#drivers
cp -rv ${SRC_DIR}/drivers/* ${DST_DIR}/drivers/

#include
cp -rv ${SRC_DIR}/include/* ${DST_DIR}/include/

# scripts, support for overlay
cp -rv ${SRC_DIR}/scripts/* ${DST_DIR}/scripts/

#.vscode
if [ ! -d ${DST_DIR}/.vscode ]; then
    mkdir ${DST_DIR}/.vscode
fi
cp -rv ${SRC_DIR}/.vscode/settings.json ${DST_DIR}/.vscode/