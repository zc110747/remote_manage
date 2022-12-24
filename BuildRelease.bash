#嵌入式linux服务器ip地址(需要修改)，需要支持ssh，上传到/tmp/目录下
#REMOTE_IPADDRESS=192.168.2.99
KERNELDIR=/home/center/application/Project/linux/

#asio库以压缩包形式进行上传,首次使用需要解压
if [ ! -d "lower_app/manage/lib/asio/" ]; then
    echo "asio not exist, create."
    cd lower_app/manage/lib/
    tar -xvf asio.tar.bz2
    cd ../../../
else
    echo "asio already exist, start build."
fi

#compiler linux driver
#compiler 
if [ -d "$KERNELDIR" ]; then
    echo "start bulid kernal driver"
    cd kernal_mod/led/
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
    mv kernal_led.ko ../../demo/driver/
    make clean
    cd ../../

    cd kernal_mod/beep/
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
    mv kernal_beep.ko ../../demo/driver/
    make clean
    cd ../../

    cd kernal_mod/i2c_ap/
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
    mv kernal_i2c_ap.ko ../../demo/driver/
    make clean
    cd ../../

    cd kernal_mod/spi_icm/
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
    mv kernal_spi_icm.ko ../../demo/driver/
    make clean
    cd ../../

    cd kernal_mod/key/
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
    mv kernal_key.ko ../../demo/driver/
    make clean
    cd ../../
else
    echo "without linux kernal, not build kernal module"
fi

#覆盖配置文件
cp -f demo/config.json lower_app/manage/config.json
cp -f demo/config.json lower_app/server/config.json

#编译程序, 并拷贝到输出目录下
#如果报错, 需要使用较新的编译器, 参考document/构建Linux编程环境.md的说明
cd lower_app/manage/
make
cd ../../
cp lower_app/Executables/app_demo demo/

#打包服务器程序(包含node的后台和前端页面),拷贝到输出目录
cd lower_app/
tar -vcjf server.tar.bz2 server/ 
cd ../
mv lower_app/server.tar.bz2 demo/

#将打包后的系统通过ssh提交到服务器中
cd demo/
scp -r app_demo server.tar.bz2 startApp driver/ root@$REMOTE_IPADDRESS:/tmp/
