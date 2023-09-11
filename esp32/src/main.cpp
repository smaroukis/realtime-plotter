// For debugln() and RETURN_NULL
#include "sensorStd.h" // RETURN_NULL is -999

// Custom Headers
#include "wifiHelper.h"
#include "mqttHelper.h"
#include "bme280.h"

void setup()
{
  // Setup Serial Monitor and Hardware
  if (DEBUG == 1) {
    Serial.begin(115200);
  } 

  delay(1);
  
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  setupWifi();
  setupMqtt();

  // Setup Sensors
  setupBME();
}

void loop()
{

  if (!mqttClient.connected())
  { 
    // if (WiFi.status() != WL_CONNECTED) Serial.println("Not Connected to WiFi");
    // WiFi.mode(WIFI_STA); // from https://github.com/knolleary/pubsubclient/issues/203
    mqttClientReconnect();
  }

  mqttClient.loop(); // MQTT keep alive, callbacks, etc

  // ---- END WIFI and MQTT -----

  // ----- START SENSORS -----

  val_temp = getTemperature_c();
  if (!publishSensorVal(mqttClient, "temperature", "C", val_temp)) {
    debug("Failed to publish val_temperature, val = {");
    debug(val_temp);
    debugln("}");
    // Try to tell the outside world that we failed to publish temperature
  }
  delay(100); // wasn't publishing next payload wihout a delay

  // val_humidity = getHumidity_pct();
  // if (!publishSensorVal(mqttClient, "humidity", "pct", val_humidity)) {
  //   debug("Failed to publish humidity, val = {");
  //   debug(val_humidity);
  //   debugln("}");
  // }
  // delay(10); // wasn't publishing next payload wihout a delay

  // // Warn - need to convert to cPA otherwise will overflow the publish string
  // val_pressure = getPressure_kpa();
  // if (!publishSensorVal(mqttClient, "pressure", "kPA", val_pressure)) { //   debug(F("Failed to publish pressure, val = {"));
  //   debug("Failed to publish humidity, val = {");
  //   debug(val_pressure);
  //   debugln(F("}"));
  // }
  // delay(10); // wasn't publishing next payload wihout a delay

  // ----- END SENSORS -----
}
