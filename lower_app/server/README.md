# remote_manage

## web服务应用的实现  
### 安装node到嵌入式平台
web应用服务主要包含后端应用和前端界面实现.  
后端应用基于node.js的服务器应用，由两部分组成.  
1.作为客户端维持和设备管理模块的可靠连接(TCP+心跳包).  
2.支持服务端允许前端界面获取信息刷新(短连接，请求刷新).  
前端主要是包含控制和显示的管理界面，计划采用vue实现.  

支持嵌入式平台运行的node可通过在嵌入式平台执行如下命令下载，如我使用的是13.14版本，命令如下.  
```bash
wget https://nodejs.org/dist/latest-v13.x/node-v13.14.0-linux-armv7l.tar.gz
tar -xvf node-v13.14.0-linux-armv7l.tar.gz
ln -sf [install]/bin/node /usr/bin/node
node -v
```
如果能够正常显示版本信息，表示node安装成功.  

### 启动服务器
如果在PC平台测试，直接在lower_app/server目录下，执行.  
```bash
node server.js
```
如果显示如下.  
node server start!  
server listen ip:127.0.0.1, port 15,060  
则表示启动成功，此时通过在浏览器输入127.0.0.1:15060访问页面  

在嵌入式linux平台，需要将config.json中的ip地址修改为嵌入式平台ip地址，在通过上诉命令测试.  