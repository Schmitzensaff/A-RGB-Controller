// 2.2 Зашито сохранение в EEPROM

#define INIT_ADDR 511  // номер резервной ячейки
#define INIT_KEY 250 // ключ резервной ячейки
#define ST1_NUMLED 9
#define ST2_NUMLED 45

#define ST1_LEDPIN 5
#define ST2_LEDPIN 6
#define PARSE_AMOUNT 12
#define PHOTO A1
#include <FastLED.h>
#include <Arduino_FreeRTOS.h>
#include <GParsingStream.h>
#include <EEPROM.h>
CRGB st1[ST1_NUMLED];
CRGB st2[ST2_NUMLED];
uint32_t SHOWTIMER = 0;
int dataArray[PARSE_AMOUNT] = {
  0, 0,
  255, 255,
  0, 0, 0,
  0, 0, 0,
  1, 0};
byte counter = 0;
byte counter2 = 0;

void st1Task(void *pvParameters);
void st2Task(void *pvParameters);
void parsingTask(void *pvParameters);
void setup() {
  Serial.begin(9600);
  Serial.println("Connection Succesful");
  if (EEPROM.read(INIT_ADDR) != INIT_KEY) { // первый запуск
    Serial.println("This is my first time, Sempai...");
    for (int i = 0, j = 0; i < PARSE_AMOUNT; i++, j += 2) {
      EEPROM.put(j, dataArray[i]);
    }
    EEPROM.write(INIT_ADDR, INIT_KEY);    // записали ключ
  }else{
    for (int i = 0, j = 0; i < PARSE_AMOUNT; i++, j += 2) {
      EEPROM.get(j, dataArray[i]);
    }
  }
   Serial.println("A-RGB-Controller v.2.2");
  FastLED.addLeds<WS2812, ST1_LEDPIN, GRB>(st1, ST1_NUMLED).setCorrection( TypicalLEDStrip);;
  FastLED.addLeds<WS2812, ST2_LEDPIN, GRB>(st2, ST2_NUMLED).setCorrection( TypicalLEDStrip);;
  xTaskCreate(st1Task, "strip 1 task", 128, NULL, 1, NULL);
  xTaskCreate(st2Task, "strip 2 task", 128, NULL, 1, NULL);
  xTaskCreate(parsingTask, "parsing task", 128, NULL, 1, NULL);
  Serial.println("Willkommen Herr Schmitz");
  vTaskStartScheduler();
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}
byte * Wheel2(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}
void loop(){}
void brightnessVoid(){
    if(dataArray[10] == 0){
       FastLED.setBrightness(map(analogRead(PHOTO), 0, 1023, 0, 100));
    }else if(dataArray[10] == 1){
      FastLED.setBrightness(dataArray[11]);
    }// Уровень яркости
   }  

void ST1RS(){
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    if (dataArray[2] != 0){ 
      break;
    }
    for(i=0; i< ST1_NUMLED; i++) {
       if (dataArray[2] != 0){ 
      break;
    }
      c=Wheel(((i * 256 / ST1_NUMLED) + j) & 255);
      st1[i].setRGB(*c, *(c+1), *(c+2));
      
    }
    
    FastLED.show();
    
}
}

void ST2RS(){
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    if (dataArray[3] != 0){ 
      break;
    }
    for(i=0; i< ST2_NUMLED; i++) {

      if (dataArray[3] != 0){ 
      break;
    }
      c=Wheel2(((i * 256 / ST2_NUMLED) + j) & 255);
      st2[i].setRGB(*c, *(c+1), *(c+2));
    }
    FastLED.show();
    
}
}


