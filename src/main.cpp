// Закончил на 309 строке. Делал меню.
//------------Настройки------------------
#define RESIST_10K 10000 // Вместо 10000 указ. точн. сопр. Резистора
#define ENC_TYPE 1 // Тип энка
#define RESIST_BASE 10000   // сопротивление при TEMP_BASE градусах по Цельсию (Ом)
#define TEMP_BASE 25        // температура, при которой измерено RESIST_BASE (градусов Цельсия)
#define B_COEF 3435         // бета коэффициент термистора (3000-4000)
#define STRIP_LED_NUM 9 // Колво светоидиотов в 1 ленте
#define STRIP_LED_NUM2 50 // Колво светоидиотов во 2 ленте
#define RING_LED_NUM 45 // Колво светоидиотов в 1 кольце
#define STRIP_LED_TYPE WS2812 // тип светодиодов в ленте
#define RING_LED_TYPE WS2812 // тип светодиодов в кольцах
#define CS_TEMP_MIN 25 // Минимальная темп в корпусе
#define CS_TEMP_MAX 60 // Максимальная темп в корпусе
//---------------------------------------


//--------------Пины---------------------
#define THERM A0 //Вместо A0 указ. анал.пин
#define LED_PIN 5 // Пин 1 ленты
#define LED2_PIN 6 // Пин 2 ленты
#define ARGB_round 8 // Пин колец
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
#include <LiquidCrystal_I2C_Menu.h>
#include <Wire.h> // Надо
//---------------------------------------

CRGB led1[STRIP_LED_NUM]; // Массив на 1 ленту(Вроде)
CRGB led2[STRIP_LED_NUM2]; // Массив на 2 ленту(Вроде)
CRGB ring1[RING_LED_NUM]; // Массив на 1 кольцо(Вроде)
LiquidCrystal_I2C_Menu lcd(I2C_LCD_ADDRESS,COLS,ROWS);

int x = 0;
// Яркость
byte brightness;
byte brightnessMenu;
boolean brightnessStatus;
//---------------------------------


// переменные цветов
byte RGB_R[2] = {0, 0}, RGB_G[2] = {0, 0}, RGB_B[2] = {0, 0};
byte RGB_R_R = 0, RGB_G_R = 0, RGB_B_R = 0;
//---------------------------------


// Состояние  on/off
boolean StripStatus[2] = {0, 0};
boolean RingStatus = 0;
//---------------------------------


// Режимы
byte ringMode = constrain(ringMode, 0, 3); 
byte stripMode[2] = {constrain(stripMode[0], 0, 2), constrain(stripMode[1], 0, 2)};

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
enum {mkBack, mkRoot, mkStrip, mkStrip1, mkStrip2,  mkStripStatus1, mkStripMode1, mkStripColor1, mkStripColor1r, mkStripColor1g, mkStripColor1b, mkStripStatus2, mkStripMode2, mkStripColor2, mkStripColor2r,mkStripColor2g,mkStripColor2b, mkRing, mkRing1, mkRingStatus, mkRingMode, mkRingColor, mkRingColorr, mkRingColorg, mkRingColorb, mkBrightness, mkBrightnessStatus, mkBrightnessPercent };

