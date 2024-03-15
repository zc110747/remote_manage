if [ ${FIRMWARE_CURRENT_PLATFORMS} == "ARM" ]; then
    export cross_compiler=arm-none-linux-gnueabihf
else
    export cross_compiler=aarch64-none-linux-gnu
fi
export CPU_NUM=8

#解压和编译openssl
function process_openssl()
{
    openssl_ver=openssl-3.1.4
  
    #判断是否存在ssl库，存在则不编译
    if [ -f "${PROGRAM_INSTALL}"/lib/libssl.so ]; then
        echo "libssl.so exist, not compiler!"
        return 0
    fi

    #下载openssl
    cd "${PROGRAM_DOWNLOAD}"/
    if [ ! -f "${openssl_ver}".tar.gz ]; then
        wget https://www.openssl.org/source/"${openssl_ver}".tar.gz
    fi

    #解压openssl
    if [ ! -d "${openssl_ver}" ]; then
        tar -xvf "${openssl_ver}".tar.gz
    fi

    cd "${openssl_ver}"/

    if [ -f Makefile ]; then
        make clean
    fi

    #编译openssl
    export CC=gcc
    export CXX=g++

    ./config --cross-compile-prefix="${cross_compiler}"- no-asm --prefix="${PROGRAM_INSTALL}" linux-armv4

    make -j${CPU_NUM} && make install
}

#解压和编译zlib
function process_zlib()
{
    zlib_ver=zlib-1.3.1

    #判断是否存在libz库，存在则不编译
    if [ -f ${PROGRAM_INSTALL}/lib/libz.so ]; then
        echo "libz.so exist, not compiler!"
        return 0
    fi

    #下载zlib
    cd ${PROGRAM_DOWNLOAD}/
    if [ ! -f ${zlib_ver}.tar.gz ]; then
        wget https://zlib.net/${zlib_ver}.tar.gz
    fi

    #解压zlib
    if [ ! -d ${zlib_ver} ]; then
        tar -xvf ${zlib_ver}.tar.gz
    fi

    cd ${zlib_ver}/

    if [ -f Makefile ]; then
        make clean
    fi

    #编译zlib
    export CHOST=${cross_compiler}
    ./configure --prefix=${PROGRAM_INSTALL} 
    make -j${CPU_NUM} && make install
}

#编译和安装openssh
function process_openssh()
{
    openssh_ver=openssh-9.5p1
    
    #判断是否存在libz库，存在则不编译
    if [ -f ${PROGRAM_INSTALL}/usr/local/bin/sshd ]; then
        echo "openssh exist, not compiler!"
        return 0
    fi

    echo "start openssh software compiler!"

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
    if [ -f Makefile ]; then
        make clean
    fi

    #编译openssh
    ./configure --host=${cross_compiler} --with-libs --with-zlib=${PROGRAM_INSTALL} \
                --with-ssl-dir=${PROGRAM_INSTALL} --disable-etc-default-login \
                --prefix=${PROGRAM_INSTALL} --disable-strip

    make -j${CPU_NUM}

    mkdir -m 755 -p ${PROGRAM_INSTALL}/usr/local/bin/
    mkdir -m 755 -p ${PROGRAM_INSTALL}/usr/libexec/
    mkdir -m 755 -p ${PROGRAM_INSTALL}/usr/local/etc/

    cp scp sftp ssh sshd ssh-agent ssh-keygen ssh-keyscan ${PROGRAM_INSTALL}/usr/local/bin/
    cp sftp-server ssh-keysign ${PROGRAM_INSTALL}/usr/libexec/
    cp ssh_config sshd_config ${PROGRAM_INSTALL}/usr/local/etc/
}