void ST1RnS(int SpeedDelay){
  for(uint16_t i=0; i<ST1_NUMLED; i++) {
    if (dataArray[2] != 2){ 
      break;
    }
      st1[i].setRGB(dataArray[4], dataArray[5], dataArray[6]);
      FastLED.show();
      vTaskDelay(SpeedDelay);
  }
  for(uint16_t i=0; i<ST1_NUMLED; i++) {
    if (dataArray[2] != 2){ 
      break;
    }
      st1[i].setRGB(0, 0, 0);
      FastLED.show();
      vTaskDelay(SpeedDelay);
  }
}
void ST2RnS(int SpeedDelay){
  for(uint16_t i=0; i<ST2_NUMLED; i++) {
    if (dataArray[3] != 2){ 
      break;
    }
      st2[i].setRGB(dataArray[7], dataArray[8], dataArray[9]);
      FastLED.show();
      vTaskDelay(SpeedDelay);
  }
  for(uint16_t i=0; i<ST2_NUMLED; i++) {
    if (dataArray[3] != 2){ 
      break;
    }
      st2[i].setRGB(0, 0, 0);
      FastLED.show();
      vTaskDelay(SpeedDelay);
  }
}
void ST1STC(){
  for(int i = 0; i < ST1_NUMLED; i++){
    if (dataArray[2] != 1){ 
      break;
    }
    st1[i].setRGB(dataArray[4], dataArray[5], dataArray[6]);
    FastLED.show();
  }
}
void ST2STC(){
  for(int i = 0; i < ST2_NUMLED; i++){
    if (dataArray[3] != 1){ 
      break;
    }
    st2[i].setRGB(dataArray[7], dataArray[8], dataArray[9]);
    FastLED.show();
  }
}
void ST1CLD(){
  for(int i = 0; i < ST1_NUMLED; i++){
      st1[i] = CHSV(0, 255, 255);
     if(dataArray[2] != 3 || i == ST1_NUMLED){
 

      break;
     }
     
    }
     vTaskDelay(20);
  for(int i = 0; i < ST1_NUMLED; i++){
      st1[i] = CHSV(42, 255, 255);
      if(dataArray[2] != 3 || i == ST1_NUMLED){
  

      break;
     }
     
  }
 vTaskDelay(20);
  for(int i = 0; i < ST1_NUMLED; i++){
      st1[i] = CHSV(85, 255, 255);
      if(dataArray[2] != 3 || i == ST1_NUMLED){
   
  
      break;
     }
  }
vTaskDelay(20);
  for(int i = 0; i < ST1_NUMLED; i++){
      st1[i] = CHSV(128, 255, 255);
    if(dataArray[2] != 3 || i == ST1_NUMLED){
      
      break;
     }
  }
  vTaskDelay(20);
  for(int i = 0; i < ST1_NUMLED; i++){
      st1[i] = CHSV(171, 255, 255);
    if(dataArray[2] != 3 || i == ST1_NUMLED){

    
      break;
     }
  } 
  vTaskDelay(20);
  for(int i = 0; i < ST1_NUMLED; i++){
      st1[i] = CHSV(213, 255, 255);
     if(dataArray[2] != 3 || i == ST1_NUMLED){
      
      break;
     }
    }
vTaskDelay(20);
}


void ST2CLD(){
  for(int i = 0; i < ST2_NUMLED; i++){
      st2[i] = CHSV(0, 255, 255);
     if(dataArray[3] != 3 || i == ST2_NUMLED){
 

      break;
     }
     
    }
     vTaskDelay(20);
  for(int i = 0; i < ST2_NUMLED; i++){
      st2[i] = CHSV(42, 255, 255);
      if(dataArray[3] != 3 || i == ST2_NUMLED){
  

      break;
     }
     
  }
 vTaskDelay(20);
  for(int i = 0; i < ST2_NUMLED; i++){
      st2[i] = CHSV(85, 255, 255);
      if(dataArray[3] != 3 || i == ST2_NUMLED){
   
  
      break;
     }
  }
vTaskDelay(20);
  for(int i = 0; i < ST2_NUMLED; i++){
      st2[i] = CHSV(128, 255, 255);
    if(dataArray[3] != 3 || i == ST2_NUMLED){
      
      break;
     }
  }
  vTaskDelay(20);
  for(int i = 0; i < ST2_NUMLED; i++){
      st2[i] = CHSV(171, 255, 255);
    if(dataArray[3] != 3 || i == ST2_NUMLED){

    
      break;
     }
  } 
  vTaskDelay(20);
 
  for(int i = 0; i < ST2_NUMLED; i++){
      st2[i] = CHSV(213, 255, 255);
     if(dataArray[3] != 3 || i == ST2_NUMLED){
      
      break;
     }
    }
vTaskDelay(20);}
void st1Task(void *pvParameters){
  while(1){
  
    if(dataArray[0] == 1 && dataArray[2] == 0) ST1RS();
    if(dataArray[0] == 1 && dataArray[2] == 1) ST1STC();
    if(dataArray[0] == 1 && dataArray[2] == 2) ST1RnS(5);  
    if(dataArray[0] == 1 && dataArray[2] == 3) ST1CLD();
   }
}
void st2Task(void *pvParameters){
  while(1){
    if(dataArray[1] == 1 && dataArray[3] == 0) ST2RS();
    if(dataArray[1] == 1 && dataArray[3] == 1) ST2STC();
    if(dataArray[1] == 1 && dataArray[3] == 2) ST2RnS(5);
    if(dataArray[1] == 1 && dataArray[3] == 3) ST2CLD();
  }
}
void parsingTask(void *pvParameters){
  while(1){
    parsingStream((int*)&dataArray);
    if(dataReady()){
    for(int i = 0, j = 0; i < PARSE_AMOUNT; i++, j += 2){
      EEPROM.put(j, dataArray[i]);
    }
    }
    FastLED.show();
    brightnessVoid();
  }
}
