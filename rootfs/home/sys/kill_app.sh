#/usr/bin/bash

app_list=("node server.js" "main_process" "logger_tool" "local_device" "mosquitto")

for app in "${app_list[@]}"; do
    run_id=$(ps aux | grep '${app}' | grep -v grep | awk '{print $2}')
    echo "${app}, id:${run_id}"

    if [ ! -z ${run_id} ]; then
        kill -9 ${run_id}
    fi
done
