
PLATFORM_BOOT=$(pwd)

SRC_DIR=${PLATFORM_BOOT}
DST_DIR=${SUPPORT_ENV_BOOT_DIR}

echo "start copy u-boot patch..."

#boot script
cp -fv ${SRC_DIR}/boot.cmd ${DST_DIR}/

#configs
cp -fv ${SRC_DIR}/configs/* ${DST_DIR}/configs/

#dts
cp -fv ${SRC_DIR}/arch/arm/dts/* ${DST_DIR}/arch/arm/dts/
cp -fv ${SRC_DIR}/arch/arm/mach-imx/mx6/Kconfig ${DST_DIR}/arch/arm/mach-imx/mx6/

#drivers
cp -fv ${SRC_DIR}/drivers/net/phy/phy.c ${DST_DIR}/drivers/net/phy/

#include 
cp -fv ${SRC_DIR}/include/configs/mx6ullrmk.h ${DST_DIR}/include/configs/

#board
if [ ! -d ${DST_DIR}/board/freescale/mx6ullrmk/ ]; then
    mkdir -p ${DST_DIR}/board/freescale/mx6ullrmk
fi
cp -rfv ${SRC_DIR}/board/freescale/mx6ullrmk/* ${DST_DIR}/board/freescale/mx6ullrmk/

#configs
if [ ! -d "${DST_DIR}/.vscode" ]; then
    mkdir -p "${DST_DIR}/.vscode"
fi
cp -rv ${SRC_DIR}/.vscode/settings.json ${DST_DIR}/.vscode/

echo "end copy u-boot patch, success!"
sleep 1