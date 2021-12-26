// Alpha 1.2
// Переход на Adafruit Neopixel
// Цифровые наводки исправлены.
// Беда со скоростью. При включении на полную катушку скорость обновления очень низкая
// Закончил на 123 строке.
//------------Настройки------------------
#define RESIST_10K 10000 // Вместо 10000 указ. точн. сопр. Резистора
#define RESIST_BASE 10000   // сопротивление при TEMP_BASE градусах по Цельсию (Ом)
#define TEMP_BASE 25        // температура, при которой измерено RESIST_BASE (градусов Цельсия)
#define B_COEF 3435         // бета коэффициент термистора (3000-4000)
#define STRIP_LED_NUM 9 // Колво светоидиотов в 1 ленте
#define STRIP_LED_NUM2 50 // Колво светоидиотов во 2 ленте
#define RING_LED_NUM 45 // Колво светоидиотов в 1 кольце
#define STRIP_LED_TYPE LED_WS2812 // тип светодиодов в ленте
#define RING_LED_TYPE LED_WS2812 // тип светодиодов в кольцах
#define CS_TEMP_MIN 25 // Минимальная темп в корпусе
#define CS_TEMP_MAX 60 // Максимальная темп в корпусе
#define PARSE_AMOUNT 17 // число значений в массиве, который хотим получить
#define ST1_RR_TIMER_CONST 10 // Таймер Рад. Строки 1 ленты
#define ST2_RR_TIMER_CONST 20 // Таймер Рад. Строки 2 ленты
#define RG_RR_TIMER_CONST 20 // Таймер Рад. Строки колец
//---------------------------------------


//--------------Пины---------------------
#define THERM A0 //Вместо A0 указ. анал.пин
#define LED_PIN 8 // Пин 1 ленты
#define LED2_PIN 6 // Пин 2 ленты
#define ARGB_round 5 // Пин колец
#define PHOTO A1 // Пин фоторез.
//---------------------------------------





//---------------Библы-------------------
#include <Arduino.h> // Стандартка для пердуины
#include <GParsingStream.h>
#include <Adafruit_NeoPixel.h>
//---------------------------------------

Adafruit_NeoPixel led1 = Adafruit_NeoPixel(STRIP_LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel led2 = Adafruit_NeoPixel(STRIP_LED_NUM2, LED2_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring = Adafruit_NeoPixel(RING_LED_NUM, ARGB_round, NEO_GRB + NEO_KHZ800);
int x = 0;
int intData[PARSE_AMOUNT];     // массив численных значений после парсинга


//--------------Таймеры------------------
uint32_t ledTimer;
uint32_t parserTimer = 0;
uint32_t ST1_RR_TIMER; // Таймер Рад. Строки 1 ленты
uint32_t ST2_RR_TIMER; // Таймер Рад. Строки 2 ленты
uint32_t RG_RR_TIMER; // Таймер Рад. Строки колец
//---------------------------------------
//----------------------------------
// Термистор. Особо не суетить. Работать через getThermTemp(analogRead(THERM))
float getThermTemp(int resistance){
  float thermistor;
  thermistor = RESIST_10K / ((float)1023 / resistance - 1);
  thermistor /= RESIST_BASE;                        // (R/Ro)
  thermistor = log(thermistor) / B_COEF;            // 1/B * ln(R/Ro)
  thermistor += (float)1.0 / (TEMP_BASE + 273.15);  // + (1/To)
  thermistor = (float)1.0 / thermistor - 273.15;    // инвертируем и конвертируем в градусы по Цельсию
  return thermistor;
}
//---------------------------------


void strip1Show(){
    led1.show();
    
  }


void strip2Show(){
    led2.show();
    
}
void ringShow(){
    ring.show();  
}

//----------------------------------
//--Логический модуль светоидиотов--
//----------------------------------



// Радужный поток
byte counter;
byte counter2;
byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void stripRaindowRiver(){
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i < STRIP_LED_NUM2; i++) {
      c=Wheel(((i * 256 / STRIP_LED_NUM2) + j) & 255);
      led1.setPixelColor(i, *c, *(c+1), *(c+2));
    }
    strip1Show();
    delay(20);
    }
    
  
}


  
  
//}

void strip2RainbowRiver(){
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< STRIP_LED_NUM2; i++) {
      c=Wheel(((i * 256 / STRIP_LED_NUM) + j) & 255);
      led2.setPixelColor(i, *c, *(c+1), *(c+2));
      
    }
    strip2Show();
    delay(20);
    }
    
}



