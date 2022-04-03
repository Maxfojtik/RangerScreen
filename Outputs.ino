//void setOutput(byte num, bool out)
//{
//  outs[num] = out;
//}
byte bedLightsFunction(byte time)
{
  int out = pow(1.022,time)-1.022;
  if(out > 255)
  {
    out = 255;
  }
  return out;
}
float bedTime = 0;
long bedDownTime = 0;
void outputsLogic()
{
  if(bedDown && !ignoreTailgate && bedDownTime==0)
  {
    bedDownTime = millis();
  }
  boolean bedLightsOn = (bedDown && !ignoreTailgate && millis()-bedDownTime<60*60*1000) || switchA;//60 minutes
  //digitalWrite(13, bedLightsOn);
  //volts = 12-(millis()%20000/20000.0*12);
  if(bedLightsOn && bedTime<255)
  {
    bedTime = bedTime + (1.0/loopTime)*255/2;
    if(bedTime > 255)//accounts for fractions, 254.9999 < 255 then 254.9999 + .05 = 255.0499 which gets clamped = 255.
    {
      bedTime = 255;
    }
  }
  if(!bedLightsOn && bedTime>0)
  {
    bedTime = bedTime - (1.0/loopTime)*255/2;
    if(bedTime < 0)
    {
      bedTime = 0;
    }
  }
//  double minVolts = 10.5;
//  double maxVolts = 11;
//  double scaledVolts = mapDouble(volts, minVolts, maxVolts, 0, 255);
  if(bedTime>254)
  {
    outs[0] = 255;
  }
  else
  {
    outs[0] = bedLightsFunction((byte)bedTime);
  }
  if(bedTime<1)
  {
    outs[0] = 0;
  }
  else
  {
    outs[0] = bedLightsFunction((byte)bedTime);
  }
//  Serial.print(volts);
//  Serial.print("\t");
//  Serial.println(outs[0]);
  analogWrite(OUTPUT0, outs[0]);
  analogWrite(OUTPUT1, outs[1]);
  analogWrite(OUTPUT2, outs[2]);
  analogWrite(OUTPUT3, outs[3]);
}
long mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
