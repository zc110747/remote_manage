/*
配置文件解析，提前信息包含
1.本地服务器地址
*/
//global require
const fs = require('fs')
const os = require('os')

let config_info = {
    //文件版本信息
    version: "0.0.0.0",

    //ip地址
    ipaddress: "0.0.0.0",
    
    //web端口地址
    web_port:0,

    //mqtt信息
    mqtt_device: {
        id  : "",
        port : 0,
        sub_topic : "",
        pub_topic : "",
        keepalive : 0,
        qos: 0
    }
};

function load_config_file(filePath)
{
    try
    {
        let resdata = fs.readFileSync(filePath).toString();
        let jsonValue = JSON.parse(resdata);

        config_info.version = jsonValue.version;
        config_info.ipaddress = jsonValue.ipaddress;
        config_info.web_port = jsonValue.node_server.web_port;

        for (let key in config_info.mqtt_device) {
            if (key in jsonValue.node_server.mqtt_device){
                config_info.mqtt_device[key] = jsonValue.node_server.mqtt_device[key];
            }   
        }
    }
    catch(err)
    {
        console.log(err);
        return false;
    }
    
    console.log(config_info);
    return true;
}

module.exports.load_config_file = load_config_file;
module.exports.config_info = config_info;