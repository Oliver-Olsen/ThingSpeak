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
#include <MQUnifiedsensor.h>

#define button D0 
#define placa "Arduino"
#define Voltage_Resolution 5
#define pin ADC0 //Analog input 0 of your arduino
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm  



const char* ssid = "LW_POCO";
const char* pass = "ezpz42069";
WiFiClient client;
unsigned long channelID = 2808206; //your TS channal
const char * APIKey = "KG5QAN33MVL7CXIR"; //your TS API
const char* server = "api.thingspeak.com";
const int postDelay = 20 * 1000; //post data every 20 seconds


bool buttonPressed = false; 


MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);



void setup() 
{
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  pinMode(button, INPUT_PULLUP); 
  MQ135.setRegressionMethod(1);
  MQ135.setA(102.2); MQ135.setB(-2.473);
  MQ135.init();

  // Calibration code form MQ135 LIB
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ135.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 
  MQ135.serialDebug(true);
  // End Calibration
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
  MQ135.update();
  MQ135.readSensor();
  MQ135.serialDebug();

  data = 42.0;
  ThingSpeak.begin(client);
  client.connect(server, 80); //connect(URL, Port)
  ThingSpeak.setField(2, buttonPressed); //set data on the X graph
  ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
  client.stop();
  delay(postDelay); //wait and then post again
}