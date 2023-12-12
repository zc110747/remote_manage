#解压和编译openssl
function process_openssl()
{
    openssl_ver=openssl-3.1.4

    #判断是否存在ssl库，存在则不编译
    if [ -f ${GLOBAL_INTALL}/lib/libssl.so ]; then
        echo "libssl.so exist, not build!"
        return 0;
    fi

    #下载openssl
    cd ${PROGRAM_DOWNLOAD}/
    if [ ! -f ${openssl_ver}.tar.gz ]; then
        wget https://www.openssl.org/source/${openssl_ver}.tar.gz
    fi

    #解压openssl
    if [ ! -d ${openssl_ver} ]; then
        tar -xvf ${openssl_ver}.tar.gz
    fi

    cd ${openssl_ver}/

    #编译openssl
    export CC=gcc
    export CXX=g++

    ./config --cross-compile-prefix=arm-none-linux-gnueabihf- no-asm --prefix=${GLOBAL_INTALL} linux-armv4

    make -j2 && make install
}
process_openssl

#解压和编译zlib
function process_zlib()
{
    zlib_ver=zlib-1.3

    #判断是否存在libz库，存在则不编译
    if [ -f ${GLOBAL_INTALL}/lib/libz.so ]; then
        echo "libz.so exist, not build!"
        return 0;
    fi

    #下载zlib
    cd ${PROGRAM_DOWNLOAD}/
    if [ ! -f ${zlib_ver}.tar.gz ]; then
        wget http://www.zlib.net/${zlib_ver}.tar.gz
    fi

    #解压zlib
    if [ ! -d ${zlib_ver} ]; then
        tar -xvf ${zlib_ver}.tar.gz
    fi

    cd ${zlib_ver}/

    #编译zlib
    export CHOST=arm-none-linux-gnueabihf
    ./configure --prefix=${GLOBAL_INTALL}
    make -j4 && make install
}
process_zlib

#编译和安装openssh
function process_openssh()
{
    openssh_ver=openssh-9.5p1

    # #判断是否存在libz库，存在则不编译
    # if [ -f ${GLOBAL_INTALL}/lib/libz.so ]; then
    #     echo "libz.so exist, not build!"
    #     return 0;
    # fi

    #下载openssh
    cd ${PROGRAM_DOWNLOAD}/
    if [ ! -f ${openssh_ver}.tar.gz ]; then
        wget https://mirrors.tuna.tsinghua.edu.cn/OpenBSD/OpenSSH/portable/${openssh_ver}.tar.gz
    fi

    #解压openssh
    if [ ! -d ${openssh_ver} ]; then
        tar -xvf ${openssh_ver}.tar.gz
    fi
    cd ${openssh_ver}/

    #编译openssh
    # ./configure --host=arm-none-linux-gnueabihf --with-libs --with-zlib=${GLOBAL_INTALL} \
    #             --with-ssl-dir=${GLOBAL_INTALL} --disable-etc-default-login \
    #             --prefix=${GLOBAL_INTALL}

    # make -j2

    cp scp sftp ssh sshd ssh-agent ssh-keygen ssh-keyscan ${APP_ROOTFS}/usr/local/bin/
    cp sftp-server ssh-keysign ${APP_ROOTFS}/usr/libexec/
    cp ssh_config sshd_config ${APP_ROOTFS}/usr/local/etc/
}
process_openssh

function install_library()
{
    
}
install_library