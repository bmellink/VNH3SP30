#include <VNH3SP30.h>

VNH3SP30 Motor1;    // define control object for 1 motor

// motor pins
#define M1_PWM 3    // pwm pin motor
#define M1_INA 4    // control pin INA
#define M1_INB 5    // control pin INB
#define M1_DIAG 6   // diagnose pins (combined DIAGA/ENA and DIAGB/ENB)

void setup() {
  Motor1.begin(M1_PWM, M1_INA, M1_INB, M1_DIAG);    // Motor 1 object connected to VNH3SP30 board through pins 
  Serial.begin(115200);   
}

void loop() {
  Serial.println("3/4 speed forward");
  Motor1.setSpeed(300); // motor full-speed "forward"
  delay(2000); // wait for 2 seconds
 
  Serial.println("Motor stop (coast)");
  Motor1.setSpeed(0); // motor stop (coasting)
  delay(2000); // wait for 2 seconds
 
  Serial.println("Half speed backward");
  Motor1.setSpeed(-200); // motor full-speed "backward"
  delay(2000); // wait for 2 seconds
 
  Serial.println("Motor stop (coast)");
  Motor1.setSpeed(0); // motor stop 
  delay(2000); // wait for 2 seconds

  Serial.println("Full speed backward");
  Motor1.setSpeed(-400); // motor full-speed "backward"
  delay(2000); // wait for 2 seconds
 
  Serial.println("Break at 3/4 power");
  Motor1.brake(300); // motor stop 
  delay(2000); // wait for 2 seconds
}