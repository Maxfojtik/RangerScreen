void writeCentered(U8G2 display, char* str, byte y)
{
  int strWidth = display.getStrWidth(str);
  int x = display.getDisplayWidth()/2-strWidth/2;
  display.drawStr(x, y, str);
}
void writeCentered1(U8G2 display, char* str, int charHeight)
{
  int strWidth = display.getStrWidth(str);
  int x = display.getDisplayWidth()/2-strWidth/2;
  int y = display.getDisplayHeight()/2-charHeight/2;
  display.drawStr(x, y, str);
}
void writeCentered2(U8G2 display, char* str1, char* str2, int charHeight)
{
  int strWidth = display.getStrWidth(str1);
  int x = display.getDisplayWidth()/2-strWidth/2;
  int y = display.getDisplayHeight()/4-charHeight/2;
  display.drawStr(x, y, str1);
  
  strWidth = display.getStrWidth(str2);
  x = display.getDisplayWidth()/2-strWidth/2;
  y = display.getDisplayHeight()/4*3-charHeight/2;
  display.drawStr(x, y, str2);
}
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
  //volts = millis()%1000/50.0;
  String str =  String(volts, 2);
  display.setCursor(5, 31);
  display.setFont(u8g2_font_profont12_tf);
  display.print(str.c_str());
  if(volts<10)
  {
    display.setCursor(30, 31);
    display.setFont(u8g2_font_profont10_tf);
    display.print("v");
  }
  else
  {
    display.setCursor(36, 31);
    display.setFont(u8g2_font_profont10_tf);
    display.print("v");
  }
}
void drawVoltage(U8G2 display)
{
  display.setDrawColor(1);
  String str =  String(volts, 2);
  if(volts<10)
  {
    display.setCursor(27, 31);
    display.setFont(u8g2_font_logisoso28_tf);
    display.print(str.c_str());
    display.setCursor(100, 32);
    display.setFont(u8g2_font_logisoso16_tf);
    display.print("v");
  }
  else
  {
    display.setCursor(10, 31);
    display.setFont(u8g2_font_logisoso28_tf);
    display.print(str.c_str());
    display.setCursor(105, 32);
    display.setFont(u8g2_font_logisoso16_tf);
    display.print("v");
  }
}
void rotatePoint(float* xy, float* origin, float* returnArr, double degrees)
{
  float xold = xy[0];
  float yold = xy[1];
  float xori = origin[0];
  float yori = origin[1];
  float sinval = sin(degrees*0.0174533);//PI/180
  float cosval = cos(degrees*0.0174533);//PI/180
  float xnew = (xold-xori)*cosval-(yold-yori)*sinval+xori;
  float ynew = (xold-xori)*sinval+(yold-yori)*cosval+yori;
  returnArr[0] = xnew;
  returnArr[1] = ynew;
}
void drawRotatedLine(U8G2 display, float* xy1, float* xy2, float* origin, double degrees)
{
  int x1 = 0;
  int y1 = 0;
  int x2 = 0;
  int y2 = 0;
  float ret[] = {0, 0};
  rotatePoint(xy1, origin, ret, degrees);
  x1 = int(ret[0]);
  y1 = int(ret[1]);
  rotatePoint(xy2, origin, ret, degrees);
  x2 = int(ret[0]);
  y2 = int(ret[1]);
  display.drawLine(x1,y1,x2,y2);
}
void drawHeading(U8G2 display, float heading)
{
  float origin[] = {20,16};
  float stemBottom[] = {origin[0], origin[1]+10};//stem bottom
  float stemTop[] = {origin[0], origin[1]-10};//stem top
  drawRotatedLine(display, stemTop, stemBottom, origin, heading);
  //rightSide top covered by stemtop
  float rightBottom[] = {stemTop[0]+5, stemTop[1]+5};//rightSide bottom
  drawRotatedLine(display, stemTop, rightBottom, origin, heading);
  //leftSide top covered by stemtop
  float leftBottom[] = {stemTop[0]-5, stemTop[1]+5};//rightSide bottom
  drawRotatedLine(display, stemTop, leftBottom, origin, heading);
  //display.drawCircle(origin[0], origin[1], 12, U8G2_DRAW_ALL);
}
void drawSpeed(U8G2 display)
{
  display.setDrawColor(1);
  if(GPSError)
  {
    display.setCursor(15, 24);
    display.setFont(u8g2_font_logisoso16_tf);
    display.print("GPS Error!");
  }
  else if(speedMPH<-.5)
  {
    display.setCursor(15, 24);
    display.setFont(u8g2_font_logisoso16_tf);
    display.print("No GPS Fix");
  }
  else
  {
    float mySpeed = speedMPH;
    //mySpeed = (millis()/100)%19;
    if(mySpeed<2)
    {
      mySpeed = 0;
    }
    if(mySpeed>99)
    {
      mySpeed = 99;
    }
    String str =  String(mySpeed,0);
    if(mySpeed<10)
    {
      display.setCursor(28, 31);
      display.setFont(u8g2_font_logisoso28_tf);
      display.print(str.c_str());
      display.setCursor(70, 31);
      display.setFont(u8g2_font_logisoso16_tf);
      display.print("MPH");
    }
    else
    {
      display.setCursor(35, 31);
      display.setFont(u8g2_font_logisoso28_tf);
      display.print(str.c_str());
      display.setCursor(77, 31);
      display.setFont(u8g2_font_logisoso16_tf);
      display.print("MPH");
    }
    //Serial.println(heading);
    if(heading>0)
    {
      drawHeading(display, heading);
    }
  }
  //drawHeading(display, (millis()/10)%360);
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
  else if(mode == 3)
  {
    drawSpeed(display);
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
    if(isInMenu && false)
    {
      determineAndRenderMenu(u8g2l, true);
    }
    else
    {
      determineAndRenderMode(leftMode, u8g2l, true);
    }
    u8g2l.sendBuffer();
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
