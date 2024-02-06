#include <GyverOLED.h>
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled; // oled

#include <MQ135.h> 
#define analogPin A3 
MQ135 gasSensor = MQ135(analogPin); // CO2

#include <GyverBME280.h>
GyverBME280 bme; // метеостанция

#include <microDS3231.h>
MicroDS3231 rtc; // время

#include <iarduino_SensorPulse.h> 
iarduino_SensorPulse Pulse(A2); // пульс


const int analogInPin = A0;
const int digitalInPin = 2;
int dValue; // шум

#define MY_PERIOD 1000  // частота обновления экрана
uint32_t tmr1;        
// кнопка
#define BTN_PIN 3    
#define MODE_AM 3    // количество видов вывода
#include <EncButton.h>
EncButton<EB_TICK, BTN_PIN> btn;
byte mode = 0;       

void setup() {
  Serial.begin(9600);
  oled.init(); 
  oled.clear(); 
  oled.setScale(2); 
  rtc.begin();
  bme.begin(0x76); 
  pinMode(digitalInPin, INPUT); 
  pinMode(4, OUTPUT);
  Pulse.begin();
}

void loop() {
  if (millis() - tmr1 >= MY_PERIOD) { 
  tmr1 = millis();                  
  switch (mode) {
    case 0: task_data();
      break;
    case 1: task_meteo();
      break;
    case 2: task_ppm();
      break;
    }
  }
  // шум
  dValue = digitalRead(digitalInPin);
  if (dValue == HIGH){
    digitalWrite(4, HIGH);}
    delay(10);
    digitalWrite(4, LOW);
  btn.tick();
  if (btn.click()) {
    if (++mode >= MODE_AM) mode = 0;
  }

}

void task_data() {
  oled.clear();
  oled.setCursor(1, 1);
  oled.print(rtc.getTimeString()); 
  oled.setCursor(1, 5);
  if(Pulse.check(ISP_VALID)==ISP_CONNECTED){
    oled.print(Pulse.check(ISP_PULSE));
  }
}

void task_meteo() {
  oled.clear();
  oled.setCursor(1, 1);
  oled.print(bme.readHumidity()); // влажность
  oled.setCursor(60, 1);
  oled.print("%");
  oled.setCursor(1, 3);
  oled.print(bme.readPressure()); // давление
  oled.setCursor(97, 3);
  oled.print("Па");
  oled.setCursor(1, 5);
  oled.print(bme.readTemperature()); // температура
  oled.setCursor(60, 5);
  oled.print("*");
}

void task_ppm() {
  oled.clear();
  oled.setCursor(1, 2);
  //погнали чекать CO2
  if (gasSensor.getPPM() < 450) {   
    oled.print("Very good");
  }
  else if (gasSensor.getPPM() < 600) {   
    oled.print("Good");
  }
  else if (gasSensor.getPPM() < 1000) {   
    oled.print("Acceptable");
  }
  else if (gasSensor.getPPM() < 2500) {   
    oled.print("Bad");
  }
  else if (gasSensor.getPPM() > 2500)  {   
    oled.print("Health risk");
  }
  oled.setCursor(1, 5);
  oled.print(gasSensor.getPPM()); // CO2
  oled.setCursor(75, 5);
  oled.print("ppm");
}
