// D0 output of comparator; normally HIGH (LED off); goes LOW (LED on) when Vsense < Vref
// Vref set by potentiometer
// A0 output is Vsense
// Vsense is pulled up to VCC by 10k resistor
// When it rains it pulls Vsense toward GND
// Pot: Clockwise more sensitive (raises Vref)


#ifndef waterLvlSensor_h
#define waterLvlSensor_h

#include <Arduino.h>
#include "sensorStd.h" // RETURN_NULL is -999

// WARN - Board specific
const unsigned int RAIN_PIN_A0{32}; // Note we cannot use ADC2 pins as they are used for WiFi
const unsigned int RAIN_PIN_D0{4}; 
const int AREF_mV{3300}; // 5000 or 3300 mV
const int MAX_ADC{4095}; // Max ADC int value: 12bit=4095 ; 10bit=1023

// Vars

// Returns: millivolts as integer - WARN board specific
// Requires: AREF_mV, MAX_ADC
int adcToVolts(int n){
    return n * (AREF_mV / (float)MAX_ADC); // rounding errors are fine
}

// Sets the sensor pin to input
// Requires RAIN_PIN
void setupWetness() {
    // setup analog pin
    pinMode(RAIN_PIN_A0, INPUT);
    // setup digial pin
    // TODO
    pinMode(RAIN_PIN_D0, INPUT_PULLUP); 
}

// returns: an int that is the analog voltage level in MILLIVOLTS or RETURN_NULL if not ready
// requires: ADC capable pin
// n.b.: timing is handled here
// TODO: refactor to interrupt on positive change
int getRain_mV() {
        auto value = analogRead(RAIN_PIN_A0);
        return adcToVolts(value);
}

int getRain_D0() {
    auto value = digitalRead(RAIN_PIN_D0);
    return value;
}

int getRain_A0() {
    auto value = digitalRead(RAIN_PIN_D0);
    return value;
}

// Returns the wetness level as a percentage, based on the analog sensor value 
int getWetness_pct() {
    return float(AREF_mV - getRain_mV())/AREF_mV * 100;
}

int getRain_timed() {
    static uint16_t previous{0}; // type matches analogRead()
    static unsigned long delayWater_ms{4000};
    static unsigned long prevWater_ms{millis()};

    if (millis() - prevWater_ms >= delayWater_ms) {
        prevWater_ms = millis();
        auto value = getRain_mV();

        if (previous - value > 10 || value - previous > 10) {
            previous = value;
            debugln("DEBUG: Water Level Changed");
            return value;
        }
        else return RETURN_NULL;
    }
    else return RETURN_NULL;
}

void loopWetness() {
    
    static int previous_mV{0};
    static int previous_d{0};
    int val_mV = getRain_mV();
    int val_d = getRain_D0();

    if (val_mV != RETURN_NULL && (previous_mV - val_mV > 10 || val_mV - previous_mV > 10)) {
        previous_mV = val_mV;
        debug("Water Sense A0 is {"); debug(val_mV); debugln("}");
    }

    if (val_d != RETURN_NULL && val_d != previous_d) {
        previous_d = val_d;
        debugln();
        debugln();
        debug("Water Sense D0 is {"); debug(val_d); debugln("}");
        debugln();
        debugln();

    }

}

#endif

// Improvements
// Refactor to class based
// Then we can more easily handle timing 
// and have a simple main getRain loop
// see Adafruit unified sensor lib for class based example
// figure out better control flow when the sensor is not 
// ready to update `main` with a value (eg timing not yet met)

/* Example Usage in main.cpp:main()
  loopWetness();   
*/