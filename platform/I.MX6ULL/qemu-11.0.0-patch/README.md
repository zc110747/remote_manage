# qemu添加patch支持器件的方法

```shell
# 编译选项
./configure --prefix=$PWD/ --target-list="arm-softmmu arm-linux-user" --enable-debug --enable-sdl --enable-slirp --enable-kvm --enable-tools --disable-curl --disable-coreaudio --disable-pa --disable-sdl --disable-sndio 
make -j6

# 运行命令
kernel_file="${BUILD_TFTP_PATH}/zImage"
dtb_file="${BUILD_TFTP_PATH}/imx6ull-qemu.dtb"

if [ "${ROOTFS_RUN}" == "buildroot" ]; then
    rootfs_file="${SUPPORT_ENV_PACKAGE}/buildroot.img"
elif [ "${ROOTFS_RUN}" == "debian" ]; then
    rootfs_file="${SUPPORT_ENV_PACKAGE}/debian.img"
else
    rootfs_file="${SUPPORT_ENV_PACKAGE}/ubuntu.img"
fi

# nogui
# qemu_tools=qemu-system-arm
qemu_tools=${PROGRAM_SDK_PATH}/qemu/qemu-11.0.0/build/qemu-system-arm
${qemu_tools} -M mcimx6ul-evk -m 512M -kernel "${kernel_file}" \
    -dtb "${dtb_file}"  \
    -nographic -serial mon:stdio \
    -drive  file="${rootfs_file}",format=raw,id=mysdcard -device sd-card,drive=mysdcard \
    -append "console=ttymxc0,115200 rootfstype=ext4 root=/dev/mmcblk1 rw rootwait init=/sbin/init loglevel=8"
    -monitor telnet:127.0.0.1:4444,server,nowait \
    -nic user \
    -nic user,hostfwd=tcp::2222-:22

telnet 127.0.0.1 4444
(qemu) gpio-set 0 0
(qemu) gpio-set 0 1

# 查看qtree支持
qemu-system-arm -M mcimx6ul-evk -monitor stdio

(qemu)info qtree

# 修复buildroot.img ext4文件错误
./e2fsck/e2fsck -fy /home/freedom/sdk/arm/package/buildroot.img

# e2fsck的编译方法
sudo apt update

sudo apt install -y \
    build-essential \
    pkg-config \
    uuid-dev \
    libblkid-dev 

wget https://mirrors.edge.kernel.org/pub/linux/kernel/people/tytso/e2fsprogs/v1.47.2/e2fsprogs-1.47.2.tar.gz

tar xf e2fsprogs-1.47.2.tar.gz
cd e2fsprogs-1.47.2

./configure
make -j$(nproc)

sudo ./e2fsck/e2fsck -fy buildroot.img

ifconfig eth0 up
udhcpc -i eth0

qemu-system-arm -M mcimx6ul-evk -netdev help

# 开启网络
vi /etc/network/interfaces

auto eth1
iface eth1 inet dhcp
```