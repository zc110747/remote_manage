// 1 引入模块
const net = require('net');

// 3 链接
var client = net.connect({port:8000, host:"192.168.93.1"}, function(){
    this.setEncoding('utf8');
    this.setTimeout(500);
    this.on('data',(data)=>{
        var buf =Buffer.from(data);
        console.log(typeof data);
        console.log(data.length);
        console.log(buf);
    })
    this.on('error',(e)=>{
        console.log(e.message);
    });

    this.on('close',()=>{
        console.log("Socket Close\n");
    });
});

const led_cmd = [0x5a, 0x01, 0x32, 0x23, 
                0x00, 0x08, 0x02, 0x00, 
                0x00, 0x00, 0x03, 0x03, 
                0x00, 0x01, 0xFF, 0xFF];
const buf1 = Buffer.from(led_cmd);
client.write(buf1);