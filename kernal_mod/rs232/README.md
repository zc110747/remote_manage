# remote_manage

UART驱动的实现在 内核源码/drivers/tty/serial/imx.c中
这部分由Motorola/Freescale官方实现


对于设备树的修改:
1.检索pinctrl_uart1,在后面添加引脚配置信息pinctrl_uart3
pinctrl_uart3: uart3grp {
    fsl,pins = <
        MX6UL_PAD_UART3_RX_DATA__UART3_DCE_RX	0x1b0b1
        MX6UL_PAD_UART3_TX_DATA__UART3_DCE_TX	0x1b0b1
    >;
};
2.检索uart2,参考在后面增加uart3节点
&uart3 {
	pinctrl-names = "default";
	pinctrl-0 = <&pinctrl_uart3>;
	status = "okay";
};
重新编译设备树，更新即可。