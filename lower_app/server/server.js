//module
//node module
const http = require('http');

//user module
const static_engine = require('./static_engine.js');

//global parameter
var server = http.createServer();
var netInfo = {
    ipaddr: '192.168.204.1',
    port: 8080
};

//server on process
server.on('request', function (request, response) {
   if(static_engine.engine_process(request, response)){
        console.log("static engine process success!");
   }
   else{
        console.log(request.url);
   }
});

server.listen(netInfo.port, netInfo.ipaddr, function () {
    console.log("server listen ip:".concat(netInfo.ipaddr, ", port ").concat(netInfo.port.toLocaleString()));
});
