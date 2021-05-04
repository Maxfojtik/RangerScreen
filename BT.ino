void notUsed()//leftover code from attempts at using an HC-05 to communicate with a bluetooth OBD2
{

if(millis()-lastCar>250 && false)
  {
    lastCar = 10000000;
    digitalWrite(13, false);
    digitalWrite(BTATPIN, false);
  }
  if(Serial.available()>0 && timer==-1)
  {
    timer = millis();
    digitalWrite(13, true);
    digitalWrite(BTATPIN, true);
    BTSerial.end();
    BTSerial.begin(ATBAUD);
  }
  if(millis()-timer>500 && Serial.available()>0 && timer!=-1)
  {
    timer = millis()-500;
    char r = Serial.read();
    Serial.write(r);
    BTSerial.write(r);
  }
  if(millis()-timer>1000)
  {
    timer = -1;
    digitalWrite(13, false);
    digitalWrite(BTATPIN, false);
    //BTSerial.begin(COMBAUD);
  }
  if(BTSerial.available()>0)
  {
    char c = BTSerial.read();
    if(c=='K')
    {
      time = 1000;
    }
    Serial.write(c);
  }
  
  if(millis()-lastNext>time && next && SETUP && false)
    {
      lastNext = millis();
        digitalWrite(13, true);
        digitalWrite(BTATPIN, true);
        delay(50);
      //Serial.println(str);
      //if(str=="OK")
      //{
        if(state==0)
        {
          BTSerial.print("AT+RMAAD\r\n");
        }
        else if(state==1)
        {
          BTSerial.print("AT+ORGL\r\n");
        }
        else if(state==2)
        {
          BTSerial.print("AT+ROLE=1\r\n");
          state++;
        }
        else if(state==3)
        {
          BTSerial.print("AT+RESET\r\n");
          delay(100);
        }
        else if(state==4)
        {
          BTSerial.print("AT+CMODE=0\r\n");
        }
        else if(state==5)
        {
          BTSerial.print("AT+BIND=AABB,CC,112233\r\n");//AABB,CC,112233 DCDC,E2,76FB47
        }
        else if(state==6)
        {
          BTSerial.print("AT+PSWD=1234\r\n");
        }
        else if(state==7)
        {
          BTSerial.print("AT+INIT\r\n");
          Serial.println("INIT");
          time = 10000000;
        }
        else if(state==8)
        {
          BTSerial.print("AT+PAIR=AABB,CC,112233,10\r\n");
          time = 10000000;
        }
        else if(state==9)
        {
          Serial.println("PAIRED");
          BTSerial.print("AT+LINK=AABB,CC,112233\r\n");
          time = 10000000;
        }
        else if(state==10)
        {
          next = false;
          digitalWrite(13, false);
          digitalWrite(BTATPIN, false);
          state=9;
        }
      //}
        Serial.print(state);
      BTSerial.flush();
      lastCar = millis();
      //delay(1000);
      state++;
    }
}
