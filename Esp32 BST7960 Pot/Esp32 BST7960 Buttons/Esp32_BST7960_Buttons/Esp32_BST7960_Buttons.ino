
//- - - - - - - - - - pin designations  - - - - - - - - - -
const int BST_LPwm = 21 ;  // I/O channel setup ESP32 pin 
const int BST_RPwm = 19 ;  // I/O chennel setup ESP32 pin
const int En_LR    = 22 ;  // I/O pin for BST l_EN & R_EN (enable)
const int Fwd      = 13 ;  // motor forward
const int Stp      = 12 ;  // motor stop
const int Rev      = 14 ;  // motor forward

//- - - - - - - - - - inital values - - - - - - - - - -
int Direction = 2 ;        // direction 1=Left turn 2=stop 3=right turn
int Fwdstate  = 0 ;
int Revstate  = 0 ;
int Stpstate  = 0 ;
int Speed = 255 ;

//- - - - - - - - - - channel assignments - - - - - - - - - -
const int  Channel_15 = 15 ;   // & PWM channel 0, for BST pin L_PWM 
const int  Channel_14 = 14 ;   // & PWM channel 1, for BST pin R_PWM 

//- - - - - - - - - - Channel Variables  - - - - - - - - - -
const int freq = 1000;      //  Set up PWM Frequency
const int  res = 8;         //  Set up PWM Resolution 

void setup(){   
  
  ledcSetup(Channel_15, freq,res) ; // setup PWM channel for BST L_PWM
  ledcSetup(Channel_14, freq,res) ; // setup PWM channel for BST R_PWM
  ledcAttachPin( BST_LPwm , Channel_15) ; // Attach BST L_PWM
  ledcAttachPin( BST_RPwm , Channel_14) ; // Attach BST R_PWM
  pinMode(Fwd ,INPUT) ; 
  pinMode(Stp ,INPUT) ; 
  pinMode(Rev ,INPUT) ; 
  pinMode(En_LR ,OUTPUT) ; 
 
}
void loop()
{
  digitalWrite(En_LR,HIGH) ; // enable BST   

//- - - - - - - - - - Motor control - - - - - - - - - -
//  this section dictates direction to variable "Direction"
//   "1" = Left turn "2" = Stop "3" = Right turn

   Fwdstate = digitalRead(Fwd) ;
   Stpstate = digitalRead(Stp) ;
   Revstate = digitalRead(Rev) ;
 
   if (Fwdstate == 1) { Direction = 1 ; }  
   if (Revstate == 1) { Direction = 3 ; } 
   if (Stpstate == 1) { Direction = 2 ; } 

//- - - - - - - - - - writes to motor - - - - - - - - - - 
   if (Direction == 2) {               // Direction 2 stop all
       ledcWrite(Channel_14,0) ;       // stop channel 14  (R_PWM)
       ledcWrite(Channel_15,0) ;       // stop chennel 15  (L_PWM)
}
 if (Direction == 1) {                // Direction 1 rotate left
       ledcWrite(Channel_14,0) ;       // stop channel 14  (R_PWM)     
       ledcWrite(Channel_15,Speed) ;     // run  channel 15 (L_PWM) at "speed"
 }
 if (Direction == 3) {                // Direction 3 rotate Right
       ledcWrite(Channel_15,0) ;       // stop channel 15 (L_PWM)
       ledcWrite(Channel_14,Speed) ;     // run  channel 14 (R_PWM) at "speed"
}
} 
