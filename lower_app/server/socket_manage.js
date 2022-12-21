
const { read } = require("fs");
const net = require("net");

//timeout wait for server start
//delay a few time for app_demo start
let client_timewait = 2000;
let client = new net.Socket();
let is_connect = false;

function socket_connect(ipaddr, port)
{ 
    client.setEncoding('utf8');

    client.on('data', (message)=>{
        console.log(message);
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
            client.write("conenct\n");
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