/*
A-RGB Controller v. 2.7
Породил сие безобразие: Schmitzensaff

Изменения:
- Контроль за статами ПК(Основано на проекте AlexGyver)
- Дисплей
- Шестая лента теперь вентиляторы
На заметку: Не заливать код в киоски
Если при исполнении кода на вас случайно упадёт дирижабль - меньше жрите и зовите Бригадира Пожарных

ПЫ.СЫ. Никому не советую лезть в эту индусятину. 

ПЫ.ПЫ.СЫ Мне пора к врачу лечиться. И, думаю, русский врач здесь уже не поможет.
*/


#define DEBUG 0 // 1 - логи, 0 - штатная
// АХТУНГ! дебаг существенно замедляет скорость обновления ленты
// Использовать с осторожностью

//----Настройки----
#define ST1_NUMLED 9  // Колво светодиодов 1 ленты
#define ST2_NUMLED 45 // Колво светодиодов 2 ленты
#define ST3_NUMLED 30 // Колво светодиодов 3 ленты
#define ST4_NUMLED 45 // Колво светодиодов 4 ленты
#define ST5_NUMLED 9  // Колво светодиодов 5 ленты
#define ST6_NUMLED 4 // Колво светодиодов 6 ленты
//----Настройки----

//----Пины----
#define ST1_LEDPIN 3  // Пин 1 ленты
#define ST2_LEDPIN 4  // Пин 2 ленты
#define ST3_LEDPIN 5  // Пин 3 ленты
#define ST4_LEDPIN 6  // Пин 4 ленты
#define ST5_LEDPIN 7  // Пин 5 ленты
#define ST6_LEDPIN 8  // Пин 6 ленты
#define TEMPDAT A1    // Пин термодатчика
#define BTN1 A2       // Кнопка 1
#define BTN2 A3       // Кнопка 2
//----Пины----

//----Отладошное----
#define INIT_ADDR 510   // номер резервной ячейки
#define INIT_KEY 249    // ключ резервной ячейки
#define STSUM_NUMLED ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+ST4_NUMLED+ST5_NUMLED+ST6_NUMLED
#define PARSE_AMOUNT 32 // Размер массива с данными
#define EB_DEB 50       // дебаунс кнопки, мс
#define EB_CLICK 400    // таймаут накликивания кнопки, мс
#define ERROR_DUTY 90
//----Отладошное----

#include <FastLED.h>
#include <GParsingStream.h>
#include <VirtualButton.h>
#include <LiquidCrystal_I2C.h>
#include <GyverPWM.h>
#include <EEPROM.h>

CRGB st[STSUM_NUMLED];
CLEDController *stcont[6];
LiquidCrystal_I2C lcd(0x27, 20, 4);
PStream pst1(36);
VButton btn1;
VButton btn2;
int dataArray[PARSE_AMOUNT] = { // Массив с данными
  0, 0, 0, 0, 0, 0,
  255, 255, 255, 255, 255, 255,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  1, 0};
int dataPC[20];
int duty, LEDcolor;
uint32_t stTimer = 0;
uint32_t lcdTimeout= 0;
static bool lupa[6] = {1,1,1,1,1,1};
static int pupa[6] = {0,0,0,0,0,0};
bool buttFlag[2] = {1,  1};
bool timeOut_flag;
bool lcdFlag = 1;
byte mainTemp, temp1, temp2;


String string_convert;

char inData[82];
byte index = 0;
void setup(){ // сетуп
    Serial.begin(9600);
    #if (DEBUG == 1)
    Serial.println("Connection Succesful");
    #endif
    pinMode(BTN1, INPUT_PULLUP);
    pinMode(BTN2, INPUT_PULLUP);
    pinMode(9, OUTPUT);
    // Опрашивем EEPROM
         if (EEPROM.read(INIT_ADDR) != INIT_KEY) { // первый запуск
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
          //$1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 255 0 255 0 0 255 0 0 255 255 255 255 255 0 0 0 255 0 0 0 255 255 255 255 1 100;      
            }    
            lcd.init();
            lcd.backlight();
            lcd.clear(); 
            PWM_20KHZ_D9(400);
    Serial.println("A-RGB-Controller v.2.7");
  stcont[0] =  &FastLED.addLeds<WS2812, ST1_LEDPIN, GRB>(st, 0, ST1_NUMLED).setCorrection( TypicalLEDStrip);;
  stcont[1] =  &FastLED.addLeds<WS2812, ST2_LEDPIN, GRB>(st, ST1_NUMLED, ST2_NUMLED).setCorrection( TypicalLEDStrip);;
  stcont[2] =  &FastLED.addLeds<WS2812, ST3_LEDPIN, GRB>(st, ST1_NUMLED+ST2_NUMLED, ST3_NUMLED).setCorrection( TypicalLEDStrip);;
  stcont[3] =  &FastLED.addLeds<WS2812, ST4_LEDPIN, GRB>(st, ST1_NUMLED+ST2_NUMLED+ST3_NUMLED, ST4_NUMLED).setCorrection( TypicalLEDStrip);;
  stcont[4] =  &FastLED.addLeds<WS2812, ST5_LEDPIN, GRB>(st, ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+ST4_NUMLED, ST5_NUMLED).setCorrection( TypicalLEDStrip);;
  stcont[5] =  &FastLED.addLeds<WS2812, ST6_LEDPIN, GRB>(st, ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+ST4_NUMLED+ST5_NUMLED, ST6_NUMLED).setCorrection( TypicalLEDStrip);;
 }

