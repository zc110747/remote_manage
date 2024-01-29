
//node module
const mqtt = require('mqtt')

//user module
const { config_info } = require('./config_json.js')

let subscribe_info = {
};

function init()
{
    try
    { 
        let mqtt_addr = `mqtt://${config_info.ipaddress}:${config_info.mqtt_device.port}`;
        console.log(mqtt_addr)

        let client = mqtt.connect(mqtt_addr,  { 
            clientId: config_info.mqtt_device.id
        });
    
        client.on('connect', function(){
            console.log('Connected to MQTT broker')
            client.subscribe(config_info.mqtt_device.sub_topic, {qos: 1});
        });
    
        client.on('message', function (topic, message) {
            const json_obj = JSON.parse(message.toString())
            subscribe_info = json_obj
            console.log(subscribe_info)
        })
    }
    catch(e)
    {
        console.log(e);
    }
}

//export var and function
module.exports.init = init
module.exports.subscribe_info = subscribe_info