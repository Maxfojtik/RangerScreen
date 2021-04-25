void putToDisplay(float avg)
{
  float min = 8;
  float max = 15;
  byte displayHeight = 31;
  if(avg<min)
  {
    avg = min;
  }
  if(avg>max)
  {
    avg = max;
  }
  byte toArray = (byte)((avg-min)/(max-min)*displayHeight);
  for(int i = numGraphPix-1; i > 0; i--)
  {
    voltGraph[i] = voltGraph[i-1];
//    Serial.print(voltGraph[i]);
//    Serial.print("\t");
  }
  //Serial.println();
  voltGraph[0] = toArray;
}
void drawVoltageGraph(U8G2 display)
{
  display.setDrawColor(1);
  for(int i = 0; i < numGraphPix; i++)
  {
    if(voltGraph[i]!=64)
    {
      display.drawPixel(numGraphPix-i, 31-voltGraph[i]);
    }
  }
}
void drawVoltage(U8G2 display)
{
  display.setDrawColor(1);
  String str =  String(volts, 2);
  if(volts<10)
  {
    display.setCursor(27, 32);
    display.setFont(u8g2_font_logisoso30_tf);
    display.print(str.c_str());
    display.setCursor(100, 32);
    display.setFont(u8g2_font_logisoso16_tf);
    display.print("v");
  }
  else
  {
    display.setCursor(10, 32);
    display.setFont(u8g2_font_logisoso30_tf);
    display.print(str.c_str());
    display.setCursor(105, 32);
    display.setFont(u8g2_font_logisoso16_tf);
    display.print("v");
  }
}
void determineAndRenderMode(int mode, U8G2 display, bool left)
{
  display.clearBuffer();
  if(mode == -1 || mode == -2)//startup
  {
    int FPS = 15;
    int framesLeft = 49;
    int framesRight = 41;
    int totalFrames = 49+30+30;
    int frameNum = (millis()-startAnimation)/(1000/FPS);
    if(mode == -2)
    {
      frameNum = totalFrames-frameNum;
    }
    if(frameNum<=0 && mode == -2)
    {
      frameNum = 0;
      digitalWrite(LATCH_PIN, false);
    }
    if(left)//run normal animation
    {
      if(frameNum>3)//pause for 30 frames at 3rd frame
      {
        frameNum = frameNum-30;
        if(frameNum<3)
        {
          frameNum = 3;
        }
      }
      if(frameNum>(framesLeft+30))//pause for 30 frames at main logo
      {
        loadModes();
      }
      if(frameNum>framesLeft)//if we are running overtime then dont.
      {
        frameNum = framesLeft;
      }
      runLogoASyncLeft(display, frameNum);
    }
    else//display wake reason or half animation if none
    {
      int waitFrames = 29;
      int secondWaitFrame = 16;
      int secondWaitFrameTime = 7;
      if(frameNum<waitFrames)
      {
        frameNum = -1;
      }
      else
      {
        frameNum = frameNum - waitFrames;
      }
      if(frameNum>secondWaitFrame)
      {
        if(frameNum<secondWaitFrame+secondWaitFrameTime)
        {
          frameNum = secondWaitFrame;
        }
        else
        {
          frameNum = frameNum - secondWaitFrameTime;
        }
      }
      if(frameNum>framesRight)//if we are running overtime then dont.
      {
        frameNum = framesRight;
      }
      if(frameNum>=0)
      {
        runLogoASyncRight(display, frameNum);
      }
    }
  }
  else if(mode == 0)//turn off
  {
  }
  else if(mode == 1)
  {
    drawVoltage(display);
  }
  else if(mode == 2)
  {
    drawVoltageGraph(display);
  }
  if(mode!=-1 || mode == -2)
  {
    if(left)
    {
      if(outs[0]>0)
      {
        display.drawBox(0,0,64,2);
      }
      if(outs[1]>0)
      {
        display.drawBox(64,0,64,2);
      }
    }
    else
    {
      if(outs[2]>0)
      {
        display.drawBox(0,0,64,2);
      }
      if(outs[3]>0)
      {
        display.drawBox(64,0,64,2);
      }
    }
  }
}
void writeCentered(U8G2 display, char* str, byte y)
{
  int strWidth = display.getStrWidth(str);
  int x = display.getDisplayWidth()/2-strWidth/2;
  display.drawStr(x, y, str);
}
void writeCentered1(U8G2 display, char* str)
{
  int strWidth = display.getStrWidth(str);
  int x = display.getDisplayWidth()/2-strWidth/2;
  int y = display.getDisplayHeight()/2-display.getMaxCharHeight()/2;
  display.drawStr(x, y, str);
}
void writeCentered2(U8G2 display, char* str1, char* str2)
{
  int strWidth = display.getStrWidth(str1);
  int x = display.getDisplayWidth()/2-strWidth/2;
  int y = display.getDisplayHeight()/4-display.getMaxCharHeight()/2;
  display.drawStr(x, y, str1);
  
  strWidth = display.getStrWidth(str2);
  x = display.getDisplayWidth()/2-strWidth/2;
  y = display.getDisplayHeight()/4*3-display.getMaxCharHeight()/2;
  display.drawStr(x, y, str2);
}
void determineAndRenderMenu(U8G2 display, bool left)
{
  if(left)
  {
    if(menuMode==0)
    {
      u8g2l.clearBuffer();
      u8g2l.setFont(u8g2_font_crox4h_tf);
      writeCentered1(display, "Exit");
    }
  }
}
void displayLogic()
{
  bool illNow = analogRead(ILL_INPUT)>100;
  if(illNow && !ill)
  {
    u8g2r.setContrast(0);
    u8g2l.setContrast(0);
  }
  if(!illNow && ill)
  {
    u8g2r.setContrast(255);
    u8g2l.setContrast(255);
  }
}
boolean writeLeft = false;
void render()
{
  writeLeft = !writeLeft;
  if(writeLeft)
  {
    if(isInMenu)
    {
      determineAndRenderMenu(u8g2l, true);
    }
    else
    {
      determineAndRenderMode(leftMode, u8g2l, true);
      u8g2l.sendBuffer();
    }
  }
  else
  {
    if(ill && volts<6)
    {
      u8g2r.clearBuffer();
      u8g2r.setCursor(0, 23);
      u8g2r.setFont(u8g2_font_crox4h_tf);
      u8g2r.print("Headlights!");
      u8g2r.sendBuffer();
    }
    else if(bedDown)
    {
      u8g2r.clearBuffer();
      u8g2r.setCursor(0, 23);
      u8g2r.setFont(u8g2_font_crox4h_tf);
      u8g2r.print("Tailgate Down!");
      u8g2r.sendBuffer();
    }
    else if(switchA)
    {
      u8g2r.clearBuffer();
      u8g2r.setCursor(0, 23);
      u8g2r.setFont(u8g2_font_crox4h_tf);
      u8g2r.print("Bed Lights On");
      u8g2r.sendBuffer();
    }
    else
    {
      determineAndRenderMode(rightMode, u8g2r, false);
      u8g2r.sendBuffer();
    }
  }
}
