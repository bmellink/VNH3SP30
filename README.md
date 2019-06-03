# Arduino VNH3SP30 motor controller library
Arduino library for VNH3SP30, VNH2SP30, VNH3ASP30 based motor controller boards.


## Getting Started

To install this library use the **Clone or download > Download ZIP** button on the repository home page and then install the library in your Arduino environment using **Sketch > Include Library > Add .ZIP Library...**

This library supports all AVR based Arduino boards that support the analogWrite() function to generate PWM signals (most boards). If you install the ESP_AnalogWrite library, this library should also work with ESP32 boards (not yet tested).

### VNH3SP30 board and variations

There are three types of connections:
- **control** lines to control the motor (3)
- **diagnose** lines to detect overload (temperature)
- **current** measurement line (not on all boards)

Most boards have **3** key **control** lines:
- PWM - duty cycle to control motor speed
- INA + INB - to control the function:

```
 INA=1, INB=1 brake to Vcc
 INA=0, INB=0 brake to Gnd
 INA=1, INB=0 forward spin
 INA=0, INB=1 backward spin
```
**Tip**: If you want to reverse the motor (forwards becomes backwards), you can either use negative values for the speed() function or simply reverse the pins for INA and INB.

All chips also have 2 **enable/diagnose** pins: **DIAGA/ENA** and **DIAGB/ENB**. These pins should have a pull up resister to Vcc (which is on most boards). These pins each have 2 functions:
- enable the board (driving the DIAGA/ENA pin low will disable the A-side of the H bridge, driving DIAGB/ENB will disable the B-side of the H bridge)
- fault condition: the VNH3SP30 chip will drive DIAG1/ENA or DIAGB/ENB low when the chip is overloaded (too high temperature on either the A or B side)

These pins are so called "open collector" lines. Due to the fact Arduino pins can not be programmed as open collector, this library only implements sensing fault conditions. This means you should connect both pins together to use as diagnose pin in this library. On some boards the DIAGA/ENA and DIAGB/ENB pins are already connected together as a single "**EN**" board connection.

**Tip**: You do not need to connect the enable/diagnose pins and can simply supply -1 for the diagnose pin number when calling the begin() function.

### Current sense

The **current** sense line (typically marked as CS) is not available on all boards. The VNH3SP30 chip does not support this pin, but the VNH2SP30 and VNH3ASP30 do. The CS pin provides a current proportional to the motor current. The factor varies for each chip due to tolerance differences, but in general the value is around 4700. Most boards already have a resistor of 1.5k between the CS line and GND to translate the current into a voltage the Arduino can measure (using analogRead()). An additional RC circuit may be present to further stabilize the CS signal.

**Example**: if the motor current in your setup is 5A, the CS pin of the chip will provide a current of 5/4700 = 0.00106 A = 1.06 mA. With a board value of 1.5k = 1500 ohm, the voltage will be 1.06 * 1500 = 1590 mV = 1.590 Volt. The motorcurrent() function uses analogRead() to read this value, which means the value returned depends on the setting of analogReference() and the working voltage of your Arduino board:
- If your Arduino runs at 5V and analogReference() is set to DEFAULT, motorcurrent() will return a value of 1.590 / 5 * 1023 = 325 for the above case
- If you first change your analog reference setting with analogReference(INTERNAL2V56), the motorcurrent() function will return a value of 1.590 / 2.56 * 1023 = 636.

**Tip**: It is always good practice to use analogReference() with one of the internal reference voltage options when you want to use analogRead() as the Arduino Vcc power may not be stable. This will improve stability of your readings.


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
#define M1_CS A0     // current sense pin

void setup() {
  Motor1.begin(M1_PWM, M1_INA, M1_INB, M1_DIAG, M1_CS);    // Motor 1 object connected through specified pins 
  Serial.begin(115200);   
  analogReference(DEFAULT);  // use one of the INTERNAL reference settings to improve accuracy
  analogRead(M1_CS); // dummy
}

void loop() {
  Serial.println("3/4 speed forward");
  Motor1.setSpeed(300); // motor 3/4-speed "forward"
  delay(2000); // wait for 2 seconds
  Serial.print("Current="); Serial.println(Motor1.motorcurrent());

  Serial.println("Motor stop (coast)");
  Motor1.setSpeed(0); // motor stop (coasting)
  delay(2000); // wait for 2 seconds
  Serial.print("Current at stop="); Serial.println(Motor1.motorcurrent());
 
  Serial.println("Half speed backward");
  Motor1.setSpeed(-200); // motor half-speed "backward"
  delay(2000); // wait for 2 seconds
 
  Serial.println("Motor stop (coast)");
  Motor1.setSpeed(0); // motor stop 
  delay(2000); // wait for 2 seconds

  Serial.println("Full speed backward");
  Motor1.setSpeed(-400); // motor full-speed "backward"
  delay(2000); // wait for 2 seconds
  Serial.print("Current="); Serial.println(Motor1.motorcurrent());

  Serial.println("Break at 3/4 power");
  Motor1.brake(300); // motor brake at 3/4 power
  delay(10);
  Serial.print("Current during brake="); Serial.println(Motor1.motorcurrent());
  delay(4000); // wait for 4 seconds
  Serial.print("Current after brake="); Serial.println(Motor1.motorcurrent());
}

```

## Setting speed and breaking

A motor controller will provide a fixed voltage to the motor, depending on the speed setting and the supply voltage of the VNH3SP30 board. A setSpeed() setting of 400 or -400 will provide the full supply voltage. A speed setting of 0 is equivalent to a non-connected motor. This means a motor will run free and the vehicle will not brake (except due to friction in the motor).

The brake() function will force the motor to a halt. The braking power can be 0, which is free run and the same as setSpeed(0). A setting of brake(400) represents maximum brake level and is equivalent to a motor that is not connected to a power source, but with its power wires (black and red) connected.

## Class member functions and data members

The VNH3SP30 class exposes the following functions:

```
- void begin(int8_t pwmPin, inaPin, inbPin, diagPin);
```
This initializes the library and allocates the defined pins. You can provide a -1 for the diagPin parameter if you do not want to use the temperature alarm from the VNH3SP30 board.

```
uint8_t setSpeed(int speed); 
```
Sets motor speed, returns true if success, returns false when VNH3SP30 is overloaded. Speed should be between -400 and +400. A speed of 0 means free run.

```
uint8_t brake(int brakepower);
```
Brake motor. brakepower=0 (or negative) same as setSpeed(0) - free run. If brakepower is positive, the motor will brake faster. A value of 400 is equivalent to full brake.

```
uint8_t status(); 
```
Returns controller status. true is ok, false means overloaded

```
int motorcurrent();
```
Returns the value of analogRead() for the CS pin. This is a value proportional to the motor current. See above.

```
int speed;
```
This variable contains the current speed setting.

## Motor 
## Example sketches provided

Example sketches:

- **Single**: Controls a single motor (sketch as shown above).
- **Dual**: Controls two motors, including illustration how to implement vehicle turning.
