#include "AzEscControl.h"

#define RET_SUCCESS       1
#define RET_ERROR         0
#define MOTOR_MAX         7
#define MOTOR_MIN         -7
#define MOTOR_ANALOG_VAL  150
#define MOTOR_PIN_LEFT    3
#define MOTOR_PIN_RIGHT   9

//macros for movements
#define MOVEMENT_STOP     0
#define MOVEMENT_DIR_FWD  1
#define MOVEMENT_DIR_BKWD 2
#define MOVEMENT_DIR_LT   3
#define MOVEMENT_DIR_RT   4
#define MOVEMENT_DIR_LS   5
#define MOVEMENT_DIR_RS   6

//left motor
EscControl escL(3);
//right motor
EscControl escR(9);

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

int value;

void setup() {
  Serial.begin(115200);
  Serial.println("Startup...");
  delay(1000);

  /*
  pinMode(trigPinL, OUTPUT);  
	pinMode(echoPinL, INPUT); 
  pinMode(trigPinMid, OUTPUT);  
	pinMode(echoPinMid, INPUT);  
  pinMode(trigPinR, OUTPUT);  
	pinMode(echoPinR, INPUT);

  int mid = distance(trigPinMid, echoPinMid);
  int left = distance(trigPinL, echoPinL);
  int right = distance(trigPinR, echoPinR);
  */

  pinMode(MOTOR_PIN_LEFT, OUTPUT);  
  pinMode(MOTOR_PIN_RIGHT, OUTPUT);  

  escL.begin();
  escR.begin();
}


void loop() {
  // put your main code here, to run repeatedly:

  //Serial.println("Press 0 if start");

  if (Serial.available()) 
  {
    int incomingByte = Serial.read();
    int value = incomingByte - '0';

    //receive value from jetson [0..1..2..3..4..5..6]
    if(value == MOVEMENT_STOP)
    {
      move(0, 0, 0, 0);
    }
    else if (value == MOVEMENT_DIR_FWD)
    {
      move(10, 10, 1, 1);
    }
    else if (value == MOVEMENT_DIR_BKWD)
    {
      move(-10, -10, 0, 0);
    }
    else if (value == MOVEMENT_DIR_LT)
    {
      move(0, 7, 0, 0);
    }
    else if (value == MOVEMENT_DIR_RT)
    {
      move(7, 0, 1, 0);
    }
    else if (value == MOVEMENT_DIR_LS)
    {
      move(-7, 7, 0, 0);
    }
    else if (value == MOVEMENT_DIR_RS)
    {
      move(7, -7, 0, 0);
    }
    else 
    {
      Serial.println("invalid input");
    }

  }

  //test block for movement control
  // int input = Serial.parseInt();

  // if(input == 1)
  // {
  //     int ret = move(-10, -9, 0, 0);
  //     delay(3000);
  // }
  // else if(input == 2)
  // {
  //   // ... left
  // }
  // else if(input == 3)
  // {
  //   // ... right
  // }
  // else if(input == 0)
  // {
  //     int ret = move(0, 0, 0, 0);
  // }
 
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


// NOTE: max val: 50, min val: -50
int move(int speedL, int speedR, int offsetL, int offsetR)
{
  // sanity check
  if((speedL >= AZ_ESC_CTRL_BACKWARD_MAX_SPEED && speedL <= AZ_ESC_CTRL_FORWARD_MAX_SPEED) &&
      (speedR >= AZ_ESC_CTRL_BACKWARD_MAX_SPEED && speedR <= AZ_ESC_CTRL_FORWARD_MAX_SPEED))
  {
    //escL.setSpeed(speedL, 10); // 1000 msec = 1 sec
    //escR.setSpeed(speedR, 10);

    analogWrite(MOTOR_PIN_LEFT, MOTOR_ANALOG_VAL + speedL + offsetL);
    analogWrite(MOTOR_PIN_RIGHT, MOTOR_ANALOG_VAL + speedR + offsetR);
    return RET_SUCCESS;
  }
  else
  {
    return RET_ERROR;
  }
}
/* EOL */
