/*
 * The fix type is as follows:
    0 = no fix
    1 = dead reckoning (requires external sensors)
    2 = 2D (not quite enough satellites in view)
    3 = 3D (the standard fix)
    4 = GNSS + dead reckoning (requires external sensors)
    5 = Time fix only
 

void GPSLogic()
{
  long speed = myGNSS.getGroundSpeed();
  MPH = speed*0.00223694;//convert mm/s to MPH
  long headingRaw = myGNSS.getHeading();
  heading = headingRaw/100000;//degrees
  fixType = myGNSS.getFixType();
  Serial.println(fixType);
}*/
#define numMPHAvgs 10//1 second
float mphAvg[numMPHAvgs];
byte mphAvgIndex = 0;

float avgMPH()
{
  float avg = 0;
  for(byte i = 0; i < numMPHAvgs; i++)
  {
    avg = avg + mphAvg[i];
  }
  return avg/numMPHAvgs;
}
float putAvgMPH(float mph)
{
  mphAvg[mphAvgIndex] = mph;
  float avg = avgMPH();
  mphAvgIndex++;
  if(mphAvgIndex>=numMPHAvgs)
  {
    mphAvgIndex = 0;
  }
  return avg;
}


#define numHeadingAvgs 5//.5 seconds
float headingAvg[numHeadingAvgs];
byte headingAvgIndex = 0;

float avgHeading()
{
  float avg = 0;
  for(byte i = 0; i < numHeadingAvgs; i++)
  {
    avg = avg + headingAvg[i];
  }
  return avg/numHeadingAvgs;
}
float putAvgHeading(float headingin)
{
  headingAvg[headingAvgIndex] = headingin;
  float avg = avgHeading();
  headingAvgIndex++;
  if(headingAvgIndex>=numHeadingAvgs)
  {
    headingAvgIndex = 0;
  }
  return avg;
}
long timeInvalid = 0;
void GPSLogic()
{
//  if(Serial2.available()>0)
//  {
//    Serial.write(Serial2.read());
//  }
//  return;
  if(gps.available(Serial2)) 
  {
    //Serial.println("----------------------read---------------------");
    fix = gps.read();
    if(fix.valid.speed)
    {
      speedMPH = putAvgMPH(fix.speed_mph());
      timeInvalid = -1;
      //Serial.println(speedMPH);
    }
    else
    {
      if(timeInvalid==-1)//if we have an invalid time for more than 1 second start putting 0s.
      {
        timeInvalid = millis();
      }
      if(millis()-timeInvalid>1000)
      {
        speedMPH = putAvgMPH(0);
      }
      //speedMPH = -1;
    }
    if(fix.valid.heading)
    {
      heading = putAvgHeading(fix.heading());
      //Serial.println(heading);
    }
  }
  else
  {
    //Serial.println("no read");
  }
}
