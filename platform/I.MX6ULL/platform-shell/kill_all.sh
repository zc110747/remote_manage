#!/bin/sh

#stop all device
kill -9 $(pidof -s logger_tool)
kill -9 $(pidof -s local_device)
kill -9 $(pidof -s main_process)
kill -9 $(pidof -s lower_device)
kill -9 $(pidof -s mosquitto)
