// RB-02S048 (China Harbin Okumatsu Robot Technology Co)
// Sense pin outputs 0V when no liquid present
// and an analog voltage proportional to the amount of water present
// 3V3 supply showed typ. water drop sensing above 0.1V
// fully submerged: 0.795V fully submerged
// weak water drop: 0.139V to 0. 560V 

// 1) set signal pin to input
// 2) analog read the pin
// 3) compare to previous value - may need hysteresis
// 4) return data (mqtt handled in main) - here just return analog voltage as float

#ifndef waterLvlSensor_h
#define waterLvlSensor_h

#include <Arduino.h>
#include "sensorStd.h" // RETURN_NULL is -999

// WARN - Board specific
const int WATER_PIN{34}; // Note we cannot use ADC2 pins as they are used for WiFi
const int AREF_mV{3300}; // 5000 or 3300 mV
const int MAX_ADC{4095}; // 12bit=4095 ; 10bit=1023

// Vars

// Returns: millivolts as integer - WARN board specific
// Requires: AREF_mV, MAX_ADC
int adcToVolts(int n){
    return n * (AREF_mV / (float)MAX_ADC); // rounding errors are fine
}

// Sets the sensor pin to input
// Requires WATER_PIN
void setupWaterLvl() {
    pinMode(WATER_PIN, INPUT);
}

// returns: an int that is the analog voltage level in MILLIVOLTS or RETURN_NULL if not ready
// requires: ADC capable pin
// n.b.: timing is handled here
// TODO: refactor to interrupt on positive change
int getWaterLvl_mV() {
    static uint16_t previous{0}; // type matches analogRead()
    static unsigned long delayWater_ms{4000};
    static unsigned long prevWater_ms{millis()};

    if (millis() - prevWater_ms >= delayWater_ms) {
        prevWater_ms = millis();
        auto value = analogRead(WATER_PIN);
        
        if (previous - value > 10 || value - previous > 10) {
            previous = value;
            Serial.println("DEBUG: Water Level Changed");
            return adcToVolts(value);
        }
        else return RETURN_NULL;
    }
    else return RETURN_NULL;
}

#endif

// Improvements
// Refactor to class based
// Then we can more easily handle timing 
// and have a simple main getWaterLvl loop
// see Adafruit unified sensor lib for class based example
// figure out better control flow when the sensor is not 
// ready to update `main` with a value (eg timing not yet met)