// Release 1.0
//------------Настройки------------------
#define RESIST_10K 10000 // Вместо 10000 указ. точн. сопр. Резистора
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
#define PARSE_AMOUNT 17         // число значений в массиве, который хотим получить
//---------------------------------------


//--------------Пины---------------------
#define THERM A0 //Вместо A0 указ. анал.пин
#define LED_PIN 5 // Пин 1 ленты
#define LED2_PIN 6 // Пин 2 ленты
#define ARGB_round 8 // Пин колец
#define PHOTO A1 // Пин фоторез.
//---------------------------------------





//---------------Библы-------------------
#include <Arduino.h> // Стандартка для пердуины
#include <FastLED.h> // Работа с адреской
#include <Wire.h> // Надо
#include <GParsingStream.h>
//---------------------------------------

CRGB led1[STRIP_LED_NUM]; // Массив на 1 ленту(Вроде)
CRGB led2[STRIP_LED_NUM2]; // Массив на 2 ленту(Вроде)
CRGB ring1[RING_LED_NUM]; // Массив на 1 кольцо(Вроде)



int x = 0;
int intData[PARSE_AMOUNT];     // массив численных значений после парсинга
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
//----------------------------------


uint32_t parserTimer = 0;

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


// Иерархия меню.



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




//----------------------------------
//--Логический модуль светоидиотов--
//----------------------------------


// ==+Режимы лент+==

//Радужной режим
void stripRainbow(){
  
    byte red = random(255);
    byte green = random(255);
    byte blue = random(255);
    int SpeedDelay = 0;
    int Pixel = random(STRIP_LED_NUM); // Определяем, какой светоидиот загорится
    setPixelStrip(Pixel,red,green,blue); // отправляем  Значения на логмодуль
    FastLED.show();
    // delay(SpeedDelay); // делей
    setPixelStrip(Pixel,0,0,0); // Гасим
  
}
void stripRainbow2(){
  
    byte red = random(255);
    byte green = random(255);
    byte blue = random(255);
    int SpeedDelay = 0;
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
  //if(/*StripStatus[0] == true && stripMode[0] == 2 &&*/ millis() - strip1RaindowRiverModeTimer >= 5){
    //strip1RaindowRiverModeTimer = millis();
    for (int i = 0; i < STRIP_LED_NUM; i++ ) {         // от 0 до первой трети
      led1[i] = CHSV(counter + i * 2, 255, 255);  // HSV. Увеличивать HUE (цвет)
      // умножение i уменьшает шаг радуги
    } 
    counter++;        // counter меняется от 0 до 255 (тип данных byte)
    FastLED.show();
    Serial.println("Pizdets");
  
  }
//}

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
  
    for (int i = 0; i < STRIP_LED_NUM; i++){
    led1[i].setRGB(intData[6], intData[7], intData[8]);  
    FastLED.show();
     // delay(30); // FPS
    }
    return;
  
}
void stripStatic2(){
  
    for (int i = 0; i < STRIP_LED_NUM2; i++){
    led2[i].setRGB(intData[9], intData[10], intData[11]);  
    FastLED.show();
    // delay(30); // FPS
    return;
   
  }
}
//-----------------------


// --Режимы колец--


// Радужной режим
void ringRainbow(){
  
    byte red = random(255);
    byte green = random(255);
    byte blue = random(255);
    int SpeedDelay = 0;
    int Pixel = random(ARGB_round);
    setPixelStrip(Pixel,red,green,blue);
    FastLED.show();
    //delay(SpeedDelay);
    setPixelStrip(Pixel,0,0,0);
  
}

