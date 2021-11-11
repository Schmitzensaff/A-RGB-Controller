// Закончил на 309 строке. Делал меню.
//------------Настройки------------------
#define RESIST_10K 10000 // Вместо 10000 указ. точн. сопр. Резистора
#define ENC_TYPE 1 // Тип энка
#define RESIST_BASE 10000   // сопротивление при TEMP_BASE градусах по Цельсию (Ом)
#define TEMP_BASE 25        // температура, при которой измерено RESIST_BASE (градусов Цельсия)
#define B_COEF 3435         // бета коэффициент термистора (3000-4000)
#define STRIP_LED_NUM 50 // Колво светоидиотов в 1 ленте
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
byte brightness = 0;
byte brightnessMenu = 0;
boolean brightnessStatus = 0;
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
boolean ringMode; 
boolean stripMode[2];

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

void strip1st(){} // Прекурсоры
void strip1md(){}
void strip1cl(){}
void strip1sp(){}
void strip2st(){}
void strip2md(){}
void strip2cl(){}
void strip2sp(){}
void ring1st(){}
void ring1md(){}
void ring1cl(){}
void ring1sp(){}
void setBrightness(){}
// Иерархия меню.
enum {mkBack, mkRoot, mkStrip, mkStrip1, mkStrip2,  mkStripStatus1, mkStripMode1, mkStripColor1, mkStripSpeed1, mkStripStatus2, mkStripMode2, mkStripColor2, mkStripSpeed2,  mkRing, mkRing1, mkRingStatus, mkRingMode, mkRingColor, mkRingSpeed, mkBrightness, };

sMenuItem menu[] = {
  {mkBack, mkRoot, "A-RGB"},
    {mkRoot, mkStrip, "Strip"},
      {mkStrip, mkStrip1, "Strip 1"},
        {mkStrip1, mkStripStatus1, NULL, strip1st},
        {mkStrip1, mkStripMode1, NULL, strip1md},
        {mkStrip1, mkStripColor1, NULL, strip1cl},
        {mkStrip1, mkStripSpeed1, NULL, strip1sp},
        {mkStrip1, mkBack, "Back",},
      {mkStrip, mkStrip2, "Strip 2"},
        {mkStrip2, mkStripStatus2, NULL, strip2st},
        {mkStrip2, mkStripMode2, NULL, strip2md},
        {mkStrip2, mkStripColor2, NULL, strip2cl},
        {mkStrip2, mkStripSpeed2, NULL, strip2sp},
        {mkStrip2, mkBack, "Back"},
      {mkStrip, mkBack, "Back"},
    {mkRoot, mkRing, "Ring"},
      {mkRing, mkRingStatus, NULL, ring1st},
        {mkRing, mkRingMode, NULL, ring1md},
        {mkRing, mkRingColor, NULL, ring1cl},
        {mkRing, mkRingSpeed, NULL, ring1sp},
        {mkRing, mkBack, "Back"},
    {mkRoot, mkBrightness, NULL, setBrightness},
      {mkRing, mkBack, "Back"},
    
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
  delay(SpeedDelay); // делей
  setPixelStrip(Pixel,0,0,0); // Гасим
}
void stripRainbow2(byte red, byte green, byte blue, int SpeedDelay){
  int Pixel = random(STRIP_LED_NUM2); // Определяем, какой светоидиот загорится
  setPixelStrip(Pixel,red,green,blue); // отправляем  Значения на логмодуль
  FastLED.show();
  delay(SpeedDelay); // делей
  setPixelStrip(Pixel,0,0,0); // Гасим
}


// Статической режим
void stripStatic(){
  led1[0].setRGB(RGB_R[0], RGB_G[0], RGB_B[0]);  
  FastLED.show();
    delay(30); // FPS
    return;
}
void stripStatic2(){
  led2[0].setRGB(RGB_R[1], RGB_G[1], RGB_B[1]);  
  FastLED.show();
    delay(30); // FPS
    return;
}

//-----------------------


// --Режимы колец--


// Радужной режим
void ringRainbow(byte red, byte green, byte blue, int SpeedDelay){
  int Pixel = random(ARGB_round);
  setPixelStrip(Pixel,red,green,blue);
  FastLED.show();
  delay(SpeedDelay);
  setPixelStrip(Pixel,0,0,0);
}


// Статической режим
void ringStatic(){
  ring1[0].setRGB(RGB_R_R, RGB_G_R, RGB_B_R); // Присваиваем статическое значение
  FastLED.show();
    delay(30); // FPS
  return;
}

/*// Температурный режим
void ringTemp(){
  byte val = (getThermTemp(analogRead(THERM))); // Получаем температуру в цельсии
  val = constrain(val, CS_TEMP_MIN, CS_TEMP_MAX); // Ограничиваем
  map(val, CS_TEMP_MIN, CS_TEMP_MAX, 0, ARGB_round); // Конвертируем диапазон
  for(int led = 0; led < val; led++) { 
            ring1[led].setRGB(RGB_R_R, RGB_G_R, RGB_B_R); // Цикл
  };
  FastLED.show();
    delay(30); // FPS
    return;
}*/
//------------------------------


// Даем данные на ленты
void strip1(){
  switch(stripMode[0]){ // Выбор режима работы
    case 0: stripRainbow(random(255), random(255), random(255), 0); //Отправляем значения
    break;
    case 1: stripStatic();
    break;
  }
  return;
}
void strip2(){
  switch(stripMode[1]){ // Выбор режима работы
    case 0: stripRainbow(random(255), random(255), random(255), 0); //Отправляем значения
    break;
    case 1: stripStatic();
    break;
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
  }
 return;
}
//=================================
//---------------------------------
//=================================


// Настоящие функции
void strip1st(){ 
  StripStatus[0] = lcd.inputVal("Strip 1 status", 0, 1, 0);
  updateCaption(mkStripStatus1, "Status: (%d)", StripStatus[0]);
} 
void strip1md(){}
void strip1cl(){}
void strip1sp(){}
void strip2st(){
  StripStatus[1] = lcd.inputVal("Strip 2 status", 0, 1, 0);
  updateCaption(mkStripStatus2, "Status: (%d)", StripStatus[1]);
}
void strip2md(){}
void strip2cl(){}
void strip2sp(){}
void ring1st(){
  RingStatus = lcd.inputVal("Ring status", 0, 1, 0);
  updateCaption(mkRingStatus, "Status: (%d)", RingStatus); // Тут закончил
}
void ring1md(){}
void ring1cl(){}
void ring1sp(){}
void setBrightness(){}

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
  // FastLED.addLeds<LMUSIC_LED_TYPE,ARGB_LMusic, GRB>(led3, LMUSIC_LED_NUM); // Инициализация светомузыки(Вроде)
  delay(1000);
}


void loop() {
   
  map(brightnessMenu, 0, 255, 0, 100);                      // Яркость
    if (brightnessStatus == true){
      brightness = map(analogRead(PHOTO), 0, 1023, 0, 255);
    } else{
      brightness = map(brightnessMenu, 0, 100, 0, 255); 
    }
  FastLED.setBrightness(brightness); // Уровень яркости
  eEncoderState EncoderState = lcd.getEncoderState();
    uint8_t selectedMenuItem = lcd.showMenu(menu, menuLen, 1);
   
  if (RingStatus == 1) ring();
   switch(selectedMenuItem){
    case mkStripStatus1:{
      
    };
    break;
    case mkStripStatus2:{
      
    }
    break;
    case mkRingStatus:{
      
    }
    break;

   }
  
}
