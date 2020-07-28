# node_for_imx6ull

编译用于执行node的运行环境，支持imx6ull平台运行。   
## node压缩包使用方法  
```bash
#在开发板创建目录
mkdir -r /usr/node  

#将压缩包上传到/usr/node中，解压压缩包
tar -xvf node.tar.bz2

#修改path，将node环境添加到系统环境变量中
vi /etc/init.d/rcS
#内部添加
    export PATH = $PATH:/usr/node/bin

#更新PATH
source /etc/init.d/rcS
echo $PATH

#查看node是否安装成功
node -v
```  

## node交叉编译      
1)在路径https://nodejs.org/dist/v0.12.9/ 下载node-v0.12.9.tar.gz  
2)确定交叉编译环境arm-linux-gnueabihf-gcc和编译工具python是否支持，且为2.7及以上版本  
3)执行如下流程  
```bash
#创建存放的文件夹, 并将下载的node-v0.12.9.tar.gz上传到文件夹中
mkdir -r /usr/code/node
cd /usr/code/nod
mkdir install

#解压压缩包，进入并执行编译指令
tar -xvf node-v0.12.9.tar.gz

cd node-v0.12.9/

#导入编译环境
export CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ LD=arm-linux-gnueabihf-ld RANLIB=arm-linux-gnueabihf-ranlib AR=arm-linux-gnueabihf-ar CFLAGS=-static CXXFLAGS=-static LDFLAGS=-static

#执行编译选项配置
./configure --prefix=/usr/code/node/install --dest-cpu=arm --dest-os=linux --without-snapshot

#具体的编译指令
make –j4
make install

#打包获得压缩文件
cd ../install
tar -vcjf node.tar.bz2 *
```

