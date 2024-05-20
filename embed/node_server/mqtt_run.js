
//node module
const mqtt = require('mqtt')

//user module
const { config_info } = require('./config_json.js')

let subscribe_info = {
    command: 1,
    data: {
      angle: 0,
      ap: { als: 0, ir: 0, ps: 0 },
      beep: 0,
      hx711: 0,
      icm: {
        accel_x: 0,
        accel_y: 0,
        accel_z: 0,
        gyro_x: 0,
        gyro_y: 0,
        gyro_z: 0,
        temp_act: 0
      },
      led: 0,
      sysinfo: {
        cpu_info: '',
        disk_total: 0,
        disk_used: 0,
        kernel_info: '',
        ram_total: 0,
        ram_used: 0
      },
      vf610_adc: 0
    }
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

            for (let key in json_obj){
                if (subscribe_info.hasOwnProperty(key)){
                    subscribe_info[key] = json_obj[key]
                }
            }
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