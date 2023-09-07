#include "AzEscControl.h"

EscControl esc1(3);
EscControl esc2(9);

//left sesnor pins
const int trigPinL = 12;  
const int echoPinL = 8; 

float durationL, distanceL; 


//center sensor pins
const int trigPinMid = 5;  
const int echoPinMid = 7; 

float durationMid, distanceMid; 


//right sensor pins
const int trigPinR = 10;  
const int echoPinR = 11; 

float durationR, distanceR;

void setup() {
  Serial.begin(115200);
  Serial.println("ESC test");
  pinMode(trigPinL, OUTPUT);  
	pinMode(echoPinL, INPUT); 
  pinMode(trigPinMid, OUTPUT);  
	pinMode(echoPinMid, INPUT);  
  pinMode(trigPinR, OUTPUT);  
	pinMode(echoPinR, INPUT);

  int mid = distance(trigPinMid, echoPinMid);
  int left = distance(trigPinL, echoPinL);
  int right = distance(trigPinR, echoPinR);

  esc1.begin();
  esc2.begin();
}


int8_t value = 0;
uint32_t readTimeout = 0;

#define WAIT_INPUT  (1)
#define READ_INPUT  (2)
#define SET_INPUT   (3)
#define VACUUM_MODE  (4)

uint32_t state = WAIT_INPUT;
String svalue = "";


void loop() {

  esc1.loop();
  esc2.loop();

  switch(state)
  {

    case WAIT_INPUT:
    {
      if (Serial.available() > 0)
      {
        readTimeout = millis() + 50;
        state = READ_INPUT;
      }
    }
    break;


    case READ_INPUT:
    {
      if (millis() < readTimeout)
      {
        char ch = (char)Serial.read();
        
        if ((ch != '\0') && (ch != '\r') && (ch != '\n') && (ch != -1))
        {
          svalue += ch;
          Serial.write(ch);
        }
      }      
      else
      {
        readTimeout = 0;

        if (svalue.length() > 0)
        {
          state = SET_INPUT;
        }
        else
        {
          state = WAIT_INPUT;
          svalue = ""; 
        }
      }
    }
    break;


    case SET_INPUT:
    {
      value = (uint8_t) svalue.toInt();
      //Robot vaccum cleaner mode activates by inputing value more than 100.
      if (value > 100)
      {
        Serial.println(value);
        state = VACUUM_MODE;
        break;
      }
      else
      {
        Serial.println(svalue.toInt());    
    
        esc1.setSpeed(value, 300);
        esc2.setSpeed(value, 300);
      }
      
      readTimeout = 0;
      svalue = "";

      state = WAIT_INPUT;
    }
    break;

    case VACUUM_MODE:
    {
      // esc1.setSpeed(4, 300);
      // esc2.setSpeed(4, 300);
      // Serial.println("Robot vaccum cleaner mode activated");

      // while(true)
      // {
      //   int mid = distance(trigPinMid, echoPinMid);
      //   int left = distance(trigPinL, echoPinL);
      //   int right = distance(trigPinR, echoPinR);

      //   if (mid < 80)
      //   {
      //     esc1.setSpeed(0, 300);
      //     esc2.setSpeed(0, 300);
      //     Serial.println("Object in front");
      //   }
      //   else if (left < 40)
      //   {
      //     esc1.setSpeed(0, 300);
      //     esc2.setSpeed(0, 300);
      //     Serial.println("Object in front");

      //   }

      // }
      //rotateLeft(7,7);
      forward(5);
      //

      readTimeout = 0;
      svalue = "";

      state = WAIT_INPUT;
    }
    break;
  }

}

int distance(int trig, int echo)
{
  float duration, distance;
  digitalWrite(trig, LOW);  
	delayMicroseconds(2);  
	digitalWrite(trig, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(trig, LOW);  

  duration = pulseIn(echo, HIGH); 

  distance = (duration*.0343)/2;  

  return distance;
}

//methods for rotations
//turn right
void rotateRight(int m1, int m2)
{
  analogWrite(3, 150+m1);
  analogWrite(9, 150-m2);
}

//turn left
void rotateLeft(int m1, int m2)
{
  analogWrite(3, 150-m1);
  analogWrite(9, 150+m2);
  // delay(1500);
  // analogWrite(3, 150);
  // analogWrite(9, 150);
}

//move backwards
void backward(int speed)
{
  analogWrite(3, 150-speed);
  analogWrite(9, 150-speed);
}

//move forward
void forward(int speed)
{
  analogWrite(3, 150+speed);
  analogWrite(9, 150+speed);
  delay(2000);
  // analogWrite(3, 150);
  // analogWrite(9, 150);
}

