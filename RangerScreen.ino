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

#define VOLTAGE_INPUT A7
#define ENCODER_PUSH 12
#define ENCODER_A 11
#define ENCODER_B 10
#define ENCODER_PUSH 12
#define BED_INPUT 9

#define NUMBER_OUTPUTS 4
bool outs[NUMBER_OUTPUTS];

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2l(U8G2_R2);
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2r(U8G2_R0, 16, 17);

//TinyGPSPlus gps;
Encoder encoder(ENCODER_A, ENCODER_B);
#define numAvgs 5
float voltAvg[numAvgs];
int leftMode = 0;
int rightMode = 0;
float volts;
double getVoltage()//read car voltage with a voltage divider
{
  double vin = (analogRead(VOLTAGE_INPUT)/1023.0*3.3);
  double R1 = 47400;
  double R2 = 7540;
  return (vin*(R1+R2))/R2;
}
void setOutput(byte num, bool out)
{
  outs[num] = out;
}
void turnOff()
{
  runLogoReversed();
  digitalWrite(LATCH_PIN, false);
  u8g2l.setDrawColor(0);
//  while(analogRead(IGPIN)<512)
//  {}
}
void setup() {
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, true); //latch myself on
  pinMode(13,OUTPUT);
  digitalWrite(13, true);
  pinMode(OUTPUT0,OUTPUT);
  pinMode(OUTPUT1,OUTPUT);
  pinMode(OUTPUT2,OUTPUT);
  pinMode(OUTPUT3,OUTPUT);
  pinMode(VOLTAGE_INPUT,INPUT);
  pinMode(ENCODER_PUSH,INPUT_PULLUP);

  for(int i = 0; i < NUMBER_OUTPUTS; i++)
  {
    outs[i] = false;
  }
  //ECUinit(500000);
//  Serial2.begin(9600);
  Serial.begin(9600);
  delay(500);
//  setTime(getTeensy3Time());
//  debugMode = analogRead(A1)<500;
//  Serial.println(debugMode ? "DEBUG MODE":"");
  u8g2l.begin();
  u8g2l.setContrast(1);
  u8g2l.clearBuffer();
  u8g2r.begin();
  u8g2r.setContrast(1);
  u8g2r.clearBuffer();
//  if(!debugMode)
//  {
    runLogo();
//  }
  digitalWrite(13, false);
  //turnOff();
}
long lastFrame = 0;
void loop() 
{
  outs[0] = millis()%1000>(1000/5);
  outs[1] = millis()%1000>(1000/5*2);
  outs[2] = millis()%1000>(1000/5*3);
  outs[3] = millis()%1000>(1000/5*4);
  if(millis()-lastFrame>16)
  {
    volts = getVoltage();
    lastFrame = millis();
    render();
  }
  delay(1);
}
