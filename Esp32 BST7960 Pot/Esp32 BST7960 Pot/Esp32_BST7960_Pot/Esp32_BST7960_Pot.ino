
//- - - - - - - - - - pin designations  - - - - - - - - - -
const int BST_LPwm = 21;    // I/O channel setup ESP32 pin 
const int BST_RPwm = 19;    // I/O chennel setup ESP32 pin
const int En_LR    = 22;    // I/O pin for BST l_EN & R_EN (enable)
const int pot      = 35;    // I/O  potentiometer signal  pin

//- - - - - - - - - - inital values - - - - - - - - - -
               // potentiometer value
int potvalue = 2047;         //  potentiometer converted value
int Direction = 2;        // direction 1=Left turn 2=stop 3=right turn
int Speed = 255 ;

//- - - - - - - - - - channel assignments - - - - - - - - - -
const int  Channel_15 = 15;   // & PWM channel 0, for BST pin L_PWM 
const int  Channel_14 = 14;   // & PWM channel 1, for BST pin R_PWM 

//- - - - - - - - - - Channel Variables  - - - - - - - - - -
const int freq = 1000;      //  Set up PWM Frequency
const int  res = 8;         //  Set up PWM Resolution 

void setup()
{   
   ledcSetup(Channel_15, freq,res); // setup PWM channel for BST L_PWM
    ledcSetup(Channel_14, freq,res); // setup PWM channel for BST R_PWM
    ledcAttachPin( BST_LPwm , Channel_15); // Attach BST L_PWM
    ledcAttachPin( BST_RPwm , Channel_14); // Attach BST R_PWM
    pinMode(En_LR ,OUTPUT); // declare pin as output 

}
void loop()
{
    digitalWrite(En_LR,HIGH); // enable BST   

//- - - - - - - - - - Motor control - - - - - - - - - -
//  this section dictates direction to variable "Direction"
//   "1" = Left turn "2" = Stop "3" = Right turn
 
  int potvalue = analogRead(pot); // Read pot
if (potvalue < 2050) { Direction=1; } else { Direction=3; }
if (potvalue > 2000 && potvalue < 2100 ) { Direction=2 ; } // center dead zone
                     
//- - - - - - - - - - writes to motor - - - - - - - - - - 
   if (Direction == 2) {               // Direction 2 stop all
       ledcWrite(Channel_14,0);       // stop channel 14  (R_PWM)
       ledcWrite(Channel_15,0);       // stop chennel 15  (L_PWM)
}
 if (Direction == 1) {               // Direction 1 rotate left
       ledcWrite(Channel_14,0);       // stop channel 14  (R_PWM)     
       ledcWrite(Channel_15,Speed);   // run channel 15 (L_PWM) at "speed"
 }
 if (Direction == 3) {               // Direction 3 rotate Right
       ledcWrite(Channel_15,0);       // stop channel 15 (L_PWM)
       ledcWrite(Channel_14,Speed);   // run channel 14 (R_PWM) at "speed"
}
 
}
