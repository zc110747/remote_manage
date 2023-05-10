/*
基于node实现的web服务器,具体功能
1.解析json配置文件，用于ip地址和端口定义
2.web服务器，支持静态文件读取和动态处理
3.基于TCP的socket,和C++的axios服务器通讯
*/
//node module
const http = require('http');
const fs = require('fs')

//user module
const static_engine = require('./static_engine.js');
const dynamic_engine = require('./dynamic_engine.js');
const sock_m = require('./socket_manage');
const { config } = require('process');

//global parameter
let server = http.createServer();
let netInfo = {
    ipaddr: '127.0.0.1',
    web_port: 5056,
    socket_port: 5057 
};
//for internal link, need use loopback host
const local_ipaddr = "127.0.0.1"; 
const filepath = "config.json";

function server_process(fileContents)
{
    let jsonValue = JSON.parse(fileContents)

    netInfo.ipaddr = jsonValue.socket.ipaddr;
    netInfo.web_port = jsonValue.node.web_port;
    netInfo.socket_port = jsonValue.node.socket_port;

    //server on process
    server.on('request', function (request, response) {
        if(static_engine.engine_process(request, response)){
                //console.log("static engine process success!");
        }
        else{
                dynamic_engine.dynamic_engine_process(request, response);
        }
    });

    //建立web服务器, 支持动态和静态处理
    //访问的界面文件在webpage/目录下
    server.listen(netInfo.web_port, netInfo.ipaddr, function (err) {
        if(err) throw err;
        console.log(`server start ok, server ip:${netInfo.ipaddr}, port:${netInfo.web_port}`);
    }); 

    //启动客户端, 建立和服务器通讯
    sock_m.socket_connect(local_ipaddr, netInfo.socket_port);
}

function start_server()
{
    console.log("node server start!")
    
    fs.readFile(filepath, 'utf-8', (err, fileContents) => {
        if(err) throw err;

        server_process(fileContents);
    });
}

start_server();





