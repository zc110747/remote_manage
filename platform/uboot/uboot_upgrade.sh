
PLATFORM_BOOT=${ENV_PATH_ROOT}/platform/uboot

#configs
cp -f ${ENV_BOOT_DIR}/configs/mx6ull_14x14_hello_emmc_defconfig ${PLATFORM_BOOT}/configs/

#dts
cp -f ${ENV_BOOT_DIR}/arch/arm/dts/imx6ull-14x14-hello-emmc.dts ${PLATFORM_BOOT}/arch/arm/dts/
cp -f ${ENV_BOOT_DIR}/arch/arm/dts/imx6ull-14x14-hello.dts ${PLATFORM_BOOT}/arch/arm/dts/
cp -f ${ENV_BOOT_DIR}/arch/arm/dts/imx6ull.dtsi ${PLATFORM_BOOT}/arch/arm/dts/
cp -f ${ENV_BOOT_DIR}/arch/arm/dts/imx6ul-14x14-hello.dtsi ${PLATFORM_BOOT}/arch/arm/dts/
cp -f ${ENV_BOOT_DIR}/arch/arm/dts/imx6ul-14x14-hello-u-boot.dtsi ${PLATFORM_BOOT}/arch/arm/dts/
cp -f ${ENV_BOOT_DIR}/arch/arm/dts/imx6ul.dtsi ${PLATFORM_BOOT}/arch/arm/dts/
cp -f ${ENV_BOOT_DIR}/arch/arm/mach-imx/mx6/Kconfig ${PLATFORM_BOOT}/arch/arm/mach-imx/mx6/

#drivers
cp -f ${ENV_BOOT_DIR}/drivers/net/phy/phy.c ${PLATFORM_BOOT}/drivers/net/phy/

#include 
cp -f ${ENV_BOOT_DIR}/include/configs/mx6ullhello.h ${PLATFORM_BOOT}/include/configs/

#board
cp -f ${ENV_BOOT_DIR}/board/freescale/mx6ullhello/* ${PLATFORM_BOOT}/board/freescale/mx6ullhello/