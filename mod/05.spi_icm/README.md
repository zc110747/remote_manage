# remote_manage

SPI驱动的实现包含三部分
1.设备树的添加，修改
注:设备树文件为kernal/dts/imx6ull-14x14-evk.dts
添加设备树与SPI3相关的引脚说明到iomuxc/imx6ul-evk下
pinctrl_ecspi3: ecspi3grp {
        fsl,pins = <
                MX6UL_PAD_UART2_RTS_B__ECSPI3_MISO        0x100b1  /* MISO*/
                MX6UL_PAD_UART2_CTS_B__ECSPI3_MOSI        0x100b1  /* MOSI*/
                MX6UL_PAD_UART2_RX_DATA__ECSPI3_SCLK      0x100b1  /* CLK*/
                MX6UL_PAD_UART2_TX_DATA__GPIO1_IO20       0x100b0  /* CS*/
        >;
};  
在最外面添加或者完善节点ecspi3
&ecspi3 {
        fsl,spi-num-chipselects = <1>;
        cs-gpio = <&gpio1 20 GPIO_ACTIVE_LOW>;
        pinctrl-names = "default";
        pinctrl-0 = <&pinctrl_ecspi3>;
        status = "okay";

	spidev: icm20608@0 {
	compatible = "alientek,icm20608";
        spi-max-frequency = <8000000>;
        reg = <0>;
    };
};
重新编译设备树，即完成了设备树的修改。
执行make dtbs，获得修改后的设备树

2. 在kernal_mod/spi_icm/路径下执行make指令，获取模块kernal_spi_icm.ko(firmware下有已经编译完成的固件)

3. 将模块传输到嵌入式Linux平台，执行insmod kernal_spi_icm.ko指令，此时在/dev/路径下就添加了icm20608设备，后续即可实现传感器应用

