# platform

## U-Boot

U-Boot下载适配方法。

```shell
#解压U-Boot
wget https://codeload.github.com/nxp-imx/uboot-imx/zip/refs/heads/lf_v2022.04
unzip lf_v2022.04
cd uboot-imx-lf_v2022.04/

#将uboot-imx-lf_v2022.04-patch目录下内容复制到上述目录下

#执行编译
make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf- mx6ull_14x14_rmk_emmc_defconfig
make -j8 ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf-
```

## Linux

Linux下载适配方法。

```shell
#解压U-Boot
wget https://codeload.github.com/nxp-imx/linux-imx/zip/refs/heads/lf-6.1.y
unzip linux-imx-lf-6.1.y.zip
cd linux-imx-lf-6.1.y/

#将linux-imx-lf-6.1.y-patch目录下内容复制到上述目录下

#执行编译
make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf- imx_rmk_v7_defconfig
make -j8 ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf-
```
