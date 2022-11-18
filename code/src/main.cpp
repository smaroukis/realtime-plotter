/*
  Basic ESP32 MQTT example
  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP32 board/library.
  It connects to an MQTT server then:
  - publishes "connected to MQTT" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the "ON" payload is send to the topic "inTopic" , LED will be turned on, and acknowledgement will be send to Topic "outTopic"
  - If the "OFF" payload is send to the topic "inTopic" , LED will be turned OFF, and acknowledgement will be send to Topic "outTopic"
  It will reconnect to the server if the connection is lost using a
  reconnect function.
*/

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

#include "secrets.h"
// put ssid and pwd in secrets.h as SECRET_SSID and SECRET_PASS

WiFiClient espClient;
PubSubClient client(espClient); //lib required for mqtt

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
    client.publish("outTopic", "LED turned ON");
  }

  // If payload is "OFF"
  else if ((char)payload[0] == 'O' && (char)payload[1] == 'F' && (char)payload[2] == 'F') //off
  {
    digitalWrite(BUILTIN_LED, LOW);
    Serial.println(" off");
    client.publish("outTopic", "LED turned OFF");
  }

  Serial.println();
}


void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32_clientID")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "Nodemcu connected to MQTT");
      // ... and resubscribe
      client.subscribe("inTopic");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void connectmqtt()
{
  client.connect("ESP32_clientID");  // ESP will connect to mqtt broker with clientID
  {
    Serial.println("connected to MQTT");
    client.subscribe("inTopic"); //topic=Demo
    client.publish("outTopic",  "connected to MQTT");

    if (!client.connected())
    {
      reconnect();
    }
  }
}


void setup()
{
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.println("connected");
  client.setServer(MQTT_SERVER, 1883);//connecting to mqtt server
  client.setCallback(callback);
  //delay(5000);
  connectmqtt();
}


void loop()
{
  // put your main code here, to run repeatedly:
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}

