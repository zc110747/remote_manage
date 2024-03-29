if [ ${FIRMWARE_CURRENT_PLATFORMS} == "ARM" ]; then
    export cross_compiler=arm-none-linux-gnueabihf
else
    export cross_compiler=aarch64-none-linux-gnu
fi

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

    if [ -f Makefile ]; then
        make clean
    fi

    #编译openssl
    export CC=gcc
    export CXX=g++

    ./config --cross-compile-prefix=${cross_compiler}- no-asm --prefix=${GLOBAL_INTALL} linux-armv4

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

    if [ -f Makefile ]; then
        make clean
    fi

    #编译zlib
    export CHOST=${cross_compiler}
    ./configure --prefix=${GLOBAL_INTALL} 
    make -j4 && make install
}
process_zlib

#编译和安装openssh
function process_openssh()
{
    openssh_ver=openssh-9.5p1

    echo "start openssh software build!"
    
    #判断是否存在libz库，存在则不编译
    if [ -f ${NFS_PATH}/usr/local/bin/sshd ]; then
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
    if [ -f Makefile ]; then
        make clean
    fi

    #编译openssh
    ./configure --host=${cross_compiler} --with-libs --with-zlib=${GLOBAL_INTALL} \
                --with-ssl-dir=${GLOBAL_INTALL} --disable-etc-default-login \
                --prefix=${GLOBAL_INTALL}

    make -j4

    if [ ! -d ${NFS_PATH}/usr/local/bin/ ]; then
        sudo mkdir -m 755 -p ${NFS_PATH}/usr/local/bin/
    fi

    if [ ! -d ${NFS_PATH}/usr/libexec/ ]; then
        sudo mkdir -m 755 -p ${NFS_PATH}/usr/libexec/
    fi

    if [ ! -d ${NFS_PATH}/usr/local/etc/ ]; then
        sudo mkdir -m 755 -p ${NFS_PATH}/usr/local/etc/
    fi

    sudo cp scp sftp ssh sshd ssh-agent ssh-keygen ssh-keyscan ${NFS_PATH}/usr/local/bin/
    sudo cp sftp-server ssh-keysign ${NFS_PATH}/usr/libexec/
    sudo cp ssh_config sshd_config ${NFS_PATH}/usr/local/etc/
}
process_openssh

#安装node服务
function process_node()
{
    node_ver=v20.10.0

    if [ ${FIRMWARE_CURRENT_PLATFORMS} == "ARM" ]; then
        node_file=node-${node_ver}-linux-armv7l
    else
        node_file=node-${node_ver}-linux-arm64
    fi

    if [ -d ${NFS_PATH}/usr/bin/node/bin ]; then
        echo "node already install, finished!"
        return 0
    fi

    cd ${PROGRAM_DOWNLOAD}/
    if [ ! -f ${node_file}.tar.xz ]; then
        echo "download file $node_file:$node_addr"
        wget https://nodejs.org/dist/${node_ver}/${node_file}.tar.xz
    fi

    tar -xvf ${node_file}.tar.xz
    sudo mkdir -m 777 -p ${NFS_PATH}/usr/bin/node
    cp -Rv ${node_file}/* ${NFS_PATH}/usr/bin/node/
}
process_node

#安装cJson
function process_cJSON()
{
    #下载cjson库
    #cjson默认使用gcc编译，需要修改支持交叉编译(Makefile)
    if [ ! -f ${GLOBAL_INTALL}/lib/libcjson.so ]; then
       
        cd ${APPLICATION_THIRDPARTS}/cJSON-master/
        
        if [ -f Makefile ]; then
            make clean
        fi

        make CC="${cross_compiler}-gcc -std=c89"

        cp -rv *.so* ${GLOBAL_INTALL}/lib/
        mkdir -p ${GLOBAL_INTALL}/include/cjson/
        cp -rv *.h ${GLOBAL_INTALL}/include/cjson/

        make clean
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

    if [ -f Makefile ]; then
        make clean
    fi

    export CC=gcc
    export CXX=g++
    export CPPFLAGS="-I${GLOBAL_INTALL}/include/ -fPIC"
    export CFLAGS="-I${GLOBAL_INTALL}/include/ -fPIC"
    export LDFLAGS="-L${GLOBAL_INTALL}/lib -fPIC -lssl -lcrypto"
    export DESTDIR="${GLOBAL_INTALL}/"
    export CROSS_COMPILE=${cross_compiler}-
    make -j4 && make install   
}
process_mosquitto

function process_asio()
{
    asio_ver=asio-1.28.0

    cd ${APPLICATION_THIRDPARTS}/
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

    cd ${APPLICATION_THIRDPARTS}/
    if [ ! -d ${json_ver}/ ]; then
        tar -xvf ${json_ver}.tar.bz2
    fi

    cd ${json_ver}/
    make 
}
process_jsoncpp

function install_library()
{
    sudo cp -Rv ${GLOBAL_INTALL}/usr/local/lib/* ${GLOBAL_INTALL}/lib/

    sudo mkdir -p ${NFS_PATH}/usr/lib/
    sudo mkdir -p ${NFS_PATH}/usr/include/
    sudo mkdir -p ${NFS_PATH}/usr/local/
    sudo cp -Rv ${GLOBAL_INTALL}/lib/* ${NFS_PATH}/usr/lib/
    sudo cp -Rv ${GLOBAL_INTALL}/usr/local/lib/* ${NFS_PATH}/usr/lib/
    sudo cp -Rv ${GLOBAL_INTALL}/usr/local/include/* ${NFS_PATH}/usr/include/
    sudo cp -Rv ${GLOBAL_INTALL}/include/* ${NFS_PATH}/usr/include/
    sudo cp -Rv ${GLOBAL_INTALL}/usr/local/* ${NFS_PATH}/usr/local/
    sudo cp -Rv ${GLOBAL_INTALL}/etc/* ${NFS_PATH}/etc/
}
install_library