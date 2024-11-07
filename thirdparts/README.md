# thirdparts

本目录下用于保存第三方构建的应用，对于应用执行，需要的第三方库包含如下。

- openssl, 提供加解密的库，以动态库形式提供。
- zlib, 用于解压缩的库，以动态库形式提供。
- openssh，提供远程访问的服务器。
- cJSON, 基于C语言实现的json解析库，mosquitto编译时需要。
- jsoncpp，基于CPP实现的json解析库。
- mosquitto，mqtt服务和应用接口。
- asio, 提供socket应用的接口。

在交叉编译前，系统已经创建以下变量如下。

- PROGRAM_DOWNLOAD=/home/program/download
- APPLICATION_ROOTFS=${ENV_PATH_ROOT}/rootfs
- SUPPORT_ENV_INSTALL=${PROGRAM_PATH}/install/arm

详细脚本见preThirdParts.sh.
