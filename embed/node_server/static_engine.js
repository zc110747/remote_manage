
//modules
const fs = require('fs');
const path = require('path');
const {join, resolve, extname, sep } = path;

//user modules
const _tools = require('./tool.js');
const STATIC_ROOT = resolve(__dirname, 'webpage');

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
    var pathname;

    // 1. Parse URL
    try
    {
        pathname = new URL(
            request.url,
            'http://localhost'
        ).pathname;

        pathname = decodeURIComponent(pathname);
    }
    catch (error)
    {
        response.statusCode = 400;
        response.end("400 bad request");
        return true;
    }

    // 2. Default document
    if (pathname === '/')
    {
        pathname = '/index.html';
    }

    // 3. Resolve local file path
    var filepath = resolve(
        STATIC_ROOT,
        '.' + pathname
    );

    // 4. Security check: prevent path traversal
    if (
        filepath !== STATIC_ROOT &&
        !filepath.startsWith(STATIC_ROOT + sep)
    )
    {
        response.statusCode = 403;
        response.end("403 forbidden");
        return true;
    }

    // 5. Get file extension
    var ext = extname(filepath).slice(1).toLowerCase();

    // Static document process
    if (_tools.hasOwnKey(contentTypeStatic, ext))
    {
        response.setHeader(
            'content-type',
            contentTypeStatic[ext]
        );

        response.setHeader('server', 'node');

        // 6. Check file existence
        if (fs.existsSync(filepath))
        {
            var stat = fs.statSync(filepath);

            if (!stat.isFile())
            {
                response.statusCode = 404;
                response.end("404 not found");
                return true;
            }

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

    return false;
}

module.exports.engine_process = engine_process;
