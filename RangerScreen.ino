#include <U8g2lib.h>
#include <U8x8lib.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <NMEAGPS.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

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
#define SWITCH_B 9

#define NUMBER_OUTPUTS 4
byte outs[NUMBER_OUTPUTS];

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2l(U8G2_R2);
U8G2_SSD1306_128X32_UNIVISION_F_2ND_HW_I2C u8g2r(U8G2_R0);

NMEAGPS  gps;
gps_fix  fix;
SFE_UBLOX_GNSS myGNSS;

#define REASON_IGNITION 0
#define REASON_TAILGATEDOWN 1
#define REASON_TAILGATESW 2
#define REASON_SWITCHA 3
#define REASON_SWITCHB 4

#define loopTime 20

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
float speedMPH = 0;
float heading = -1;

void setup() {
  long start = millis();
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, true); //latch myself on
  pinMode(13,OUTPUT);
  digitalWrite(13, true);
  pinMode(OUTPUT0,OUTPUT);
  pinMode(OUTPUT1,OUTPUT);
  pinMode(OUTPUT2,OUTPUT);
  pinMode(OUTPUT3,OUTPUT);
  //pinMode(BTATPIN, OUTPUT);
  //digitalWrite(BTATPIN, SETUP);
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
  delay(10);
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
  Serial.begin(115200);
  
  //delay(500);
//  setTime(getTeensy3Time());
//  debugMode = analogRead(A1)<500;
//  Serial.println(debugMode ? "DEBUG MODE":"");
  //u8g2l.setBusClock(1000000);
  u8g2l.begin();
  u8g2l.setContrast(255);
  u8g2l.clearBuffer();
  //u8g2r.setBusClock(1000000);
  u8g2r.begin();
  u8g2r.setContrast(255);
  u8g2r.clearBuffer();
//  if(!debugMode)
//  {
    //runLogo();
//  }


  //Wire.begin();
  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
  }
  else
  {
    Serial2.begin(250000);
    myGNSS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_UART1); //Several of these are on by default on ublox board so let's disable them
    myGNSS.enableNMEAMessage(UBX_NMEA_GSA, COM_PORT_UART1);
    myGNSS.enableNMEAMessage(UBX_NMEA_GSV, COM_PORT_UART1);
    myGNSS.disableNMEAMessage(UBX_NMEA_RMC, COM_PORT_UART1);
    myGNSS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_UART1); //Only leaving GGA & VTG enabled at current navigation rate
    myGNSS.enableNMEAMessage(UBX_NMEA_VTG, COM_PORT_UART1);
    myGNSS.configureMessage(UBX_CLASS_NAV, UBX_NAV_PVT, COM_PORT_UART1, 0); //Message Class, ID, and port we want to configure, sendRate of 0 (disable).
    myGNSS.setUART1Output(COM_TYPE_NMEA);
    myGNSS.setSerialRate(250000);
    myGNSS.setNavigationFrequency(10);
    Serial.println(F("GPS configured"));
  }
  digitalWrite(13, false);
  startAnimation = millis();
  Serial.println(millis()-start);
  //turnOff();
}
void loadVariables()
{
  //loadModes();
  ignoreTailgate = false;// EEPROM.read(2)==1;
}
void loadModes()
{
  leftMode = 3;//EEPROM.read(0);
  rightMode = 2;//EEPROM.read(1);
}
long lastFrame = 0;
long lastGPS = 0;
int state = 0;
String str;
bool next = true;
long lastNext = 1000;
int time = 500;
int timer = -1;
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
    //Serial.print("time:\t");
    //Serial.println(millis()-lastFrame);
  }
  GPSLogic();
  //delay(1);
}
