#include <U8g2lib.h>
#include <U8x8lib.h>
//#include <TinyGPS++.h>
#include <EEPROM.h>
#include <Encoder.h>

#define OUTPUT0 3
#define OUTPUT1 4
#define OUTPUT2 5
#define OUTPUT3 6
#define LATCH_PIN 21

#define VOLTAGE_INPUT A6
#define ILL_INPUT A1
#define ENCODER_PUSH 12
#define ENCODER_A 11
#define ENCODER_B 10
#define ENCODER_PUSH 12
#define BED_INPUT A8
#define BED_SWITCH A9
#define SWITCH_A 2
#define SWITCH_B 1

#define NUMBER_OUTPUTS 4
byte outs[NUMBER_OUTPUTS];

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2l(U8G2_R2);
U8G2_SSD1306_128X32_UNIVISION_F_2ND_HW_I2C u8g2r(U8G2_R0);

#define REASON_IGNITION 0
#define REASON_TAILGATEDOWN 1
#define REASON_TAILGATESW 2
#define REASON_SWITCHA 3
#define REASON_SWITCHB 4

#define loopTime 20

//TinyGPSPlus gps;
Encoder encoder(ENCODER_A, ENCODER_B);
#define numAvgs 5
float voltAvg[numAvgs];
byte voltAvgIndex = 0;
#define numGraphPix 128
byte voltGraph[numGraphPix];//values: 0-32
byte voltDisplayIndex = 0;
int leftMode = -1;
int rightMode = -1;
float volts;
int onReason = 0;//0 = ignition, 1 = tailgate, 2 = tailgate sw, 3 = switch_a, 4 = switch_b
long startAnimation = 0;
long oldPosition = 0;
boolean bedDown = false;
boolean bedSwitch = false;
boolean ill = false;
boolean switchA = false;
boolean switchB = false;
boolean tailgateToggle = false;
boolean encoderPush = false;
bool isInMenu = false;
int menuMode = 0;
bool ignoreTailgate = false;
bool editingModeLeft = false;
bool editingModeRight = false;

void setup() {
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, true); //latch myself on
  pinMode(13,OUTPUT);
  digitalWrite(13, true);
  pinMode(OUTPUT0,OUTPUT);
  pinMode(OUTPUT1,OUTPUT);
  pinMode(OUTPUT2,OUTPUT);
  pinMode(OUTPUT3,OUTPUT);
  digitalWrite(OUTPUT0, 0);
  digitalWrite(OUTPUT1, 0);
  digitalWrite(OUTPUT2, 0);
  digitalWrite(OUTPUT3, 0);
  pinMode(VOLTAGE_INPUT,INPUT);
  pinMode(ENCODER_PUSH,INPUT_PULLUP);
  pinMode(ILL_INPUT,INPUT);
  pinMode(BED_INPUT,INPUT);
  pinMode(SWITCH_A,INPUT_PULLDOWN);
  pinMode(SWITCH_B,INPUT_PULLDOWN);
  delay(100);
  loadVariables();
  if(digitalRead(SWITCH_A))
  {
    onReason = 3;
  }
  else if(digitalRead(SWITCH_B))
  {
    onReason = 4;
  }
  else if(analogRead(BED_INPUT)>100)
  {
    onReason = 1;
  }
  else if(analogRead(BED_SWITCH)>100)
  {
    onReason = 2;
    tailgateToggle = true;
    bedSwitch = true;
  }
  for(int i = 0; i < NUMBER_OUTPUTS; i++)
  {
    outs[i] = 0;
  }
  for(int i = 0; i < numGraphPix; i++)
  {
    voltGraph[i] = 64;
  }
  //ECUinit(500000);
//  Serial2.begin(9600);
  Serial.begin(9600);
  //delay(500);
//  setTime(getTeensy3Time());
//  debugMode = analogRead(A1)<500;
//  Serial.println(debugMode ? "DEBUG MODE":"");
  u8g2l.begin();
  u8g2l.setContrast(255);
  u8g2l.clearBuffer();
  u8g2r.begin();
  u8g2r.setContrast(255);
  u8g2r.clearBuffer();
//  if(!debugMode)
//  {
    //runLogo();
//  }
  digitalWrite(13, false);
  startAnimation = millis();
  //turnOff();
}
void loadVariables()
{
  loadModes();
  ignoreTailgate = EEPROM.read(2)==1;
}
void loadModes()
{
  leftMode = EEPROM.read(0);
  rightMode = EEPROM.read(1);
}
long lastFrame = 0;
void loop() 
{
  if(millis()-lastFrame>loopTime)
  {
    lastFrame = millis();
    digitalWrite(13, true);
    if(volts<6 && !bedDown && leftMode!=-1 && !ill)
    {
      if(leftMode!=-2)
      {
        startAnimation = millis();
      }
      leftMode = -2;
      rightMode = -2;
    }
    else if(leftMode==-2)
    {
      loadModes();
    }
    displayLogic();
    render();
    inputLogic();
    outputsLogic();
    digitalWrite(13, false);
    //Serial.println(millis()-lastFrame);
  }
  //delay(1);
}
