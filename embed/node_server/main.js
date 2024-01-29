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
const config_json = require('./config_json.js')
const mqtt_run = require('./mqtt_run.js');
const static_engine = require('./static_engine.js');
const dynamic_engine = require('./dynamic_engine.js');
const sock_m = require('./socket_manage.js');

//global parameter
const filepath = "/home/sys/configs/config.json";
let server = http.createServer();

function server_process()
{
    server.on('request', function (request, response) {
        if (static_engine.engine_process(request, response)){
            //console.log("static engine process success!");
        }
        else{
            dynamic_engine.dynamic_engine_process(request, response);
        }
    });

    //建立web服务器, 支持动态和静态处理
    //访问的界面文件在webpage/目录下
    server.listen(config_json.config_info.web_port, config_json.config_info.ipaddress, function (err) {
        if (err) throw err;
        console.log(`server start ok, server ip:${config_json.config_info.ipaddress}, port:${config_json.config_info.web_port}`);
    }); 

    //启动客户端, 建立和服务器通讯(替换为mqtt方式)
    //sock_m.socket_connect(local_ipaddr, netInfo.socket_port);
    mqtt_run.init();
}

function start_server()
{ 
    if(config_json.load_config_file(filepath))
    {
        console.log("config load success, server start!")
        server_process();
    }
    else
    {
        console.log("config load file, please check!")
    }
}

start_server();





