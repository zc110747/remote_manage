
//modules
const fs = require('fs');
const _a = require('path'), extname = _a.extname, join = _a.join;

//user modules
const _tools = require('./tool.js');

var contentTypeStatic = {
    "html": "text/html",
    "xml": "text/xml",
    "asp": "text/asp",
    "css": "text/css",
    "jpg": "image/jpeg",
    "png": "image/png",
    "ico": "image/icon",
    "js": "application/x-javascript",
    "map": "text/map"
};

function engine_process(request, response)
{
    var pathname = request.url;
    if(pathname === '/'){
        pathname = 'index.html'
    }

    var localpath = "".concat(__dirname, "/webpage/");
    var filepath = join(localpath, pathname);
    var ext = extname(filepath).slice(1);

    //static document process
    if (_tools.hasOwnKey(contentTypeStatic, ext))
    {
        //static data struct
        response.setHeader('content-type', contentTypeStatic[ext]);
        response.setHeader('server', 'node'); 
        if (fs.existsSync(filepath)) 
        {
            var FileData = fs.readFileSync(filepath);
            response.end(FileData);
        }
        else 
        {
            console.log(`invalid pathname:${pathname}`);
            response.statusCode = 404;
            response.end("404 not found");
        }
        return true;
    }
    else
    {
        //console.log("Not Static Request");
    }
    
    return false;
}

module.exports.engine_process = engine_process;
