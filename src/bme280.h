// source   https://lastminuteengineers.com/bme280-esp32-weather-station/
#ifndef bme280_h
#define bme280_h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

float temperature, humidity, pressure, altitude;

void bmpSetup() {
  Serial.begin(115200);
  delay(100);
  
  bme.begin(0x76);   
}

void bmpLoop() {
  static unsigned long lastMillis{0};

    if (millis() - lastMillis > 3000) {
        lastMillis = millis();
        temperature = bme.readTemperature();
        humidity = bme.readHumidity();
        pressure = bme.readPressure() / 100.0F;
        altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

        Serial.print("Temperature = "); Serial.println(temperature);
        Serial.print("Humidity = "); Serial.println(humidity);
        Serial.print("Pressure = "); Serial.println(pressure);
        Serial.print("Altitude = "); Serial.println(altitude);
        Serial.println();
    }
}

#endif