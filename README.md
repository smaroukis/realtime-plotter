Various home automation projects centered around a RaspberryPi as a MQTT broker with ESP32 edge-devices.
See [P36-Home-Automation](P36-Home-Automation.md) for more

**Overview**:

![](assets/README-1668739333080.jpeg)

### Learning Breakpoint 01 (tag: `learn-bp-01`)
To run: 
- Start a mqtt broker on a local server (e.g. `mosquitto` on a RasPi)
- Get the broker local IP address (for the next step)
- Rename `fakeSecrets.h` to `secrets.h` and modify as necessary (WiFi name, password, MQTT server IP)
- Compile and Upload to ESP32

The ESP32 will now toggle its built-in led every 3 seconds and print "hello I'm blinking" to the Serial Monitor.

When it receives an MQTT message to the "inTopic" it will print it on the serial monitor.
- If the message is "ON" or "OFF" it will turn the built-in led ON or OFF
    - It will also deliver an MQTT message to the "outTopic" topic

#### Commands

**On local machine**
```
ssh pi@raspberrypi.local 
# ... enter user/pass
hostname -I # prints IP address
# ...
systemctl status mosquitto.service # check the `mosquitto` status
# ... if status is running try
mosquitto_pub -d -t inTopic -m "ON" 
# which should turn the builtin on the ESP32 ON if it is OFF
```

> If necessary restart the mosquitto service with
`sudo systemctl restart mosquitto`

> If we want to see the `outTopic` responses from the ESP32 we can open up another terminal on the MQTT broker and subscripe to this topic e.g.
`mosquitto_sub -d -t outTopic`

**Notes/Gotchas**
- the mosquitto broker has to be configured to accept anonymous connections (disabled by default)
- put the ESP32 in "flash" mode by (1) pressing and holding BOOT (2) pressing and releasing ENABLE (3) releasing BOOT