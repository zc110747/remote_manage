#stop all device
pkill -9 logger_tool
pkill -9 local_device
pkill -9 main_process
pkill -9 mosquitto

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
/usr/bin/node server.js &
