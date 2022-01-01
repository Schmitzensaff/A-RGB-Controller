// Release 2.0
// Переход на FastLED
// Переключение ражимов исправно
// Проблем не наблюдается
// Закончил.
// Осталось реализовать работу терморезистора.
//------------Настройки------------------
#define RESIST_10K 10000 // Вместо 10000 указ. точн. сопр. Резистора
#define RESIST_BASE 10000   // сопротивление при TEMP_BASE градусах по Цельсию (Ом)
#define TEMP_BASE 25        // температура, при которой измерено RESIST_BASE (градусов Цельсия)
#define B_COEF 3435         // бета коэффициент термистора (3000-4000)
#define STRIP_LED_NUM 9 // Колво светоидиотов в 1 ленте
#define STRIP_LED_NUM2 9 // Колво светоидиотов во 2 ленте
#define RING_LED_NUM 9 // Колво светоидиотов в 1 кольце
#define STRIP_LED_TYPE WS2812 // тип светодиодов в ленте
#define RING_LED_TYPE WS2812 // тип светодиодов в кольцах
#define CS_TEMP_MIN 25 // Минимальная темп в корпусе
#define CS_TEMP_MAX 60 // Максимальная темп в корпусе
#define PARSE_AMOUNT 17 // число значений в массиве, который хотим получить
#define ST1_RR_TIMER_CONST 7 // Таймер Рад. Строки 1 ленты
#define ST2_RR_TIMER_CONST 20 // Таймер Рад. Строки 2 ленты
#define RG_RR_TIMER_CONST 10 // Таймер Рад. Строки колец
//---------------------------------------


//--------------Пины---------------------
#define THERM A0 //Вместо A0 указ. анал.пин
#define LED_PIN 5// Пин 1 ленты
#define LED2_PIN 6 // Пин 2 ленты
#define ARGB_round 8 // Пин колец
#define PHOTO A1 // Пин фоторез.
//---------------------------------------





//---------------Библы-------------------
#include <Arduino.h> // Стандартка для пердуины
#include <GParsingStream.h>
#include <FastLED.h>
//---------------------------------------

CRGB led1[STRIP_LED_NUM];
CRGB led2[STRIP_LED_NUM2];
CRGB ring[RING_LED_NUM];

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



//----------------------------------
//--Логический модуль светоидиотов--
//----------------------------------



// Радужный поток
byte counter;
byte counter2;

void stripRaindowRiver(){
  if(millis() - ST1_RR_TIMER >= ST1_RR_TIMER_CONST){
  for(int i = 0; i < STRIP_LED_NUM; i++) {
    led1[i] = CHSV(counter + i * 2, 255, 255);
    counter++;
  FastLED.show();
    }
    ST1_RR_TIMER == millis();
  }
}


  
  
//}

void strip2RainbowRiver(){
  if(millis() - ST2_RR_TIMER >= ST2_RR_TIMER_CONST){
 for(int i = 0; i < STRIP_LED_NUM2; i++) {
    led2[i] = CHSV(counter2 + i * 2, 255, 255);
    counter2++;
  FastLED.show();
  
    }
    ST2_RR_TIMER == millis();
  }
}



// Статической режим
void stripStatic(){
    for(int i = 0; i < STRIP_LED_NUM; i++) {
      led1[i].setRGB(intData[6], intData[7], intData[8]);
      FastLED.show();
    } 
}
void stripStatic2(){
  for(int i = 0; i < STRIP_LED_NUM2; i++) {
    led2[i].setRGB(intData[9], intData[10], intData[11]);
     FastLED.show();
  }
 
}
//-----------------------

// --Режимы колец--

// Статической режим
void ringStatic(){
  for(int i = 0; i < RING_LED_NUM; i++) {
      if(intData[5] != 1) return;
      ring[i].setRGB(intData[12], intData[13], intData[14]);
      FastLED.show();
    }
  
}
// Температурный режим
/*void ringTemp(){
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
 */
byte counter3;
void ringRaindowRiver(){
  if(millis() - RG_RR_TIMER >= RG_RR_TIMER_CONST){
 for(int i = 0; i < RING_LED_NUM; i++) {
    ring[i] = CHSV(counter3 + i * 2, 255, 255);
      counter3++;
  FastLED.show();
    }
    RG_RR_TIMER == millis();
  }
}
//=================================
//---------------------------------
//=================================
void brightnessVoid(){
    if(intData[15] == 0){
       FastLED.setBrightness(map(analogRead(PHOTO), 0, 1023, 0, 100));
    }else if(intData[15] == 1){
      FastLED.setBrightness(intData[16]);
    }// Уровень яркости
   }  
  
void setup() {
  Serial.begin(9600);
  Serial.println("Connection Successful");
  FastLED.addLeds<STRIP_LED_TYPE, LED_PIN, GRB>(led1, STRIP_LED_NUM).setCorrection( TypicalLEDStrip );;
  FastLED.addLeds<STRIP_LED_TYPE, LED2_PIN, GRB>(led2, STRIP_LED_NUM2).setCorrection( TypicalLEDStrip );;
  FastLED.addLeds<RING_LED_TYPE, ARGB_round, GRB>(ring, RING_LED_NUM).setCorrection( TypicalLEDStrip );;
  // FastLED.addLeds<LMUSIC_LED_TYPE,ARGB_LMusic, GRB>(led3, LMUSIC_LED_NUM); // Инициализация светомузыки(Вроде)
}


void loop() {
parsingStream((int*)&intData);
/*if (dataReady()) {
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
  Serial.println("ABOBA");
}else{*/
// Устанавливаем Яркость
  brightnessVoid();
//---------------------------------
 if(intData[0] == 1 && intData[3] == 1) stripStatic();

 if(intData[0] == 1 && intData[3] == 0) stripRaindowRiver(); 

 if(intData[1] == 1 && intData[4] == 1) stripStatic2();

 if(intData[1] == 1 && intData[4] == 0) strip2RainbowRiver();

 if(intData[2] == 1 && intData[5] == 1) ringStatic();

 if(intData[2] == 1 && intData[5] == 0) ringRaindowRiver();

//if(intData[2] == 1 && intData[5] == 2) ringTemp();
 
 //-----------------------------
}