sMenuItem menu[] = {
  {mkBack, mkRoot, "A-RGB"},
    {mkRoot, mkStrip, "Strip"},
      {mkStrip, mkStrip1, "Strip 1"},
        {mkStrip1, mkStripStatus1, "Status", },
        {mkStrip1, mkStripMode1, "Mode", },
        {mkStrip1, mkStripColor1, "Color", },
          {mkStripColor1, mkStripColor1r, "R", },
          {mkStripColor1, mkStripColor1g, "G", },
          {mkStripColor1, mkStripColor1b, "B", }, 
          {mkStripColor1, mkBack, "Back",},
        {mkStrip1, mkBack, "Back",},
      {mkStrip, mkStrip2, "Strip 2"},
        {mkStrip2, mkStripStatus2, "Status", },
        {mkStrip2, mkStripMode2, "Mode", },
        {mkStrip2, mkStripColor2, "Color",},
          {mkStrip2, mkStripColor2r, "R", },
          {mkStrip2, mkStripColor2g, "G", },
          {mkStrip2, mkStripColor2b, "B", }, 
          {mkStripColor2, mkBack, "Back",},
        {mkStrip2, mkBack, "Back"},
      {mkStrip, mkBack, "Back"},
    {mkRoot, mkRing, "Ring"},
      {mkRing, mkRingStatus, "Status", },
        {mkRing, mkRingMode, "Mode",},
        {mkRing, mkRingColor, "Color", },
          {mkRingColor, mkRingColorr, "R", },
          {mkRingColor, mkRingColorg, "G", },
          {mkRingColor, mkRingColorb, "B", },
          {mkRingColor, mkBack, "Back",}, 
        {mkRing, mkBack, "Back"},
    {mkRoot, mkBrightness, "Brightness", },
      {mkBrightness, mkBrightnessStatus, "Status", },
      {mkBrightness, mkBrightnessPercent, "Level", },
      {mkBrightness, mkBack, "Back"},
    
};
//----------------------------------
void setPixelStrip (int Pixel, byte red, byte green, byte blue) {// Присваиваем значения ленте
    led1[Pixel].r = red;
    led1[Pixel].g = green;
    led1[Pixel].b = blue;

}
void setPixelStrip2 (int Pixel, byte red, byte green, byte blue) {// Присваиваем значения ленте
    led2[Pixel].r = red;
    led2[Pixel].g = green;
    led2[Pixel].b = blue;

}

void setPixelRing (int Pixel, byte red, byte green, byte blue) { // Присваиваем значения кольцам
    ring1[Pixel].r = red;
    ring1[Pixel].g = green;
    ring1[Pixel].b = blue;

}
uint8_t menuLen = sizeof(menu) / sizeof(sMenuItem);
int getItemIndexByKey(uint8_t key){
  for (uint8_t i = 0; i < menuLen; i++)
    if (menu[i].key == key)
      return i;
  return -1;
}

void updateCaption(uint8_t key, char format[], int value){
  // key - ключ пункта меню, для которого обновляется навание
  // format - шаблон названия со значением
  // value - значение, добавляемое в название
  uint8_t index = getItemIndexByKey(key);
  char* buf = (char*) malloc(40);
  sprintf(buf, format, value);
  menu[index].caption = (char*) realloc(menu[index].caption, strlen(buf)+1);
  strcpy(menu[index].caption, buf);
  free(buf);
}

//----------------------------------
//--Логический модуль светоидиотов--
//----------------------------------


// ==+Режимы лент+==

//Радужной режим
void stripRainbow(byte red, byte green, byte blue, int SpeedDelay){
  int Pixel = random(STRIP_LED_NUM); // Определяем, какой светоидиот загорится
  setPixelStrip(Pixel,red,green,blue); // отправляем  Значения на логмодуль
  FastLED.show();
 // delay(SpeedDelay); // делей
  setPixelStrip(Pixel,0,0,0); // Гасим
}
void stripRainbow2(byte red, byte green, byte blue, int SpeedDelay){
  int Pixel = random(STRIP_LED_NUM2); // Определяем, какой светоидиот загорится
  setPixelStrip2(Pixel,red,green,blue); // отправляем  Значения на логмодуль
  FastLED.show();
  //delay(SpeedDelay); // делей
  setPixelStrip2(Pixel,0,0,0); // Гасим
}


// Радужный поток
byte counter;
byte counter2;
void stripRaindowRiver(){
  for (int i = 0; i < STRIP_LED_NUM; i++ ) {         // от 0 до первой трети
    led1[i] = CHSV(counter + i * 2, 255, 255);  // HSV. Увеличивать HUE (цвет)
    // умножение i уменьшает шаг радуги
  }
  counter++;        // counter меняется от 0 до 255 (тип данных byte)
  FastLED.show();
 // delay(5); 
}

