// Requires: wifiHelper::<WiFiClient wifiClient> and secrets.h
// After the wifi connection is established, we can connect to the MQTT broker
#ifndef MQTT_HELPER_H
#define MQTT_HELPER_H

#include <PubSubClient.h>
#include "wifiHelper.h"
#include <string.h> // for strcmp
#include "sensorStd.h" // RETURN_NULL is -999
#include "secrets.h" // for MQTT_SERVER, MQTT_PORT

// Mqtt publish string specifics
const char* DEVICE_ID = "esp-01";
const char* GATEWAY_ID = "home";

void callback(char* topic, byte* payload, unsigned int length) ;   //callback includes topic and payload ( from which (topic) the payload is comming)
PubSubClient mqttClient(MQTT_SERVER, MQTT_PORT, callback, wifiClient);

// --- Function Declarations for Compiler ----
void connectMqtt();
void mqttClientReconnect();
const char* createTopicStr(const char* function, const char* sensor, const char* units) ;
const char* createPayload(int value);
const char* createPayload(float value);
void subscribeAll(PubSubClient& mqttClient);

// ---- Manage Connections and Subscriptions -----

// Connect to the MQTT broker and subscribe to topics through the callback
void setupMqtt(){
// TODO delete below once the initialization is tested and working
//   mqttClient.setServer(MQTT_SERVER, 1883);//connecting to mqtt server
//   mqttClient.setCallback(callback);
//   delay(5000);
  connectMqtt();
}

void connectMqtt()
{
  // TODO decide on client ID
  mqttClient.connect("ESP32_clientID");  // ESP will connect to mqtt broker with clientID
  {
    debugln("connected to MQTT");
    // TODO sub and pub topics
    subscribeAll(mqttClient);
    debugln("Subscribed to 'inTopic'");
    // TODO - Create topic for this client's status
    mqttClient.publish("outTopic",  "connected to MQTT");

    if (!mqttClient.connected())
    {
      mqttClientReconnect();
    }
  }
}

void disconnectMqtt()
{
  // Publish a message saying we're disconnecting
  mqttClient.publish("outTopic", "Disconnecting from MQTT");
  mqttClient.disconnect();
}

void subscribeAll(PubSubClient& mqttClient) {
  // TODO - Subscribe to all topics
  mqttClient.subscribe("inTopic");
}

void mqttClientReconnect() {
  while (!mqttClient.connected()) {
    debug("Attempting MQTT connection...to {"); debug(MQTT_SERVER); debugln("}");

    if (mqttClient.connect("ESP32_clientID")) {
      debugln("connected");
      // Once connected, publish an announcement...
      mqttClient.publish("outTopic", "Nodemcu connected to MQTT");
      // ... and resubscribe
        subscribeAll(mqttClient);

    } else {
      debug("failed, rc=");
      debug(mqttClient.state());
      debugln(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// ---- CALLBACK -----

// Note topic and payload will be overwritten since the client uses the same buffer for both inbound and outbound messages
void callback(char* topic, byte* payload, unsigned int length) {   //callback includes topic and payload ( from which (topic) the payload is comming)

  // Print topic and payload
  debug("Message arrived [");
  debug(topic);
  debug("] ");
  for (int i = 0; i < length; i++)
  {
    debug((char)payload[i]);
  }

  // If payload is "ON"
  if ((char)payload[0] == 'O' && (char)payload[1] == 'N') //on
  {
    digitalWrite(BUILTIN_LED, HIGH);
    debugln("on");
    mqttClient.publish("outTopic", "LED turned ON");
  }

  // If payload is "OFF"
  else if ((char)payload[0] == 'O' && (char)payload[1] == 'F' && (char)payload[2] == 'F') //off
  {
    digitalWrite(BUILTIN_LED, LOW);
    debugln("off");
    mqttClient.publish("outTopic", "LED turned OFF");
  }
  // TODO get temperature and publish to `status/temp` 

  debugln();
}

// ---- PUBLISH HELPERS ----

// Publish the temperature or water values to the mqtt broker
// Returns FALSE if not published or if invalid sensor type
// Note defined as float but sometimes we pass in an int
// Although there are limits to the size - see the createPayload function
boolean publishSensorVal(PubSubClient& mqttClient, const char* sensorType, const char* units, float value) {

    // Create topic and payload, don't forget to clean up dyn. memory
    const char* topic = createTopicStr("status", sensorType, units);
    const char* payload = createPayload(value);
    debugf("Publishing to  topic: %s\n", topic);
    debug("\tValue: ");
    debugln(payload);

    // 3) publish
    auto published = mqttClient.publish(topic, payload);

    // Cleanup dyn. alloc. memory from createTopicStr
    delete[] topic;
    delete[] payload;
    return published;
}

// ---- TOPIC STRING CREATION HELPERS ----

// Creates topic string as "<GATEWAY_ID>/<DEVICE_ID>/<function>/<sensor>/<units>"
// WARN - user must clean up dynamic memory when done
const char* createTopicStr(const char* function, const char* sensor, const char* units) {
  // Since we are creating the string on the fly we use dynamic allocation
  // this way we don't have to pass a buffer and length in
  // but we do have to clean up the memory afterwards
  int bLen = 1 + // for trailing null
              strlen(GATEWAY_ID) + 1 
              + strlen(DEVICE_ID) + 1 
              + strlen(function) + 1 
              + strlen(sensor) + 1 
              + strlen(units); // counting w forward slashes e.g. "GATEWAY_ID/DEVICE_ID/status/water\0"
  char* topic = new char[bLen];  
  snprintf(topic, bLen, "%s/%s/%s/%s/%s", GATEWAY_ID, DEVICE_ID, function, sensor, units); // note size of buffer includes the trailing \0

  return topic;
  // FUTURE - make array based to support any amount of N_topics
}

const char* createPayload(int value){
  if(value > 32767) value = RETURN_NULL;
  char* buffer = new char[6];
  sprintf(buffer, "%d", value);
  return buffer;
}

// TODO - probably better to use strings - is this properly null terminated?
const char* createPayload(float value){
  char* buffer = new char[8];
  dtostrf(value, 6, 2, buffer); // dtostrf(float, width, precision, buffer)
  return buffer;
}

void testMqtt() {
  // TODO - Test publishSensorVal
  // TODO - Test createTopicStr
  // TODO - Test createPayload
}

#endif


// WARNING - there are issues for float values above e.g. 1000.0
// overflow in the buffer when trying to create the string
/*
USAGE:
================
setup(){
  setupWifi();
} 
================
loop() {
  if (!mqttClient.connected())
  { 
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Not Connected to WiFi");
      }
    mqttClientReconnect();
  }
  mqttClient.loop(); // MQTT keep alive, callbacks, etc
}

if publishSensorVal(mqttClient, "temperature", "C", 23.5) {
  Serial.println("Published");
} else {
  Serial.println("Not Published");
}
*/