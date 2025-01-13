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

#define trigger D2 //Trigger is set to pin 2
#define echo D3 //Echo is set to pin 3
#define placa "Arduino"
#define Voltage_Resolution 5
#define pin ADC0 //Analog input 0 of your arduino
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm  


float timer = 0; //Creates a timer
float distance = 0; //Creates a distance variable

const char* ssid = "LW_POCO";
const char* pass = "ezpz42069";
WiFiClient client;
unsigned long channelID = 2808206; //your TS channal
const char * APIKey = "KG5QAN33MVL7CXIR"; //your TS API
const char* server = "api.thingspeak.com";
const int postDelay = 20 * 1000; //post data every 20 seconds

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

void setup() 
{
  Serial.begin(115200);
  WiFi.begin(ssid, pass); 

  pinMode(trigger, OUTPUT); //Trigger is set as an output
  pinMode(echo, INPUT); //Echo is set as an input

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

  MQ135.update();
  MQ135.readSensor();
  MQ135.serialDebug();

  ThingSpeak.begin(client);
  client.connect(server, 80); //connect(URL, Port)
  ThingSpeak.setField(1, distance); //set data on the X graph
  ThingSpeak.setField(2, MQ135.readSensor()); //set data on the X graph
  ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
  client.stop();
  delay(postDelay); //wait and then post again
}