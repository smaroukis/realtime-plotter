/************************* Debugging *********************************/
#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x) 
#define debugln(x)
#endif
/********************************************************************/

// Custom Headers
#include "wifiHelper.h"
#include "mqttHelper.h"
#include "sensorStd.h" // RETURN_NULL is -999
#include "bme280.h"
#include "waterLevelSensor.h"

void setup()
{
  // Setup Serial Monitor and Hardware
  if (DEBUG == 1) {
    Serial.begin(115200);
  } 

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  // Setup Wifi and MQTT
  /*
  setupWifi();
  setupMqtt();
  */

  // Setup Sensors
  setupWaterLvl();
  setupBME();
}

 // Timed Loop to blink led in loop() and print hello
void blinkHello(unsigned long& lastMillis) {
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    debugln("hello I'm blinking");
    debugln();
    // toggle built in led
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}

void loop()
{
  // ---- START WIFI and MQTT -----
  // TODO - want to retry wifi three times, then go to sleep if it fails 
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

  // Run once to emulate sleep mode
  static boolean loop{true};
  while (loop){

  // ----- START LOOP CODE ------

  digitalWrite(LED_BUILTIN, HIGH);

  // Sensors
  float temp = getTemperature_c();
  if (!publishSensorVal(mqttClient, "temperature", temp)) {
    debug("Failed to publish temperature, val = {");
    debug(temp);
    debugln("}");
    // Try to tell the outside world that we failed to publish temperature
  }

  float humid = getHumidity_pct();
  if (!publishSensorVal(mqttClient, "humidity", humid)) {
    debug("Failed to publish humidity, val = {");
    debug(humid);
    debugln("}");
  }

  // Warn - need to convert to cPA otherwise will overflow the publish string
  float pressure = getPressure_kpa();
  if (!publishSensorVal(mqttClient, "pressure", pressure)) { //   debug(F("Failed to publish pressure, val = {"));
    debug("Failed to publish humidity, val = {");
    debug(pressure);
    debugln(F("}"));
  }

  // ------ END LOOP CODE -----

  loop = false;
  }

}

/*  Debugging Checklist
1) Check pin numbers in code
2) Check breadboarding
3) 
*/

/* Improvements
- see FUTURE comments
*/