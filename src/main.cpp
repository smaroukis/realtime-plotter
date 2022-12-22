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

// Definitions and GLobal Vars for Sleeping
#define INTERRUPT_PIN GPIO_NUM_33
RTC_DATA_ATTR float val_temp{0};
RTC_DATA_ATTR float val_humidity{0};
RTC_DATA_ATTR float val_pressure{0};

void setup()
{
  // Setup Serial Monitor and Hardware
  if (DEBUG == 1) {
    Serial.begin(115200);
  } 

  esp_sleep_enable_ext0_wakeup(INTERRUPT_PIN, 0); // setup a wake up interrupt on pin 33, 0 = Low

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
  // If we woke up from the interrupt, do something here
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    debugln("....Woke up from interrupt");
    debugln();
    debug("Previous temperature was {"); debug(val_temp); debugln("}");
    debug("Previous humidity was {"); debug(val_humidity); debugln("}");
    debug("Previous pressure was {"); debug(val_pressure); debugln("}");
    debugln();
  }

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

  digitalWrite(LED_BUILTIN, HIGH);

  // Sensors
  val_temp = getTemperature_c();
  if (!publishSensorVal(mqttClient, "temperature", val_temp)) {
    debug("Failed to publish val_temperature, val = {");
    debug(val_temp);
    debugln("}");
    // Try to tell the outside world that we failed to publish temperature
  }

  val_humidity = getHumidity_pct();
  if (!publishSensorVal(mqttClient, "humidity", val_humidity)) {
    debug("Failed to publish humidity, val = {");
    debug(val_humidity);
    debugln("}");
  }

  // Warn - need to convert to cPA otherwise will overflow the publish string
  val_pressure = getPressure_kpa();
  if (!publishSensorVal(mqttClient, "pressure", val_pressure)) { //   debug(F("Failed to publish pressure, val = {"));
    debug("Failed to publish humidity, val = {");
    debug(val_pressure);
    debugln(F("}"));
  }

  // ------ Prepare To Go to Sleep -------
  // disconnect mqtt client and wifi
  // mqttClient.disconnect();
  // WiFi.disconnect();
  // TODO - what if it fails to disconnect?
  // TODO - make sure power is set where we need it

  // sleep for 30 minutes
  debugln("Going to sleep");
  esp_sleep_enable_timer_wakeup(30 * 60 * 1000000); // 30 minutes
  esp_deep_sleep_start();
}

/*  Debugging Checklist
1) Check pin numbers in code
2) Check breadboarding
3) 
*/

/* Improvements
- see FUTURE comments
*/

/* Hardware Setups 
=====================
Interrupt Setup (Interrupt on LOW)
  Pin 33 -> T1 of Pushbutton
  T1 of Pushbutton -> Resistor -> 3V3 (PULLUP)
  T2 of Pushbutton -> GND
*/