
if [ x$1 == x ]; then
    OPTION="all";
else
    OPTION=$1;   
fi

function pull_asio()
{
    ver=1.28.0
    asio_ver=asio-$ver
    asio_gz=$asio_ver.tar.gz
    asio_addr="https://sourceforge.net/projects/asio/files/asio/$ver%20%28Stable%29/$asio_gz"
    asio_out=asio

    if [ ! -d $asio_out/include ]; then

        #从服务器下载asio项目文件
        if [ ! -f $asio_gz ]; then
            echo "download fimrware $asio_gz: $asio_addr"
            wget $asio_addr
        fi

        #解压asio文件, 重命名为asio
        tar -xvf $asio_gz
        mv $asio_ver/* $asio_out/
        rm -rf $asio_ver

    else
        echo "asio alread install, skip this step!"
    fi
}

function tar_and_build_jsoncpp()
{
    JSONCPP_FILE=jsoncpp
    JSONCPP_FILE_TAR=$JSONCPP_FILE.tar.bz2

    if [ ! -d $JSONCPP_FILE/include ]; then
        tar -xvf $JSONCPP_FILE_TAR
    fi

    cd $JSONCPP_FILE/
    make
}

function pull_node()
{
    node_ver=v20.9.0
    node_file=node-${node_ver}-linux-armv7l
    node_tar_file=${node_file}.tar.xz
    node_addr=https://nodejs.org/dist/${node_ver}/${node_tar_file}

    if [ ! -d $APPLICATION_RUN/support/node/bin ]; then
        if [ ! -f ${node_tar_file} ]; then
            echo "download file $node_tar_file:$node_addr"
            wget ${node_addr}
        fi

        tar -xvf ${node_tar_file}
        if [ ! -d $APPLICATION_RUN/support/node ]; then
            mkdir -p $APPLICATION_RUN/support/node
        fi

        mv ${node_file}/* $APPLICATION_RUN/support/node/
        rm -rf ${node_file}
    else
        echo "node already exist, not install!"
    fi
}

echo "run option: $OPTION"
if [ $OPTION == "all" ]; then
    pull_asio
    tar_and_build_jsoncpp
    pull_node
elif [ $OPTION == "asio" ]; then
    pull_asio
fi