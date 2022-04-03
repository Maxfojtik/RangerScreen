int mostMode = 2;
void encoderDown()
{
  if(editingModeLeft)
  {
    leftMode--;
    if(leftMode<0)
    {
      leftMode = 0;
    }
    EEPROM.write(0, leftMode); 
  }
  else if(editingModeRight)
  {
    rightMode--;
    if(rightMode<0)
    {
      rightMode = 0;
    }
    EEPROM.write(1, rightMode);
  }
  else
  {
    if(menuMode<0)
    {
      menuMode = 0;
    }
  }
}
void encoderUp()
{
  if(editingModeLeft)
  {
    leftMode++;
    if(leftMode>mostMode)
    {
      leftMode = mostMode;
    }
    EEPROM.write(0, leftMode); 
  }
  else if(editingModeRight)
  {
    rightMode++;
    if(rightMode>mostMode)
    {
      rightMode = mostMode;
    }
    EEPROM.write(1, rightMode); 
  }
  else
  {
    if(menuMode>3)
    {
      menuMode = 3;
    }
  }
}
long lastEncoderPushed = 0;
void encoderPushed()
{
  if(millis()-lastEncoderPushed>200)
  {
    lastEncoderPushed = millis();
    if(!isInMenu)
    {
      isInMenu = true;
    }
    else
    {
      if(menuMode==0)
      {
        isInMenu = false;
      }
      else if(menuMode == 1)
      {
        editingModeLeft = !editingModeLeft;
      }
      else if(menuMode == 2)
      {
        editingModeRight = !editingModeRight;
      }
      else if(menuMode == 3)
      {
        //ignoreTailgate = !ignoreTailgate;
        EEPROM.write(3, ignoreTailgate ? 1 : 0); 
      }
    }
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
  float vin = (analogRead(VOLT_INPUT)/1023.0*3.3);
  float R1 = 47089;
  float R2 = 8400;
  float out = (vin*(R1+R2))/R2 + 0.6;//volt drop from diodes
  voltAvg[voltAvgIndex] = out;
  float avg = avgVoltage();
  voltAvgIndex++;
  if(voltAvgIndex>=numAvgs)
  {
    voltAvgIndex = 0;
    putToDisplay(avg);
  }
  return avg;
}
void inputLogic()
{
  int half = 1024/8;
  bedDown = digitalRead(BED_INPUT);//analogRead(BED_INPUT)>half;
  ill = digitalRead(ILL_INPUT);
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
  else
  {
    onReason = REASON_NONE;
  }
  if(!bedSwitch && digitalRead(BED_SWITCH))
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
  if(encoderPushNow && !encoderPush)
  {
    encoderPushed();
  }
  bedSwitch = digitalRead(BED_SWITCH);
  encoderPush = encoderPushNow;
}