#安装node服务
function process_node()
{
    node_ver=v20.10.0

    if [ ${FIRMWARE_CURRENT_PLATFORMS} == "ARM" ]; then
        node_file=node-${node_ver}-linux-armv7l
    else
        node_file=node-${node_ver}-linux-arm64
    fi

    if [ -d ${PROGRAM_INSTALL}/usr/bin/node/bin ]; then
        echo "node already exist, not install!"
        return 0
    fi

    cd ${PROGRAM_DOWNLOAD}/
    if [ ! -f ${node_file}.tar.xz ]; then
        echo "download file $node_file:$node_addr"
        wget https://nodejs.org/dist/${node_ver}/${node_file}.tar.xz
    fi

    tar -xvf ${node_file}.tar.xz
    mkdir -m 777 -p ${PROGRAM_INSTALL}/usr/bin/node
    cp -Rv ${node_file}/* ${PROGRAM_INSTALL}/usr/bin/node/
}

#安装cJson
function process_cJSON()
{
    #下载cjson库
    #cjson默认使用gcc编译，需要修改支持交叉编译(Makefile)
    if [ ! -f ${PROGRAM_INSTALL}/lib/libcjson.so ]; then
       
        cd ${APPLICATION_THIRDPARTS}/cJSON-master/
        
        if [ -f Makefile ]; then
            make clean
        fi

        make CC="${cross_compiler}-gcc -std=c89" -j${CPU_NUM}

        cp -rv *.so* ${PROGRAM_INSTALL}/lib/
        mkdir -p ${PROGRAM_INSTALL}/include/cjson/
        cp -rv *.h ${PROGRAM_INSTALL}/include/cjson/

        make clean
    else
        echo "cJson already exist, not install."
    fi
}

#安装mosquitto
function process_mosquitto()
{
    if [ ! -f ${PROGRAM_INSTALL}/lib/libcjson.so ]; then
        echo "libcjson not exist, not compiler mosquitto"
        return -1;
    fi
    
    if [ -f ${PROGRAM_INSTALL}/usr/local/sbin/mosquitto ]; then
        echo "mosquitto already install, not compiler!"
        return 0
    fi

    mkdir -m 777 -p ${PROGRAM_INSTALL}/usr/local/sbin/
    mkdir -m 777 -p ${PROGRAM_INSTALL}/usr/local/lib/

    下载和解压mosquitto
    mosquitto_ver=mosquitto-2.0.18
    cd ${PROGRAM_DOWNLOAD}/
    if [ ! -f ${mosquitto_ver}.tar.gz ]; then
        wget https://mosquitto.org/files/source/${mosquitto_ver}.tar.gz
        sleep 2
    fi
    tar -xvf ${mosquitto_ver}.tar.gz

    #编译mosquitto
    cd ${mosquitto_ver}/

    if [ -f Makefile ]; then
        make clean
    fi

    export CC=gcc
    export CXX=g++
    export CPPFLAGS="-I${PROGRAM_INSTALL}/include/ -fPIC"
    export CFLAGS="-I${PROGRAM_INSTALL}/include/ -fPIC"
    export LDFLAGS="-L${PROGRAM_INSTALL}/lib -fPIC -lssl -lcrypto"
    export DESTDIR="${PROGRAM_INSTALL}"
    export CROSS_COMPILE=${cross_compiler}-
    make -j${CPU_NUM}
    make install
}

function process_asio()
{
    asio_ver=asio-1.28.0

    cd ${APPLICATION_THIRDPARTS}/
    if [ ! -d ${APPLICATION_THIRDPARTS}/asio/ ]; then
        tar -xvf ${asio_ver}.tar.gz
        sleep 1

        mv ${asio_ver} asio/
    else
        echo "asio already exist, not install!"
    fi
}

function process_jsoncpp()
{
    json_ver=jsoncpp

    if [ -f ${APPLICATION_BUILDOUT}/libjsoncpp.a ]; then
        echo "libjsoncpp already build, not install!"
        return 0
    fi

    cd ${APPLICATION_THIRDPARTS}/
    if [ ! -d ${json_ver}/ ]; then
        tar -xvf ${json_ver}.tar.bz2
    fi

    cd ${json_ver}/
    make -j${CPU_NUM}
}

function install_all()
{
    process_openssl

    process_zlib

    process_openssh

    process_node

    process_cJSON

    process_mosquitto

    process_asio
    
    process_jsoncpp
}
install_all