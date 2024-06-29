// PINOUT MOTOR 1 - rotation
// L_EN -> 6  R_EN -> 6 Common
// L_PWM -> 8
// R_PWM -> 9

// PINOUT MOTOR 2 - cattle
// L_EN -> 7  R_EN -> 7 Common
// L_PWM -> 4
// R_PWM -> 5

#include "BTS7960.h"

int motorSpeed1 = 85.5; //max 255-100%; then 127,5-50%, 102-40%, 89.2-35%
int motorSpeed2 = 255; //max
int mStartTime = 300;
int deselTimeM1 = 50; //deceleration time to be set up of center of catle

const uint8_t EN = 6;                       //motor 1
const uint8_t L_PWM = 9;                    //motor 1
const uint8_t R_PWM = 8;                    //motor 1
BTS7960 motorController(EN, L_PWM, R_PWM);  //motor 1


const uint8_t EN_2 = 7;                             //motor 2
const uint8_t L_PWM_2 = 5;                          //motor 2
const uint8_t R_PWM_2 = 4;                          //motor 2
BTS7960 motorController_2(EN_2, L_PWM_2, R_PWM_2);  //motor 2

int led1 = 12, led2 = 13;   //the pin the LED is connected to

boolean didOneTimeActionAccelerate = true;
boolean didOneTimeActionDecelerate = true;
int monitoring;           //proximity
int metalDetection = 0;   //proximity A0 analogRead(metalDetection)
float metalDetected;      //proximity
boolean proxyOn = false;  //proximity
int proxyCounter = 0;     //proximity
int mainLoop = 0;         //for program
/******************************************
 *************** void setup ***************
 ******************************************/
void setup()
{
  pinMode(led1, OUTPUT); // Declare the LED as an output
  pinMode(led2, OUTPUT); // Declare the LED as an output

  Serial.begin(9600);
}
/******************************************
 *************** void loop* ***************
 ******************************************/
void loop()
{
  // переменная времени
  unsigned long currentMillis = millis();
  //proximity switch
  monitoring = analogRead(metalDetection);
  metalDetected = (float) monitoring * 100 / 1024.0;
 // Serial.print("#Analog read value proximity switch val Monitoring == ");Serial.println(metalDetected);
  //proximity
  if (monitoring < 250 && proxyCounter < 3) { // initial start of motor with delay 2s
    proxyOn = true;
    proxyCounter++;
    Serial.print("#Metal is Detected proxyOn AND 250 && proxyCounter < 3 == "); Serial.println(proxyOn);
    Serial.print("proxyCounter = "); Serial.println(proxyCounter);
    delay(1400); //get time delay to read metal detector 1 time ????????????????????
  } else if (monitoring < 250) { 
    Serial.print("#Metal is Detected proxyOn AND monitoring < 250 == "); Serial.println(proxyOn);
    Serial.print("proxyCounter = "); Serial.println(proxyCounter);
    
    delay(0);//align center here sz!!!!!!! v zavisimosti ot skorosti M1 - ne trogatj !
    proxyOn = true;
    proxyCounter++;
    //delay(1000);
  }
  else {
    proxyOn = false;
    //Serial.print("Metal is not Detected proxyOn == "); Serial.println(proxyOn);

  }

  if (proxyCounter <= 1 ) { // tut nado 3 ver 1.0 // ver 1.0.1 1 raz
    //start acceleration
    if (didOneTimeActionAccelerate == true ) { //run one time aceleration and continue full speed
      didOneTimeActionAccelerate = false;
      motorCWaccelerate(motorSpeed1, 200);// accelerate motor
    }
    //run motor full speed 3 touches by proximity
    runMotorCW(motorSpeed1);
  
  } else {
    //START MAIN LOOP PROGRAM
    switch (mainLoop) {
      case 0:
        if (proxyOn == true) {
          //delay(7000); //rotate befefore deceleration ALIGN Center here // ver 1.0
         // motorCWdecelerate(motorSpeed1, 200, deselTimeM1); // ver 1.0
         
          delay(2000);//align here!!! 
          motorController.Stop();// ver 1.0.1
          Serial.println("#Motor M1 Stop");
          mainLoop = 1;
         
        }
        break;
      case 1: // Catle DOWN and UP
        Serial.println("#Cainik posel vniz");
      delay(5000); //zaderzka pered tem kak cajnik powel vniz
        runMotorCCW_2(150, 10000); // run catle UP spped, time
        delay(10000); //delay before cattle goes up
        Serial.println("#Cainik posel naverh");
        runMotorCW_2(200, 15000); // run catle down
        didOneTimeActionAccelerate = true;
        mainLoop = 2; 
        delay(25000); //delay before rotation start
 motorCWaccelerate(motorSpeed1, 200);
        break;
      case 2: // start motor
        if (proxyOn == false) {
          if (didOneTimeActionAccelerate == true) {
           
            motorCWaccelerate(motorSpeed1, 200);// accelerate motor
            
             didOneTimeActionAccelerate = false;
          }
          runMotorCW(motorSpeed1);
          mainLoop = 0;
        }
        break;

      default:
        motorController.Stop();
        break;
    } //START MAIN LOOP PROGRAM
  }

} //void loop()




/*************************************
 ******** MOTOR1 RUNNING ***********
 *************************************/

void runMotorCW(int value) {
  motorController.Enable();
  motorController.TurnLeft(value);
}


void motorCCW(int value) {
  motorController.Enable();
  motorController.TurnLeft(value);
}

/*************************************
 ******** MOTOR2 RUNNING ***********
 *************************************/

void runMotorCW_2(int value, int timeRun) {
  motorController_2.Enable();
  motorController_2.TurnLeft(value);
  delay(timeRun);
  motorController_2.Stop();
  motorController_2.Disable();
}


void runMotorCCW_2(int value, int timeRun) {

  motorController_2.Enable();
  motorController_2.TurnRight(value);
  delay(timeRun);
  motorController_2.Stop();
  motorController_2.Disable();
}

/*************************************
 ******** MOTOR ACCEL.& DECEL.********
 *************************************/
void motorCWaccelerate(int motorSpeed1, int mStartTime) {  //MotorAcceleration
  motorController.Enable();
  motorController.TurnLeft(motorSpeed1);
  Serial.println("#Start acceleration");
  for (int speed = 0 ; speed < motorSpeed1; speed += 10)
  {
    motorController.TurnLeft(speed);
    Serial.print("Acceleration -> "); Serial.println(speed);
    delay(mStartTime);
  }

}


void motorCWdecelerate(int motorSpeed1, int mStartTime, int decTm ) { //MotorDeceleration
  Serial.println("#Start deceleration");
  for (int speed = motorSpeed1 ; speed > 0; speed -= decTm )
  {
    motorController.TurnLeft(speed);
    Serial.print("Deceleration -> "); Serial.println(speed);
    // delay(mStartTime);
  }
  motorController.Stop();
  motorController.Disable();

}
/*update from 1.0.2. to 1.0.3
line 13 
int motorSpeed1 = 80;
int motorSpeed1 = 100;

line 108
delay(50000);
delay(5000);

line 115
delay(50000);
delay(25000);
*/
