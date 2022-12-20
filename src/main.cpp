// Custom Headers
#include "wifiHelper.h"
#include "mqttHelper.h"
#include "sensorStd.h" // RETURN_NULL is -999
#include "bme280.h"
#include "waterLevelSensor.h"

void setup()
{
  // Setup Serial Monitor and Hardware
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  // Setup Wifi and MQTT
  /*
  setupWifi();
  setupMqtt();
  */

  // Setup Sensors
  setupWaterLvl();
  bmpSetup();
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
  // ---- START WIFI and MQTT -----
  /*
  if (!mqttClient.connected())
  { 
    // if (WiFi.status() != WL_CONNECTED) Serial.println("Not Connected to WiFi");
    // WiFi.mode(WIFI_STA); // from https://github.com/knolleary/pubsubclient/issues/203
    mqttClientReconnect();
  }

  mqttClient.loop(); // MQTT keep alive, callbacks, etc
  */
  // ---- END WIFI and MQTT -----

  // Blink LED and print hello
  static unsigned long lastMillis = 0;
  blinkHello(lastMillis);

  bmpLoop();

}

/*  Debugging Checklist
1) Check pin numbers in code
2) Check breadboarding
3) 
*/

/* Improvements
- see FUTURE comments
*/