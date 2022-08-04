//module
var http = require('http');
var fs = require('fs');
var _a = require('path'), extname = _a.extname, join = _a.join;
//global parameter
var server = http.createServer();
var netInfo = {
    ipaddr: '127.0.0.1',
    port: 8080
};
var contentType = {
    "html": "text/html",
    "xml": "text/xml",
    "asp": "text/asp",
    "jpg": "image/jpeg",
    "png": "image/png",
    "ico": "image/icon",
    "js": "application/x-javascript"
};
var localpath = "".concat(__dirname, "\\web\\");
function hasOwnKey(obj, key) {
    return (key in obj);
}
server.on('request', function (request, response) {
    //request do
    var pathname = request.url;
    pathname = pathname === '/' ? 'index.html' : pathname;
    var filepath = join(localpath, pathname);
    if (fs.existsSync(filepath)) {
        var ext = extname(filepath).slice(1);
        if (hasOwnKey(contentType, ext))
            response.setHeader('content-type', contentType[ext]);
        else
            console.log("content-type:".concat(ext));
        var html = void 0;
        html = fs.readFileSync(filepath);
        response.end(html);
    }
    else {
        console.log("invalid pathname ".concat(pathname));
        response.statusCode = 404;
        response.end("not found");
    }
});
server.listen(netInfo.port, netInfo.ipaddr, function () {
    console.log("server listen ip:".concat(netInfo.ipaddr, ", port ").concat(netInfo.port));
});
