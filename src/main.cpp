/*
A-RGB Controller v. 2.5beta
Породил сие безобразие: Schmitzensaff

Изменения:
-Откат
-ЁБНРТ этого гита. 

На заметку: Не заливать код в киоски
Если при исполнении кода на вас случайно упадёт дирижабль - меньше жрите и зовите Бригадира Пожарных

ПЫ.СЫ. Никому не советую лезть в эту индусятину. 

ПЫ.ПЫ.СЫ Мне пора к врачу лечиться. И, думаю, русский врач здесь уже не поможет.
*/


#define DEBUG 0 // 1 - логи, 0 - штатная
// АХТУНГ! дебаг существенно замедляет скорость обновления ленты
// Использовать с осторожностью

#define ST1_NUMLED 9 // Колво светодиодов первой ленты
#define ST2_NUMLED 45 // Колво светодиодов второй ленты
#define ST3_NUMLED 30 // Колво светодиодов третьей ленты
#define ST4_NUMLED 45 // Колво светодиодов четвертой ленты

#define ST1_LEDPIN 5 // Пин первой ленты
#define ST2_LEDPIN 6 // Пин второй ленты
#define ST3_LEDPIN 7 // Пин третьей ленты
#define ST4_LEDPIN 8 // Пин четвертой ленты

#define INIT_ADDR 510  // номер резервной ячейки
#define INIT_KEY 250 // ключ резервной ячейки



#define PARSE_AMOUNT 22 // Размер массива с данными
#define PHOTO A1 // Пин фоторезистора(НЕ РЕАЛИЗОВАНО)

#include <FastLED.h>
#include <GParsingStream.h>
#include <EEPROM.h>

CRGB st[ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+ST4_NUMLED];
CLEDController *stcont[4];
int dataArray[PARSE_AMOUNT] = { // Массив с данными
  0, 0, 0, 0,
  255, 255, 255, 255,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  1, 0};
PStream pst1(36);
const int CLDarray[6] = { // массив для режима CLD
  0, 42, 85, 128, 171, 213
};
uint32_t stTimer[4] = {0, 0, 0, 0};
void setup(){ // сетуп
    Serial.begin(9600);
    #if (DEBUG == 1)
    Serial.println("Connection Succesful");
    #endif
  
    
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
                
            }     
    Serial.println("A-RGB-Controller v.2.5beta");
  stcont[0] = &FastLED.addLeds<WS2812, ST1_LEDPIN, GRB>(st, 0, ST1_NUMLED).setCorrection( TypicalLEDStrip);;
  stcont[1] = &FastLED.addLeds<WS2812, ST2_LEDPIN, GRB>(st, ST1_NUMLED, ST2_NUMLED).setCorrection( TypicalLEDStrip);;
  stcont[2] = &FastLED.addLeds<WS2812, ST3_LEDPIN, GRB>(st, ST1_NUMLED+ST2_NUMLED, ST3_NUMLED).setCorrection( TypicalLEDStrip);;
  stcont[3] = &FastLED.addLeds<WS2812, ST4_LEDPIN, GRB>(st, ST1_NUMLED+ST2_NUMLED+ST3_NUMLED, ST4_NUMLED).setCorrection( TypicalLEDStrip);;
}
/*void brightnessVoid(){ // остатки от старой идеи(выкидывать жалко) 
  #if (DEBUG == 1)
  Serial.println("Bvoid");
  #endif
    if(dataArray[10] == 0){
       FastLED.setBrightness(map(analogRead(PHOTO), 0, 1023, 0, 100));
    }else if(dataArray[10] == 1){
      FastLED.setBrightness(dataArray[11]);
    }// Уровень яркости
   }  */ /*
void ST1RS(){ // Задел на режим RS для первой ленты
  #if (DEBUG == 1)
  Serial.println("ST1RS");
  #endif
       uint8_t thisHue = beat8(dataArray[4],255);
      fill_rainbow(st, ST1_NUMLED, thisHue, 10);
      st1cont->showLeds(dataArray[11]);
}
void ST2RS(){ // Задел на будущее
  #if (DEBUG == 1)
  Serial.println("ST2RS");
  #endif
      uint8_t thisHue = beat8(dataArray[7],255);
      fill_rainbow(st2, ST2_NUMLED, thisHue, 10);
      st2cont->showLeds(dataArray[11]);
}*/
void ST1STC(){ // статический режим (STC) для 1 ленты
  #if (DEBUG == 1)
  Serial.println("ST1STC");
  #endif
   static int i = 0;
    st[i].setRGB(dataArray[8], dataArray[9], dataArray[10]);
    stcont[0]->showLeds(dataArray[21]);
    i++;
    if(i > ST1_NUMLED) i = 0; // Ленивые голубцы
}
void ST2STC(){ // Статический режим(STC) для 2 ленты
  #if (DEBUG == 1)
  Serial.println("ST2STC");
  #endif
   static int i = 0;
    st[ST1_NUMLED+i].setRGB(dataArray[11], dataArray[12], dataArray[13]);
    stcont[1]->showLeds(dataArray[21]);
    i++;
    if(i > ST2_NUMLED) i = 0; // Ленивые голубцы
}
void ST3STC(){ // Статический режим(STC) для 3 ленты
  #if (DEBUG == 1)
  Serial.println("ST3STC");
  #endif
   static int i = 0;
    st[ST1_NUMLED+ST2_NUMLED+i].setRGB(dataArray[14], dataArray[15], dataArray[16]);
    stcont[2]->showLeds(dataArray[21]);
    i++;
    if(i > ST3_NUMLED) i = 0; // Ленивые голубцы
}
void ST4STC(){ // Статический режим(STC) для 4 ленты
  #if (DEBUG == 1)
  Serial.println("ST4STC");
  #endif
   static int i = 0;
    st[ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+i].setRGB(dataArray[17], dataArray[18], dataArray[19]);
    stcont[3]->showLeds(dataArray[21]);
    i++;
    if(i > ST4_NUMLED) i = 0; // Ленивые голубцы
}

