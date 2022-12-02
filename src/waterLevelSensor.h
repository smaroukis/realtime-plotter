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

#ifndef waterLevelSensor_h
#define waterLevelSensor_h

#include <Arduino.h>

// Vars
const float RISING_TRIG_VOLTS{0.1f};
const float FALLING_TRIG_VOLTS{0.05f};

// TODO change pin for ESP32
const int WATER_PIN{54};

namespace waterLevelSensor {
    void setup() {
        Serial.begin(9600);
    }
}

int adcToFloat12(float f){
    // convert a float to the proper ADC value recog. by the ESP
    // for 12 bit = 4095 (e.g. esp)
}

int adcToFloat10(float f){
    // for 10 bit = 1023 (eg mega)
    // Serial.print("Analog integer value is ");
    // Serial.println(f);

    // map(val, fromLow, fromHigh, toLow, toHigh)
}

float floatToAdc(int n){
    // conver the adc value to a float
    // requires: 
    // 
}

float normalizeAdc (int adc_val) {

}

float getWaterLevel() {
    // handle timing here since this will go inside the `main` loop
    // requires: trigger voltage based on 3.3V supply, ADC capable pin
    // modifies:
    // returns: a float that is the analog voltage level (note 3.3V supply)
    pinMode(WATER_PIN, INPUT);

    // TODO check what ESP returns versus Arduinoa
    auto volts = adcToFloat10(analogRead(WATER_PIN)); 




}

#endif