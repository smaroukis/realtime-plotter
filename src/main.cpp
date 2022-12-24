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
#include "esp_task_wdt.h"
#define WDT_TIMEOUT_MAIN_s (60*3) // 3 minutes, in seconds
#define WIFI_TIMEOUT_s (60*2) // 2 minutes, in seconds
#define DEEP_SLEEP_30 (30 * 60 * 1000000) // 30 mins, in microseconds

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
  
  // Check if we were reset by the watchdog timer
  esp_reset_reason_t reset_reason = esp_reset_reason();
  switch (reset_reason) {
    ESP_RST_UNKNOWN:
      debugln("Reset reason: ESP_RST_UNKNOWN");
      break;
    ESP_RST_POWERON:
      debugln("Reset reason: ESP_RST_POWERON");
      break;
    ESP_RST_EXT:
      debugln("Reset reason: ESP_RST_EXT");
      break;
    ESP_RST_SW:        //!< Software reset via esp_restart
      debugln("Reset reason: ESP_RST_SW");
      break;
    ESP_RST_PANIC:      //!< Software reset due to exception/panic
      debugln("Reset reason: ESP_RST_PANIC");
      break;
    ESP_RST_INT_WDT:    //!< Reset (software or hardware) due to interrupt watchdog
      debugln("Reset reason: ESP_RST_INT_WDT");
      break;
    ESP_RST_TASK_WDT:   //!< Reset due to task watchdog
      debugln("Reset reason: ESP_RST_TASK_WDT");
      break;
    ESP_RST_WDT:
      debugln("Reset reason: ESP_RST_WDT");
      break;
    ESP_RST_DEEPSLEEP:  //!< Reset after exiting deep sleep mode
      debugln("Reset reason: ESP_RST_DEEPSLEEP");
      break;
    ESP_RST_BROWNOUT:   //!< Brownout reset (software or hardware)
      debugln("Reset reason: ESP_RST_BROWNOUT");
      break;
    ESP_RST_SDIO:
      debugln("Reset reason: ESP_RST_SDIO");
      break;
  }

  // Check to see if we wokeup from deep sleep due to the interrupt
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    debugln("....Woke up from interrupt");
    debugln();
    debug("Previous temperature was {"); debug(val_temp); debugln("}");
    debug("Previous humidity was {"); debug(val_humidity); debugln("}");
    debug("Previous pressure was {"); debug(val_pressure); debugln("}");
    debugln();
  }

  esp_sleep_enable_ext0_wakeup(INTERRUPT_PIN, 0); // setup a wake up interrupt on pin 33, 0 = Low

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  setupWifi();
  setupMqtt();

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

  // Setup Watchdog
  esp_task_wdt_init(WDT_TIMEOUT_MAIN_s, true); 
  esp_task_wdt_add(xTaskGetCurrentTaskHandle()); // add the current thread to the watch dog

  // ---- START WIFI and MQTT -----
  // TODO - want to retry wifi three times, then go to sleep if it fails 
  if (!mqttClient.connected())
  { 
    // if (WiFi.status() != WL_CONNECTED) Serial.println("Not Connected to WiFi");
    // WiFi.mode(WIFI_STA); // from https://github.com/knolleary/pubsubclient/issues/203
    mqttClientReconnect();
  }

  mqttClient.loop(); // MQTT keep alive, callbacks, etc
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
  delay(10); // wasn't publishing next payload wihout a delay

  val_humidity = getHumidity_pct();
  if (!publishSensorVal(mqttClient, "humidity", val_humidity)) {
    debug("Failed to publish humidity, val = {");
    debug(val_humidity);
    debugln("}");
  }
  delay(10); // wasn't publishing next payload wihout a delay

  // Warn - need to convert to cPA otherwise will overflow the publish string
  val_pressure = getPressure_kpa();
  if (!publishSensorVal(mqttClient, "pressure", val_pressure)) { //   debug(F("Failed to publish pressure, val = {"));
    debug("Failed to publish humidity, val = {");
    debug(val_pressure);
    debugln(F("}"));
  }
  delay(10); // wasn't publishing next payload wihout a delay
  // ------ Prepare To Go to Sleep -------
  // disconnect mqtt client and wifi
  disconnectMqtt(); // print a message before disconnecting
  WiFi.disconnect();
  // TODO - what if it fails to disconnect?
  // TODO - make sure power is routed where we need it
  esp_task_wdt_reset();

  // sleep for 30 minutes
  debugln("Going to sleep");
  esp_sleep_enable_timer_wakeup(DEEP_SLEEP_30);
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