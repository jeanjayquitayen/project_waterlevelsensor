//INCLUDED LIBRARIES
#include "GPRS_Shield_Arduino.h"
#include <SoftwareSerial.h>
#include <Wire.h>




//***********************************************//
//        GSM/GPRS SEND AND READ SMS             //
//                                               //
//  This sketch is used to test              //
//  e-Gizmo SIM800,800L,900D,900 modules/kits  //
//  Wiring Connection:                           //
//  SIM800/900 modules to UARTPORT/gizDuino MCUs //
//    RXD - TX(D3)           //
//    TXD - RX(D2)           //
//    GND - GND              //
//  by e-Gizmo Mechatronix Central           //
//          http://www.egizmo.com                //
//***********************************************//



//SERIAL PIN ASSIGNMENT, BAUDRATE, PHONE NUMBER, MESSAGE
#define PIN_TX    3
#define PIN_RX    2
#define BAUDRATE  9600
#define PHONE_NUMBER "09xxxxxxxxx"       //CELLPHONE NUMBER
#define MESSAGE  "Test Message"  //COMPOSE YOUR MESSAGE HERE
#define MESSAGE_LENGTH 160
#define echoPin 7 // Echo Pin
#define trigPin 8 // Trigger Pin
#define Alarmpin 13 // Onboard LED

bool flagLow = false;
bool flagMid = false;
bool flagCritical = false;
char message[MESSAGE_LENGTH];
int messageIndex = 0;
char phone[16];
char datetime[24];

//JSN Parameters
int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance
float MeasuredDistance = 0.0;
GPRS GSMTEST(PIN_RX,PIN_TX,BAUDRATE);//RX,TX,BAUDRATE

void setup() {

  Serial.begin(9600);
  Serial.setTimeout(10);
  //  /pinMode(7,OUTPUT);
  while(!GSMTEST.init()) {
      delay(1000);
      //digitalWrite(7,LOW);
      Serial.print("INIT ERROR\r\n");
  } 
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
   pinMode(Alarmpin,OUTPUT);

   
  //GSMTEST.sendSMS(PHONE_NUMBER,MESSAGE); //DEFINE PHONE NUMBER AND TEXT
  delay(1000);
}

void loop() {
     MeasuredDistance = measureDistance();
     messageIndex = GSMTEST.isSMSunread();
   if (messageIndex > 0) { //AT LEAST, THERE IS ONE UNREAD SMS
      GSMTEST.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);           
      //IN ORDER NOT TO FUL SIM Memory, IS BETTER TO DELETE IT
      GSMTEST.deleteSMS(messageIndex);
      //Serial.print("FROM NUMBER: ");
      Serial.println(phone);  
      //Serial.print("DATE/TIME");
      Serial.println(datetime);        
      //Serial.print("RECEIVED MESSAGE: ");
      Serial.println(message);


   }

   if(MeasuredDistance >= 60.0 && MeasuredDistance < 90.0){
      if(flagLow == false){
        flagLow = true;
        //GSMTEST.sendSMS(PHONE_NUMBER,MESSAGE); //DEFINE PHONE NUMBER AND 
      }

      if(flagMid == true){
        flagMid = false;
      }
      
   }
   else if(MeasuredDistance >= 90.0 && MeasuredDistance < 120.0){
      if(flagMid == false){
        flagMid = true;
        //GSMTEST.sendSMS(PHONE_NUMBER,MESSAGE); //DEFINE PHONE NUMBER AND
        
      }
      if(flagCritical == true){
        flagCritical = false;
      }
       
   }

   else if(MeasuredDistance >= 120.0){
    if(flagCritical == false){
      flagCritical = true;
      digitalWrite(Alarmpin,HIGH);
      //GSMTEST.sendSMS(PHONE_NUMBER,MESSAGE); //DEFINE PHONE NUMBER AND
    }
     
   }

   else{
    if(flagLow == true){
      flagLow = false;
    }
   }

}





float measureDistance(){

    /* The following trigPin/echoPin cycle is used to determine the
   distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin, LOW); 
 delayMicroseconds(2); 

 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin, LOW);
 duration = pulseIn(echoPin, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance = duration/58.2;
 
 if (distance >= maximumRange || distance <= minimumRange){
   return -1.0;

 } else {
   
   return distance;

 }
 
 //Delay 50ms before next reading.
 delay(50);
  
}
