// Define the pins
const int motorRPWM = 27; // RPWM pin for speed control
const int motorLPWM = 14; // LPWM pin for speed control
const int motorR_EN = 12; // R_EN pin for direction control
const int motorL_EN = 13; // L_EN pin for direction control

void setup() {
  // Set motor control pins as outputs
  pinMode(motorRPWM, OUTPUT);
  pinMode(motorLPWM, OUTPUT);
  pinMode(motorR_EN, OUTPUT);
  pinMode(motorL_EN, OUTPUT);

  // Set the motor direction and speed
  digitalWrite(motorR_EN, LOW); // Enable the right motor (you can set LOW for left motor)
  digitalWrite(motorL_EN, LOW);  // Enable the left motor (you can set HIGH for right motor)
  
  analogWrite(motorRPWM, 0);    // 20% duty cycle for right motor (adjust for desired speed)
  analogWrite(motorLPWM, 25);     // 0% duty cycle for left motor (stopped)
}

void loop() {
  // You can add additional code here if needed
}