void strip2RainbowRiver(){
  for (int i = 0; i < STRIP_LED_NUM2; i++ ) {         // от 0 до первой трети
    led2[i] = CHSV(counter + i * 2, 255, 255);  // HSV. Увеличивать HUE (цвет)
    // умножение i уменьшает шаг радуги
  }
  counter2++;        // counter меняется от 0 до 255 (тип данных byte)
  FastLED.show();
 // delay(5);
}



// Статической режим
void stripStatic(){
  int i = 0; i < ARGB_round; i++;
  led1[i].setRGB(RGB_R[0], RGB_G[0], RGB_B[0]);  
  FastLED.show();
   // delay(30); // FPS
    return;
}
void stripStatic2(){
  led2[0].setRGB(RGB_R[1], RGB_G[1], RGB_B[1]);  
  FastLED.show();
   // delay(30); // FPS
    return;
}

//-----------------------


// --Режимы колец--


// Радужной режим
void ringRainbow(byte red, byte green, byte blue, int SpeedDelay){
  int Pixel = random(ARGB_round);
  setPixelStrip(Pixel,red,green,blue);
  FastLED.show();
  //delay(SpeedDelay);
  setPixelStrip(Pixel,0,0,0);
}


// Статической режим
void ringStatic(){
  ring1[0].setRGB(RGB_R_R, RGB_G_R, RGB_B_R); // Присваиваем статическое значение
  FastLED.show();
    //delay(30); // FPS
  return;
}

// Температурный режим
void ringTemp(){
  byte val = (getThermTemp(analogRead(THERM))); // Получаем температуру в цельсии
  val = constrain(val, CS_TEMP_MIN, CS_TEMP_MAX); // Ограничиваем
  map(val, CS_TEMP_MIN, CS_TEMP_MAX, 0, ARGB_round); // Конвертируем диапазон
  for(int led = 0; led < val; led++) { 
            ring1[led].setRGB(RGB_R_R, RGB_G_R, RGB_B_R); // Цикл
  };
  FastLED.show();
    // delay(30); // FPS
    return;
}

byte counter3;
void ringRaindowRiver(){
  for (int i = 0; i < ARGB_round; i++ ) {         // от 0 до первой трети
    ring1[i] = CHSV(counter + i * 2, 255, 255);  // HSV. Увеличивать HUE (цвет)
    // умножение i уменьшает шаг радуги
  }
  counter3++;        // counter меняется от 0 до 255 (тип данных byte)
  FastLED.show();
  // delay(5); 
}
//------------------------------


// Даем данные на ленты
void strip1(){
  switch(stripMode[0]){ // Выбор режима работы
    case 0: stripRainbow(random(255), random(255), random(255), 0); //Отправляем значения
    break;
    case 1: stripStatic();
    break;
    case 2: stripRaindowRiver();
  }
  return;
}
void strip2(){
  switch(stripMode[1]){ // Выбор режима работы
    case 0: stripRainbow(random(255), random(255), random(255), 0); //Отправляем значения
    break;
    case 1: stripStatic();
    break;
    case 2: strip2RainbowRiver();
  }
  return;
}

//-----------------------------


// Даем команды ленте и кольцам
void strip(){
  if (StripStatus[0] == 1) strip1();
  if (StripStatus[1] == 1) strip2();
}
void ring(){ 
  switch(ringMode){ // Выбор режима работы
    /*case 0: ringTemp();
    break;*/
    case 0: ringStatic();
    break;
    case 1: ringRainbow(random(255), random(255), random(255), 0);
    break;
    case 2: ringTemp();
    break;
    case 3: ringRaindowRiver();
  }
 return;
}
//=================================
//---------------------------------
//=================================



