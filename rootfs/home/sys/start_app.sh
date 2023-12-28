#kill application
pkill -9 node
pkill -9 main_process
pkill -9 logger_tool
pkill -9 mosquitto

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
#/home/sys/support/node/bin/node server.js &