void ST1RnS(){ // режим RnS для 1 ленты
  #if (DEBUG == 1)
  Serial.println("ST1RnS");
  #endif
static boolean switcher = 1;
static int i = 0;
//uint32_t rgtimer = 0;
if(switcher){ // если флаг 1
  st[i].setRGB(dataArray[8], dataArray[9], dataArray[10]); // задаем цвет
      stcont[0]->showLeds(dataArray[21]);
      i++;
      #if (DEBUG == 1)
      Serial.println("ST1RnS Switcher 0");
     #endif
      if(i >= ST1_NUMLED){// Ленивые голубцы
        i = 0;
        switcher = !switcher;
        #if (DEBUG == 1)
        Serial.println("ST1RnS loop reset 1");
        #endif
      }
      }else{ // Если флаг 0
        st[i].setRGB(0, 0, 0); // убираем цвет
      stcont[0]->showLeds(dataArray[21]);
      i++;
      #if (DEBUG == 1)
      Serial.println("ST1RnS Switcher 1");
      #endif
      if(i >= ST1_NUMLED){ // Ленивые голубцы
        i = 0;
        switcher = !switcher;
        #if (DEBUG == 1)
        Serial.println("ST1RnS loop reset 2");
        #endif
      }
      }
}
void ST2RnS(){ // Режим RnS для 2 ленты
  #if (DEBUG == 1)
   Serial.println("ST2RnS");
   #endif
static boolean switcher = 1;
static int i = 0;
//uint32_t rgtimer = 0;
if(switcher){ // Если флаг поднят
  st[ST1_NUMLED+i].setRGB(dataArray[11], dataArray[12], dataArray[13]); // присваиваем цвет
      stcont[1]->showLeds(dataArray[21]);
      i++;
      #if (DEBUG == 1)
      Serial.println("ST2RnS Switcher 0");
     #endif
      if(i >= ST2_NUMLED){ // Ленивые голубцы
        i = 0;
        switcher = !switcher; // поднимаем флаг
        #if (DEBUG == 1)
        Serial.println("ST2RnS loop reset 1");
        #endif
      }
      }else{ // если флаг опущен 
        st[ST1_NUMLED+i].setRGB(0, 0, 0); // убираем цвет
      stcont[1]->showLeds(dataArray[21]);
      i++;
      #if (DEBUG == 1)
      Serial.println("ST2RnS Switcher 1");
      #endif
      if(i >= ST2_NUMLED){// Ленивые голубцы
        i = 0;
        switcher = !switcher; // опускаем флаг
        #if (DEBUG == 1)
        Serial.println("ST2RnS loop reset 2");
        #endif
      }
      }
}
void ST3RnS(){ // Режим RnS для 3 ленты
  #if (DEBUG == 1)
   Serial.println("ST3RnS");
   #endif
static boolean switcher = 1;
static int i = 0;
//uint32_t rgtimer = 0;
if(switcher){ // Если флаг поднят
  st[ST1_NUMLED+ST2_NUMLED+i].setRGB(dataArray[14], dataArray[15], dataArray[16]); // присваиваем цвет
      stcont[2]->showLeds(dataArray[21]);
      i++;
      #if (DEBUG == 1)
      Serial.println("ST3RnS Switcher 0");
     #endif
      if(i >= ST3_NUMLED){ // Ленивые голубцы
        i = 0;
        switcher = !switcher; // поднимаем флаг
        #if (DEBUG == 1)
        Serial.println("ST3RnS loop reset 1");
        #endif
      }
      }else{ // если флаг опущен 
        st[ST1_NUMLED+ST2_NUMLED+i].setRGB(0, 0, 0); // убираем цвет
      stcont[2]->showLeds(dataArray[21]);
      i++;
      #if (DEBUG == 1)
      Serial.println("ST3RnS Switcher 1");
      #endif
      if(i >= ST3_NUMLED){// Ленивые голубцы
        i = 0;
        switcher = !switcher; // опускаем флаг
        #if (DEBUG == 1)
        Serial.println("ST3RnS loop reset 2");
        #endif
      }
      }
}
void ST4RnS(){ // Режим RnS для 4 ленты
  #if (DEBUG == 1)
   Serial.println("ST4RnS");
   #endif
static boolean switcher = 1;
static int i = 0;
//uint32_t rgtimer = 0;
if(switcher){ // Если флаг поднят
  st[ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+i].setRGB(dataArray[17], dataArray[18], dataArray[19]); // присваиваем цвет
      stcont[3]->showLeds(dataArray[21]);
      i++;
      #if (DEBUG == 1)
      Serial.println("ST4RnS Switcher 0");
     #endif
      if(i >= ST4_NUMLED){ // Ленивые голубцы
        i = 0;
        switcher = !switcher; // поднимаем флаг
        #if (DEBUG == 1)
        Serial.println("ST4RnS loop reset 1");
        #endif
      }
      }else{ // если флаг опущен 
        st[ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+i].setRGB(0, 0, 0); // убираем цвет
      stcont[3]->showLeds(dataArray[21]);
      i++;
      #if (DEBUG == 1)
      Serial.println("ST4RnS Switcher 1");
      #endif
      if(i >= ST4_NUMLED){// Ленивые голубцы
        i = 0;
        switcher = !switcher; // опускаем флаг
        #if (DEBUG == 1)
        Serial.println("ST4RnS loop reset 2");
        #endif
      }
      }
}

