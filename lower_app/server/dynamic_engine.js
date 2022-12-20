
const url = require('url');

function dynamic_engine_process(request, response)
{
    var Query = url.parse(request.url);
    var action = Query.pathname.slice(1);
    console.log(action + " " + Query.query);
    switch(action)
    {
        case 'axiosDeviceSet':
            response.end("set ok");
            break;
    }
}

module.exports.dynamic_engine_process = dynamic_engine_process;