void determineAndRenderMenu(U8G2 display, bool left)
{
  display.clearBuffer();
  if(left)
  {
    if(menuMode==0)
    {
      display.setFont(u8g2_font_crox4h_tf);
      writeCentered1(display, "Exit", 14);
    }
    else if(menuMode==1)
    {
      writeCentered1(display, "Left Mode", 14);
    }
    else if(menuMode==2)
    {
      writeCentered1(display, "Right Mode", 14);
    }
    else if(menuMode==3)
    {
      u8g2l.setFont(u8g2_font_crox4h_tf);
      writeCentered1(display, "Ignore Tailgate", 14);
    }
  }
  else
  {
    if(menuMode==1)
    {
      writeCentered1(display, String(leftMode).c_str(), 14);
    }
    else if(menuMode==2)
    {
      writeCentered1(display, String(rightMode).c_str(), 14);
    }
    else if(menuMode==3)
    {
      display.setFont(u8g2_font_crox4h_tf);
      if(ignoreTailgate)
      {
        writeCentered1(display, "True", 14);
      }
      else
      {
        writeCentered1(display, "False", 14);
      }
    }
  }
}
