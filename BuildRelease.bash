#嵌入式linux服务器ip地址(需要修改)，需要支持ssh，上传到/tmp/目录下
#REMOTE_IPADDRESS=192.168.2.99

#asio库以压缩包形式进行上传,首次使用需要解压
if [ ! -d "lower_app/manage/lib/asio/" ]; then
    echo "asio not exist, create."
    cd lower_app/manage/lib/
    tar -xvf asio.tar.bz2
    cd ../../../
else
    echo "asio already exist, start build."
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
scp -r app_demo server.tar.bz2 startApp root@$REMOTE_IPADDRESS:/tmp/