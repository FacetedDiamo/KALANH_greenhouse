#include <TroykaDHT.h>
#include <TroykaLedMatrix.h>
#include <QuadDisplay.h>
#include <TroykaRTC.h>
#include <Wire.h>

#define SPEED_1      5
#define DIR_1        4

#define SPEED_2      6
#define DIR_2        7

#define DISPLAY_PIN 9

#define LEN_TIME 12
#define LEN_DATE 12
#define LEN_DOW 12

#define AUDIO 11

#define LED_RELAY 2
//#define LED_RESIST 3

#define DEMO_BUTTON 3

char time[LEN_TIME];
char date[LEN_DATE];
char weekDay[LEN_DOW];

const byte divePin = 8, humDirtPin = A1, photoResPin, states_button = 5;
unsigned long drinkTime, ventChangeTime = 0, massageTime = 0, warningTime;
float time_f;
bool displayOutput_change = false, warningNeed = false;
int freq = 1000;
byte matrix_cnt = 0, button_state = 0;

DHT dht(12, DHT22);
TroykaLedMatrix matrix;
RTC clock;

const uint8_t pic_thermometer[] {
  0b00000011,
  0b00000101,
  0b00001010,
  0b00010100,
  0b00101000,
  0b01010000,
  0b01100000,
  0b10000000,
};

const uint8_t pic_drop[] {
  0b00010000,
  0b00010000,
  0b00111000,
  0b01110100,
  0b11110110,
  0b11111110,
  0b01111100,
  0b00111000,
};

const uint8_t pic_clocks[] {
  0b00111000,
  0b01010100,
  0b10010010,
  0b11011110,
  0b10000010,
  0b01010100,
  0b00111000,
  0b00000000,
};

const uint8_t pic_alarm[] {
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00000000,
  0b00011000,
  0b00011000,
};

void setup() {
  Serial.begin(9600);
  matrix.begin();
  matrix.clear();
  clock.begin();
  clock.set(8, 59, 45, 17, 11, 2018, SUNDAY);
  Serial.println("AaAAAA");
  dht.begin();
  pinMode(SPEED_1, OUTPUT);
  pinMode(DIR_1, OUTPUT);
  pinMode(SPEED_2, OUTPUT);
  pinMode(DIR_2, OUTPUT);
  pinMode(LED_RELAY, OUTPUT);
  pinMode(AUDIO, OUTPUT);
  pinMode(divePin, INPUT);
  pinMode(humDirtPin, INPUT);
  //pinMode(LED_RESIST, INPUT);
  pinMode(DEMO_BUTTON, INPUT_PULLUP);
  attachInterrupt(DEMO_BUTTON - 2, change_buttonState, RISING);
  digitalWrite(DIR_1, LOW);
  digitalWrite(DIR_2, HIGH);
  randomSeed(A3);
}

/*struct resData{
  int arr[100];
  int sign = 0;
  unsigned long timeForCheck;
  int deltaTime;
  };*/

