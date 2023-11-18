base_build=/home/program/install/arm
run_path=$(pwd)

if [ ! -d ${base_build}/include ]; then
    sudo mkdir -p ${base_build}/include
    sudo mkdir -p ${base_build}/lib
    sudo chmod -Rv 777 ${base_build}
fi

function exit_if_last_error() {
    if [[ $? -ne 0 ]]; then	
		read -p "last command run failed, exit."

        exit 1
    fi
}

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

######################################################################################################
#mosquitto build
######################################################################################################
function build_mosquitto()
{
    #下载cjson库
    #cjson默认使用gcc编译，需要修改支持交叉编译(Makefile)
    if [ ! -f ${base_build}/lib/libcjson.so ]; then
        cjson_ver=cJSON-master
        if [ ! -d ${cjson_ver} ];then
            echo "no exist cjson, need clone from github."
            return -1;
        fi
        cd ${run_path}/${cjson_ver}/
        make clean
        export CC="arm-none-linux-gnueabihf-gcc -std=c89"
        make
        if [ ! -f ${base_build}/lib/libcjson.so ];then
            cp -rv *.so* ${base_build}/lib/
            mkdir -p ${base_build}/include/cjson/
            cp -rv *.h ${base_build}/include/cjson/
        fi
    fi

    mosquitto_ver=mosquitto-2.0.18

    if [ ! -d ${mosquitto_ver}/ ]; then
        wget https://mosquitto.org/files/source/${mosquitto_ver}.tar.gz
        tar -xvf ${mosquitto_ver}.tar.gz
    fi

    cd ${run_path}/${mosquitto_ver}/
    export CC=gcc
    export CXX=g++
    export CPPFLAGS="-I${base_build}/include/ -fPIC"
    export CFLAGS="-I${base_build}/include/ -fPIC"
    export LDFLAGS="-L${base_build}/lib -fPIC -lssl -lcrypto"
    export DESTDIR="${base_build}/mosquitto"
    export CROSS_COMPILE=arm-none-linux-gnueabihf-
    make -j4 && make install
}
build_mosquitto