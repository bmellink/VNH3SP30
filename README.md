# Arduino VNH3SP30 motor controller library
Arduino library for the VNH3SP30 or VNH3ASP30 motor controller.


## Getting Started

To install this library use the **Clone or download > Download ZIP** button on the repository home page and then install the library in your Arduino environment using **Sketch > Include Library > Add .ZIP Library...**

This library supports all AVR based Arduino boards that support the analogwrite function to generate PWM signals (most boards). If you install the ESP_AnalogWrite library, this library should also work with ESP32 boards (not yet tested).

### VNH3SP30 board

The VNH3SP30 chip has **3** key control lines:
- PWM - duty cycle to control motor speed
- INA + INB - to control the function:

```
 INA=1, INB=1 brake to Vcc
 INA=0, INB=0 brake to Gnd
 INA=1, INB=0 forward spin
 INA=0, INB=1 backward spin
```
**Tip**: If you want to reverse the motor (forwards becomes backwords), you can either use negative values for the speed() function or simply reverse the pins for INA and INB.

The VNH3SP30 chip also has 2 enable/diagnose pins: **DIAGA/ENA** and **DIAGB/ENB**. These pins should have a pull up resister to Vcc (which is on most boards). These pins each have 2 functions:
- enable the board (driving the DIAGA/ENA pin low will disable the A-side of the H bridge, driving DIAGB/ENB will disable the B-side of the H bridge)
- fault condition: the VNH3SP30 chip will drive DIAG1/ENA or DIAGB/ENB low when the chip is overloaded (too high temperature on either the A or B side)

These pins are so called "open collector" lines. Due to the fact Arduino can not be programmed as open collector, this library only implements sensing the fault condition. This means you can simply connect both pins and sense if the value is a 0 to report a fault condition.

On some boards the DIAGA/ENA and DIAGB/ENB pins are already connected together as a single "**EN**" board connection.

**Tip**: You do not need to connect the enable/diagnose pins and simply supply -1 for the pin number for the begin() function.

### Example code (AVR)

This example is for any AVR Arduino board. Note: this example is for AVR based boards only as the esp32 library does not support the analogwrite() function used by the servo.h library.

```
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

```


## Class member functions and data members

The VNH3SP30 class exposes the following functions:

```
- void begin(int8_t pwmPin, inaPin, inbPin, diagPin);
```
This initializes the library and allocates the defined pins

```
uint8_t setSpeed(int speed); 
```
Sets motor speed, returns true if success, returns false when VNH3SP30 is overloaded. Speed should be between -400 and +400. A speed of 0 means free run.

```
uint8_t brake(int brakepower);
```
Brake motor. brakepower=0 (or negative) same as setSpeed(0) - free run. If brakepower is positive (max 400) brake faster.

```
uint8_t status(); 
```
Returns controller status. true is ok, false means overloaded

## Example sketches provided

Example sketches:

- **Demo**: Controls a single motor (sketch as shown above)