// Статической режим
void ringStatic(){
  
    for (int i = 0; i < RING_LED_NUM; i++){
      ring1[i].setRGB(intData[12], intData[13], intData[14]); // Присваиваем статическое значение
      FastLED.show();
      //delay(30); // FPS
    
  return;
  }
}
// Температурный режим
void ringTemp(){
  byte val = (getThermTemp(analogRead(THERM))); // Получаем температуру в цельсии
  val = constrain(val, CS_TEMP_MIN, CS_TEMP_MAX); // Ограничиваем
  map(val, CS_TEMP_MIN, CS_TEMP_MAX, 0, ARGB_round); // Конвертируем диапазон
  for(int led = 0; led < val; led++) { 
            ring1[led].setRGB(intData[12], intData[13], intData[14]); // Цикл
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
/*void strip1(){
  if (StripStatus[0] == true && millis() - strip1ModeTimer >= 0){
    strip1ModeTimer = millis();
  switch(stripMode[0]){ // Выбор режима работы
    case 0: stripRainbow(random(255), random(255), random(255), 0); //Отправляем значения
    break;
    case 1: stripStatic();
    break;
    case 2: stripRaindowRiver();
    default: delay(0);
  }
  }else{delay(0);}
  return;
}
void strip2(){
  if (StripStatus[1] == true) {
  switch(stripMode[1]){ // Выбор режима работы
    case 0: stripRainbow(random(255), random(255), random(255), 0); //Отправляем значения
    break;
    case 1: stripStatic();
    break;
    case 2: strip2RainbowRiver();
    default: delay(0);
  
  }
  }else{delay(0);}
  return;
}*/

//-----------------------------


// Даем команды ленте и кольцам

//=================================
//---------------------------------
//=================================




 void brightnessVoid(){
    if(intData[15] == 0){
       FastLED.setBrightness(map(analogRead(PHOTO), 0, 1023, 0, 100));
         
    }else if(intData[15] == 1){
      FastLED.setBrightness(intData[16]);
      Serial.print("BTSL");
      Serial.println(intData[16]);
        Serial.print("BTS");
        Serial.println(intData[15]);
    }
     // Уровень яркости
    
   }  
  
  
void setup() {
  Serial.begin(9600);
  Serial.println("Connection Successful");
  FastLED.addLeds<STRIP_LED_TYPE, LED_PIN, GRB>(led1, STRIP_LED_NUM).setCorrection( TypicalLEDStrip );; // Инициализация 1 ленты(Вроде) 
  FastLED.addLeds<STRIP_LED_TYPE, LED2_PIN, GRB>(led2, STRIP_LED_NUM2).setCorrection( TypicalLEDStrip );; // Инициализация 2 ленты(Вроде) 
  FastLED.addLeds<RING_LED_TYPE, ARGB_round, GRB>(ring1, RING_LED_NUM).setCorrection( TypicalLEDStrip );; // Инициализация 1 кольца(Вроде)

  // FastLED.addLeds<LMUSIC_LED_TYPE,ARGB_LMusic, GRB>(led3, LMUSIC_LED_NUM); // Инициализация светомузыки(Вроде)
  
}


void loop() {
  if(millis() - parserTimer >= 50){
    parserTimer = millis();
  
parsingStream((int*)&intData);
if (dataReady()) {
  Serial.println("SUKA");
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
  brightnessVoid();
 
//---------------------------------

 if(intData[0] == 0 && intData[3] == 1) stripStatic();

 if(intData[0] == 1 && intData[3] == 0) stripRainbow();

 if(intData[0] == 1 && intData[3] == 2) stripRaindowRiver(); 


 if(intData[1] == 1 && intData[4] == 1) stripStatic2();

 if(intData[1] == 1 && intData[4] == 0) stripRainbow2();

 if(intData[1] == 1 && intData[4] == 2) strip2RainbowRiver();

 if(intData[2] == 1 && intData[5] == 1) ringStatic();

 if(intData[2] == 1 && intData[5] == 0) ringRainbow();

 if(intData[2] == 1 && intData[5] == 2) ringRaindowRiver();

 if(intData[2] == 1 && intData[5] == 3) ringTemp();





    
   

    




//-----------------------------

}