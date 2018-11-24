#include<Wire.h>

#define SPEED_1      5 
#define DIR_1        4
 
#define SPEED_2      6
#define DIR_2        7

const int divePin = 8, humDirtPin = A1, photoResPin;
unsigned long drinkTime, ventChangeTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(divePin, INPUT);
  pinMode(SPEED_1, OUTPUT);
  pinMode(DIR_1, OUTPUT);
  pinMode(SPEED_2, OUTPUT);
  pinMode(DIR_2, OUTPUT);
  pinMode(humDirtPin, INPUT);
  randomSeed(A3);
  digitalWrite(DIR_1, LOW);
  digitalWrite(DIR_2, HIGH);
}

struct resData{
  int arr[100];
  byte sign = 0;
  unsigned long timeForCheck;
  int deltaTime;
};

void loop() {
  
  byte pumpPWM = 170, ventPWM;
  //bool drinkNeed = false;
  
  if(digitalRead(divePin)){//////////////Poplavok
    Serial.println("Poplavok!!!");
  }
  
  Serial.print("Hum: "); Serial.println(analogRead(humDirtPin));
  if(analogRead(humDirtPin) >= 50){//////Dirt Humadity Checker. Don't forget to change
    drinkTime = millis();
    Serial.print("change: ");  Serial.print(drinkTime); Serial.print(" from ");  Serial.println(millis());
    analogWrite(SPEED_1, pumpPWM);
  }
  Serial.print("DT: ");  Serial.println(drinkTime);
  if(millis() - drinkTime >= 5000){
    Serial.print("time: ");  Serial.print(drinkTime); Serial.print(" to ");  Serial.println(millis());Serial.print(" is delta: ");  Serial.println(millis() - drinkTime);
    analogWrite(SPEED_1, 0);
  }
  
  ventChangeTime = vent_changePWM(ventChangeTime);
  
  Serial.println("-------------------------------------\n");
}

unsigned long vent_changePWM(unsigned long vTime){
  if(millis() - vTime >= 2000){
    analogWrite(SPEED_2, random(50, 256));
    Serial.println("AAAAAAAA");
    return millis();
  }
  else
    return vTime;
}
