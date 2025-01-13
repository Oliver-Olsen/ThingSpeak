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
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>


#define DHTTYPE   DHT11
#define DHTPIN    D4
#define trigger   D2 //Trigger is set to pin 2
#define echo      D3 //Echo is set to pin 3


DHT dht(DHTPIN, DHTTYPE);


float timer = 0; //Creates a timer
float distance = 0; //Creates a distance variable
float t = 0;

const char* ssid = "LW_POCO";
const char* pass = "ezpz42069";
WiFiClient client;
unsigned long channelID = 2808206; //your TS channal
const char * APIKey = "KG5QAN33MVL7CXIR"; //your TS API
const char* server = "api.thingspeak.com";
const int postDelay = 10 * 1000; //post data every 20 seconds


void setup() 
{
  Serial.begin(115200);
  WiFi.begin(ssid, pass); 

  dht.begin();

  pinMode(trigger, OUTPUT); //Trigger is set as an output
  pinMode(echo, INPUT); //Echo is set as an input
  
 
}



void loop() 
{
  timer = 0; //Timer is reset
  digitalWrite(trigger, HIGH); //Trigger pin is initailized
  delayMicroseconds(10); 
  digitalWrite(trigger, LOW); 
  timer = pulseIn(echo, HIGH); //Time between Trigger LOW and Echo HIGH is found
  if (timer <= 23200){ //Out-of-range is checked, and if in range: 
    timer += 10; //Delay between Trigger HIGH and Trigger LOW added
    distance = timer / 58; //Distance is calculated from time
    Serial.print(distance); //Distance is printed to serial
  } else { //If out of range: 
    Serial.print(distance); //Previous valid distance is printed
  }

  t = dht.readTemperature();
  

  ThingSpeak.begin(client);
  client.connect(server, 80); //connect(URL, Port)
  ThingSpeak.setField(1, distance); //set data on the X graph
  ThingSpeak.setField(2, t); //set data on the X graph
  ThingSpeak.setField(3, WiFi.RSSI()); //set data on the X graph
  ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
  client.stop();
  delay(postDelay); //wait and then post again
}