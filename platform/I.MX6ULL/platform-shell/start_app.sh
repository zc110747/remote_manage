#!/bin/sh

#stop all device
kill -9 $(pidof -s logger_tool)
kill -9 $(pidof -s local_device)
kill -9 $(pidof -s main_process)
kill -9 $(pidof -s lower_device)
kill -9 $(pidof -s mosquitto)

if [ ! -d /tmp/app ]; then
    mkdir -p /tmp/app
fi

#start application
/usr/local/sbin/mosquitto -c /etc/mosquitto/mosquitto.conf &

sleep 1

#start all device
/home/sys/executable/logger_tool -f /home/sys/configs/config.json &
sleep 2
/home/sys/executable/local_device -f /home/sys/configs/config.json &
sleep 2
/home/sys/executable/main_process -f /home/sys/configs/config.json &
sleep 2

#run server.js
cd /home/sys/server/
if [ ! -d /home/sys/server/node_module ]; then
    if [ -f node_modules.tar.bz2 ]; then
        tar -xvf node_modules.tar.bz2
    fi
fi
#/usr/bin/node/bin/node main.js &
