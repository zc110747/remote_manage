######################################################################################################
#zlib build
######################################################################################################
function build_zlib()
{
    zlib_ver=zlib-1.3
    lib_build=${base_build}/zlib

    if [ -f $lib_build/lib/libz.so ]; then
        echo "zlib already build, not need build"
        return 0
    fi

    #下载zlib
    if [ ! -d ${zlib_ver}/ ]; then 
        wget http://www.zlib.net/${zlib_ver}.tar.gz
        tar -xvf ${zlib_ver}.tar.gz
    fi
    
    cd ${run_path}/${zlib_ver}/
    make clean

    export CHOST=arm-none-linux-gnueabihf
    ./configure --prefix=${lib_build}
    make -j4 && make install

    exit_if_last_error
}
build_zlib

######################################################################################################
#openssl build
######################################################################################################
function build_openssl()
{
    openssl_ver=openssl-3.1.4
    lib_build=${base_build}/openssl

    if [ -f $lib_build/lib/libssl.so ]; then
        echo "libssl already build, not need build!"
        return 0
    fi

    if [ ! -d ${openssl_ver}/ ]; then 
        if [ ! -f ${openssl_ver}.tar.gz ]; then
            echo "${openssl_ver}.tar.gz"
            wget https://www.openssl.org/source/${openssl_ver}.tar.gz
        fi
        tar -xvf ${openssl_ver}.tar.gz
    fi

    cd ${run_path}/${openssl_ver}/

    if [ ! -f Makefile ]; then
        export CC=gcc
        export CXX=g++

        ./config --cross-compile-prefix=arm-none-linux-gnueabihf- no-asm --prefix=${lib_build} linux-armv4
    fi

    make -j2 && make install

    exit_if_last_error
}
build_openssl

#安装openssl库    
if [ ! -f ${base_build}/lib/libssl.so ];then
    cp -rv ${base_build}/openssl/lib/* ${base_build}/lib/
    cp -rv ${base_build}/openssl/include/* ${base_build}/include/
    echo "need support libssl, address:${base_build}/lib/libssl.so"
fi

######################################################################################################
#openssh build
######################################################################################################
function build_openssh()
{   
    openssh_ver=openssh-9.5p1

    if [ -f ${openssh_ver}/sshd ]; then
        echo "openssh already build, return!"
        return 0
    fi

    if [ ! -d ${openssh_ver}/ ]; then
        wget https://mirrors.tuna.tsinghua.edu.cn/OpenBSD/OpenSSH/portable/${openssh_ver}.tar.gz
        tar -xvf ${openssh_ver}.tar.gz
    fi

    cd ${run_path}/${openssh_ver}/
    ./configure --host=arm-none-linux-gnueabihf --with-libs --with-zlib=${base_build}/zlib \
                --with-ssl-dir=${base_build} --disable-etc-default-login \
                --prefix=${base_build}/openssh

    make -j2

    exit_if_last_error
}
build_openssh