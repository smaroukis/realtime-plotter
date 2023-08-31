// source   https://lastminuteengineers.com/bme280-esp32-weather-station/
#ifndef bme280_h
#define bme280_h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

float temperature, humidity, pressure, altitude;

void setupBME() {
  Serial.begin(115200);
  delay(100);
  
  bme.begin(0x76);   
}

float getTemperature_c() {
  return bme.readTemperature();
}

float getHumidity_pct() {
  return bme.readHumidity();
}

// 101.325 kPa is sea level atmospheric pressure, decreases with altitude
float getPressure_kpa() {
  return bme.readPressure()/1000.0F;
}

// TODO - add error handling
void bmeLoop() {
      temperature = getTemperature_c();
      humidity = getHumidity_pct();
      pressure = getPressure_kpa();

      Serial.print("Temperature = "); Serial.println(temperature);
      Serial.print("Humidity = "); Serial.println(humidity);
      Serial.print("Pressure = "); Serial.println(pressure);
      Serial.println();
}

#endif