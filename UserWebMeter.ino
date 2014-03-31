/*
 * MegaBitMeter Firmware v0.1
 *
 * by Thorsten Haas and Sascha Ludwig / 2010
 * for more informations visit http://megabitmeter.de/
 *
 * MegaBitMeter Firmware is free software, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MegaBitMeter Firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with MegaBitMeter Firmware.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The circuit:
 * - Arduino Nano (ATmega328)
 * - MegaBitMeter attached to digital pin 3 and ground.
 *   WARNING: there is more than one way to attach your MegaBitMeter to an Arduino Nano.
 *            Please read the DIY howto on http://megabitmeter.de/
 *
 * The usage:
 * - Send one to four digits to the USB serial port of the arduino, followed by CR or LF
 */
 
static int meterPin = 9;      
static int yellowPin = 10;
static int redPin = 11;
static float minimum = 0;     // minimum value for 0% meter
static float maximum = 255;   // maximum value for 100% meter (this depends on the resistor tollerance)
static float lightsMaximum = 225; // maximum value for light brightness
static float units = 1000;    // # of display units
static int halfWay = units/2;
int oldValue;

void setup()  {
  //do some fancy init moves with needle
  SetMeter(0, units);
  SetMeter(units, 1);
  //SetLights(0, units);
  //SetLights(units, 1);
  //analogWrite(meterPin, 255);
  //start serial communication
  //pinMode(sirenPin, OUTPUT);
  Serial.begin(9600);
  randomSeed(analogRead(0));
} //setup

void loop()  {
  //quite simple: read serial data, set meter value
  int meterCount = SerialReadInt();
  
  //SetLights( oldValue, meterCount );
  SetMeter( oldValue, meterCount );
  
  oldValue = meterCount;
}

void SetLights ( int oldCount, int newCount )
{
  if (oldCount > newCount)
  {
    DecreaseLight(oldCount, newCount);
  }
  else
  {
    IncreaseLight(oldCount, newCount);
  }
}

void DecreaseLight(int oldCount, int newCount)
{
  if (oldCount > halfWay)
  {
    if (newCount > halfWay)
    {
      ChangeLight(oldCount, newCount - halfWay, redPin);
    }
    else
    {
      ChangeLight(oldCount, 1, redPin);
      ChangeLight(halfWay, newCount, yellowPin);
    }    
  }
  else
  {
    ChangeLight(oldCount, newCount, yellowPin);
  }
}

void IncreaseLight(int oldCount, int newCount)
{
  if (oldCount > halfWay)
  {
    ChangeLight(oldCount, newCount - halfWay, redPin);
  }
  else
  {
    if (newCount > halfWay)
    {
      ChangeLight(oldCount, halfWay, yellowPin);
      ChangeLight(1, newCount - halfWay, redPin);
    }
    else
    {
      ChangeLight(oldCount, newCount, yellowPin);
    }
  }
}

void ChangeLight(int oldCount, int newCount, int pin)
{
  float newLevel = ((newCount/halfWay) * lightsMaximum);
  float oldLevel = ((oldCount/halfWay) * lightsMaximum);
  
  if (oldLevel > newLevel)
  {
    while (oldLevel > newLevel)
    {
      analogWrite(pin, oldLevel);
      delay(10);
      oldLevel--;
    }
  }
  else
  {
    while (oldLevel < newLevel)
    {
      analogWrite(pin, oldLevel);
      delay(10);
      oldLevel++;
    }
  }
}

void SetMeter( int oldCount, int meterValue )
{
  float newLevel = ((meterValue/units) * maximum);
  float priorLevel = ((oldCount/units) * maximum);
  
  if (newLevel > priorLevel)
  {
    while (newLevel > priorLevel)
    {
      priorLevel++;
      delay(10);
      analogWrite(meterPin, priorLevel);
    }
  }
  else
  {
      while (newLevel < priorLevel)
    {
      priorLevel--;
      delay(10);
      analogWrite(meterPin, priorLevel);
    }
  }
  
}

int SerialReadInt() {
  //the string we read from serial will be stored here:
  char str[32];
  str[0] = '\0';
  int c=0;
  while(1<2) {
    //see if we have serial data available:
    if (Serial.available() > 0) {
      //yes, we have!
      //store it at next position in string
      str[c] = Serial.read();
      //if it is newline, return or we find a variable separator, then terminate the string
      //and leave the infinite loop:
      if (str[c] == '\n' || str[c] == '\0' || str[c] == '\r') {
        str[c] = '\0';
        break;
      }
      //ok, we are not at the end of the string, go on with next:
      else
        c++;
    }
    //in case there is no serial data available, make a short pause before retrying:
    else
      delayMicroseconds(500);
  }
  //convert the string to int and return:
  return(atoi(str));
} //SerialReadInt
