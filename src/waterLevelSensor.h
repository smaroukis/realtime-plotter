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
const int WATER_PIN{54};
const int AREF_mV{5000}; // 5000 or 3300 mV
const int MAX_ADC{1023}; // 12bit=4095 ; 10bit=1023

// Vars
uint16_t previous{};
unsigned long delayWater_ms{4000};
unsigned long prevWater_ms{};

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
/*
int getWaterLvl_mV() {

    if (millis() - prevWater_ms >= delayWater_ms) {
        auto value = analogRead(WATER_PIN);
        
        if (previous != value) {
            auto val_mV = adcToVolts(value); 
            previous = value;
            return val_mV;
        }
        else return RETURN_NULL;
        prevWater_ms = millis();
    }
    else return RETURN_NULL;
}
*/

// REMOVE AFTER TESTING
int getWaterLvl_mv() {
    if(millis() - prevWater_ms >= delayWater_ms) {
        return 4200;
        prevWater_ms = millis();
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