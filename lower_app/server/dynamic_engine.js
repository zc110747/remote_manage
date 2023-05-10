/*
动态引擎处理
对于界面提交的命令, 进行解析并处理

*/
const url = require('url');
const sock_m = require('./socket_manage');
const { stat } = require('fs');

function device_set_process(query)
{
    let group = query.split('&');
    let dev = group[0].split('=')[1];
    let status = group[1].split('=')[1];
    let dev_num = 0;
    let dev_status = 0;

    if(dev.toLowerCase() === 'led')
    {
        dev_num = 0;
        if(status.toLowerCase() == 'off')
            dev_status = 0;
        else if(status.toLowerCase() == 'on')
            dev_status = 1;
        else
            return false;
    }
    else if(dev.toLowerCase() === 'beep')
    {
        dev_num = 1;
        if(status.toLowerCase() == 'off')
            dev_status = 0;
        else if(status.toLowerCase() == 'on')
            dev_status = 1;
        else
            return false;
    }
    else
        return false;

    let outStr = `!setdev ${dev_num},${dev_status}\n`;
    if(sock_m.sock_send(outStr))
        return true;

    return false;
}
function dynamic_engine_process(request, response)
{
    let Query = url.parse(request.url);
    let is_process_ok = false;

    if(Query.pathname && Query.query)
    {
        let action = Query.pathname.slice(1);
        console.log(action + " " + Query.query);
        switch(action)
        {
            case 'axiosDeviceSet':
                is_process_ok = device_set_process(Query.query);
                break;
            default:
                break;
        }
    }

    response.end("ack " + (is_process_ok?"ok":"false"));
}

module.exports.dynamic_engine_process = dynamic_engine_process;