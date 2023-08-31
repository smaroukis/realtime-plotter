// TODO refactor to .cpp file when done

#ifndef dhtSensor_h
#define dhtSensor_h

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Custom Headers
#include "sensorStd.h" // for RETURN_NULL

#define DHTPIN 13

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

void dhtSetup() {
  Serial.begin(9600);
  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  sensor_t sensor;
}

void dhtLoop() {
  // delay(delayTemp_ms);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: ")); Serial.print(event.temperature); Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: ")); Serial.print(event.relative_humidity); Serial.println(F("%"));
  }
}

// requires: DHT object and sensor event from DHT_unified
// modifies: prints to the serial monitor
// returns: temperature as float or NAN if delay has been met, else RETURN_NULL
float getTemperature_C(){

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    float temperature = event.temperature;

    if (isnan(temperature)) {
      Serial.println(F("Error reading temperature from Sensor! (NAN)"));
      temperature = NAN;
    } 

    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));

    return temperature;
}

// HERE - deciding on code flow - where and how should timing be handled
// error is returning from a void function
float loopTemperature() {
  static uint16_t previous{0};  // previous Value type matches analogRead()
  static unsigned long delayTemp_ms{4000};
  static unsigned long prevTemp_ms{millis()};

  if (millis() - prevTemp_ms >= delayTemp_ms) {
    prevTemp_ms = millis(); // delay time is more important than constant cycle time
    auto val = getTemperature_C();
    return val;
  }
  else return RETURN_NULL;

}

// REMOVE AFTER TESTING
// float getTemperature(){
//     if (millis() - previous >= delayTemp_ms) {
//         previous= millis();
//         return 42.00f;
//     }
//     return RETURN_NULL;
// }

#endif


/*  EXAMPLE USAGE IN MAIN LOOP (main.cpp:main)
  // delay handled in temperatureSensor.cpp
  // if delay has not been met, RETURN_NULL is returned
  auto temperature = getTemperature();
  if ((temperature != RETURN_NULL) && (!isnan(temperature))) 
  {
    // publishSensorVal returns mqttClient.publish() which is false if failed
   if (!publishSensorVal(mqttClient, "temperature", temperature))
      { Serial.println("Error publishing temperature"); }
  }
*/