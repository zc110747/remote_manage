#kill application
pkill -9 node
pkill -9 main_process
pkill -9 logger_tool
pkill -9 mosquitto

#start application
mosquitto -c /etc/mosquitto/mosquitto.conf &

sleep 10

#start all device
/home/sys/executable/logger_tool -f /home/sys/configs/config.json &
/home/sys/executable/main_process -f /home/sys/configs/config.json &

sleep 5

#run server.js
cd /home/sys/server
/home/sys/support/node/bin/node server.js &
