

/*
 * TimeLapseCamera
 *
 * digital camera control
 *
 * Copyright 2013 Greg Norman <normgr00@yahoo.com>
 *
 * This is a test
 */
#include <Wire.h>
#include "RTClib.h"
#include <TimeAlarms.h>
#include <Time.h>
#include <EEPROM.h>

RTC_DS1307 RTC;

int cameraCapasity = 200;                   // 3476 at high resolution 6588 at next lower resolution Number of pictures camera can take before running out of memory.
unsigned long countDownTimerSeconds = 3UL;   //  delay in seconds before starting serries of pictures
unsigned long countDownTimerMin = 0UL;       //  delay in minuets before starting serries of pictures
unsigned long countDownTimerHours = 0UL;     //  delay in hours before starting serries of pictures
unsigned long countDownTimer = 0UL;           // Do not edit. This value is recalculated later.
unsigned long frameIntervalSeconds = 2UL;    //time in seconds between images
unsigned long frameIntervalMin = 0UL;        //time in minuets between images
unsigned long frameIntervalHours = 0UL;      //time in hours between images
unsigned long frameInterval = 0UL;            // Do not edit. This value is recalculated later.
int DailySunrise = 0;                        // Set to 1 if you want to take a picture every moprning
int powerOnPin = 2;                          // Reed relay on digital pin 4
int focusPin = 3;                            // Reed relay on digital pin 5
int shutterPin = 5;                          // Reed relay on digital pin 6
int ledPin = 13;                             // LED connected to digital pin 13
int lightPin = A3;  //define a pin for Photo resistor
int lightRecord = 0;
int lightReading = 0;

void setup()
{
  Serial.begin(57600);
  Wire.begin();
  RTC.begin();
  
   if (! RTC.isrunning()) {
      Serial.println("RTC is NOT running!");
      // following line sets the RTC to the date & time this sketch was compiled
      //RTC.adjust(DateTime(__DATE__, __TIME__));
   }
 
  pinMode(powerOnPin, OUTPUT);      // Set the powerOn pin as an output
  digitalWrite(powerOnPin, LOW);
  pinMode(focusPin, OUTPUT);      // Set the focus pin as an output
  digitalWrite(focusPin, LOW);
  pinMode(shutterPin, OUTPUT);    // Set the shutter pin as an output
  digitalWrite(shutterPin, LOW);
  pinMode(ledPin, OUTPUT);        // Set the LED pin as an output
  digitalWrite(ledPin, LOW);
}


void loop()
{ 
  countDownTimer = countDownTimerMin*60000+countDownTimerHours*3600000+countDownTimerSeconds*1000;  
  frameInterval = frameIntervalHours*3600000+frameIntervalMin*60000+frameIntervalSeconds*1000;
  DateTime now = RTC.now();
  
  if (DailySunrise == 0){ 
      timeDelayTriggeredCamera();}
  else (DailySunrise == 1);{
    Alarm.alarmRepeat(23,51,0,alarmTriggeredCamera);}//When to start taking pictures. Comment out if not in use.
  Serial.println("End of series");
  delay(4000000000);
}

   

void timeStamp(){   
    DateTime now = RTC.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println(); 
    }
    
    
   void Quickinterval(){
      printDelayValues();
      Serial.println("Quickinterval running!");
      digitalWrite(powerOnPin, HIGH);   // Turn on powerOn relay
      delay(500);                       // Wait for camera to boot
      digitalWrite(powerOnPin, LOW);    // Turn off powerOn relay
      delay(4500);     // Wait for camera to boot
      digitalWrite(focusPin, HIGH);   // Focus camera relay 
      delay(60);                     // Cannon requires 600 with flash 50 without flash
      for (int i = 0; i < cameraCapasity; i++) {
        digitalWrite(ledPin, HIGH);     // Turn on activity LED
        timeStamp();
        Serial.println(analogRead(lightPin)); //Write the value of the photoresistor to the serial monitor.
        
        lightReading = analogRead(lightPin);
        lightRecord = map (lightReading, 40, 1023, 0, 255);
        Serial.print("  LightRecord = ");
        Serial.println(lightRecord);
        EEPROM.write(i, lightRecord);
        
        
        
        digitalWrite(shutterPin, HIGH); // Turn on shutter relay
        delay(30);                     // Hold the button for 30 msec
        digitalWrite(shutterPin, LOW);  // Turn off shutter relay
        digitalWrite(ledPin, LOW);      // Turn off activity LED
        delay((1000 * frameIntervalSeconds)-30);    // Wait the required interval before repeating  
      }  
    digitalWrite(powerOnPin, HIGH);   // to shut off camera 
    delay(30);
    digitalWrite(powerOnPin, LOW);   // camera now off 
    }
    
    
    
 void longInterval(){   
     printDelayValues();
     Serial.println("longInterval running!");
     for (int i = 0; i < cameraCapasity; i++) {
        digitalWrite(powerOnPin, HIGH);   // Turn on powerOn relay
        delay(50);                       // Wait for camera to boot
        digitalWrite(powerOnPin, LOW);    // Turn off powerOn relay
        delay(4500);     // Wait for camera to boot
        digitalWrite(focusPin, HIGH);   // Focus camera relay 
        delay(60);                     // Cannon requires 600 with flash 50 without flash
        digitalWrite(ledPin, HIGH);     // Turn on activity LED
        timeStamp();  
        Serial.println(analogRead(lightPin)); //Write the value of the photoresistor to the serial monitor.
        digitalWrite(shutterPin, HIGH); // Turn on shutter relay
        delay(30);                     // Hold the button for 20 msec
        digitalWrite(shutterPin, LOW);  // Turn off shutter relay
        delay(20);
        digitalWrite(focusPin, LOW);   // Focus camera relay 
        digitalWrite(ledPin, LOW);      // Turn off activity LED
        delay((1000 * frameIntervalSeconds)-70);    // Wait the required interval before repeating  
        digitalWrite(powerOnPin, HIGH);   // Turn on powerOn relay
        delay(30);                       // Wait for camera to shut off
        digitalWrite(powerOnPin, LOW);    // Turn off powerOn relay
        delay((1000 * frameIntervalSeconds)-4620);    // Wait the required interval before repeating  
    }  
 }
 
 void printDelayValues(){
   Serial.print("countDownTimer = ");
   Serial.print(countDownTimer);
   Serial.println();
   Serial.print("frameInterval = ");
   Serial.print(frameInterval);
   Serial.println();
 }
 
 void alarmTriggeredCamera(){
      Serial.println("alarmTriggeredCamera running!");
      if (frameInterval<30000){
        Quickinterval();}   
      else{
      longInterval();}
 }
   
 void timeDelayTriggeredCamera(){
         Serial.println("timeDelayTriggeredCamera running!");
    delay(countDownTimer);     // Wait the required interval before starting 
      if (frameInterval<30000){
        Quickinterval();}   
      else{     longInterval();}
 }
