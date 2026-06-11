# web_application

web应用服务主要包含后端应用和前端界面实现。其中后端应用基于node.js的服务器应用，前端则是基于vue.js的管理界面。

本节目录如下所示。

- [web_server](#web_server)

## web_server

基于node实现的web服务器。

1. 作为客户端维持和设备管理模块的可靠连接(TCP+心跳包)。
2. 支持服务端允许前端界面获取信息刷新(短连接，请求刷新)。

支持嵌入式平台运行的node可通过在嵌入式平台执行如下命令下载，如我使用的是13.14版本，命令如下.<br/>

```bash
# 下载node文件
wget https://nodejs.org/dist/latest-v13.x/node-v13.14.0-linux-armv7l.tar.gz

# 解压node文件，添加到系统中
tar -xvf node-v13.14.0-linux-armv7l.tar.gz
ln -sf [install]/bin/node /usr/bin/node

# 显示node版本
node -v
```

如果能够正常显示版本信息，表示node安装成功。

## web_pages

前端主要是包含控制和显示的管理界面，采用vue实现.


### 启动服务器
如果在PC平台测试，直接在lower_app/server目录下，执行.<br/>
```bash
node server.js
```
如果显示如下.<br/>
```
node server start!
server listen ip:127.0.0.1, port 15,060
```
则表示启动成功，此时通过在浏览器输入127.0.0.1:15060访问页面.<br/>
在嵌入式linux平台，需要将config.json中的ip地址修改为嵌入式平台ip地址，在通过上诉命令测试.<br/>