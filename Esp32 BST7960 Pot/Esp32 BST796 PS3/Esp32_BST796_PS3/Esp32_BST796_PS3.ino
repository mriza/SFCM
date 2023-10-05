#include <Ps3Controller.h>
#include <ESP32Servo.h> 
Servo myservo; // names servo

//- - - - - - - - - - pin designations  - - - - - - - - - -
const int servoPin = 27;
const int BST_LPwm = 21;    // I/O channel setup ESP32 pin 
const int BST_RPwm = 19;    // I/O chennel setup ESP32 pin
const int En_LR    = 22;    // I/O pin for BST l_EN & R_EN (enable)
const int pot      = 35;    // I/O  potentiometer signle pin

//- - - - - - - - - - inital values - - - - - - - - - -
int Idle = 25 ;           // idle (0+)
int throttleservo = Idle; // thottle servo position
int TL = 180 ;            // full throttle limet (180-)
int pos = 0;              // potentiometer value
int potvalue = 0;         //  potentiometer converted value
int Direction = 2;  // direction 1=Left turn 2=stop 3=right turn
int Speed = 255 ;


//- - - - - - - - - - channel assignments - - - - - - - - - -
const int  Channel_15 = 15;   // & PWM channel 0, for BST pin L_PWM 
const int  Channel_14 = 14;   // & PWM channel 1, for BST pin R_PWM 

//- - - - - - - - - - Channel Variables  - - - - - - - - - -
const int freq = 1000;      //  Set up PWM Frequency
const int  res = 8;         //  Set up PWM Resolution 

void setup()
{
   Serial.begin(9600); // comment out if not using
   pinMode(servoPin,OUTPUT);
    Ps3.attach(notify);
    Ps3.attachOnConnect(onConnect);
    Ps3.begin("8c:7c:b5:e7:f3:32");
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 1200, 1600);
   ledcSetup(Channel_15, freq,res); // setup PWM channel for BST L_PWM
    ledcSetup(Channel_14, freq,res); // setup PWM channel for BST R_PWM
    pinMode(En_LR ,OUTPUT); // declare pin as output    
    ledcAttachPin( BST_LPwm , Channel_15); // Attach BST L_PWM
    ledcAttachPin( BST_RPwm , Channel_14); // Attach BST R_PWM

}
void notify()
{

//- - - - - - - - - - PS3 controller input - - - - - - - - - -
//  int L2 = (abs(Ps3.data.analog.button.l2) ); //L2 trigger/brakes
    int R2 = (abs(Ps3.data.analog.button.r2) ); //R2 trigger/throttle
    int LJX = (Ps3.data.analog.stick.lx); //left joystick X axis/steering
//   int RJY = (Ps3.data.analog.stick.ry); //Right joystick Y axis/gearshift
// int STOP =  (Ps3.data.analog.button.cross); // X button/Engine kill

    digitalWrite(En_LR,HIGH); // enable BST   

//- - - - - - - - - - Motor control - - - - - - - - - -
//  this section dictates direction to variable "Direction"
//   "1" = Left turn "2" = Stop "3" = Right turn
 
int potvalue = analogRead(pot); // Read pot
 int LX = (map(LJX, -128, 128, 0, 4095)); // converts left X joystick to 0-4095
 int X = (LX-potvalue); // figures distance between pot and X L joystick 

if (X<0) { Direction=1; } else { Direction=3; }
if ( abs(X)<200) { Direction=2; } // 200 in this line is the dead zone for the joystick to
//                                   stop the motor from jumping/chattering when close
                       
//- - - - - - - - - - writes to motor - - - - - - - - - - 
   if (Direction == 2) {               // Direction 2 stop all
       ledcWrite(Channel_14,0);       // stop channel 1  (R_PWM)
       ledcWrite(Channel_15,0);       // stop chennel 0  (L_PWM)
}
 if (Direction == 1) {               // Direction 1 rotate left
       ledcWrite(Channel_14,0);       // stop channel 1  (R_PWM)     
       ledcWrite(Channel_15,Speed);   // run channel 0 (L_PWM) at "speed"
 }
 if (Direction == 3) {               // Direction 3 rotate Right
       ledcWrite(Channel_15,0);       // stop channel 0 (L_PWM)
       ledcWrite(Channel_14,Speed);   // run channel 1 (R_PWM) at "speed"
}
  // - - - - - - - - Throttle servo - - - - - - - - - - - - 
   int throttle =map(R2,0 , 255, Idle,  TL); // converts R2 input to servo range
   
      if (throttleservo>throttle){       // this section  raises or lowers
    throttleservo=throttleservo-6 ;  }   // servo position incrementaly
      if (throttleservo<throttle){       //
    throttleservo=throttleservo+6 ;  }   //
    
    myservo.write(throttleservo);    // tell servo to go to position
        
}

void onConnect(){
    Serial.println("Connected.");
}
void loop()
{
    if(!Ps3.isConnected())
        return;
}

 
