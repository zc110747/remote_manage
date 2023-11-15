
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

echo "run option: $OPTION"
if [ $OPTION == "all" ]; then
    pull_asio
    tar_and_build_jsoncpp
elif [ $OPTION == "asio" ]; then
    pull_asio
fi