void STSTC(int startPoint, int length, int stripId, byte R, byte G, byte B){ // статический режим (STC) для 1 ленты
  #if (DEBUG == 1)
  Serial.println("STSTC");
  #endif
   
    st[startPoint+pupa[stripId]].setRGB(R, G, B);
    stcont[stripId]->showLeds(dataArray[31]);
    pupa[stripId]++;
    if(pupa[stripId] > length) pupa[stripId] = 0; // Ленивые голубцы
}

void STRnS(int startPoint, int length, int stripId, byte R, byte G, byte B){ // режим RnS для 1 ленты
  #if (DEBUG == 1)
  Serial.println("STRnS");
  #endif


if(lupa[stripId]){ // если флаг 1
stcont[stripId]->showLeds(dataArray[31]);
  st[startPoint+pupa[stripId]].setRGB(R,G,B); // задаем цвет
      stcont[stripId]->showLeds(dataArray[31]);
      pupa[stripId]++;
      #if (DEBUG == 1)
      Serial.println("STRnS Switcher 1");
     #endif
      if(pupa[stripId] >= length){// Ленивые голубцы
        pupa[stripId] = 0;

        stcont[stripId]->showLeds(dataArray[31]);
        lupa[stripId] = !lupa[stripId];
        #if (DEBUG == 1)
        Serial.println("STRnS loop reset 1");
        #endif
      }
      }else{ // Если флаг 0
       stcont[stripId]->showLeds(dataArray[31]);
        st[startPoint+pupa[stripId]].setRGB(0, 0, 0); // убираем цвет
     stcont[stripId]->showLeds(dataArray[31]);
      pupa[stripId]++;
      #if (DEBUG == 1)
      Serial.println("STRnS Switcher 1");
      #endif
      if(pupa[stripId] >= length){ // Ленивые голубцы
        pupa[stripId] = 0;
        stcont[stripId]->showLeds(dataArray[31]);
        lupa[stripId] = !lupa[stripId];
        #if (DEBUG == 1)
        Serial.println("STRnS loop reset 2");
        #endif
        
      }
      }
}
 byte hue;
void RGTHM(int startPoint, int length, int stripId){
  if(dataPC[13] == 1) hue = map(dataPC[15], 0, 1000, 180, 0);
  st[startPoint+pupa[stripId]].setHSV(hue, 255,255);
  stcont[stripId]->showLeds(dataArray[31]);
  pupa[stripId]++;
 
  if ((pupa[stripId] > length)) pupa[stripId] = 0;
  
  
}
void dutyCalculate() {
  if (dataPC[12] == 1)                  // если стоит галочка ManualFAN
    duty = dataPC[14];                  // скважность равна установленной ползунком
  else {                                // если нет
    switch (dataPC[18]) {
      case 0: mainTemp = dataPC[0];                   // взять опорную температуру как CPU
        break;
      case 1: mainTemp = dataPC[1];                   // взять опорную температуру как GPU
        break;
      case 2: mainTemp = max(dataPC[0], dataPC[1]);   // взять опорную температуру как максимум CPU и GPU
        break;
      case 3: mainTemp = temp1;
        break;
      case 4: mainTemp = temp2;
        break;
    }
  
   if(dataPC[13] == 0) hue = map(mainTemp, dataPC[11], dataPC[10], 180, 0);
    duty = map(mainTemp, dataPC[11], dataPC[10], dataPC[9], dataPC[8]);
    duty = constrain(duty, dataPC[9], dataPC[8]);
  }
 // if (!timeOut_flag) duty = ERROR_DUTY;               // если пропало соединение, поставить вентиляторы на ERROR_DUTY
}
void button(){
   btn1.poll(digitalRead(BTN1));
  if(btn1.click()){
    buttFlag[0] = !buttFlag[0];
  }
  btn2.poll(digitalRead(BTN2));
  if(btn2.click()){
    buttFlag[1] = !buttFlag[1];
  }
   }



