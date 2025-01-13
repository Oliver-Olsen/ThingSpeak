#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
const char* ssid = "WiFi SSID";
const char* pass = "Password";
WiFiClient client;
unsigned long channelID = 424242; //your TS channal
const char * APIKey = "ABCD1234"; //your TS API
const char* server = "api.thingspeak.com";
const int postDelay = 20 * 1000; //post data every 20 seconds


void setup() 
{
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
}

float data; //measured data



void loop() 
{
  data = 42.0;
  ThingSpeak.begin(client);
  client.connect(server, 80); //connect(URL, Port)
  ThingSpeak.setField(X, data); //set data on the X graph
  ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
  client.stop();
  delay(postDelay); //wait and then post again
}