# u-boot扩展脚本

- u-boot网络启动

```shell
setenv boot_mod net; tftp 80800000 boot.scr; source
```

- u-boot mmc启动

```shell
setenv boot_mod mmc; fatload mmc 1:1 80800000 boot.scr; source
```

- u-boot uImage启动

```shell
setenv boot_mod net; tftp 80800000 uImage; tftp 83000000 imx6ull-14x14-emmc-4.3-800x480-c.dtb; bootm 80800000 - 83000000;
```