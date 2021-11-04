//------------Настройки------------------
#define RESIST_10K 10000 // Вместо 10000 указ. точн. сопр. Резистора
#define ENC_TYPE 1 // Тип энка
#define RESIST_BASE 10000   // сопротивление при TEMP_BASE градусах по Цельсию (Ом)
#define TEMP_BASE 25        // температура, при которой измерено RESIST_BASE (градусов Цельсия)
#define B_COEF 3435         // бета коэффициент термистора (3000-4000)
#define LED_NUM 50 // Колво светоидиотов
#define LED_TYPE WS2812 // тип ленты
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
//---------------------------------------


//---------------Библы-------------------
#include <Arduino.h> // Стандартка для пердуины
#include <FastLED.h> // Работа с адреской
#include <LiquidCrystal_I2C.h> // Дисплей
#include <GyverEncoder.h> // Энкодер
#include <Wire.h> // Надо
//---------------------------------------

CRGB leds[LED_NUM]; // Колво светоидиотов
LiquidCrystal_I2C lcd(I2C_LCD_ADDRESS,20,4);
Encoder enc(ENC_CLK, ENC_DT, ENC_SW, ENC_TYPE); // Инициализируем энкодер



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



void setup() {
  Serial.begin(9600);
  Serial.println("Connection Successful");
  lcd.init();
  lcd.backlight();
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, LED_NUM); // Хз че за херня, но нужно 
 
}


void loop() {
  FastLED.setBrightness(map(analogRead(PHOTO), 0, 1023, 0, 255)); // Уровень яркости
  byte RGB_R = 0, RGB_G = 0, RGB_B = 0; // переменные цветов
  leds[0].setRGB(RGB_R, RGB_G, RGB_B);  // Хз че за херня, но нужно
  FastLED.show(); // Выводим массив на ленту
  delay(30); // FPS
}
