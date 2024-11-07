################################################################################
#
# This file is used to Install Debian System In address
# 
#
################################################################################

# define global environment
if [ "${FIRMWARE_CURRENT_PLATFORMS}" == "ARM" ]; then
    chip_arch=armhf
    qemu_arch=arm
else
    chip_arch=arm64
    qemu_arch=aarch64
fi
opt_debian_ver=bookworm

# install support
sudo apt-get install debootstrap debian-archive-keyring qemu-user-static -y

# functional region
run_as_client() {
    $@ > /dev/null 2>&1
}

exit_if_last_error() {
    if [[ $? -ne 0 ]]; then
		read -p "shell faild, wheather exit?(y/n, default is y)" exit_val

		if [ -z ${exit_val} ] || [ ${exit_val} == 'y' ]; then
        	exit 1
		fi
    fi
}

mount_chroot()
{
    sudo mount -t proc chproc "${SUPPORT_FS_DEBIAN_DIR}"/proc
    sudo mount -t sysfs chsys "${SUPPORT_FS_DEBIAN_DIR}"/sys
    sudo mount -t devtmpfs chdev "${SUPPORT_FS_DEBIAN_DIR}"/dev || sudo mount --bind /dev "${SUPPORT_FS_DEBIAN_DIR}"/dev
    sudo mount -t devpts chpts "${SUPPORT_FS_DEBIAN_DIR}"/dev/pts
}

umount_chroot()
{
    while grep -Eq "${SUPPORT_FS_DEBIAN_DIR}.*(dev|proc|sys)" /proc/mounts
    do
        sudo umount -l --recursive "${SUPPORT_FS_DEBIAN_DIR}"/dev >/dev/null 2>&1
        sudo umount -l "${SUPPORT_FS_DEBIAN_DIR}"/proc >/dev/null 2>&1
        sudo umount -l "${SUPPORT_FS_DEBIAN_DIR}"/sys >/dev/null 2>&1
        sleep 5
    done
}

# download debian from 
echo "Start Install Debian, platform:${greenText}${chip_arch}, ${qemu_arch}${defText}"
echo "Address:${greenText}${SUPPORT_FS_DEBIAN_DIR}${defText}"

if [ ! -d "${SUPPORT_FS_DEBIAN_DIR}"/bin/ ]; then
    sudo debootstrap --foreign --verbose --arch=${chip_arch} ${opt_debian_ver} "${SUPPORT_FS_DEBIAN_DIR}" https://mirrors.tuna.tsinghua.edu.cn/debian/
else
    echo "Debian already Install."
    exit 0
fi
#exit_if_last_error

sudo chmod -Rv 777 "${SUPPORT_FS_DEBIAN_DIR}"/usr/

cp /usr/bin/qemu-${qemu_arch}-static "${SUPPORT_FS_DEBIAN_DIR}"/usr/bin/
chmod +x "${SUPPORT_FS_DEBIAN_DIR}"/usr/bin/qemu-${qemu_arch}-static

#基于debootstrap完成后续安装
cd "${SUPPORT_FS_DEBIAN_DIR}"/

mount_chroot
LC_ALL=C LANGUAGE=C LANG=C sudo chroot "${SUPPORT_FS_DEBIAN_DIR}" /debootstrap/debootstrap --second-stage --verbose
LC_ALL=C LANGUAGE=C LANG=C sudo chroot "${SUPPORT_FS_DEBIAN_DIR}" apt-get install vim libatomic1 -y
umount_chroot