void ST1CLD(){ // Режим CLD для 1 ленты
  #if (DEBUG == 1)
  Serial.println("ST1CLD");
  #endif
static int i=0, j=0;
  st[i] = CHSV(CLDarray[j], 255, 255); // Поочередное считывание Hue из массива
  i++;
  if(i >= ST1_NUMLED){ // Ленивые голубцы
    stcont[0]->showLeds(dataArray[21]);
    i = 0;
    j++;
  }
  if(j >= 6) j=0; // Ленивые голубцы
}
void ST2CLD(){ // Режим CLD для 2 ленты
  #if (DEBUG == 1)
  Serial.println("ST2CLD");
  #endif
static int i=0, j=0;
  st[ST1_NUMLED+i] = CHSV(CLDarray[j], 255, 255); // Поочередное присвоение Hue из массива
  
  i++;
  if(i >= ST2_NUMLED){ // Ленивые голубцы
    stcont[1]->showLeds(dataArray[21]);
    i = 0;
    j++;
  }
  if(j >= 6) j=0;
}
void ST3CLD(){ // Режим CLD для 3 ленты
  #if (DEBUG == 1)
  Serial.println("ST3CLD");
  #endif
static int i=0, j=0;
  st[ST1_NUMLED+ST2_NUMLED+i] = CHSV(CLDarray[j], 255, 255); // Поочередное присвоение Hue из массива
  
  i++;
  if(i >= ST3_NUMLED){ // Ленивые голубцы
    stcont[2]->showLeds(dataArray[21]);
    i = 0;
    j++;
  }
  if(j >= 6) j=0;
}
void ST4CLD(){ // Режим CLD для 4 ленты
  #if (DEBUG == 1)
  Serial.println("ST4CLD");
  #endif
static int i=0, j=0;
  st[ST1_NUMLED+ST2_NUMLED+ST3_NUMLED+i] = CHSV(CLDarray[j], 255, 255); // Поочередное присвоение Hue из массива
  
  i++;
  if(i >= ST4_NUMLED){ // Ленивые голубцы
    stcont[3]->showLeds(dataArray[21]);
    i = 0;
    j++;
  }
  if(j >= 6) j=0;
}
void st1task(){ // Выборка заданий для первой ленты
  #if (DEBUG == 1)
  Serial.println("st1task");
  #endif
   // if(dataArray[0] == 1 && dataArray[4] == 0) ST1RS(); 
    if(dataArray[0] == 1 && dataArray[4] == 1) ST1STC();
    if(dataArray[0] == 1 && dataArray[4] == 2) ST1RnS();  
    if(dataArray[0] == 1 && dataArray[4] == 3) ST1CLD();
}
void st2task(){ // Выборка заданий для второй ленты
  #if (DEBUG == 1)
  Serial.println("st2task");
  #endif
    //if(dataArray[1] == 1 && dataArray[5] == 0) ST2RS(); 
    if(dataArray[1] == 1 && dataArray[5] == 1) ST2STC();
    if(dataArray[1] == 1 && dataArray[5] == 2) ST2RnS();
    if(dataArray[1] == 1 && dataArray[5] == 3) ST2CLD();
}
void st3task(){ // Выборка заданий для второй ленты
  #if (DEBUG == 1)
  Serial.println("st3task");
  #endif
    //if(dataArray[2] == 1 && dataArray[6] == 0) ST3RS(); 
    if(dataArray[2] == 1 && dataArray[6] == 1) ST3STC();
    if(dataArray[2] == 1 && dataArray[6] == 2) ST3RnS();
    if(dataArray[2] == 1 && dataArray[6] == 3) ST3CLD();
}
void st4task(){ // Выборка заданий для второй ленты
  #if (DEBUG == 1)
  Serial.println("st4task");
  #endif
    //if(dataArray[3] == 1 && dataArray[7] == 0) ST2RS(); 
    if(dataArray[3] == 1 && dataArray[7] == 1) ST4STC();
    if(dataArray[3] == 1 && dataArray[7] == 2) ST4RnS();
    if(dataArray[3] == 1 && dataArray[7] == 3) ST4CLD();
}
void loop(){
  #if (DEBUG == 1)
      Serial.println("parsing task");
      #endif
        pst1.parsingStream((int*)&dataArray); // Принимаем пакет, пишем в массив dataArray
    if(pst1.dataReady()){// Если данные приняты...
      Serial.println("Command accepted"); 
    for(int i = 0, j = 0; i < PARSE_AMOUNT; i++, j += 2){ // Голубцы. 
      EEPROM.put(j, dataArray[i]); // ....Заносим данные в EEPROM
    }
    }
     
       // brightnessVoid();
        if(millis() - stTimer[0] >= 50){ // Каждые 50 мс...
            stTimer[0] = millis();
               st1task(); // ..стучаться на выборку для 1 ленты
        }
     if(millis() - stTimer[1] >= 50){ // Каждые 50 мс...
      stTimer[1] = millis();
      st2task(); // стучаться на выборку для 2 ленты
     }
     if(millis() - stTimer[2] >= 50){ // Каждые 50 мс...
      stTimer[2] = millis();
      st3task(); // стучаться на выборку для 3 ленты
     }
     if(millis() - stTimer[3] >= 50){ // Каждые 50 мс...
      stTimer[3] = millis();
      st4task(); // стучаться на выборку для 4 ленты
     }
    
    
}
 
