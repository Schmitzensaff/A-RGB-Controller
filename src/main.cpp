/*
A-RGB Controller v. 2.4
Породил сие безобразие: Schmitzensaff

Изменения:
- Доделан режим RS
- Немного переработан пакет
- Сьездил в Татарстан
Даже не знаю, что важнее

Данные отправляются в порт на скорости 9600 бод, в виде пакета типа
"$data0 data1 data2 data3 data4 data5...... data11;"
Порядок
0 - Вкл/выкл первая лента(1 или 0 соответственно)
1 - Вкл/выкл вторая лента(1 или 0 соответственно)
2 - Режим первой ленты (1 - статика, 2 - RnS, 3 - CLD)
3 - Режим второй ленты (1 - статика, 2 - RnS, 3 - CLD)
4, 5, 6 - для первой ленты R, G, B соответственно(от 0 до 255) (Элемент 4 - Регулировать скорость в режиме RS 1 ленты)
7, 8, 9 - для второй ленты R, G, B соответственно(от 0 до 255) (Элемент 7 - Регулировать скорость в режиме RS 2 ленты)
10 - Отладочная ячейка (Любое число от 0 до 255)
11 - Уровень яркости(от 0 до 100)
Если при исполнении кода на вас случайно упадёт дирижабль - звать Бригадира Пожарных

ПЫ.СЫ. Я наконец-то научился кодить. (наверное)
*/


#define DEBUG 0 // 1 - логи, 0 - штатная
// АХТУНГ! дебаг существенно замедляет скорость обновления ленты
// Использовать с осторожностью


#define INIT_ADDR 511  // номер резервной ячейки
#define INIT_KEY 250 // ключ резервной ячейки
#define ST1_NUMLED 9 // Колво светодиодов первой ленты
#define ST2_NUMLED 45 // Колво светодиодов второй ленты

#define ST1_LEDPIN 5 // Пин первой ленты
#define ST2_LEDPIN 6 // Пин второй ленты
#define PARSE_AMOUNT 12 // Размер массива с данными
#define PHOTO A1 // Пин фоторезистора(НЕ РЕАЛИЗОВАНО)


#include <FastLED.h> // Библа Светодиодов
#include <GParsingStream.h> //Библа парса
#include <EEPROM.h> // Библа EEPROM

CRGB st1[ST1_NUMLED];
CRGB st2[ST2_NUMLED];
CLEDController *st1cont;
CLEDController *st2cont;
int dataArray[PARSE_AMOUNT] = { // Массив с данными
  0, 0,
  255, 255,
  0, 0, 0,
  0, 0, 0,
  1, 0};

const int CLDarray[6] = { // массив для режима CLD
  0, 42, 85, 128, 171, 213
};
uint32_t st1Timer = 0;
uint32_t st2Timer = 0;
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
    Serial.println("A-RGB-Controller v.2.4");
  st1cont = &FastLED.addLeds<WS2812, ST1_LEDPIN, GRB>(st1, ST1_NUMLED).setCorrection( TypicalLEDStrip);;
  st2cont = &FastLED.addLeds<WS2812, ST2_LEDPIN, GRB>(st2, ST2_NUMLED).setCorrection( TypicalLEDStrip);;
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
   }  */ 
void ST1RS(){ // Задел на режим RS для первой ленты
  #if (DEBUG == 1)
  Serial.println("ST1RS");
  #endif
       uint8_t thisHue = beat8(dataArray[4],255);
      fill_rainbow(st1, ST1_NUMLED, thisHue, 10);
      st1cont->showLeds(dataArray[11]);
}
void ST1STC(){ // статический режим (STC) для первой ленты
  #if (DEBUG == 1)
  Serial.println("ST1STC");
  #endif
   static int i = 0;
    st1[i].setRGB(dataArray[4], dataArray[5], dataArray[6]);
    st1cont->showLeds(dataArray[11]);
    i++;
    if(i > ST1_NUMLED) i = 0; // Ленивые голубцы
}
void ST1RnS(){ // режим RnS для первой ленты
  #if (DEBUG == 1)
  Serial.println("ST1RnS");
  #endif
static boolean switcher = 0;
static int ci = 0;
//uint32_t rgtimer = 0;
if(switcher == 0){ // если флаг 0
  st1[ci].setRGB(dataArray[4], dataArray[5], dataArray[6]); // задаем цвет
      st1cont->showLeds(dataArray[11]);
      ci++;
      #if (DEBUG == 1)
      Serial.println("ST1RnS Switcher 0");
     #endif
      if(ci >= ST1_NUMLED){// Ленивые голубцы
        ci = 0;
        switcher = !switcher;
        #if (DEBUG == 1)
        Serial.println("ST1RnS loop reset 1");
        #endif
      }
      }
if(switcher == 1){ // Если флаг 1
        st1[ci].setRGB(0, 0, 0); // убираем цвет
      st1cont->showLeds(dataArray[11]);
      ci++;
      #if (DEBUG == 1)
      Serial.println("ST1RnS Switcher 1");
      #endif
      if(ci >= ST1_NUMLED){ // Ленивые голубцы
        ci = 0;
        switcher = !switcher;
        #if (DEBUG == 1)
        Serial.println("ST1RnS loop reset 2");
        #endif
      }
      }
}