// Статической режим
void stripStatic(){
    Serial.println("SUKA");
    led1.fill(intData[6], intData[7], intData[8]);  //intData[6], intData[7], intData[8]);
    Serial.println("BLYAT");
    led1.show();
  
}
void stripStatic2(){
    led2.fill(intData[9], intData[10], intData[11]);
    led2.show();
  
}
//-----------------------


// --Режимы колец--


// Статической режим
void ringStatic(){
  ring.fill(intData[12], intData[13], intData[14]);
  ring.show();
}
// Температурный режим
void ringTemp(){
  byte val = (getThermTemp(analogRead(THERM))); // Получаем температуру в цельсии
  val = constrain(val, CS_TEMP_MIN, CS_TEMP_MAX); // Ограничиваем
  map(val, CS_TEMP_MIN, CS_TEMP_MAX, 0, ARGB_round); // Конвертируем диапазон
  for(int led = 0; led < val; led++) { 
            ring.setPixelColor(led, intData[12], intData[13], intData[14]); // Цикл
  };
  ring.show();
    // delay(30); // FPS
    return;
}
 
byte counter3;
void ringRaindowRiver(){
   byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< RING_LED_NUM; i++) {
      c=Wheel(((i * 256 / RING_LED_NUM) + j) & 255);
      ring.setPixelColor(i, *c, *(c+1), *(c+2));
      
    }
    ringShow();
    delay(20);
    }
    
  
}
//=================================
//---------------------------------
//=================================




 void brightnessVoid(){
    if(intData[15] == 0){
       led1.setBrightness(map(analogRead(PHOTO), 0, 1023, 0, 100));
       led2.setBrightness(map(analogRead(PHOTO), 0, 1023, 0, 100));
       ring.setBrightness(map(analogRead(PHOTO), 0, 1023, 0, 100));  
    }else if(intData[15] == 1){
      led1.setBrightness(intData[16]);
      led2.setBrightness(intData[16]);
      ring.setBrightness(intData[16]);
     
    }
     // Уровень яркости
    
   }  
  
  
void setup() {
  Serial.begin(9600);
  Serial.println("Connection Successful");
  led1.begin();
  led2.begin();
  ring.begin();

  // FastLED.addLeds<LMUSIC_LED_TYPE,ARGB_LMusic, GRB>(led3, LMUSIC_LED_NUM); // Инициализация светомузыки(Вроде)
  
}


void loop() {
  if(millis() - parserTimer >= 50){
    parserTimer = millis();
  
parsingStream((int*)&intData);
if (dataReady()) {
  Serial.println("Correct Parse.");
  intData[0] == constrain(intData[0], 0, 1); // лента 1 статус
  intData[1] == constrain(intData[1], 0, 1); // лента 2 статус
  intData[2] == constrain(intData[2], 0, 1); // кольцо статус
  intData[3] == constrain(intData[3], 0, 2); // лента 1 режим
  intData[4] == constrain(intData[4], 0, 2); // лента 2 режим
  intData[5] == constrain(intData[5], 0, 3); // кольцо режим
  intData[6] == constrain(intData[6], 0, 255); // Лента 1 R
  intData[7] == constrain(intData[7], 0, 255); // Лента 1 G
  intData[8] == constrain(intData[8], 0, 255); // Лента 1 B
  intData[9] == constrain(intData[9], 0, 255); // Лента 2 R
  intData[10] == constrain(intData[10], 0, 255); // Лента 2 G
  intData[11] == constrain(intData[11], 0, 255); // Лента 2 B
  intData[12] == constrain(intData[12], 0, 255); // Кольцо R
  intData[13] == constrain(intData[13], 0, 255); // Кольцо G
  intData[14] == constrain(intData[14], 0, 255); // Кольцо B
  intData[15] == constrain(intData[15], 0, 1); // Яркость режим
  intData[16] == constrain(intData[16], 0, 100); // Яркость уровень
}
}
// Устанавливаем Яркость
  //brightnessVoid();
  led1.setBrightness(50);
 
//---------------------------------

 if(intData[0] == 1 && intData[3] == 1) stripStatic();

 if(intData[0] == 1 && intData[3] == 0) stripRaindowRiver(); 

 if(intData[1] == 1 && intData[4] == 1) stripStatic2();

 if(intData[1] == 1 && intData[4] == 0) strip2RainbowRiver();

 if(intData[2] == 1 && intData[5] == 1) ringStatic();

 if(intData[2] == 1 && intData[5] == 0) ringRaindowRiver();

 if(intData[2] == 1 && intData[5] == 2) ringTemp();
 
 
 //-----------------------------

}