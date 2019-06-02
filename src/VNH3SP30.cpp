/*
 * VNH3SP30 motor driver library
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public 
 * License as published by the Free Software Foundation; either  
 * version 2.1 of the License, or (at your option) any later version.
 *   
 * Created 2 June 2019 Bart Mellink
 */

#include <Arduino.h>
#include <VNH3SP30.h>

void VNH3SP30::begin(int8_t pwmPin, int8_t inaPin, int8_t inbPin, int8_t diagPin) {
  this->_pwmPin = pwmPin;
  this->_inaPin = inaPin;
  this->_inbPin = inbPin;
  this->_diagPin = diagPin;
  if (pwmPin>0) pinMode(pwmPin, OUTPUT);
  if (inaPin>0) pinMode(inaPin, OUTPUT);
  if (inbPin>0) pinMode(inbPin, OUTPUT);
  if (diagPin>0) pinMode(diagPin, INPUT_PULLUP);
  this->setSpeed(0);
}

uint8_t VNH3SP30::setSpeed(int speed) {
  if (speed>0 || (speed==0 && this->forward)) {
  	digitalWrite(this->_inaPin, HIGH);
  	digitalWrite(this->_inbPin, LOW);
  	this->forward = true;
  } else if (speed<0) {
  	digitalWrite(this->_inaPin, LOW);
  	digitalWrite(this->_inbPin, HIGH);
  	speed = - speed;
  	this->forward = false;
  }
  if (speed>400) speed = 400;
  analogWrite(this->_pwmPin, speed * 51 / 80); // map 400 to 255
  return this->status();
}

uint8_t VNH3SP30::brake(int brakepower) {
  if (brakepower<0) brakepower = 0;
  if (brakepower>400) brakepower = 400;
  digitalWrite(this->_inaPin, LOW);
  digitalWrite(this->_inbPin, LOW);
  analogWrite(this->_pwmPin, brakepower * 51 / 80); // map 400 to 255
  return this->status();
}

uint8_t VNH3SP30::status() {
	if (this->_diagPin<=0) return true;
	return !digitalRead(this->_diagPin);
} 