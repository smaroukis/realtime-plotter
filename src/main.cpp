// N.B.for tag `hello-blink-refactor`
// The code has been refactored with the helper code in 
// mqttHelper.h and wifiHelper.h
// sensor code will be removed for this commit s.t. it can serve as a tag to test
// ------------
// USAGE: Rename `/include/fakeSecrets.h` to `/include/secrets.h` and modify with the correct wifi and mqtt credentials
// Library Deps: PubSubClient.h, Arduino.h, WiFi.h
// Other Dependencies: A MQTT broker running locally (e.g. mosquitto broker on a RasPi) at MQTT_SERVER on MQTT_PORT (1883)
// DESCRIPTION:
// This file is in `/src/main.cpp`
// This code connects to the wifi and mqtt broker
// And creates an mqtt client subscribed to "inTopic" and publishing to "outTopic"
// The code blinks the built in led every 5 seconds
// We can turn the led on and off by publishing to "inTopic" with the payload "OFF" or "ON"
// e.g. from the command line: `mosquitto_pub -d -t inTopic -m "ON"`

// Custom Headers
#include "wifiHelper.h"
#include "mqttHelper.h"
#include "sensorStd.h" // RETURN_NULL is -999
#include "temperatureSensor.h"
#include "waterLevelSensor.h"

void setup()
{
  // Setup Serial Monitor and Hardware
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  // Setup Wifi and MQTT
  setupWifi();
  setupMqtt();

  // Setup Sensor)s
  setupWaterLvl();
}

 // Timed Loop to blink led in loop() and print hello
void blinkHello(unsigned long& lastMillis) {
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    Serial.println("hello I'm blinking");
    Serial.println();
    // toggle built in led
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}

void loop()
{
  // Handle WiFi connection
  if (!mqttClient.connected())
  { 
    // if (WiFi.status() != WL_CONNECTED) Serial.println("Not Connected to WiFi");
    // WiFi.mode(WIFI_STA); // from https://github.com/knolleary/pubsubclient/issues/203
    mqttClientReconnect();
  }

  mqttClient.loop(); // MQTT keep alive, callbacks, etc

  // Blink LED and print hello
  static unsigned long lastMillis = 0;
  blinkHello(lastMillis);
}

/*  Debugging Checklist
1) Check pin numbers in code
2) Check breadboarding
3) 
*/

/* Improvements
- see FUTURE comments
*/