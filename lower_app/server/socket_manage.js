
const { read } = require("fs");
const net = require("net");

//timeout wait for server start
//delay a few time for app_demo start
let client_timewait = 2000;
let client = new net.Socket();
let is_connect = false;

let DeviceInfo = {
    led:"on",
    beep:"on",
    ir:"0",
    als:"0",
    ps:"0",
    gypox:"0",
    gypoy:"0",
    gypoz:"0",
    accelx:"0",
    accely:"0",
    accelz:"0",
    temp:"0"
};

function socket_connect(ipaddr, port)
{ 
    client.setEncoding('utf8');

    client.on('data', (message)=>{
        let arr = message.toString().replace('\u0000', '').toLowerCase().split(' ');
        switch(arr[0])
        {
            case "!status":
                {
                    let list = arr[1].split(";");
                    let obj = {};
                    for(let val of list){
                        let dev = val.split('=');
                        obj[dev[0]] = dev[1];
                    }

                    for (let key in obj){
                        if(DeviceInfo.hasOwnProperty(key)){
                            DeviceInfo[key] = obj[key]
                        }
                    }
                    //console.log(DeviceInfo);
                }
                break;
        }
    })

    client.on('error', (err)=>{
        is_connect = false;
        console.log(`${err}`);
    })

    client.on('close', (err)=>{
        is_connect = false;
        console.log(`${err}`);
    })

    setTimeout(()=>{
        console.log(`client start, ipaddr:${ipaddr}, port:${port}!`);
        client.connect(port, ipaddr, ()=>{
            console .log('client connected!');
            is_connect = true;
            client.write("!connect");
        });
    }, client_timewait);
} 

function sock_send(data)
{
    let ret = false;
    
    if(is_connect)
    {
        client.write(data);
        ret = true;
    }   
    return ret;
}

module.exports.socket_connect = socket_connect;
module.exports.sock_send = sock_send;
module.exports.device_info = DeviceInfo;