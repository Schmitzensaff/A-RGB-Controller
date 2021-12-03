// Release 1.1
// Перешел на microLed от AlexGyver
// Убрал гирлянду. Ущербно выглядит.
// Единственный минус - наводки на ленту при выводе. Беда с библой
// Осталось сделать демона для удобной работы.
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
#define PARSE_AMOUNT 17         // число значений в массиве, который хотим получить
#define COLOR_DEBTH 3 // разрядность
#define CRT_PGM // коррекция по табличке
#define LED_TIMER 80
#define LED2_TIMER 80
#define RING_TIMER 80
#define LED_RAINBOW_DELAY 500
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
#include <microLED.h>
#include <GParsingStream.h>
//---------------------------------------

microLED < STRIP_LED_NUM, LED_PIN, -1, STRIP_LED_TYPE, ORDER_GRB > led1;
microLED < STRIP_LED_NUM2, LED2_PIN, -1, STRIP_LED_TYPE, ORDER_GRB > led2;
microLED < RING_LED_NUM, ARGB_round, -1, RING_LED_TYPE, ORDER_GRB > ring;

int x = 0;
int intData[PARSE_AMOUNT];     // массив численных значений после парсинга

uint32_t ledTimer;
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
void stripRaindowRiver(){
    for (int i = 0; i < STRIP_LED_NUM; i++) {
      led1.set(i, mWheel8(counter + i * 255 / STRIP_LED_NUM));   // counter смещает цвет
  }
  counter += 3;   // counter имеет тип byte и при достижении 255 сбросится в 0
  delay(30);
  }
  
//}

void strip2RainbowRiver(){
  
   for (int i = 0; i < STRIP_LED_NUM2; i++) {
      led2.set(i, mWheel8(counter2 + i * 255 / STRIP_LED_NUM2));   // counter смещает цвет
  }
  counter2 += 3;   // counter имеет тип byte и при достижении 255 сбросится в 0
  delay(30);

}



// Статической режим
void stripStatic(){
    Serial.println("SUKA");
    led1.fill(mRGB(intData[6], intData[7], intData[8]));
    
    Serial.println("BLYAT");
  
}
void stripStatic2(){
    led2.fill(mRGB(intData[9], intData[10], intData[11]));
  
}
//-----------------------


// --Режимы колец--


// Статической режим
void ringStatic(){
  ring.fill(mRGB(intData[12], intData[13], intData[14]));
  ring.show();
}
// Температурный режим
void ringTemp(){
  byte val = (getThermTemp(analogRead(THERM))); // Получаем температуру в цельсии
  val = constrain(val, CS_TEMP_MIN, CS_TEMP_MAX); // Ограничиваем
  map(val, CS_TEMP_MIN, CS_TEMP_MAX, 0, ARGB_round); // Конвертируем диапазон
  for(int led = 0; led < val; led++) { 
            ring.set(led, mRGB(intData[12], intData[13], intData[14])); // Цикл
  };
  ring.show();
    // delay(30); // FPS
    return;
}
 
byte counter3;
void ringRaindowRiver(){
  
    for (int i = 0; i < ARGB_round; i++) {
      ring.set(i, mWheel8(counter3 + i * 255 / ARGB_round));   // counter смещает цвет
  }
  counter3 += 3;   // counter имеет тип byte и при достижении 255 сбросится в 0
  delay(30); 
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

 if(intData[0] == 1 && intData[3] == 1) stripStatic();

 if(intData[0] == 1 && intData[3] == 0) stripRaindowRiver(); 


 if(intData[1] == 1 && intData[4] == 1) stripStatic2();

 if(intData[1] == 1 && intData[4] == 0) strip2RainbowRiver();

 if(intData[2] == 1 && intData[5] == 1) ringStatic();

 if(intData[2] == 1 && intData[5] == 0) ringRaindowRiver();

 if(intData[2] == 1 && intData[5] == 2) ringTemp();
 
 if(millis() - ledTimer >= 100){
   strip1Show();
   strip2Show();
   ringShow();
   ledTimer == millis();
 }
 //-----------------------------

}