#/usr/bin/bash

app_list=("node server.js" "main_process" "logger_tool" "local_device" "mosquitto")

for app in "${app_list[@]}"; do
    run_id=$(ps aux | grep '${app}' | grep -v grep | awk '{print $2}')
    echo "${app}, id:${run_id}"

    if [ ! -z ${run_id} ]; then
        kill -9 ${run_id}
    fi
done

if [ ! -d /tmp/app ]; then
    mkdir -p /tmp/app
fi

#start application
mosquitto -c /etc/mosquitto/mosquitto.conf &

sleep 1

#start all device
/home/sys/executable/logger_tool -f /home/sys/configs/config.json &
sleep 2
/home/sys/executable/local_device -f /home/sys/configs/config.json &
sleep 2
/home/sys/executable/main_process -f /home/sys/configs/config.json &
sleep 2

#run server.js
cd /home/sys/server
/usr/bin/node/bin/node server.js &
