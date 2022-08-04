//module
const http = require('http');
const fs = require('fs');
const { extname, join } = require('path');

//global parameter
const server = http.createServer();

let netInfo = {
    ipaddr:'127.0.0.1',
    port:8080
};
let contentType = {
    "html":"text/html",
    "xml":"text/xml",
    "asp":"text/asp",
    "jpg":"image/jpeg",
    "png":"image/png",
    "ico":"image/icon",
    "js":"application/x-javascript",
};
var localpath:string = `${__dirname}\\web\\`;

function hasOwnKey(obj, key)
{
    return (key in obj);
}

server.on('request', (request, response)=>{
    //request do
    let pathname = request.url;
    pathname = pathname==='/'? 'index.html':pathname;
    let filepath = join(localpath, pathname);
    if(fs.existsSync(filepath))
    {
        const ext = extname(filepath).slice(1);

        if(hasOwnKey(contentType, ext))
            response.setHeader('content-type', contentType[ext]);
        else
            console.log(`content-type:${ext}`);

        let html;
        html = fs.readFileSync(filepath);
        response.end(html);
    }
    else
    {
        console.log(`invalid pathname ${pathname}`);
        response.statusCode = 404;
        response.end("not found");
    }
});

server.listen(netInfo.port, netInfo.ipaddr, ()=>{
    console.log(`server listen ip:${netInfo.ipaddr}, port ${netInfo.port}`)
});
