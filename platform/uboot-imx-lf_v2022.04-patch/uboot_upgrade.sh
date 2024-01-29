
PLATFORM_BOOT=$(pwd)

#SRC_DIR=${ENV_BOOT_DIR}
#DST_DIR=${PLATFORM_BOOT}
SRC_DIR=${PLATFORM_BOOT}
DST_DIR=${ENV_BOOT_DIR}

echo "start copy u-boot patch..."

#configs
cp -fv ${SRC_DIR}/configs/mx6ull_14x14_rmk_emmc_defconfig ${DST_DIR}/configs/

#dts
cp -fv ${SRC_DIR}/arch/arm/dts/Makefile ${DST_DIR}/arch/arm/dts/
cp -fv ${SRC_DIR}/arch/arm/dts/imx6ull-14x14-rmk-emmc.dts ${DST_DIR}/arch/arm/dts/
cp -fv ${SRC_DIR}/arch/arm/dts/imx6ull-14x14-rmk.dts ${DST_DIR}/arch/arm/dts/
cp -fv ${SRC_DIR}/arch/arm/dts/imx6ull.dtsi ${DST_DIR}/arch/arm/dts/
cp -fv ${SRC_DIR}/arch/arm/dts/imx6ul-14x14-rmk.dtsi ${DST_DIR}/arch/arm/dts/
cp -fv ${SRC_DIR}/arch/arm/dts/imx6ul-14x14-rmk-u-boot.dtsi ${DST_DIR}/arch/arm/dts/
cp -fv ${SRC_DIR}/arch/arm/dts/imx6ul.dtsi ${DST_DIR}/arch/arm/dts/
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

#build
sudo chmod 777 rmk_build_emmc.sh 
cp -rv ${SRC_DIR}/rmk_build_emmc.sh ${DST_DIR}/

echo "end copy u-boot patch, success!"
sleep 1