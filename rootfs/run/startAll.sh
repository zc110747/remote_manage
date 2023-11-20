pkill -9 node
pkill -9 main_process

#run application
cd /run/bin/
./main_process -f /run/config.json &
./lower_device -f /run/config.json &

#run server.js
cd /run/server/
node server.js &