void loop() {

  byte pumpPWM = 170, ventPWM;
  bool drinkNeed = false, lightNeed = false;

  switch (button_state) {
    default:
      digitalWrite(LED_RELAY, LOW);
      digitalWrite(SPEED_1, LOW);
      digitalWrite(SPEED_2, LOW);
      break;
    case 4:
      if (digitalRead(divePin)) { //////////////Poplavok
        //Serial.println("Poplavok!!!");
        drinkNeed = true;
        if (!warningNeed) {
          warningNeed = true;
          warningTime = millis() - 5500;
        }
      } else {
        warningNeed = false;
      }
      warning_alarm();
    /*case 4:
      warningNeed = true;
      warningTime = millis() - 5500;*/
    case 3:
    if(button_state == 3){
      warningNeed = true;
      warningTime = millis();}
    case 2:
      /*if (digitalRead(divePin)) { //////////////Poplavok
        //Serial.println("Poplavok!!!");
        drinkNeed = true;
        if (!warningNeed) {
          warningNeed = true;
          warningTime = millis();
        }
      } else {
        warningNeed = false;
      }*/

      if (analogRead(humDirtPin) <= 50) { //////Dirt Humadity Checker. Turn on the pump
        drinkTime = millis();
        analogWrite(SPEED_1, pumpPWM);
      }
      if (millis() - drinkTime >= 5000) { //////Turn off the pump
        analogWrite(SPEED_1, 0);
      }

      ventChangeTime = vent_changePWM(ventChangeTime);//Randomize turning on function for vent
      //warning_alarm();

      clock.read();
      clock.getTimeStamp(time, date, weekDay);
      /*time_f =  ((time[3] - '0') * 10) + ((time[4] - '0'));  time_f *= 100;
      time_f += ((time[6] - '0') * 10) + (time[7] - '0');
      time_f /= 10000;
      float time_h = ((time[0] - '0') * 10) + ((time[1] - '0'));
      time_f += time_h;*/
      if (time_f >= 9.0000 && time_f <= 22.0000) {
        digitalWrite(LED_RELAY, HIGH);
        lightNeed = true;
      } else {
        digitalWrite(LED_RELAY, LOW);
      }


      if (millis() - massageTime >= 2500) {
        massageTime = millis();
        //Serial.println("!!!");
        clock.read();
        //Serial.println("???");
        clock.getTimeStamp(time, date, weekDay);
        //Serial.println("LLL");
        //matrix.drawBitmap(thermometer);
        //Serial.print(time[0] - '0'); Serial.print(" "); Serial.print(time[1] - '0'); Serial.print(" "); Serial.print(time[3] - '0'); Serial.print(" ");
        //Serial.print(time[4] - '0'); Serial.print(" "); Serial.print(time[6] - '0'); Serial.print(" "); Serial.print(time[7] - '0'); Serial.print(" ");
        //Serial.print("Time to long: ");  Serial.println(time_long);
        Serial.print("System "); if (!drinkNeed) Serial.print("don't "); Serial.println("need water");
        Serial.print("Dirt hum: "); Serial.println(analogRead(humDirtPin));
        dht_print();///////////////////////////Check and print out DHT data
        //Serial.print("Light: "); Serial.println(analogRead(LED_RESIST));
        Serial.print("Light is turn "); if (lightNeed) Serial.println("on"); else Serial.println("off");
        //Serial.print("Time: "); Serial.println(time);
        Serial.println("-------------------------------------\n");
      }
        
    case 1:
      ventPWM = vent_changePWM(ventPWM);
      digitalWrite(LED_RELAY, HIGH);
      //analogWrite(SPEED_1, pumpPWM);
      analogWrite(SPEED_2, ventPWM);
  }
}

  /*
     VENT
  */

  unsigned long vent_changePWM(unsigned long vTime) {
    if (millis() - vTime >= 2000) {
      analogWrite(SPEED_2, random(50, 256));
      return millis();
    }
    else
      return vTime;
  }

  /*
     COUT
  */

  void dht_print() {
    dht.read();
    bool data_error = false;
    int temp = dht.getTemperatureC();
    float hum = dht.getHumidity();
    displayClear(DISPLAY_PIN);
    //Serial.print("Et: ");   Serial.print(temp);   Serial.print("; Eh:");   Serial.println(hum);

    if (!displayOutput_change) {
      //float time_f = (time[0] - '0') * 1000 + (time[1] - '0') * 100 + (time[2] - '0') * 10 + time[3] - '0';
      //displayFloat(DISPLAY_PIN, time_f / 100, 2, true);
      //Serial.print("FLOAT: ");   Serial.println(time_f);
      displayInt(DISPLAY_PIN, -123);
    }

    switch (dht.getState()) {
      // всё OK
      case DHT_OK:
        Serial.print("Temp: ");   Serial.print(temp); Serial.println("'C");
        //if(displayOutput_change) displayTemperatureC(DISPLAY_PIN, temp);
        Serial.print("Air hum: ");   Serial.print(hum); Serial.println("%");
        break;
      // ошибка контрольной суммы
      case DHT_ERROR_CHECKSUM:
        Serial.println("Checksum error");
        data_error = true;
        if (displayOutput_change) displayDigits(DISPLAY_PIN, QD_E, QD_r, QD_r, QD_NONE);
        break;
      // превышение времени ожидания
      case DHT_ERROR_TIMEOUT:
        Serial.println("Time out error");
        data_error = true;
        if (displayOutput_change) displayDigits(DISPLAY_PIN, QD_E, QD_r, QD_r, QD_NONE);
        break;
      // данных нет, датчик не реагирует или отсутствует
      case DHT_ERROR_NO_REPLY:
        Serial.println("Sensor not connected");
        data_error = true;
        if (displayOutput_change) displayDigits(DISPLAY_PIN, QD_E, QD_r, QD_r, QD_NONE);
        break;
    }

    tui_print(temp, hum, data_error);

    displayOutput_change = !displayOutput_change;
  }

  /*
     TUI
  */

  void tui_print(int temp, float hum, bool data_error) {
    matrix.clear();
    if (warningNeed && (millis() - warningTime) >= 5000) {
      matrix.drawBitmap(pic_alarm);
      displayDigits(DISPLAY_PIN, QD_H, QD_E, QD_L, QD_P);
    }
    else if (data_error && matrix_cnt <= 1) {
      matrix.drawBitmap(pic_alarm);
      displayDigits(DISPLAY_PIN, QD_E, QD_r, QD_r, QD_NONE);
    }
    else {
      if (warningNeed) matrix_cnt = 3;
      switch (matrix_cnt) {
        case 0:
          //Serial.println("AAAAAAAAAAAAAAAAAAA?");
          matrix.drawBitmap(pic_thermometer);
          displayTemperatureC(DISPLAY_PIN, temp);
          break;
        case 1:
          matrix.drawBitmap(pic_drop);
          hum /= 100;
          //Serial.println(hum);
          displayFloat(DISPLAY_PIN, hum, 3, true);
          break;
        case 2:
          matrix.drawBitmap(pic_clocks);
          displayFloat(DISPLAY_PIN, time_f, 2, true);
          //displayDigits(DISPLAY_PIN, QD_n, QD_O, QD_n, QD_E);
          break;
        case 3:
          matrix.drawBitmap(pic_alarm);
          displayDigits(DISPLAY_PIN, QD_F, QD_I, QD_L, QD_L);
          break;
      }
      matrix_cnt = (matrix_cnt + 1) % 4;
    }
  }

  /*
     ALARM
  */

  void warning_alarm() {
    if (warningNeed && (millis() - warningTime) >= 5000) {
      long phase = (millis() - (warningTime + 5000)) % 2000 ;
      if (phase <= 250)
        tone(AUDIO, freq + 150);
      else if (phase <= 500)
        tone(AUDIO, freq);
      else if (phase <= 750)
        tone(AUDIO, freq - 150);
      else if (phase <= 1000)
        tone(AUDIO, freq + 150);
      else if (phase <= 1250)
        tone(AUDIO, freq);
      else if (phase <= 1500)
        tone(AUDIO, freq - 150);
      else if (phase <= 1750)
        tone(AUDIO, freq);
      else
        tone(AUDIO, freq + 250);
    }
    else
      noTone(AUDIO);
  }

  /*
     BUTTON
  */

  void change_buttonState() {
    button_state = (button_state + 1) % states_button;
  }

