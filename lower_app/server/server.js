//module
//node module
const http = require('http');
const fs = require('fs')

//user module
const static_engine = require('./static_engine.js');
const dynamic_engine = require('./dynamic_engine.js');
const { config } = require('process');

//global parameter
var server = http.createServer();
var netInfo = {
    ipaddr: '127.0.0.1',
    port: 80
};
const filepath = "config.json"

function start_server()
{
    console.log("node server start!")
    
    fs.readFile(filepath, 'utf-8', (err, fileContents) => {
        if(err) throw err;
        var jsonValue = JSON.parse(fileContents)
    
        netInfo.ipaddr = jsonValue.socket.ipaddr;
        netInfo.port = jsonValue.node.port;
    
            //server on process
        server.on('request', function (request, response) {
        if(static_engine.engine_process(request, response)){
                //console.log("static engine process success!");
        }
        else{
                dynamic_engine.dynamic_engine_process(request, response);
        }
        });
    
        server.listen(netInfo.port, netInfo.ipaddr, function () {
            console.log("server listen ip:".concat(netInfo.ipaddr, ", port ").concat(netInfo.port.toLocaleString()));
        });
    })
}

start_server();





