#include<Wire.h>

#define SPEED_1      5 
#define DIR_1        4
 
#define SPEED_2      6
#define DIR_2        7

const int divePin = 8, humDirtPin = A1, photoResPin;

void setup() {
  Serial.begin(9600);
  pinMode(divePin, INPUT);
  pinMode(SPEED_1, OUTPUT);
  pinMode(DIR_1, OUTPUT);
  pinMode(SPEED_2, OUTPUT);
  pinMode(DIR_2, OUTPUT);
  pinMode(humDirtPin, INPUT);
  digitalWrite(DIR_1, LOW);
}

struct resData{
  int arr[100];
  byte sign = 0;
  unsigned long timeForCheck;
  int deltaTime;
};

void loop() {
  
  byte pumpPWM = 70;
  long drinkTime;
  bool drinkNeed = false;
  
  if(digitalRead(divePin)){//////////////Poplavok
    Serial.println("Poplavok!!!");
  }

  //analogWrite(SPEED_1, pumpPWM);
  Serial.print("Hum: "); Serial.println(analogRead(humDirtPin));
  if(analogRead(humDirtPin) >= 50){//////Dirt Humadity Checker
    Serial.println("START");
    drinkTime = millis();
    analogWrite(SPEED_1, pumpPWM);
  }
  if(millis() - drinkTime >= 500){
    Serial.println("END");
    analogWrite(SPEED_1, 0);
  }
  
  
  Serial.println("-------------------------------------\n");
}

