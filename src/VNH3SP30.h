
/*
 * VNH3SP30 Arduino motor driver library
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public 
 * License as published by the Free Software Foundation; either  
 * version 2.1 of the License, or (at your option) any later version.
 *   
 * Created 2 June 2019 Bart Mellink
 */
#ifndef VNH3SP30_h
#define VNH3SP30_h

#include <inttypes.h>


class VNH3SP30 {

public:
  void begin(int8_t pwmPin=-1, int8_t inaPin=-1, int8_t inbPin=-1, int8_t diagPin=-1, int8_t csPin=-1);
  uint8_t setSpeed(int speed); 
  uint8_t brake(int brakePower);
  uint8_t status(); 
  int motorCurrent();
  int speed = 0;

private:
  uint8_t _pwmPin = -1;
  uint8_t _inaPin = -1;
  uint8_t _inbPin = -1;
  uint8_t _diagPin = -1;
  uint8_t _csPin = -1;
  uint8_t forward = true; // last speed command is forward
};

#endif
