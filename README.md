Various home automation projects centered around a RaspberryPi as a MQTT broker with ESP32 edge-devices.
See [P36-Home-Automation](P36-Home-Automation.md) for more

**Overview**:

![](assets/README-1668739333080.jpeg)

### Learning Breakpoint 01 (tag: `learn-bp-01`) - Hello Blink
Compile and build this tagged commit to verify communication between your local mqtt broker and the esp32 as a mqtt client. The esp32 will blink itself on and off every 3 seconds, but can also be turned on/off via the `inTopic` mqtt topic (note sending an "ON"/"OFF" command via mqtt will not affect the blink timing which just toggles the state every 3 seconds).

To run: 
- Start a mqtt broker on a local server (e.g. `mosquitto` on a RasPi)
- Get the broker local IP address (for the next step)
- Rename `fakeSecrets.h` to `secrets.h` and modify as necessary (WiFi name, password, MQTT server IP)
- Compile and Upload to ESP32

The ESP32 will now toggle its built-in led every 3 seconds and print "hello I'm blinking" to the Serial Monitor.

When it receives an MQTT message to the "inTopic" it will print it on the serial monitor.
- If the message is "ON" or "OFF" it will turn the built-in led ON or OFF
    - It will also deliver an MQTT message to the "outTopic" topic

#### Server Commands

**On local machine**:
```
# use SSH from a terminal to get into the Pi server
ssh pi@raspberrypi.local 
# ... enter user/pass ...
```

**In a server terminal (or after SSHing in)**:
```
# print IP address (should match the one in `secrets.h`)
hostname -I 
# ...
# check the `mosquitto` status:
systemctl status mosquitto.service 
# ... if status is running try:
mosquitto_pub -d -t inTopic -m "ON" 
# which should turn the builtin on the ESP32 ON if it is OFF
```

> If necessary restart the mosquitto service with
`sudo systemctl restart mosquitto`

> If we want to see the `outTopic` responses from the ESP32 we can open up another terminal on the MQTT broker and subscripe to this topic e.g.
`mosquitto_sub -d -t outTopic`
> Or to subscribe to all topics do:
`mosquitto_sub -d -t "#"`

**Notes/Gotchas**
- the mosquitto broker has to be configured to accept anonymous connections (disabled by default)
- put the ESP32 in "flash" mode by (1) pressing and holding BOOT (2) pressing and releasing ENABLE (3) releasing BOOT

### Learning Breakpoint 02 (tag: `learn-bp-02`) - Deep Sleep & Publish String Creation
This tagged commit demonstrates 
- creating an mqtt publish string with c-style strings, in the form `<gateway>/<client-ID>/<function>/<sensor-type>/<sensor-units>` 
- deep sleep and waking upon an interrupt
- adds a 3 min watchdog timer for the main loop, to make sure there are no hangups with e.g. connecting to the server
- reading sensor data from an `bme280` sensor with the Adafruit Library

Description: 
- In `setup()` we check the reset reason, then set an external wakeup interrupt on pin 33 
- In `loop()`, we will enter sleep at the end of the loop, so it only runs once
- First we add a watchdog timer for the current thread
- We make sure we are connected to WiFi and the MQTT broker as defined in `secrets.h` before continuing
- We read the `bme280` sensor and publish to mqtt
    - data is stored in RTC memory to be accesible after deep sleep
- We get ready to sleep
    - reset the watchdog
    - disconnect from wifi and mqtt
- Then we sleep for 30 mins

**MQTT Topic Example**:
- For example, the mqtt publishing topic for temperature is `home/esp-01/status/temperature/C`. 
- This topic string structure was chosen since later we will send commands to the device in the form `home/esp-01/set/led` = `ON`
- Units are also included in the topic string for clarity to the user/broker
- This is created on the fly with dynamic memory allocation and is destructed later in the `publishSensorVal()` function (It is better to handle strings with C++ `string` types, but I wanted to demonstrate/learn about cstrings). 

**Deep Sleep and Wake Upon Interrupt Example**
Hardware setup: Wakeup when Pin 33 -> LOW
- Pin 33 -> T1 of Pushbutton
- T1 of Pushbutton -> Resistor -> 3V3 (PULLUP)
- T2 of Pushbutton -> GND

_Pinout Drawing_:
![](assets/Example%20-%20ESP32%20Deep%20Sleep-1672037414676.jpeg)

Putting the ESP32 into deep sleep will only keep variables that have been placed into the RTC memory e.g.

```cpp
RTC_DATA_ATTR float value{0};
```

We go to sleep with:

```cpp
esp_sleep_enable_timer_wakeup(time_in_us);
esp_deep_sleep_start();
```

Set an external wakeup (`ext0` type) when Pin 33 goes `LOW`:
```cpp
esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0)
```

When starting up, check if we were woken up by the external wakeup:
```cpp
#include "esp_sleep.h"
if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    // ...
}
```

### Learning Breakpoint 03 (tag: `learn-bp-03`) - Raindrop Sensor ext0 Wake
This tagged commit demonstrates:
- implementing an external interrupt with a raindrop sensor (like this one)
- if there is rain, the ESP disables the `ext0` interrupt, since we do not want to keep waking up
- if there is no rain, the interrupt is re-enabled

_Hardware Connections_
- Rain Sensor `D0` → ESP `D4` 
- Rain Sensor `A0` → ESP `D33`
- VCC → 3.3V 
- Ground

> The sleep values were changed to 
#### Rain Drop Sensor description
`D0` is the digital output of the comparator on the rain sensor control board. This is normally `HIGH`. When a raindrop is detected, this pin goes `LOW` (and turns on the on-board `D0` LED). 
The sensitivity is set by the potentiometer (clockwise raises Vref so this will be more sensitive).

**Detailed Description**
The inputs to the comparator are `A0` and `V_SENSE`. `V_SENSE` is a voltage divider from `VCC`, based on the potentiometer value. `A0`'s voltage is from a voltage divider between a 10K pullup resistor from `VCC` and the resistance of the rain drop module (infinite when no rain). When it's not raining this rain drop module is an open circuit, so `A0`=`VCC`. When it rains, this resistance decreases, so it pulls `A0` lower, an amount depending on the amount of water on the board (more water = less resistance). If this voltage is pulled below the `V_SENSE` voltage, then the comparator output, thus `D0` swings `LOW`. 

#### Improvements
- to minimize corrosion we would want to disconnect the voltage to the raindrop monitor when we already know it's raining and we're in deep sleep, we could do this either in hardware using the `D0` pin as a gate input (since once this is low we know it is raining), or in software via the ESP. 