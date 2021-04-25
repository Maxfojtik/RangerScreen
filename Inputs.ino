void encoderDown()
{
  
}
void encoderUp()
{
  
}
void encoderPushed()
{
  if(!isInMenu)
  {
    isInMenu = true;
  }
  if(menuMode==0)
  {
    isInMenu = false;
  }
}
float avgVoltage()
{
  float avg = 0;
  for(byte i = 0; i < numAvgs; i++)
  {
    avg = avg + voltAvg[i];
  }
  return avg/numAvgs;
}
float getVoltage()//read car voltage with a voltage divider
{
  //Serial.println(analogRead(VOLTAGE_INPUT));
  float vin = (analogRead(VOLTAGE_INPUT)/1023.0*3.3);
  float R1 = 47400;
  float R2 = 7540;
  float out = (vin*(R1+R2))/R2;
  voltAvg[voltAvgIndex] = out;
  putToDisplay(out);
  float avg = avgVoltage();
  voltAvgIndex++;
  if(voltAvgIndex>=numAvgs)
  {
    voltAvgIndex = 0;
  }
  return avg;
}
void inputLogic()
{
  bedDown = analogRead(BED_INPUT)>100;
  ill = analogRead(ILL_INPUT)>100;
  switchA = digitalRead(SWITCH_A);
  switchB = digitalRead(SWITCH_B);
  volts = getVoltage();
  if(bedDown)
  {
    onReason = REASON_TAILGATEDOWN;
  }
  else if(switchA)
  {
    onReason = REASON_SWITCHA;
  }
  else if(switchB)
  {
    onReason = REASON_SWITCHB;
  }
  else if(tailgateToggle)
  {
    onReason = REASON_TAILGATESW;
  }
  else if(volts > 6)
  {
    onReason = REASON_IGNITION;//there is no other reason we are on so it must be that the ignition is on
  }
  if(!bedSwitch && analogRead(BED_SWITCH)>100)
  {
    tailgateToggle = false;//turn off the tailgate toggle if we hit the button again
  }
  if(volts > 6)
  {
    tailgateToggle = false;//turn off the tailgate toggle if the car is turned on.
  }
  if(oldPosition != encoder.read())
  {
    if(oldPosition > encoder.read())
    {
      encoderDown();
    }
    else if(oldPosition < encoder.read())
    {
      encoderUp();
    }
    oldPosition = encoder.read();
  }
  boolean encoderPushNow = digitalRead(ENCODER_PUSH);
  if(encoderPushNow && !encoderPush && !isInMenu)
  {
    encoderPushed();
  }
  bedSwitch = analogRead(BED_SWITCH)>100;
  encoderPush = encoderPushNow;
}
