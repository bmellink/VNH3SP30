/*
 * VNH3SP30 motor driver library - demo for a two motors
 *
 * In this example the first motor will turn the left wheel and the second motor will turn the right wheel.
 * This means that if the first motor runs forward, the second motor needs to run backwards.
 * This is handled by swapping the INA and INB pins when calling the begin() function.
 *
 * The turn() function is implemented to demonstrate how to implement vehicle turning while driving or when
 * standing still.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public 
 * License as published by the Free Software Foundation; either  
 * version 2.1 of the License, or (at your option) any later version.
 *   
 * Created 2 June 2019 Bart Mellink
 */

#include <VNH3SP30.h>

VNH3SP30 Motor1;    // define control object for motor 1
VNH3SP30 Motor2;    // define control object for motor 2

// motor pins (DIAG and CS pins not used in this example)
#define M1_PWM 2    // pwm pin motor
#define M1_INA 3    // control pin INA
#define M1_INB 4    // control pin INB

#define M2_PWM 5    // pwm pin motor
#define M2_INA 6    // control pin INA
#define M2_INB 7    // control pin INB


void setup() {
  // Setting up motor connections. 
  // Please note the INA and INB pins are switched for motor2 as with this vehicle
  // this motor will run the reverse direction when the vehicle moves forward.

  Motor1.begin(M1_PWM, M1_INA, M1_INB, -1, -1);    // Motor 1 object connected through specified pins 
  Motor2.begin(M2_PWM, M2_INB, M2_INA, -1, -1);    // Motor 2 object connected through specified pins 
  Serial.begin(115200);   
}

void setSpeed(int speed) {
  Motor1.setSpeed(speed);
  Motor2.setSpeed(speed);
}

void brake(int brakePower) {
  Motor1.brake(brakePower);
  Motor2.brake(brakePower);
}

void turn(int angle) {
  // turn vehicle by providing different speed settings to the motors.
  // angle can be positive (right turn) or negative (left turn).
  // If the vehicle is already stopped, the vehicle will turn in place.
  int currentSpeed = (Motor1.speed + Motor2.speed) / 2;
  Motor1.setSpeed(currentSpeed + angle);
  Motor2.setSpeed(currentSpeed - angle);
}

void loop() {
  Serial.println("half speed forward");
  setSpeed(200); // Vehicle half-speed "forward"
  delay(2000);   // drive for 2 seconds

  Serial.println("stop (coast)");
  setSpeed(0); // Vehicle stop (coasting)
  delay(2000); // wait for 2 seconds

  Serial.println("Turn in place");
  turn(50); // turn slow
  delay(2000); // turn for 2 seconds
 
  Serial.println("stop (coast)");
  setSpeed(0); // motor stop 
  delay(2000); // wait for 2 seconds

  Serial.println("1/4 speed forward");
  setSpeed(100); // Vehicle 1/4-speed "forward"
  delay(2000); // drive for 2 seconds

  Serial.println("turn right while driving");
  turn(20); // turn
  delay(2000); // drive while turning for 2 seconds

  Serial.println("straight again");
  turn(0); // turn
  delay(2000); // drive for 2 seconds

  Serial.println("turn left while driving");
  turn(-20); // turn
  delay(2000); // drive while turning for 2 seconds

  Serial.println("Break at 3/4 power");
  brake(300); // motor brake at 3/4 power
  delay(4000); // wait for 4 seconds
}