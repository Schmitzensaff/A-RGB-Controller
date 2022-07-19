/*
A-RGB Controller v. 2.5(ПОКА В РАЗРАБОТКЕ)
Породил сие безобразие: Schmitzensaff

Изменения:
- Попытки в классы и ООП
- Добавлены разделения лент на группы
- Добавлены новые протоколы
- ВЕКТОРА ЧЁРТ ИХ ДЕРИ

На заметку: CRGB очень дохрена жрёт. Использовать с осторожностью
Если при исполнении кода на вас случайно упадёт дирижабль - меньше жрите и зовите Бригадира Пожарных

ПЫ.СЫ. Никому не советую лезть в эту индусятину. 

ПЫ.ПЫ.СЫ Мне пора к врачу лечиться. И, думаю, русский врач здесь уже не поможет.
*/


#define DEBUG 0 // 1 - логи, 0 - штатная
// АХТУНГ! дебаг существенно замедляет скорость обновления ленты
// Использовать с осторожностью

#define ST1_NUMLED 9
#define ST2_NUMLED 45 
#define ST3_NUMLED 60
#define ST_NUMLED_MST 60 

#define ST1_LEDPIN 5 
#define ST2_LEDPIN 6
#define ST3_LEDPIN 7


#define INIT_ADDR 511  // номер резервной ячейки
#define INIT_KEY 250 // ключ резервной ячейки

#define PARSE_AMOUNT 12 // Размер массива с данными
#define PHOTO A1 // Пин фоторезистора(НЕ РЕАЛИЗОВАНО)



#include <GParsingStream.h> //Библа парса(доработано)
#include <EEPROM.h> // Библа EEPROM
#include <libEffects.h> // Библа Контроля




int dataArray[PARSE_AMOUNT] = { // Массив с данными
  0, 0,
  255, 255,
  0, 0, 0,
  0, 0, 0,
  1, 0};

const int CLDarray[6] = { // массив для режима CLD
  0, 42, 85, 128, 171, 213
};
uint32_t stTimer[3] = {0, 0, 0};
PStream ptk1(36);
PStream ptk2(37);
ledSetting strip1(0);
ledSetting strip2(1);
ledSetting strip3(2);
void setup(){ // сетуп
    Serial.begin(9600);
    #if (DEBUG == 1)
    Serial.println("Connection Succesful");
    #endif
    
    
    // Опрашивем EEPROM
       /*  if (EEPROM.read(INIT_ADDR) != INIT_KEY) { // первый запуск
            for (int i = 0, j = 0; i < PARSE_AMOUNT; i++, j += 2) {
                EEPROM.put(j, dataArray[i]);
                }
            EEPROM.write(INIT_ADDR, INIT_KEY);    // записали ключ
            #if (DEBUG == 1)
            Serial.println("This is my first time, Sempai...");
            #endif
        }else{// НЕ первый запуск
            for (int i = 0, j = 0; i < PARSE_AMOUNT; i++, j += 2) { // читаем данные из EEPROM
                #if (DEBUG == 1)
                Serial.println("EEPROM GET ");
                Serial.print(i);
                Serial.print(" array addr");
                #endif
                EEPROM.get(j, dataArray[i]);
                }
                
            }     */
          
    Serial.println("A-RGB-Controller v.2.5 Alpha");
  stcont[0] = &FastLED.addLeds<WS2812, ST1_LEDPIN, GRB>(st[0],  ST1_NUMLED).setCorrection( TypicalLEDStrip);;
  stcont[1] = &FastLED.addLeds<WS2812, ST2_LEDPIN, GRB>(st[1],  ST2_NUMLED).setCorrection( TypicalLEDStrip);;
  stcont[2] = &FastLED.addLeds<WS2812, ST3_LEDPIN, GRB>(st[2],  ST3_NUMLED).setCorrection( TypicalLEDStrip);;
    strip1.setBrightness(100);
    strip2.setBrightness(100);
    strip3.setBrightness(100);
    strip1.addGroup(0, 0, ST1_NUMLED);
    strip2.addGroup(0, 0, 15);
    strip2.addGroup(1, 16, 30);
    strip2.addGroup(2, 31, 45);
    strip2.addGroup(3, 46, 60);
    strip3.addGroup(0, 0, 20);
    strip3.addGroup(1, 21, 40);
    strip3.addGroup(2, 41, 60);
    strip1.setColor(0, 255, 255, 255);
    strip2.setColor(0, 255, 0, 0);
    strip2.setColor(1, 0, 255, 0);
    strip2.setColor(2, 0, 0, 255);
    strip2.setColor(3, 255, 255, 255);
    strip3.setColor(0, 255, 0, 0);
    strip3.setColor(1, 0, 255, 0);
    strip3.setColor(2, 0, 0, 255);;
    strip1.setMode(0, 2, 0);
    strip2.setMode(0, 1, 0);
    strip2.setMode(1, 1, 0);
    strip2.setMode(2, 1, 0);
    strip2.setMode(3, 1, 0);
    strip3.setMode(0, 2, 0);
    strip3.setMode(1, 2, 0);
    strip3.setMode(2, 2, 0);
    
}

void loop(){
    if(millis() - stTimer[0] >= 50){
    stTimer[0] = millis();
    strip1.work(0);
    }
    //if(millis() - stTimer[1] >= 50){
  //  stTimer[1] = millis();
     strip2.work(0);
      strip2.work(1);
      strip2.work(2);
      strip2.work(3);
      
   // }
    if(millis() - stTimer[2] >= 50){
      stTimer[2] = millis();
      strip3.work(0);
      strip3.work(1);
      strip3.work(2);
    }
 
}
