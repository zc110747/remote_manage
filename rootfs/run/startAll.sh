pkill -9 node
pkill -9 main_process

sleep(1)

#run application
./run/bin/app_demo -f /run/config.json &

#run server.js
cd /run/server/
node server.js &
