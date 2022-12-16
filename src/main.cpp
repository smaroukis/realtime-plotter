// Custom Headers
#include "wifiHelper.h"
#include "mqttHelper.h"
#include "sensorStd.h" // RETURN_NULL is -999
#include "temperatureSensor.h"
#include "waterLevelSensor.h"

// Enum for sensors
// TODO

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

  //  Water Level Sensor
  loopWaterLvl();
  
   // Uncomment to above TODO 

  // delay handled in temperatureSensor.cpp
  // if delay has not been met, RETURN_NULL is returned
  /*
  auto temperature = getTemperature();
  if ((temperature != RETURN_NULL) && (!isnan(temperature))) 
  {
    // publishSensorVal returns mqttClient.publish() which is false if failed
   if (!publishSensorVal(mqttClient, "temperature", temperature))
      { Serial.println("Error publishing temperature"); }
  }

  auto waterLvl = getWaterLvl_mv();
  if (waterLvl != RETURN_NULL)
  {
   if (!publishSensorVal(mqttClient, "water", waterLvl))
      { Serial.println("Error publishing water level"); }
  }
  */
}

/*  Debugging Checklist
1) Check pin numbers in code
2) Check breadboarding
3) 
*/

/* Improvements
- see FUTURE comments
*/