#define ST1_NUMLED 9
#define ST2_NUMLED 45

#define ST1_LEDPIN 5
#define ST2_LEDPIN 6
#define PARSE_AMOUNT 12
#define PHOTO A1
#include <FastLED.h>
#include <Arduino_FreeRTOS.h>
#include <GParsingStream.h>
CRGB st1[ST1_NUMLED];
CRGB st2[ST2_NUMLED];
int intData[PARSE_AMOUNT] = {
  0, 0,
  0, 0,
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
  FastLED.addLeds<WS2812, ST1_LEDPIN, GRB>(st1, ST1_NUMLED).setCorrection( TypicalLEDStrip);;
  FastLED.addLeds<WS2812, ST2_LEDPIN, GRB>(st2, ST2_NUMLED).setCorrection( TypicalLEDStrip);;
  xTaskCreate(st1Task, "strip 1 task", 128, NULL, 1, NULL);
  xTaskCreate(st2Task, "strip 2 task", 128, NULL, 1, NULL);
  xTaskCreate(parsingTask, "parsing task", 128, NULL, 1, NULL);
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
    if(intData[10] == 0){
       FastLED.setBrightness(map(analogRead(PHOTO), 0, 1023, 0, 100));
    }else if(intData[10] == 1){
      FastLED.setBrightness(intData[11]);
    }// Уровень яркости
   }  

void ST1RS(int SpeedDelay){
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< ST1_NUMLED; i++) {
      c=Wheel(((i * 256 / ST1_NUMLED) + j) & 255);
      st1[i].setRGB(*c, *(c+1), *(c+2));
    }
    FastLED.show();
    vTaskDelay(SpeedDelay);
}
}

void ST2RS(int SpeedDelay){
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< ST2_NUMLED; i++) {
      c=Wheel2(((i * 256 / ST2_NUMLED) + j) & 255);
      st2[i].setRGB(*c, *(c+1), *(c+2));
    }
    FastLED.show();
    vTaskDelay(SpeedDelay);
}
}

void ST2RnS(int SpeedDelay){
  for(uint16_t i=0; i<ST2_NUMLED; i++) {
      st2[i].setRGB(intData[7], intData[8], intData[9]);
      FastLED.show();
      vTaskDelay(SpeedDelay);
  }
  for(uint16_t i=0; i<ST2_NUMLED; i++) {
      st2[i].setRGB(0, 0, 0);
      FastLED.show();
      vTaskDelay(SpeedDelay);
  }
}
void ST1STC(){
  for(int i = 0; i < ST1_NUMLED; i++){
    st1[i].setRGB(intData[4], intData[5], intData[6]);
    FastLED.show();
  }
}
void ST2STC(){
  for(int i = 0; i < ST2_NUMLED; i++){
    st2[i].setRGB(intData[7], intData[8], intData[9]);
    FastLED.show();
  }
}
void st1Task(void *pvParameters){
  while(1){
    if(intData[0] == 1 && intData[2] == 0) ST1RS(0);
    if(intData[0] == 1 && intData[2] == 1) ST1STC();
    
  }
}
void st2Task(void *pvParameters){
  while(1){
    if(intData[1] == 1 && intData[3] == 0) ST2RS(0);
    if(intData[1] == 1 && intData[3] == 1) ST2STC();
    if(intData[1] == 1 && intData[3] == 2) ST2RnS(5);
    
  }
}
void parsingTask(void *pvParameters){
  while(1){
    parsingStream((int*)&intData);
    brightnessVoid();
    FastLED.show();
  }
}
