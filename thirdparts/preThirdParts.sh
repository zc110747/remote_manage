#解压和编译openssl
function process_openssl()
{
    openssl_ver=openssl-3.1.4

    #判断是否存在ssl库，存在则不编译
    if [ -f ${GLOBAL_INTALL}/lib/libssl.so ]; then
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
        return 0
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

    #判断是否存在libz库，存在则不编译
    if [ -f ${PROGRAM_DOWNLOAD}/${openssh_ver}/sshd ]; then
        echo "openssh exist, not build!"
        return 0
    fi

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

    cp scp sftp ssh sshd ssh-agent ssh-keygen ssh-keyscan ${APP_ROOTFS}/usr/local/bin/
    cp sftp-server ssh-keysign ${APP_ROOTFS}/usr/libexec/
    cp ssh_config sshd_config ${APP_ROOTFS}/usr/local/etc/
}
process_openssh

#安装node服务
function process_node()
{
    node_ver=v20.9.0
    node_file=node-${node_ver}-linux-armv7l

    if [ -d ${APPLICATION_SYS}/support/node/bin ]; then
        echo "node already install, finished!"
        return 0
    fi

    cd ${PROGRAM_DOWNLOAD}/
    if [ ! -f ${node_file}.tar.xz ]; then
        echo "download file $node_file:$node_addr"
        wget https://nodejs.org/dist/${node_ver}/${node_file}.tar.xz
    fi

    tar -xvf ${node_file}.tar.xz
    mkdir -p ${APPLICATION_SYS}/support/node/
    cp -Rv ${node_file}/* ${APPLICATION_SYS}/support/node/
}
process_node

#安装cJson
function process_cJSON()
{
    #下载cjson库
    #cjson默认使用gcc编译，需要修改支持交叉编译(Makefile)
    if [ ! -f ${GLOBAL_INTALL}/lib/libcjson.so ]; then
       
        cd ${PROGRAM_DOWNLOAD}/
        cjson_ver=cJSON-master
        if [ ! -f ${cjson_ver}.tar.bz2 ];then
            echo "no exist cjson, need clone from github."
            return -1
        fi    
        tar -xvf ${cjson_ver}.tar.bz2
        cd ${cjson_ver}

        make CC="arm-none-linux-gnueabihf-gcc -std=c89"

        cp -rv *.so* ${GLOBAL_INTALL}/lib/
        mkdir -p ${GLOBAL_INTALL}/include/cjson/
        cp -rv *.h ${GLOBAL_INTALL}/include/cjson/
    else
        echo "cJson already install."
    fi
}
process_cJSON

#安装mosquitto
function process_mosquitto()
{
    if [ ! -f ${GLOBAL_INTALL}/lib/libcjson.so ]; then
        echo "libcjson not exist, not compiler mosquitto"
        return -1;
    fi
    
    if [ -f ${GLOBAL_INTALL}/usr/local/sbin/mosquitto ]; then
        echo "mosquitto already install, not compiler!"
        return 0
    fi

    #下载和解压mosquitto
    mosquitto_ver=mosquitto-2.0.18
    cd ${PROGRAM_DOWNLOAD}/
    if [ ! -f ${mosquitto_ver}.tar.gz ]; then
        wget https://mosquitto.org/files/source/${mosquitto_ver}.tar.gz
        sleep 2
    fi
    tar -xvf ${mosquitto_ver}.tar.gz

    #编译mosquitto
    cd ${mosquitto_ver}/
    export CC=gcc
    export CXX=g++
    export CPPFLAGS="-I${GLOBAL_INTALL}/include/ -fPIC"
    export CFLAGS="-I${GLOBAL_INTALL}/include/ -fPIC"
    export LDFLAGS="-L${GLOBAL_INTALL}/lib -fPIC -lssl -lcrypto"
    export DESTDIR="${GLOBAL_INTALL}/"
    export CROSS_COMPILE=arm-none-linux-gnueabihf-
    make -j4 && make install   
}
process_mosquitto

function process_asio()
{
    asio_ver=asio-1.28.0

    if [ ! -d ${APPLICATION_THIRDPARTS}/asio/ ]; then
        tar -xvf ${asio_ver}.tar.gz
        sleep 1

        mv ${asio_ver} asio/
    else
        echo "asio exist, not process!"
    fi
}
process_asio

function process_jsoncpp()
{
    json_ver=jsoncpp

    if [ -f ${APPLICATION_BUILDOUT}/libjsoncpp.a ]; then
        echo "libjsoncpp already build, not process!"
        return 0
    fi

    if [ ! -d ${jsoncpp} ]; then
        tar -xvf ${json_ver}.tar.bz2
    fi

    cd ${json_ver}
    make 
}
process_jsoncpp

function install_library()
{
    sudo mkdir -p ${NFS_PATH}/usr/lib/
    sudo mkdir -p ${NFS_PATH}/usr/include/
    sudo mkdir -p ${NFS_PATH}/usr/local/
    sudo cp -Rv ${GLOBAL_INTALL}/lib/* ${NFS_PATH}/usr/lib/
    sudo cp -Rv ${GLOBAL_INTALL}/usr/local/lib/* ${NFS_PATH}/usr/lib/
    sudo cp -Rv ${GLOBAL_INTALL}/usr/local/include/* ${NFS_PATH}/usr/include/
    sudo cp -Rv ${GLOBAL_INTALL}/include/* ${NFS_PATH}/usr/include/
    sudo cp -Rv ${GLOBAL_INTALL}/usr/local/* ${NFS_PATH}/usr/local/
}
install_library