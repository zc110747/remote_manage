# thirdparts

本目录下用于保存第三方构建的应用，对于应用执行，需要的第三方库包含如下。

- openssl, 提供加解密的库，以动态库形式提供。
- zlib, 用于解压缩的库，以动态库形式提供。
- openssh，提供远程访问的服务器。
- cJSON, 基于C语言实现的json解析库，mosquitto编译时需要。
- asio, 提供socket应用的接口。
- jsoncpp，基于CPP实现的json解析库。
- mosquitto，mqtt服务和应用接口。

## cross_compiler

在交叉编译前，系统已经创建些变量如下。

- PROGRAM_DOWNLOAD=/home/program/download
- APP_ROOTFS=${ENV_PATH_ROOT}/rootfs
- APP_ROOTFS_LIB=${APP_ROOTFS}/lib
- GLOBAL_INTALL=${PROGRAM_PATH}/install/arm

### openssl

```shell

#编译前可将openssl-3.1.4.tar.gz放置到/home/program/download
function process_openssl(void)
{
    openssl_ver=openssl-3.1.4

    #判断是否存在ssl库，存在则不编译
    if [ -f ${APP_ROOTFS_LIB}/libssl.so ]; then
        echo "libssl.so exist, not build!"
        return 0
    fi

    #下载openssl
    cd ${PROGRAM_DOWNLOAD}/
    if [ ! -f ${openssl_ver}.tar.gz ]; then
        wget https://www.openssl.org/source/${openssl_ver}.tar.gz
    fi

    #解压openssl
    if [ ! -d ${openssl_ver} ]; then
        tar -xvf ${openssl_ver}.tar.gz
        cd ${openssl_ver}/
    fi

    #编译openssl
    export CC=gcc
    export CXX=g++

    ./config --cross-compile-prefix=arm-none-linux-gnueabihf- no-asm --prefix=${GLOBAL_INTALL} linux-armv4

    make -j2 && make install
}
process_openssl
```

### zlib

```shell

#解压和编译zlib
function process_zlib()
{
    zlib_ver=zlib-1.3

    #判断是否存在libz库，存在则不编译
    if [ -f ${GLOBAL_INTALL}/lib/libz.so ]; then
        echo "libz.so exist, not build!"
        return 0;
    fi

    #下载openzlib
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
```

### openssh

```shell
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
    ./configure --host=arm-none-linux-gnueabihf --with-libs --with-zlib=${GLOBAL_INTALL} \
                --with-ssl-dir=${GLOBAL_INTALL} --disable-etc-default-login \
                --prefix=${GLOBAL_INTALL}

    make -j2   
    make install
}
process_openssh
```