void ST1CLD(){ // Режим CLD для первой ленты
  #if (DEBUG == 1)
  Serial.println("ST1CLD");
  #endif
static int i=0, j=0;
  st1[i] = CHSV(CLDarray[j], 255, 255); // Поочередное считывание Hue из массива
  i++;
  if(i >= ST1_NUMLED){ // Ленивые голубцы
    st1cont->showLeds(dataArray[11]);
    i = 0;
    j++;
  }
  if(j >= 6) j=0; // Ленивые голубцы
}

void ST2RnS(){ // Режим RnS для второй ленты
  #if (DEBUG == 1)
   Serial.println("ST2RnS");
   #endif
static boolean switcher = 0;
static int ci = 0;
//uint32_t rgtimer = 0;
if(switcher == 0){ // Если флаг опущен
  st2[ci].setRGB(dataArray[7], dataArray[8], dataArray[9]); // присваиваем цвет
      st2cont->showLeds(dataArray[11]);
      ci++;
      #if (DEBUG == 1)
      Serial.println("ST2RnS Switcher 0");
     #endif
      if(ci >= ST2_NUMLED){ // Ленивые голубцы
        ci = 0;
        switcher = !switcher; // поднимаем флаг
        #if (DEBUG == 1)
        Serial.println("ST2RnS loop reset 1");
        #endif
      }
      }
if(switcher == 1){ // если флаг поднят
        st2[ci].setRGB(0, 0, 0); // убираем цвет
      st2cont->showLeds(dataArray[11]);
      ci++;
      #if (DEBUG == 1)
      Serial.println("ST2RnS Switcher 1");
      #endif
      if(ci >= ST2_NUMLED){// Ленивые голубцы
        ci = 0;
        switcher = !switcher; // опускаем флаг
        #if (DEBUG == 1)
        Serial.println("ST2RnS loop reset 2");
        #endif
      }
      }
}
void ST2RS(){ // Задел на будущее
  #if (DEBUG == 1)
  Serial.println("ST2RS");
  #endif
      uint8_t thisHue = beat8(dataArray[7],255);
      fill_rainbow(st2, ST2_NUMLED, thisHue, 10);
      st2cont->showLeds(dataArray[11]);
}
void ST2CLD(){ // Режим CLD для второй ленты
  #if (DEBUG == 1)
  Serial.println("ST2CLD");
  #endif
static int i=0, j=0;
  st2[i] = CHSV(CLDarray[j], 255, 255); // Поочередное присвоение Hue из массива
  
  i++;
  if(i >= ST2_NUMLED){ // Ленивые голубцы
    st2cont->showLeds(dataArray[11]);
    i = 0;
    j++;
  }
  if(j >= 6) j=0;
}
void ST2STC(){ // Статический режим(STC) для второй ленты
  #if (DEBUG == 1)
  Serial.println("ST2STC");
  #endif
   static int i = 0;
    st2[i].setRGB(dataArray[7], dataArray[8], dataArray[9]);
    st2cont->showLeds(dataArray[11]);
    i++;
    if(i > ST2_NUMLED) i = 0; // Ленивые голубцы
}
void st1task(){ // Выборка заданий для первой ленты
  #if (DEBUG == 1)
  Serial.println("st1task");
  #endif
    if(dataArray[0] == 1 && dataArray[2] == 0) ST1RS(); 
    if(dataArray[0] == 1 && dataArray[2] == 1) ST1STC();
    if(dataArray[0] == 1 && dataArray[2] == 2) ST1RnS();  
    if(dataArray[0] == 1 && dataArray[2] == 3) ST1CLD();
}
void st2task(){ // Выборка заданий для второй ленты
  #if (DEBUG == 1)
  Serial.println("st2task");
  #endif
    if(dataArray[1] == 1 && dataArray[3] == 0) ST2RS(); 
    if(dataArray[1] == 1 && dataArray[3] == 1) ST2STC();
    if(dataArray[1] == 1 && dataArray[3] == 2) ST2RnS();
    if(dataArray[1] == 1 && dataArray[3] == 3) ST2CLD();
}
void loop(){
   #if (DEBUG == 1)
      Serial.println("parsing task");
      #endif
        parsingStream((int*)&dataArray); // Принимаем пакет, пишем в массив dataArray
    if(dataReady()){// Если данные приняты...
      Serial.println("Command accepted"); 
    for(int i = 0, j = 0; i < PARSE_AMOUNT; i++, j += 2){ // Голубцы. 
      EEPROM.put(j, dataArray[i]); // ....Заносим данные в EEPROM
    }
    } 
       // brightnessVoid();
        if(millis() - st1Timer >= 50){ // Каждые 50 мс...
            st1Timer = millis();
               st1task(); // ..стучаться на выборку для первой ленты
        }
     if(millis() - st2Timer >= 50){ // Каждые 50 мс...
      st2Timer = millis();
      st2task(); // стучаться на выборку для второй ленты
     }
        
    
    
    }
