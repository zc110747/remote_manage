/*
基于node实现的web服务器,具体功能

1. 解析json配置文件，用于ip地址和端口定义
2. web服务器，支持静态文件读取和动态处理
3. 基于TCP的socket,和C++的axios服务器通讯
*/
//node module
const http = require('http');
const fs = require('fs')

//user module
const config_parser = require('./config_parser.js')
const mqtt_run = require('./mqtt_run.js');
const static_engine = require('./static_engine.js');
const dynamic_engine = require('./dynamic_engine.js');
const { server_info } = require('./config_parser.js');

//global parameter
const in_program_filepath = "../../config.template.json";
let server = http.createServer();

function server_process()
{
    // 定义server处理函数
    server.on('request', function (request, response) {
        if (static_engine.engine_process(request, response)){
            //console.log("static engine process success!");
        } else {
            dynamic_engine.dynamic_engine_process(request, response);
        }
    });

    //建立web服务器, 支持动态和静态处理
    //访问的界面文件在webpage/目录下
    server.listen(config_parser.config_info.web_port, config_parser.config_info.ipaddress, function (err) {
        if (err) throw err;
        console.log(`server start ok, server ip: ${config_parser.config_info.ipaddress}:${config_parser.config_info.web_port}`);
    }); 

    mqtt_run.init();
}

function start_server()
{
    const args = process.argv.slice(2);
    console.log('命令行参数:', args);

    if (args.length > 0) {
        for (let i = 0; i < args.length; i++) {
            if (args[i] == "-d" || args[i] == "--debug"){
                server_info.is_debug = true;
            } else if (args[i] == "-f" || args[i] == "--filepath"){
                if (i + 1 < args.length){
                    server_info.filepath = args[i + 1];
                }
            }
        }
    }

    // 判断文件是否存在
    if (!fs.existsSync(server_info.filepath)) {
        server_info.filepath = in_program_filepath;
    }

    if (config_parser.load_config_file(server_info.filepath)) {
        console.log("config load success, server start!")
        server_process();
    } else {
        console.log("config load file, please check!")
    }
}

start_server();





