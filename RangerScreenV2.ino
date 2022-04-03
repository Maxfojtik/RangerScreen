#include <U8g2lib.h>
#include <U8x8lib.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <NMEAGPS.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

#define OUTPUT0 24
#define OUTPUT1 25
#define OUTPUT2 26
#define OUTPUT3 27
#define LATCH_PIN 23

#define VOLT_INPUT A8
#define IG_INPUT A14
#define ILL_INPUT 39
#define ENCODER_PUSH 28
#define ENCODER_A 29
#define ENCODER_B 30
#define BED_INPUT 36
#define BED_SWITCH 37
#define SWITCH_A 40
#define SWITCH_B 41

#define NUMBER_OUTPUTS 4
byte outs[NUMBER_OUTPUTS];

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2l(U8G2_R2);
U8G2_SSD1306_128X32_UNIVISION_F_2ND_HW_I2C u8g2r(U8G2_R0);

NMEAGPS  gps;
gps_fix  fix;
SFE_UBLOX_GNSS myGNSS;

#define REASON_NONE -1
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
double volts;
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
float speedMPH = -1;
float heading = -1;
bool inited = false;
bool GPSError = false;
long timeInvalid = 0;
long lastGPSComm = 0;
byte GPSSats = 0;
void setup() {
  //long start = millis();
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
  pinMode(IG_INPUT,INPUT);
  pinMode(ENCODER_PUSH,INPUT_PULLUP);
  pinMode(ILL_INPUT,INPUT);
  pinMode(BED_INPUT,INPUT);
  pinMode(SWITCH_A,INPUT);
  pinMode(SWITCH_B,INPUT);
  pinMode(BED_SWITCH,INPUT_PULLDOWN);
  pinMode(VOLT_INPUT,INPUT);
  delay(10);
  loadVariables();
  Serial.begin(115200);
  for(int i = 0; i < NUMBER_OUTPUTS; i++)
  {
    outs[i] = 0;
  }
  for(int i = 0; i < numGraphPix; i++)
  {
    voltGraph[i] = 64;
  }
  if(digitalRead(SWITCH_A))
  {
    onReason = REASON_SWITCHA;
  }
  else if(digitalRead(SWITCH_B))
  {
    onReason = REASON_SWITCHB;
  }
  else if(digitalRead(BED_INPUT))
  {
    onReason = REASON_TAILGATEDOWN;
    return;//dont init if the gate went down
  }
  else if(digitalRead(BED_SWITCH))
  {
    onReason = REASON_TAILGATESW;
    tailgateToggle = true;
    bedSwitch = true;
  }
  init();
  //Serial.println(millis()-start);
}
void init()
{
  digitalWrite(13, true);
  u8g2l.begin();
  u8g2l.setContrast(255);
  u8g2l.clearBuffer();
  u8g2r.begin();
  u8g2r.setContrast(255);
  u8g2r.clearBuffer();
  if (!myGNSS.begin()) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring."));
    GPSError = true;
  }
  else
  {
    Serial5.begin(250000);
    myGNSS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_UART1);
    myGNSS.disableNMEAMessage(UBX_NMEA_RMC, COM_PORT_UART1);
    myGNSS.enableNMEAMessage(UBX_NMEA_GSA, COM_PORT_UART1);
    myGNSS.enableNMEAMessage(UBX_NMEA_GSV, COM_PORT_UART1);
    myGNSS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_UART1);
    myGNSS.enableNMEAMessage(UBX_NMEA_VTG, COM_PORT_UART1);
    myGNSS.configureMessage(UBX_CLASS_NAV, UBX_NAV_PVT, COM_PORT_UART1, 0); //Message Class, ID, and port we want to configure, sendRate of 0 (disable).
    myGNSS.setUART1Output(COM_TYPE_NMEA);
    myGNSS.setSerialRate(250000);
    myGNSS.setNavigationFrequency(10);
    Serial.println(F("GPS configured"));
  }
  startAnimation = millis();
  inited = true;
  digitalWrite(13, false);
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
    //digitalWrite(13, true);
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
    if(inited)
    {
      displayLogic();
      render();
    }
    else if(outs[0]==255 && !inited && onReason==REASON_TAILGATEDOWN)//init when the bed lights are completly on
    {
      init();
    }
    inputLogic();
    outputsLogic();
    //digitalWrite(13, false);
    //Serial.print("time:\t");
    //Serial.println(millis()-lastFrame);
    //digitalWrite(13, false);
  }
  if(inited)//only run GPS when inited, not useful anyway
  {
    GPSLogic();
  }
  //delay(1);
}
