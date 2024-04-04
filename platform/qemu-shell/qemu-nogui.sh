kernel_file="${SUPPORT_ENV_KERNEL_DIR}"/arch/arm/boot/zImage
dtb_file="${SUPPORT_ENV_KERNEL_DIR}"/arch/arm/boot/dts/imx6ull-qemu.dtb
rootfs_file="${PROGRAM_IMG}"/arm-ubuntu.img

#nogui
qemu-system-arm -M mcimx6ul-evk -m 512M -kernel "${kernel_file}" \
    -dtb "${dtb_file}"  \
    -nographic -serial mon:stdio \
    -drive  file="${rootfs_file}",format=raw,id=mysdcard -device sd-card,drive=mysdcard \
    -append "console=ttymxc0,115200 rootfstype=ext4 root=/dev/mmcblk1 rw rootwait init=/sbin/init loglevel=8"