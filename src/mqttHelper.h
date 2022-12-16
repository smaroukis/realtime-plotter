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
const char* createUnits(const char* sensorType);
const char* createTopicStr(const char* function, const char* sensor, const char* units) ;
const char* createPayload(int value);
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
    Serial.println("connected to MQTT");
    // TODO sub and pub topics
    subscribeAll(mqttClient);
    Serial.println("Subscribed to 'inTopic'");
    // TODO - Create topic for this client's status
    mqttClient.publish("outTopic",  "connected to MQTT");

    if (!mqttClient.connected())
    {
      mqttClientReconnect();
    }
  }
}

void subscribeAll(PubSubClient& mqttClient) {
  // TODO - Subscribe to all topics
  mqttClient.subscribe("inTopic");
}

void mqttClientReconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...to {"); Serial.print(MQTT_SERVER); Serial.println("}");

    if (mqttClient.connect("ESP32_clientID")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      mqttClient.publish("outTopic", "Nodemcu connected to MQTT");
      // ... and resubscribe
        subscribeAll(mqttClient);

    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// ---- CALLBACK -----

// Note topic and payload will be overwritten since the client uses the same buffer for both inbound and outbound messages
void callback(char* topic, byte* payload, unsigned int length) {   //callback includes topic and payload ( from which (topic) the payload is comming)

  // Print topic and payload
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  // If payload is "ON"
  if ((char)payload[0] == 'O' && (char)payload[1] == 'N') //on
  {
    digitalWrite(BUILTIN_LED, HIGH);
    Serial.println("on");
    mqttClient.publish("outTopic", "LED turned ON");
  }

  // If payload is "OFF"
  else if ((char)payload[0] == 'O' && (char)payload[1] == 'F' && (char)payload[2] == 'F') //off
  {
    digitalWrite(BUILTIN_LED, LOW);
    Serial.println("off");
    mqttClient.publish("outTopic", "LED turned OFF");
  }
  // TODO get temperature and publish to `status/temp` 

  Serial.println();
}

// ---- PUBLISH HELPERS ----

// Publish the temperature or water values to the mqtt broker
// Returns FALSE if not published
boolean publishSensorVal(PubSubClient& mqttClient, const char* sensorType, float value) {

  // Create thee units string
  const char* units = createUnits(sensorType);
  if (strcmp(units, "ERROR") == 0) {
    Serial.println("Invalid sensor type");
    return false; // invalid sensor type
  }

    // Create topic and payload, don't forget to clean up dyn. memory
    const char* topic = createTopicStr("status", sensorType, units);
    const char* payload = createPayload(value);
    Serial.print("Publishing to  topic: "); Serial.println(topic);
    Serial.print("Value: "); Serial.println(payload);

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

const char* createPayload(float value){
  char* buffer = new char[6];
  dtostrf(value, 5, 2, buffer); // dtostrf(float, width, precision, buffer)
  return buffer;
}

const char* createUnits(const char* sensorType){
  if(strcmp(sensorType, "temperature") == 0) return "C";
  if(strcmp(sensorType, "water") == 0) return "mV";
  return "ERROR";
}

#endif