void setup() {
  Serial.begin(9600);
  Serial.println("Connection Successful");
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.attachEncoder(ENC_DT, ENC_CLK, ENC_SW);

  FastLED.addLeds<STRIP_LED_TYPE, LED_PIN, GRB>(led1, STRIP_LED_NUM).setCorrection( TypicalLEDStrip );; // Инициализация 1 ленты(Вроде) 
  FastLED.addLeds<STRIP_LED_TYPE, LED2_PIN, GRB>(led2, STRIP_LED_NUM2).setCorrection( TypicalLEDStrip );; // Инициализация 2 ленты(Вроде) 
  FastLED.addLeds<RING_LED_TYPE, ARGB_round, GRB>(ring1, RING_LED_NUM).setCorrection( TypicalLEDStrip );; // Инициализация 1 кольца(Вроде)
  lcd.printAt(3, 1, "Schmitzensaff");
  lcd.printAt(3, 2, "Electronics");
  lcd.printAt(7, 0, "v.0.1 alpha");
  // FastLED.addLeds<LMUSIC_LED_TYPE,ARGB_LMusic, GRB>(led3, LMUSIC_LED_NUM); // Инициализация светомузыки(Вроде)
  delay(1000);
}


void loop() {
   
  map(brightnessMenu, 0, 255, 0, 100);                      // Яркость
    
      brightness = map(brightnessMenu, 0, 100, 0, 255); 
    
  FastLED.setBrightness(brightness); // Уровень яркости
  eEncoderState EncoderState = lcd.getEncoderState();
    uint8_t selectedMenuItem = lcd.showMenu(menu, menuLen, 1);

  strip();
  if (RingStatus == 1) ring();
   switch(selectedMenuItem){
    case mkStripStatus1:{
      StripStatus[0] = lcd.inputVal<boolean>("Strip 1 status", 0, 1, StripStatus[0]);
      Serial.print(StripStatus[0]);
    };
    break;
    case mkStripStatus2:{
      StripStatus[1] = lcd.inputVal<boolean>("Strip 2 status", 0, 1, StripStatus[1]);
    }
    break;
    case mkRingStatus:{
      RingStatus = lcd.inputVal<boolean>("Ring status", 0, 1, RingStatus);
    }
    break;
    case mkStripMode1:{
      stripMode[0] = lcd.inputVal<byte>("Strip 1 mode", 0, 2, stripMode[0]);
    }
    break;
    case mkStripMode2:{
      stripMode[1] = lcd.inputVal<byte>("Strip 2 mode", 0, 2, stripMode[1]);
    }
    break;
    case mkRingMode:{
      ringMode = lcd.inputVal<byte>("Ring mode", 0, 3, ringMode);
    }
    break;
    case mkStripColor1r:{
      RGB_R[0] = lcd.inputVal<byte>("Strip 1 red color", 0, 255, RGB_R[0]);
    }
    break;
    case mkStripColor2r:{
      RGB_R[1] = lcd.inputVal<byte>("Strip 2 red color", 0, 255, RGB_R[1]);
    }
    break;
    case mkRingColorr:{
      RGB_R_R = lcd.inputVal<byte>("Ring red color", 0, 255, RGB_R_R);
    }
    break;
    case mkStripColor1g:{
      RGB_G[0] = lcd.inputVal<byte>("Strip 1 green color", 0, 255, RGB_G[0]);
    }
    break;
    case mkStripColor2g:{
      RGB_G[1] = lcd.inputVal<byte>("Strip 2 green color", 0, 255, RGB_G[1]);
    }
    break;
    case mkRingColorg:{
      RGB_G_R = lcd.inputVal<byte>("Ring green color", 0, 255, RGB_G_R);
    }
    break;
    case mkStripColor1b:{
      RGB_B[0] = lcd.inputVal<byte>("Strip 1 blue color", 0, 255, RGB_B[0]);
    }
    break;
    case mkStripColor2b:{
      RGB_B[1] = lcd.inputVal<byte>("Strip 2 blue color", 0, 255, RGB_B[1]);
    }
    break;
    case mkRingColorb:{
      RGB_B_R = lcd.inputVal<byte>("Ring blue color", 0, 255, RGB_B_R);
    }
    break;
    case mkBrightnessStatus:{
      brightnessStatus = lcd.inputVal<boolean>("Brightness mode", 0, 1, brightnessStatus);
    }
    break;
    case mkBrightnessPercent:{
      brightnessMenu = lcd.inputVal<byte>("Brightness level", 0, 100, brightnessMenu);
      Serial.print(brightnessMenu);
    }
    break;
   }
  
}