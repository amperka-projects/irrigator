#include <Arduino.h>
#include "QuadDisplay.h"
void setup(void);
char intDigit(unsigned int val, char pos);
unsigned int intDigitSet(unsigned int val, char pos, char d);
unsigned int readHumidy(void);
void loop(void);
#line 1 "src/sketch.ino"
//#include "QuadDisplay.h"

const int HUMIDY_MIN = 200;

void setup(void)
{
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  
  displayInt(3, 0);
}

char intDigit(unsigned int val, char pos)
{
  char buf[6];
  
  sprintf(buf, "%04u", val);
  
  return buf[3-pos]-'0';
}

unsigned int intDigitSet(unsigned int val, char pos, char d)
{
  char buf[6];
  
  sprintf(buf, "%04u", val);
  
  buf[3-pos] = '0'+d;
  
  return atoi(buf);
}

unsigned int readHumidy(void)
{
  static unsigned int oldval = 0;
  static unsigned int res = 0;
  static unsigned int stability[4] = { 0, 0, 0, 0 };
  
  char i;
  unsigned int val = analogRead(A0);

  if(val < 50) val = 0;
  
  for(i = 0; i < 4; i++) {
    if(intDigit(oldval, i) != intDigit(val, i)) {
      oldval = intDigitSet(oldval, i, intDigit(val, i));
      stability[i] = 0;
    }
    else {
      if(stability[i] < 10) stability[i]++;
      else res = intDigitSet(res, i, intDigit(val, i));
    }
  }
  
  return res;
}

void loop(void)
{
  static unsigned long wait = millis() + 60000;
  static unsigned int oldhumidy = 0;
  static unsigned long stability = 0;
  unsigned int humidy = readHumidy();
  
  if(humidy != oldhumidy) {
    oldhumidy = humidy;
    
    displayInt(3, humidy);
  }
  
  if(wait != 0 && wait-millis() > 10) {
    return;
  } else wait = 0;
  
  if(humidy < HUMIDY_MIN) {
    digitalWrite(2, HIGH);
    analogWrite(5, 50);
    delay(1500);
    digitalWrite(2, LOW);
    digitalWrite(5, LOW);
    wait = millis() + 3*60000;
  }
}
