// For debugln() and RETURN_NULL
#include "sensorStd.h" // RETURN_NULL is -999

#include "esp_task_wdt.h"
#define WDT_TIMEOUT_MAIN_s (60*3) // 3 minutes, in seconds
#define WIFI_TIMEOUT_s (60*2) // 2 minutes, in seconds
// #define DEEP_SLEEP_30 (30 * 60 * 1000000) // 30 mins, in microseconds
// #define DEEP_SLEEP_30 (30 * 60 * 1000000) // 30 mins, in microseconds
// Testing Only Below
#define DEEP_SLEEP_30 (30 * 1000000) // 30 sec, in microseconds
#define DEEP_SLEEP_15 (15 * 1000000) // 15 sec, in microseconds

// Custom Headers
#include "wifiHelper.h"
#include "mqttHelper.h"
#include "bme280.h"
#include "rainSensor.h"

// Definitions and GLobal Vars for Sleeping
RTC_DATA_ATTR bool is_raining{false};
RTC_DATA_ATTR float val_temp{0};
RTC_DATA_ATTR float val_humidity{0};
RTC_DATA_ATTR float val_pressure{0};

void print_reset_reason() {
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
}

void print_wakeup_reason() {
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0 :
            debugln("Wakeup caused by external signal using RTC_IO");
            break;
        case ESP_SLEEP_WAKEUP_EXT1 :
            debugln("Wakeup caused by external signal using RTC_CNTL");
            break;
        case ESP_SLEEP_WAKEUP_TIMER :
            debugln("Wakeup caused by timer");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD :
            debugln("Wakeup caused by touchpad");
            break;
        case ESP_SLEEP_WAKEUP_ULP :
            debugln("Wakeup caused by ULP program");
            break;
        default :
            debugf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
            break;
    }
}

void setup()
{
  // Setup Serial Monitor and Hardware
  if (DEBUG == 1) {
    Serial.begin(115200);
  } 
  
  // note we also set this pin in rainSensor.h
  setupWetness(); // sets up the rain sensor
  debugf("States Upon Waking Up:\n\t`is_raining` value: %d\n\t`RAIN_PIN_D0`: %d\n", is_raining, digitalRead(RAIN_PIN_D0));
  
  print_reset_reason(); // prints to debug the reset reason
  print_wakeup_reason();

  // Check if Rain Sensor Value Changed and Update
  // RAIN_PIN_DO is the output of the rain sensor comparator (dry = HIGH, wet = LOW)
  delay(1);
  if (digitalRead(RAIN_PIN_D0) == LOW) { 
    if (!is_raining) {
      debugln("Rain sensor changed from HIGH to LOW");
      debug("\t changed `is_raining` value → "); debugln(!is_raining);
    }
    is_raining = true;
  }
  else if (digitalRead(RAIN_PIN_D0) == HIGH) { 
    if (is_raining) {
      debugln("Rain sensor changed from LOW to HIGH");
      debug("\t changed `is_raining` value → "); debugln(!is_raining);
    }
    is_raining = false;
  }
  
  // If is is raining, don't set a wakeup interrupt and enable a shorter sleep time
  // If it's not raining, set the wakeup interrupt and set the deep sleep timer to 30 minutes 
  if (is_raining) {
    esp_sleep_enable_timer_wakeup(DEEP_SLEEP_15);

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1 || esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
      debugln("Woke up due to rain sensor interrupt");
      debugln("\t Enabled 15 minute deep sleep timer with NO wake interrupt");
      // see more is_raining control in loop
    }
  }
  else if (!is_raining) {
    debugln("Enabled rain sensor interrupt and 30 minute deep sleep timer");
    esp_sleep_enable_ext0_wakeup((gpio_num_t)RAIN_PIN_D0, 0); 
    esp_sleep_enable_timer_wakeup(DEEP_SLEEP_30);
  }

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  /*
  setupWifi();
  setupMqtt();
  */

  // Setup Sensors
  // rain sensor already setup
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

  // ----- START SENSORS -----
  if (!publishSensorVal(mqttClient, "is_raining", "", is_raining)) {
    debug("Failed to publish is_raining, val = {");
    debug(is_raining);
    debugln("}");
  }

  if (is_raining) { 
    if (!publishSensorVal(mqttClient, "wetness", "pct", getWetness_pct())) {
      debug("Failed to publish wetness, val = {");
      debug(getWetness_pct());
      debugln("}");
    }
  }
  /*  

  val_temp = getTemperature_c();
  if (!publishSensorVal(mqttClient, "temperature", "C", val_temp)) {
    debug("Failed to publish val_temperature, val = {");
    debug(val_temp);
    debugln("}");
    // Try to tell the outside world that we failed to publish temperature
  }
  delay(10); // wasn't publishing next payload wihout a delay

  val_humidity = getHumidity_pct();
  if (!publishSensorVal(mqttClient, "humidity", "pct", val_humidity)) {
    debug("Failed to publish humidity, val = {");
    debug(val_humidity);
    debugln("}");
  }
  delay(10); // wasn't publishing next payload wihout a delay

  // Warn - need to convert to cPA otherwise will overflow the publish string
  val_pressure = getPressure_kpa();
  if (!publishSensorVal(mqttClient, "pressure", "kPA", val_pressure)) { //   debug(F("Failed to publish pressure, val = {"));
    debug("Failed to publish humidity, val = {");
    debug(val_pressure);
    debugln(F("}"));
  }
  delay(10); // wasn't publishing next payload wihout a delay

  */
  
  // Rain Sensor



  // ----- END SENSORS -----

  // ------ Prepare To Go to Sleep -------
  // disconnect mqtt client and wifi, reset watchdog
  /* 
  disconnectMqtt(); // print a message before disconnecting
  WiFi.disconnect();
  */
  esp_task_wdt_reset();

  // sleep for 30 minutes
  debugln("Going to sleep");
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