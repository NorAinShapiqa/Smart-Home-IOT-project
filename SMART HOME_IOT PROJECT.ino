#include "Arduino.h"
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h> //LCD Library
#include <SPI.h> //RFID card reader library
#include <RFID.h> //RFID card reader library
#include <Wire.h> // LCD library 4 wires library
#include <dht11.h>

#define SDA_DIO 9
#define RESET_DIO 8
#define DHT11PIN A2
#define ventilateurPIN 10 // Pin -> for Fan

/*
//lED bLUETOOTH
#define ledBath 31
#define ledBed 33
#define ledKitc 35
#define ledLiv 37
#define ledStu 39

const byte rxPin = A6;
const byte txPin = A7;
SoftwareSerial BTSerial(rxPin, txPin); // RX TX
*/

RFID rfid(SDA_DIO, RESET_DIO);
String rfidCard;

dht11 DHT11;

const int ledPin = 11;   //the number of the LED pin
const int ldrPin = A4;  //the number of the LDR pin

const int ledpin=13; // ledpin,flamepin and buzpin are not changed throughout the process
const int flamepin=A3;
const int buzpin=12;
const int threshold=100;// sets threshold value for flame sensor
int flamesensvalue=0; // initialize flamesensor reading 

LiquidCrystal_I2C lcd(0x27, 16, 2); // SDA A4, SCL A5, GDN GND, VCC 5V

void setup() {
  Serial.begin(9600);
  //BTSerial.begin(9600);
  Serial.println("Starting the RFID Reader...");
  SPI.begin();
  rfid.init();
 
  //rfid
  pinMode(7, OUTPUT);//buzzer
  pinMode(A0, OUTPUT);//led True
  pinMode(A1, OUTPUT);//led False

  //Dht
  pinMode(ventilateurPIN,OUTPUT);//relay Fan
  
  //Photoresistor
  pinMode(ledPin, OUTPUT);  //initialize the LED pin as an output
  pinMode(ldrPin, INPUT);   //initialize the LDR pin as an input

  //flame
  pinMode(ledpin,OUTPUT); 
  pinMode(flamepin,INPUT); 
  pinMode(buzpin,OUTPUT); 

  
  /*// define pin modes for tx, rx, led bluetooth:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  pinMode(ledBath, OUTPUT); 
  pinMode(ledBed, OUTPUT); 
  pinMode(ledKitc, OUTPUT); 
  pinMode(ledLiv, OUTPUT); 
  pinMode(ledStu, OUTPUT);
  */

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Scan your card");
  lcd.setCursor(0,1);
  lcd.print("on the Reader...");

}
/*String messageBuffer = "";
String message = "";
*/

void loop() {
 
 /* //bluetooth led
  while (BTSerial.available() > 0) {
    char data = (char) BTSerial.read();
    messageBuffer += data;
  
   pinMode(ledBath, OUTPUT); 
    pinMode(ledBed, OUTPUT); 
    pinMode(ledKitc, OUTPUT); 
    pinMode(ledLiv, OUTPUT); 
    pinMode(ledStu, OUTPUT);
  
    if(data == 'a'){ 
      digitalWrite(14, HIGH);
    }
    if(data == 'b'){ 
      digitalWrite(14, LOW);
    } 
    if(data == 'c'){
      digitalWrite(15, HIGH);
    }
    if(data == 'd'){ 
      digitalWrite(15, LOW);
    }
    if(data == 'e'){ 
      digitalWrite(16, HIGH);
    }
    if(data == 'f'){ 
      digitalWrite(16, LOW);
    }
    if(data == 'g'){ 
      digitalWrite(17, HIGH);
    }
    if(data == 'h'){ 
      digitalWrite(17, LOW);
    }
    if(data == 'i'){ 
      digitalWrite(18, HIGH);
    }
    if(data == 'j'){ 
      digitalWrite(18, LOW);
    }
  }
*/  
//rfid
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.println(rfidCard);
      if (rfidCard == "85 205 177 42") {
        Serial.println("********************");
        Serial.println("Door Unlocked");
        Serial.println("********************");
        delay (200);
        //for LCD
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Door Unlocked");
        delay (1000);
        
        //Buzzer & GREEN LED(TRUE)
        digitalWrite(A0, HIGH);
        tone(7,1000);
        delay (500);
        digitalWrite(A0, LOW);
        noTone(7);
        delay(5000);
        
        digitalWrite(A1, HIGH);
        tone(7,1000);
        delay (500);
        digitalWrite(A1, LOW);
        noTone(7);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Door Locked");
        delay (1000);

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Scan your card");
        lcd.setCursor(0,1);
        lcd.print("on the Reader...");
      }
      else {
        Serial.println("****************");
        Serial.println("Access Denied");
        Serial.println("****************");
        //for LCD
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Access Denied");
        lcd.setCursor(0,1);
        lcd.print("Try Again");
        delay (1000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Scan your card");
        lcd.setCursor(0,1);
        lcd.print("on the Reader...");

        //Buzzer & RED LED(FALSE)
        digitalWrite(A1, HIGH);
        tone(7,1000);
        delay (500);
        digitalWrite(A1, LOW);
        noTone(7);
        return;
      }
    }
    rfid.halt();
  }

  //DHT Sensor
  int chk = DHT11.read(DHT11PIN);

  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);
  Serial.print("Temperature (°C): ");
  Serial.println((float)DHT11.temperature, 2);
  
  if ((float)DHT11.temperature >23) {
    digitalWrite(ventilateurPIN,HIGH); // the fan starts to turn
  }
  else {
    digitalWrite(ventilateurPIN,LOW); // the fan stops
  }

//photoresistor sensor
  int ldrStatus = analogRead(ldrPin);   //read the status of the LDR value

  //check if the LDR status is <= 25    ` ` ` ``  `                     ``    
  //if it is, the LED is HIGH

    if (ldrStatus<=8) {
      digitalWrite(ledPin, HIGH);               //turn LED on
      Serial.println("LDR is DARK, LED is ON");
      delay(1000);
    }
    else {
      digitalWrite(ledPin, LOW);          //turn LED off
      Serial.println("LDR is BRIGHT, LED is OFF");
    }

//flame sensor
  flamesensvalue=analogRead(flamepin); // reads analog data from flame sensor 
  if (flamesensvalue<=threshold) { // compares reading from flame sensor with the threshold value 
    digitalWrite(ledpin,HIGH); //turns on led and buzzer 
    tone(buzpin,1000); 
    delay(100); //stops program for 1 second 
  } 
  else{ 
    digitalWrite(ledpin,LOW); //turns led off led and buzzer 
    noTone(buzpin); 
  } 
  
}