//module
const http = require('http');
const fs = require('fs');
const _a = require('path'), extname = _a.extname, join = _a.join;

//global parameter
var server = http.createServer();
var netInfo = {
    ipaddr: '192.168.204.1',
    port: 8080
};
var contentTypeStatic = {
    "html": "text/html",
    "xml": "text/xml",
    "asp": "text/asp",
    "jpg": "image/jpeg",
    "png": "image/png",
    "ico": "image/icon",
    "js": "application/x-javascript"
};

var localpath = "".concat(__dirname, "/web/");
function hasOwnKey(obj, key) {
    return (key in obj);
}
server.on('request', function (request, response) {
    //request do
    var pathname = request.url;
    pathname = pathname === '/' ? 'index.html' : pathname;
    // @ts-ignore
    var filepath = join(localpath, pathname);
    var ext = extname(filepath).slice(1);

    //static document process
    if (hasOwnKey(contentTypeStatic, ext))
    {
        //static data struct
        response.setHeader('content-type', contentTypeStatic[ext]); 
        if (fs.existsSync(filepath)) 
        {
            var FileData = fs.readFileSync(filepath);
            response.end(FileData);
        }
        else 
        {
            // @ts-ignore
            console.log("invalid pathname ".concat(pathname));
            response.statusCode = 404;
            response.end("not found");
        }
    }
    else
    {
        console.log("content-type:".concat(ext));
    }
});
server.listen(netInfo.port, netInfo.ipaddr, function () {
    // @ts-ignore
    console.log("server listen ip:".concat(netInfo.ipaddr, ", port ").concat(netInfo.port));
});
