//------------Настройки------------------
#define RESIST_10K 10000 // Вместо 10000 указ. точн. сопр. Резистора
#define ENC_TYPE 1 // Тип энка
#define RESIST_BASE 10000   // сопротивление при TEMP_BASE градусах по Цельсию (Ом)
#define TEMP_BASE 25        // температура, при которой измерено RESIST_BASE (градусов Цельсия)
#define B_COEF 3435         // бета коэффициент термистора (3000-4000)
//---------------------------------------


//--------------Пины---------------------
#define THERM A0 //Вместо A0 указ. анал.пин
#define ARGB_strip 5 // Пин ленты
#define ARGB_round 6 // Пин колец
#define ARGB_LMusic 7 // Пин Свет-ки
#define ENC_CLK 2 // Пин энка CLK
#define ENC_DT 3 // Пин энка DT 
#define ENC_SW 4 // Пин энка SW
//---------------------------------------


//---------------Библы-------------------
#include <Arduino.h> // Стандартка для пердуины
#include <FastLED.h> // Работа с адреской
#include <GyverEncoder.h> // Библа энкодера
#include <LiquidCrystal_I2C.h> // Дисплей
//---------------------------------------

Encoder enc(ENC_CLK, ENC_DT, ENC_SW, ENC_TYPE);
void setup() {
  Serial.begin(9600);
}

void loop() {
  
}


// Термистор. Особо не суетить. Работать через getThermTemp(analogRead(THERM))
float getThermTemp(int resistance) {
  float thermistor;
  thermistor = RESIST_10K / ((float)1023 / resistance - 1);
  thermistor /= RESIST_BASE;                        // (R/Ro)
  thermistor = log(thermistor) / B_COEF;            // 1/B * ln(R/Ro)
  thermistor += (float)1.0 / (TEMP_BASE + 273.15);  // + (1/To)
  thermistor = (float)1.0 / thermistor - 273.15;    // инвертируем и конвертируем в градусы по Цельсию
  return thermistor;
}