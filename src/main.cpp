/**
 * @file main.cpp
 * @author Nils Wulff s223968 & Oliver Olsen s205443
 * @brief ThingSpeak Task
 * @version 0.1
 * @date 2025-01-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
const char* ssid = "LW_POCO";
const char* pass = "ezpz42069";
WiFiClient client;
unsigned long channelID = 2808206; //your TS channal
const char * APIKey = "KG5QAN33MVL7CXIR"; //your TS API
const char* server = "api.thingspeak.com";
const int postDelay = 20 * 1000; //post data every 20 seconds

#define button D0 
bool buttonPressed = false; 


void setup() 
{
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  pinMode(button, INPUT_PULLUP); 
}

float data; //measured data



void loop() 
{
  if (digitalRead(button == 0 && buttonPressed == false)) {
    buttonPressed = true; 
  } else 
  if (digitalRead(button == 1 && buttonPressed == true)) {
    buttonPressed = false;
  }

  data = 42.0;
  ThingSpeak.begin(client);
  client.connect(server, 80); //connect(URL, Port)
  ThingSpeak.setField(2, buttonPressed); //set data on the X graph
  ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
  client.stop();
  delay(postDelay); //wait and then post again
}