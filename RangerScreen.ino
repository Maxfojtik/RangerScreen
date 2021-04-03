#include <U8g2lib.h>
#include <U8x8lib.h>
#include <TinyGPS++.h>
#include <EEPROM.h>

#define OUTPUT0 3
#define OUTPUT1 4
#define OUTPUT2 5
#define OUTPUT3 6



U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
TinyGPSPlus gps;
bool wasOn = false;
bool wasLightsOn = false;
#define numAvgs 25
float voltAvg[numAvgs];
int mode = 1;
bool midButtonPressed = false;
long midButtonPressedTime = 0;
bool leftButtonPressed = false;
long leftButtonPressedTime = 0;
bool debugMode = false;
long lastFrame = 0;
long lastTime = 0;
long frame = 0;
bool validTime = false;
int offset = -4;
bool validSpeed = false;
bool doTimeNow = false;
int timeOff = 0;

bool adjustingTime = false;
float adjustingTimeCountdown = 0;

void turnOff()
{
  runLogoReversed();
  digitalWrite(21, false);
  u8g2.setDrawColor(0);
  while(analogRead(IGPIN)<512)
  {}
}
void setup() {
  pinMode(21, OUTPUT);//latch myself on
  digitalWrite(21, true);
  pinMode(13,OUTPUT);
  digitalWrite(13, true);
  pinMode(OUTPUT0,OUTPUT);
  pinMode(OUTPUT1,OUTPUT);
  pinMode(OUTPUT2,OUTPUT);
  pinMode(OUTPUT3,OUTPUT);
  //ECUinit(500000);
//  pinMode(IGPIN, INPUT);
//  pinMode(ILPIN, INPUT);
//  pinMode(LEFTBUTTON, INPUT_PULLUP);
//  pinMode(MIDBUTTON, INPUT_PULLUP);
//  Serial2.begin(9600);
  Serial.begin(9600);
  delay(500);
//  setTime(getTeensy3Time());
//  debugMode = analogRead(A1)<500;
//  Serial.println(debugMode ? "DEBUG MODE":"");
  u8g2.begin();
  u8g2.setContrast(255);
  u8g2.clearBuffer();
//  if(!debugMode)
//  {
    runLogo();
//  }
  digitalWrite(13, false);
  turnOff();
}
long lastBlink = 0;
void loop() 
{
  if(millis()-lastBlink>1000)
  {
    digitalWrite(OUTPUT0, true);
    delay(100);
    digitalWrite(OUTPUT0, false);
  }
  delay(1);
}
