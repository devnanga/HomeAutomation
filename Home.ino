#include <Wire.h>
#include "rgb_lcd.h"
#include <math.h>

rgb_lcd lcd;

float temperature;
int lightValue;
int soundValue;
bool errorTemp = false;
bool errorLight = false;
bool errorSound = false;
bool errorTempCold = false;
bool errorTempHot = false;
int lights = 0;
int cold = 0;
int hot = 0;
int intruder = 0;

const int B = 4275;               
const int R0 = 100000; 
           
const int pinTempSensor = A0;     
const int pinLightSensor = A1; 
const int pinSoundSensor = A2;
const int pinLEDRed = 9;
const int pinLEDGreen = 8;

void setup() {
  lcd.begin(16, 2); 
  Serial.begin(9600);
}

void loop() {
  temperature = 0;
  temp();
  light();
  sound();
  lightMessages();
  temperatureMessages();
  ok();
  serialErrors();
}

void light() {
  lightValue = analogRead(pinLightSensor);
}

void sound() {
  soundValue = analogRead(pinSoundSensor);
  //Serial.println(soundValue);
  if(soundValue > 500) {
    errorSound = true;
  } else {
    errorSound = false;
  }
}

void temp() {
  int a = analogRead(pinTempSensor);
  float R = 1023.0/((float)a)-1.0;
  R = R0*R;
  temperature = 1.0/(log(R/R0)/B+1/298.15)-303.14; // convert to temperature via datasheet
  delay(100);
}

void blinkLED() {
  analogWrite(pinLEDRed, HIGH);
  delay(500);
  analogWrite(pinLEDRed, LOW);
  delay(500);
} 

void greenLED() {
  analogWrite(pinLEDGreen, HIGH);
}

void screenRed() {
  lcd.setRGB(255,0,0);
}

void screenBlue() {
  lcd.setRGB(0,0,255);
}

void screenNormal() {
  lcd.setRGB(0,50,50);
}

void serialErrors() {
 if (errorSound == false) {
  if (errorLight == true) {
      cold = 0;
      hot = 0;
      intruder = 0;
      if(lights == 0) {  
        Serial.println("Important: Lights are on at home!");
        lights++;
      } else {
        Serial.print("");
      }
  } else if (errorTempCold == true) {
      lights = 0;
      hot = 0;
      intruder = 0;
      if(cold == 0) {
        Serial.println("Important: The temperature at home is low!");
        cold++;
      } else {
        Serial.print("");
      }
  } else if (errorTempHot == true) {
      lights = 0;
      cold = 0;
      intruder = 0;
    if(hot == 0){
      Serial.println("Important: The temperature at home is high!");
      hot++;
    } else {
      Serial.print("");
    }
 } 
 } else {
    lights = 0;
    cold = 0;
    hot = 0;
    if(intruder == 0) {
    Serial.println("IMPORTANT: There was a very loud sound at home! Possible intruder.");
    intruder++;
    } else {
      Serial.print("");
    }
 }
}

void ok() {
  if(errorSound == false) {
    if (errorTemp == false && errorLight == false) {
      lcd.clear();
      analogWrite(pinLEDGreen, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("Everything is ok");
      lcd.setCursor(1,1);
      lcd.print("Temp = ");
      lcd.print(temperature);
      lcd.print("C");
      screenNormal();
    }
  }
}

void lightMessages() {
  if(lightValue > 500) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lights are on!");
    screenRed();
    blinkLED();
    errorLight = true;
  } else {
    errorLight = false;
  } 
}

void temperatureMessages() {
  if (errorSound == false) {
    if (temperature < 20) {
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Extreme Cold!");
      screenBlue();
      blinkLED();
      errorTemp = true;
      errorTempCold = true;
      errorTempHot = false;
    } else if (temperature > 30) {
      lcd.clear();    
      lcd.setCursor(0,1);
      lcd.print("Extreme Heat!");
      screenRed();
      blinkLED();
      errorTemp = true;
      errorTempHot = true;
      errorTempCold = false;
    } else {
      errorTemp = false;
      errorTempHot = false;
      errorTempCold = false;
    }
  } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("LOUD SOUND");
      lcd.setCursor(0,1);
      lcd.print("DETECTED!");
      screenRed();
      blinkLED();
      delay(5000);
      if (soundValue < 500) {
        errorSound = false;
      } else {
        errorSound = true;
      }
  }
}
