//------------Настройки------------------
#define RESIST_10K 10000 // Вместо 10000 указ. точн. сопр. Резистора
#define ENC_TYPE 1 // Тип энка
#define RESIST_BASE 10000   // сопротивление при TEMP_BASE градусах по Цельсию (Ом)
#define TEMP_BASE 25        // температура, при которой измерено RESIST_BASE (градусов Цельсия)
#define B_COEF 3435         // бета коэффициент термистора (3000-4000)
#define STRIP_LED_NUM 50 // Колво светоидиотов
#define RING_LED_NUM 45 // Колво светодиодов в кольцах
#define LMUSIC_LED_NUM 70 // Колво светодиодов в светомузыке.
#define STRIP_LED_TYPE WS2812 // тип светодиодов в ленте
#define RING_LED_TYPE WS2812 // тип светодиодов в кольцах
#define LMUSIC_LED_TYPE WS2812 // тип светодиодов в светомузыке
#define CS_TEMP_MIN 25 // Минимальная темп в корпусе
#define CS_TEMP_MAX 60 // Максимальная темп в корпусе
//---------------------------------------


//--------------Пины---------------------
#define THERM A0 //Вместо A0 указ. анал.пин
#define LED_PIN 5 // Пин ленты
#define ARGB_round 6 // Пин колец
#define ARGB_LMusic 7 // Пин Свет-ки
#define ENC_CLK 2 // Пин энка CLK
#define ENC_DT 3 // Пин энка DT 
#define ENC_SW 4 // Пин энка SW
#define PHOTO A1 // Пин фоторез.
//---------------------------------------


//------------Отладошное-----------------
#define I2C_LCD_ADDRESS 0x27 // I2C адрес дисплея
#define COLS 20 // Cols
#define ROWS 4 // ROWS
//---------------------------------------


//---------------Меню--------------------

//---------------------------------------


//---------------Библы-------------------
#include <Arduino.h> // Стандартка для пердуины
#include <FastLED.h> // Работа с адреской
#include <LiquidCrystal_I2C.h> // Дисплей
#include <LiquidCrystal_I2C_Menu.h>
#include <Wire.h> // Надо
//---------------------------------------

CRGB led1[STRIP_LED_NUM]; // Массив на ленту(Вроде)
CRGB led2[RING_LED_NUM]; // Массив на кольцо(Вроде)
CRGB led3[LMUSIC_LED_NUM]; // Массив на светомузыку(Вроде)
LiquidCrystal_I2C_Menu lcd(I2C_LCD_ADDRESS,COLS,ROWS);

int x = 0;
// Яркость
byte brightness = 0;
byte brightnessMenu = 0;
boolean brightnessStatus = 0;
//---------------------------------


// переменные цветов
byte RGB_R = 0, RGB_G = 0, RGB_B = 0;
byte RGB_R_R = 0, RGB_G_R = 0, RGB_B_R = 0;
byte RGB_R_LM = 0, RGB_G_LM = 0, RGB_B_LM = 0; 
//---------------------------------


// Состояние  on/off
boolean StripStatus = 0;
boolean RingStatus = 0;
boolean LMusicStatus = 0;
//---------------------------------


// Режимы
byte ringMode = constrain(ringMode, 0, 2);
boolean stripMode;

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


// Иерархия меню.
enum {mkBack, mkRoot, mkStrip, mkStripStatus, mkStripMode, mkStripColor, mkStripSpeed, mkStripBrightness, mkRing, mkRingStatus, mkRingMode, mkRingColor, mkRingSpeed, mkRingBrightness, mkLMusic, mkLMusicStatus, mkLMusicMode, mkLMusicColor, mkLMusicSpeed, mkLMusicBrightness};

