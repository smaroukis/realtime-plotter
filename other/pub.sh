#!/bin/bash

while true; do
	mosquitto_pub -d -t test -h 192.168.0.17 -m 3
	sleep  1
done
