
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
#include <PubSubClient.h>
#include <string.h> // for strcmp

// SECRET_SSID, SECRET_PASS, DEVICE_ID, GATEWAY_ID
#include "secrets.h"

// Custom Headers
#include "sensorStd.h" // RETURN_NULL is -999
#include "temperatureSensor.h"
#include "waterLevelSensor.h"

// Mqtt publish string specifics
const char* DEVICE_ID = "esp-01";
const char* GATEWAY_ID = "home";

// Wifi 
WiFiClient espClient;
PubSubClient mqttClient(espClient); //lib required for mqtt

// Enum for sensors
// TODO

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


void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    Serial.print("from {");
    Serial.print(espClient.localIP());
    Serial.print("}...");
    Serial.print("to {");
    Serial.print(MQTT_SERVER);
    Serial.println("}");
    if (mqttClient.connect("ESP32_clientID")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      mqttClient.publish("outTopic", "Nodemcu connected to MQTT");
      // ... and resubscribe
      mqttClient.subscribe("inTopic");

    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void connectMqtt()
{
  mqttClient.connect("ESP32_clientID");  // ESP will connect to mqtt broker with clientID
  {
    Serial.println("connected to MQTT");
    // TODO sub and pub topics
    mqttClient.subscribe("inTopic"); //topic=Demo
    Serial.println("Subscribed to 'inTopic'");
    // TODO - log which topics we are connected to
    mqttClient.publish("outTopic",  "connected to MQTT");

    if (!mqttClient.connected())
    {
      reconnect();
    }
  }
}

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

void setup()
{
  // Setup Serial Monitor and Hardware
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

// Setup Wifi and MQTT
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.println("Wifi connected");

  mqttClient.setServer(MQTT_SERVER, 1883);//connecting to mqtt server
  mqttClient.setCallback(callback);
  delay(5000);
  connectMqtt();

// Setup Sensor)s
  dhtSetup();
}


void loop()
{
  // Handle WiFi connection
  if (!mqttClient.connected())
  { 
    if (WiFi.status() != WL_CONNECTED) Serial.println("Not Connected to WiFi");
    WiFi.mode(WIFI_STA); // from https://github.com/knolleary/pubsubclient/issues/203
    reconnect();
  }

  mqttClient.loop(); // MQTT keep alive, callbacks, etc

  // Nonblocking code to print "in loop" every 5 seconds
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    Serial.println("in loop");
  }

  // Uncomment to above TODO 

  // delay handled in temperatureSensor.cpp
  // if delay has not been met, RETURN_NULL is returned
  /*
  auto temperature = getTemperature();
  if ((temperature != RETURN_NULL) && (!isnan(temperature))) 
  {
    // publishSensorVal returns mqttClient.publish() which is false if failed
   if (!publishSensorVal(mqttClient, "temperature", temperature))
      { Serial.println("Error publishing temperature"); }
  }

  auto waterLvl = getWaterLvl_mv();
  if (waterLvl != RETURN_NULL)
  {
   if (!publishSensorVal(mqttClient, "water", waterLvl))
      { Serial.println("Error publishing water level"); }
  }
  */
}

/*  Debugging Checklist
1) Check pin numbers in code
2) Check breadboarding
3) 
*/

/* Improvements
- see FUTURE comments
*/