sMenuItem menu[] = {
  {mkBack, mkRoot, "A-RGB"},
    {mkRoot, mkStrip, "Strip"},
      {mkStrip, mkStripStatus, "Status:"},
      {mkStrip, mkStripMode, "Mode:"},
      {mkStrip, mkStripColor, "Color"},
      {mkStrip, mkStripSpeed, "Speed"},
      {mkStrip, mkStripBrightness, "Brightness"},
      {mkStrip, mkBack, "Back"},
    {mkRoot, mkRing, "Ring"},
      {mkRing, mkRingStatus, "Status:"},
      {mkRing, mkRingMode, "Mode:"},
      {mkRing, mkRingColor, "Color"},
      {mkRing, mkRingSpeed, "Speed"},
      {mkRing, mkRingBrightness, "Brightness"},
      {mkRing, mkBack, "Back"},
    {mkRoot, mkLMusic, "LMusic"},
      {mkLMusic, mkLMusicStatus, "Status:"},
      {mkLMusic, mkLMusicMode, "Mode:"},
      {mkLMusic, mkLMusicColor, "Color"},
      {mkLMusic, mkLMusicSpeed, "Speed"},
      {mkLMusic, mkLMusicBrightness, "Brightness"},
      {mkLMusic, mkBack, "Back"}
};
//----------------------------------

unsigned long timing;
// Логический модуль светоидиотов
void stripRainbow(){
  led1[0].setRGB(255, 0, 0);
  FastLED.show();
  if (millis() - timing > 500){ // Вместо 10000 подставьте нужное вам значение паузы 
  timing = millis(); 
  };
}
void stripStatic(){
  led1[0].setRGB(RGB_R, RGB_G, RGB_B);  // Хз че за херня, но нужно
  return;
}
void strip(){
  switch(stripMode){ // Выбор режима работы
    case 0: stripRainbow();
    break;
    case 1: stripStatic();
    break;
  }
  return;
}
void ringRainbow(){

}
void ringStatic(){
  led2[0].setRGB(RGB_R_R, RGB_G_R, RGB_B_R); // Присваиваем статическое значение
  return;
}

// Температурный режим
void ringTemp(){
  byte val = (getThermTemp(analogRead(THERM))); // Получаем температуру в цельсии
  val = constrain(val, CS_TEMP_MIN, CS_TEMP_MAX); // Ограничиваем
  map(val, CS_TEMP_MIN, CS_TEMP_MAX, 0, ARGB_round); // Конвертируем диапазон
  for(int led = 0; led < val; led++) { 
            led2[led].setRGB(RGB_R_R, RGB_G_R, RGB_B_R); // Цикл
  };
}

void ring(){ 
  switch(ringMode){ // Выбор режима работы
    case 0: ringTemp();
    break;
    case 1: ringStatic();
    break;
    case 2: ringRainbow();
    break;
  }
 return;
}
//---------------------------------
uint8_t menuLen = sizeof(menu) / sizeof(sMenuItem);
void setup() {
  Serial.begin(9600);
  Serial.println("Connection Successful");
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.attachEncoder(ENC_DT, ENC_CLK, ENC_SW);
  FastLED.addLeds<STRIP_LED_TYPE, LED_PIN, GRB>(led1, STRIP_LED_NUM); // Инициализация ленты(Вроде) 
  FastLED.addLeds<RING_LED_TYPE, ARGB_round, GRB>(led2, RING_LED_NUM); // Инициализация колец(Вроде)
  FastLED.addLeds<LMUSIC_LED_TYPE,ARGB_LMusic, GRB>(led3, LMUSIC_LED_NUM); // Инициализация светомузыки(Вроде)
}


void loop() {
  map(brightnessMenu, 0, 255, 0, 100);                      // Яркость
    if (brightnessStatus = true){
      brightness = map(analogRead(PHOTO), 0, 1023, 0, 255);
    } else{
      brightness = map(brightnessMenu, 0, 100, 0, 255); 
    }
  FastLED.setBrightness(brightness); // Уровень яркости
  eEncoderState EncoderState = lcd.getEncoderState();
    uint8_t selectedMenuItem = lcd.showMenu(menu, menuLen, 1);
    strip();
    ring();
    FastLED.show();
    delay(30); // FPS
  
}
