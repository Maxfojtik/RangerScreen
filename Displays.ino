void drawVoltage(U8G2 display)
{
  display.setDrawColor(1);
  String str =  String(volts, 2);
  if(volts<10)
  {
    display.setCursor(27, 32);
    display.setFont(u8g2_font_logisoso32_tf);
    display.print(str.c_str());
    display.setCursor(100, 32);
    display.setFont(u8g2_font_logisoso16_tf);
    display.print("v");
  }
  else
  {
    display.setCursor(10, 32);
    display.setFont(u8g2_font_logisoso32_tf);
    display.print(str.c_str());
    display.setCursor(105, 32);
    display.setFont(u8g2_font_logisoso16_tf);
    display.print("v");
  }
}
void determineAndRender(int mode, U8G2 display, bool left)
{
  display.clearBuffer();
  if(mode == -1)//startup
  {
    int FPS = 15;
    int framesLeft = 49;
    int framesRight = 41;
    int frameNum = (millis()-startAnimation)/(1000/FPS);
    if(left)//run normal animation
    {
      if(frameNum>3)//pause for 2 seconds at 3rd frame
      {
        frameNum = frameNum-10;
        if(frameNum<3)
        {
          frameNum = 3;
        }
      }
      if(frameNum>(framesLeft+30))//pause for 2 seconds at main logo
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
      int waitFrames = 10;
      int secondWaitFrame = 16;
      int secondWaitFrameTime = 6;
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
  else if(mode == 0)
  {
  }
  else if(mode == 1)
  {
    drawVoltage(display);
  }
}
void render()
{
  determineAndRender(leftMode, u8g2l, true);
  u8g2l.sendBuffer();
  determineAndRender(rightMode, u8g2r, false);
  u8g2r.sendBuffer();
//  if(leftMode!=-1)
//  {
//    if(outs[0])
//    {
//      u8g2l.drawBox(0,0,64,2);
//    }
//    if(outs[1])
//    {
//      u8g2l.drawBox(64,0,64,2);
//    }
//    if(outs[2])
//    {
//      u8g2r.drawBox(0,0,64,2);
//    }
//    if(outs[3])
//    {
//      u8g2r.drawBox(64,0,64,2);
//    }
//  }
}
