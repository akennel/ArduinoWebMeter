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
static int yellowPin = 12;
static int redPin = 11;
static int warnPin = 7;
static int goodPin = 6;
static float minimum = 0;     // minimum value for 0% meter
static float maximum = 255;   // maximum value for 100% meter (this depends on the resistor tollerance)
static float lightsMaximum = 225; // maximum value for light brightness
static float units = 1000;    // # of display units
static float halfWay = 500;
int oldValue;
float oldRedValue;
float oldYellowValue;

void setup()  {
  //do some fancy init moves with needle
  SetMeter(0, units);
  SetMeter(units, 1);
  SetLights(0, units);
  SetLights(units, 1);
  Serial.begin(9600);
  randomSeed(analogRead(0));
} //setup

void loop()  {
  //quite simple: read serial data, set meter value
  int meterCount = SerialReadInt();
  if (meterCount < 0)
  {
    digitalWrite(goodPin, LOW);
    digitalWrite(warnPin, HIGH);
    meterCount = meterCount * -1;
  }
  else
  {
    digitalWrite(warnPin, LOW);
    digitalWrite(goodPin, HIGH);
  }
  Serial.println(meterCount); 

  SetMeter( oldValue, meterCount );
  SetLights( oldValue, meterCount );
  oldValue = meterCount;
}

void SetLights(int oldCount, int meterValue)
{
	int newYellow = GetYellow(meterValue);
	int oldYellow = GetYellow(oldCount);
	int newRed = GetRed(meterValue);
	int oldRed = GetRed(oldCount);

	if (oldCount < meterValue)
	{
		SetColor(newYellow, oldYellow, yellowPin);
		SetColor(newRed, oldRed, redPin);
	}
	else
	{
		SetColor(newRed, oldRed, redPin);
		SetColor(newYellow, oldYellow, yellowPin);
	}
	Serial.println(newYellow);
	Serial.println(newRed);
}

void SetColor(int newColor, int oldColor, int pin)
{
	float newColorLevel = ((newColor / halfWay) * lightsMaximum);
	float priorColorLevel = ((oldColor / halfWay) * lightsMaximum);

	if (newColorLevel > priorColorLevel)
	{
		while (newColorLevel > priorColorLevel)
		{
			priorColorLevel++;
			delay(10);
			analogWrite(pin, priorColorLevel);
		}
	}
	else
	{
		while (newColorLevel < priorColorLevel)
		{
			priorColorLevel--;
			delay(10);
			analogWrite(pin, priorColorLevel);
		}
	}
  Serial.println(priorColorLevel);
}

int GetRed(int newValue)
{
	int newRed;

	if (newValue > halfWay)
	{
		newRed = newValue - halfWay;
	}
	else
	{
		newRed = 0;
	}


	return newRed;
}

int GetYellow(int newValue)
{
	int newYellow;

	if (newValue > halfWay)
	{
		newYellow = halfWay;
	}
	else
	{
		newYellow = newValue;
	}

	return newYellow;
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