void parse1(){
    while (Serial.available() > 0) {
    char aChar = Serial.read();
    if (aChar != 'E') {
      inData[index] = aChar;
      index++;
      inData[index] = '\0';
    } else {
      char *p = inData;
      char *str;
      index = 0;
      String value = "";
      while ((str = strtok_r(p, ";", &p)) != NULL) {
        string_convert = str;
        dataPC[index] = string_convert.toInt();
        index++;
      }
      index = 0;
      lcdFlag = 1;
      lcdTimeout = millis();
    }
}

   
}
void parse2(){
  pst1.parsingStream((int*)&dataArray);
     if(pst1.dataReady()){// Если данные приняты...
      Serial.println("Command accepted"); 
    for(int i = 0, j = 0; i < PARSE_AMOUNT; i++, j += 2){ // Голубцы. 
      EEPROM.put(j, dataArray[i]); // ....Заносим данные в EEPROM
    }
    }
}
void sttask(int startPoint, int length, int stripId, byte R, byte G, byte B){ // Выборка заданий для первой ленты
  #if (DEBUG == 1)
  Serial.println("sttask");
  #endif
    if(dataArray[0+stripId] == 1 && dataArray[6+stripId] == 1) STSTC(startPoint, length, stripId, R, G, B);
    if(dataArray[0+stripId] == 1 && dataArray[6+stripId] == 2) STRnS(startPoint, length, stripId, R, G, B);  
   }
void stwork(){
              sttask(0, ST1_NUMLED, 0, dataArray[12], dataArray[13], dataArray[14]); // ...лента 1 - догадался Штирлиц
              sttask(ST1_NUMLED, ST2_NUMLED, 1, dataArray[15], dataArray[16], dataArray[17]); // ...лента 2 - догадался Штирлиц
              sttask(ST1_NUMLED+ST2_NUMLED, ST3_NUMLED, 2, dataArray[18], dataArray[19], dataArray[20]); // ...лента 3 - догадался Штирлиц
              sttask(ST1_NUMLED+ST2_NUMLED+ST3_NUMLED, ST4_NUMLED, 3, dataArray[21], dataArray[22], dataArray[23]); // ...лента 4 - догадался Штирлиц
              sttask(ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+ST4_NUMLED, ST5_NUMLED, 4, dataArray[24], dataArray[25], dataArray[26]); //  ...лента 5 - догадался Штирлиц
              if(buttFlag[1] == 1){
                RGTHM(ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+ST4_NUMLED+ST5_NUMLED, ST6_NUMLED, 5);
}else{

  sttask(ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+ST4_NUMLED+ST5_NUMLED, ST6_NUMLED, 5, dataArray[27], dataArray[28], dataArray[29]); // ...колечки - догадался Штирлиц
}}

void lcdPrint(){
 if(buttFlag[0] == 1){
  if(lcdFlag ){
  const char sas = 0xDF;
    
    lcd.setCursor(0,0);
    lcd.print("CPU: ");
    lcd.print(dataPC[0]);
    lcd.print(sas);
    lcd.print("C / ");
    lcd.print(dataPC[4]);
    lcd.print("% ");
    lcd.setCursor(0,1);
    lcd.print("GPU: ");
    lcd.print(dataPC[1]);
    lcd.print(sas);
    lcd.print("C / ");
    lcd.print(dataPC[5]);
    lcd.print("% ");
    lcd.setCursor(0,2);
    lcd.print("GPUmem:");
    lcd.print(dataPC[7]);
    lcd.print("% / ");
    lcd.print("RAM:");
    lcd.print(dataPC[6]);
    lcd.print("% ");
    lcd.setCursor(0,3);
    lcd.print("DTY: ");
    lcd.print(duty);
   
    lcd.print("% / ");
    lcd.print("HDD:");
    lcd.print(dataPC[3]);
    lcd.print(sas);
    lcd.print("C");
    lcdFlag = !lcdFlag;
  }
  }else{
    lcd.clear();
    lcd.setCursor(6,1);
    lcd.print("LCD MODE");
  }
}
void loop(){
  if(buttFlag[0] == 1){ parse1();
  }else{  parse2(); }
        button();
        dutyCalculate();
        PWM_20KHZ_D9(duty*10);
        if(millis() - stTimer >=  65){ // В дверь стучали каждые 65 мс...
            stTimer = millis();
            stwork(); // Ленты - подумал Мюллер
            lcdPrint(); // Дисплей - догадался Штирлиц
        }
        
       
}
 
