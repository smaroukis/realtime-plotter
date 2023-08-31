// Most of the code is in mqttHelper.h
// Here we just include the necessary WiFi libraries for ESP32
// a global WiFiClient object is created
// and we initialize the WiFi connection with the secrets.h file
// Note for some wifi issues see the return code from mqttHelper::reconnect()
#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif

// SECRET_SSID, SECRET_PASS, DEVICE_ID, GATEWAY_ID
#include "secrets.h"

// Wifi 
WiFiClient wifiClient;

// Setup WiFi client on the esp32
// Requires secrets.h
// WiFi client is global
void setupWifi(){
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  debugln("Wifi connected");
}

#endif