void drawVoltage(U8G2 u8g2)
{
  u8g2.setDrawColor(1);
  String str =  String(volts, 2);
  if(volts<10)
  {
    u8g2.setCursor(27, 32);
    u8g2.setFont(u8g2_font_logisoso32_tf);
    u8g2.print(str.c_str());
    u8g2.setCursor(100, 32);
    u8g2.setFont(u8g2_font_logisoso16_tf);
    u8g2.print("v");
  }
  else
  {
    u8g2.setCursor(10, 32);
    u8g2.setFont(u8g2_font_logisoso32_tf);
    u8g2.print(str.c_str());
    u8g2.setCursor(105, 32);
    u8g2.setFont(u8g2_font_logisoso16_tf);
    u8g2.print("v");
  }
}
void determineAndRender(int mode, U8G2 u8g2, bool left)
{
  u8g2.clearBuffer();
  if(mode == -1)//startup
  {
    if(left)//run normal animation
    {
      int FPS = 15;
      int frames = 50;
      int frameNum = millis()/(1000/FPS);
      if(frameNum>4)//pause for 2 seconds at 4th frame
      {
        frameNum = frameNum-FPS*2;
        if(frameNum<4)
        {
          frameNum = 4;
        }
      }
      if(frameNum>(frames+FPS*2))//pause for 2 seconds at main logo
      {
        loadModes();
      }
      if(frameNum>frames)//if we are running overtime then dont.
      {
        frameNum = frames;
      }
      runLogoASync(u8g2, frameNum);
    }
    else//display wake reason or half animation if none
    {
      if(wakeReason==1)
      {
        u8g2.setCursor(10, 10);
        u8g2.setFont(u8g2_font_logisoso16_tf);
        u8g2.print("Bed Lights");
      }
    }
  }
  else if(mode == 0)
  {
    
  }
  else if(mode == 1)
  {
    drawVoltage(u8g2);
  }
}
void render()
{
  determineAndRender(leftMode, u8g2l, true);
  determineAndRender(rightMode, u8g2r, false);
  if(outs[0])
  {
    u8g2l.drawBox(0,0,64,2);
  }
  if(outs[1])
  {
    u8g2l.drawBox(64,0,64,2);
  }
  if(outs[2])
  {
    u8g2r.drawBox(0,0,64,2);
  }
  if(outs[3])
  {
    u8g2r.drawBox(64,0,64,2);
  }
  u8g2l.sendBuffer();
  u8g2r.sendBuffer();
}
