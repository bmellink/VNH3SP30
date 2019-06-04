# Arduino VNH3SP30, VNH5019A-E motor controller library
Arduino library for VNH3SP30, VNH2SP30, VNH3ASP30 and VNH5019A-E based motor controller boards. The VNH3SP30 and VNH5019A-E chips contain a full H bridge to drive motors with currents up to 30A. The VNH5019A-E chips are newer and almost identical to the VNH3SP30 chips. The biggest difference is that the VNH5019A-E chips work with 3.3V and 5V logic, while the older VNH3SP30 chips require 5V logic.

## Getting Started

To install this library use the **Arduino library manager** or use the **Clone or download > Download ZIP** button on the repository home page and then install the library in your Arduino environment using **Sketch > Include Library > Add .ZIP Library...**

This library supports all AVR based Arduino boards that support the ```analogWrite()``` function to generate PWM signals (most boards). If you install the ```ESP_AnalogWrite``` library, this library also works with ESP32 boards (not yet fully tested). Supports for STM32 boards: library compiles for this environment, but is not yet tested.

## VNH3SP30 board variations and interfaces

The controller boards come in various configurations. Some have a single controller chip, other may have two chips on a single board. There are three types of interfaces:
- **control** interface to control the motor (3 for each chip)
- **diagnose** interface to detect overload (2 for each chip, sometimes connected on board)
- **current measurement** interface (not on all boards)

### Control interface

Most boards have **3** key **control** lines:
- PWM - duty cycle to control motor speed or brake power
- INA + INB - to control the function:

```
 INA=1, INB=1 brake to Vcc
 INA=0, INB=0 brake to Gnd
 INA=1, INB=0 forward spin
 INA=0, INB=1 backward spin
```
**Tip**: If you want to reverse the motor (forwards becomes backwards), you can either use negative values for the ```speed()``` function or simply reverse the INA and INB pins when calling the ```begin()``` function.

### Enable/diagnose interface

All chips also have 2 **enable/diagnose** pins: **DIAGA/ENA** and **DIAGB/ENB**. These pins should have a pull up resister to Vcc (which is on most boards). These pins each have 2 functions:
- enable the board (driving the DIAGA/ENA pin low will disable the A-side of the H bridge, driving DIAGB/ENB will disable the B-side of the H bridge).
- fault condition: the VNH3SP30 chip will drive DIAG1/ENA or DIAGB/ENB low when the chip is overloaded (too high temperature on either the A or B side or a short between a motor pin and Vcc or GND).

These pins are so called "open collector" lines. Due to the fact Arduino pins can not be programmed as open collector, this library only implements sensing fault conditions. This means you should connect both pins together to use as diagnose pin in this library. On some boards the DIAGA/ENA and DIAGB/ENB pins are already connected together as a single "**EN**" board connection. This pin should be specified as ```diagPin``` when calling the ```begin()``` function.

**Tip**: You do not need to connect the enable/diagnose pins and can supply -1 for ```diagPin``` when calling the ```begin()``` function.

### Current sense interface

The **current sense** line (typically marked as CS) is not available on all boards. The VNH3SP30 chip does not support this pin, but the VNH2SP30 and VNH3ASP30 do. The CS pin provides a current proportional to the motor current. The factor varies for each chip due to tolerance differences, but in general the value for VNH3ASP30 chips is around 4700. for VNH5019A-E chips the value is around 7100. Most boards already have a resistor of 1.5k between the CS line and GND to translate the current into a voltage the Arduino can measure (using ```analogRead()```). An additional RC circuit may be present to further stabilize the CS signal. This pin should be specified as ```csPin``` when calling the ```begin()``` function.

**Example**: if the motor current in your setup is 5A, the CS pin of the chip will provide a current of 5/4700 = 0.00106 A = 1.06 mA. With a board value of 1.5k = 1500 ohm, the voltage will be 1.06 * 1500 = 1590 mV = 1.590 Volt. The ```motorCurrent()``` function uses ```analogRead()``` to read this value, which means the value returned depends on the setting of ```analogReference()``` and the working voltage of your Arduino board:
- If your Arduino runs at 5V and ```analogReference()``` is set to ```DEFAULT```, ```motorCurrent()``` will return a value of 1.590 / 5 * 1023 = 325 for the above case
- If you first change your analog reference setting with ```analogReference(INTERNAL2V56)```, the ```motorCurrent()``` function will return a value of 1.590 / 2.56 * 1023 = 636.

**Tip**: It is always good practice to use ```analogReference()``` with one of the internal reference voltage options when you want to use ```analogRead()``` as the Arduino Vcc power may not be stable. This will improve stability of your readings.

**Tip**: You do not need to connect the current sense interface and can supply -1 for ```csPin``` when calling the ```begin()``` function.

## Example code

This example shows all basic functions controlling a single motor. The example is also provided in the **Single.ino** sketch.

```
#include <VNH3SP30.h>

VNH3SP30 Motor1;    // define control object for 1 motor

// motor pins
#define M1_PWM 3    // pwm pin motor
#define M1_INA 4    // control pin INA
#define M1_INB 5    // control pin INB
#define M1_DIAG 6   // diagnose pins (combined DIAGA/ENA and DIAGB/ENB)
#define M1_CS A0    // current sense pin

void setup() {
  Motor1.begin(M1_PWM, M1_INA, M1_INB, M1_DIAG, M1_CS);    // Motor 1 object connected through specified pins 
  Serial.begin(115200);   
}

void loop() {
  Serial.println("3/4 speed forward");
  Motor1.setSpeed(300); // motor 3/4-speed "forward"
  delay(2000); // wait for 2 seconds
  Serial.print("Current="); Serial.println(Motor1.motorCurrent());

  Serial.println("Motor stop (coast)");
  Motor1.setSpeed(0); // motor stop (coasting)
  delay(2000); // wait for 2 seconds
  Serial.print("Current at stop="); Serial.println(Motor1.motorCurrent());
 
  Serial.println("Half speed backward");
  Motor1.setSpeed(-200); // motor half-speed "backward"
  delay(2000); // wait for 2 seconds
 
  Serial.println("Motor stop (coast)");
  Motor1.setSpeed(0); // motor stop 
  delay(2000); // wait for 2 seconds

  Serial.println("Full speed backward");
  Motor1.setSpeed(-400); // motor full-speed "backward"
  delay(2000); // wait for 2 seconds
  Serial.print("Current="); Serial.println(Motor1.motorCurrent());

  Serial.println("Break at 3/4 power");
  Motor1.brake(300); // motor brake at 3/4 power
  delay(10);
  Serial.print("Current during brake="); Serial.println(Motor1.motorCurrent());
  delay(4000); // wait for 4 seconds
  Serial.print("Current after brake="); Serial.println(Motor1.motorCurrent());
}
```

## Setting speed and braking

A motor controller will provide a fixed voltage to the motor, depending on the speed setting and the supply voltage of the VNH3SP30 board. A ```setSpeed()``` setting of 400 or -400 will provide the full supply voltage. A speed setting of 0 is equivalent to a non-connected motor. This means a motor will run free and the vehicle will not brake (except due to internal motor friction).

The ```brake()``` function will force the motor to a halt. The braking power can be 0, which is free run and the same as ```setSpeed(0)```. A setting of ```brake(400)``` represents maximum brake level and is equivalent to a motor where both power wires (black and red) are connected to each other: since a motor can act as dynamo, forcing the motor to turn will produce a current that immediately tries to stop it.

## Class member functions and data members

The VNH3SP30 class exposes the following functions and internal variables:

```
- void begin(int8_t pwmPin, inaPin, inbPin, diagPin, csPin);
```
This initializes the library and allocates the defined pins. You can provide a -1 for the ```diagPin``` or the ```csPin``` parameter if you do not want to use the temperature alarm or current sense signals.

```
uint8_t setSpeed(int speed); 
```
Sets motor speed, returns true if success, returns false when VNH3SP30 is overloaded. Speed should be between -400 and +400. A speed of 0 means free run. Returns motor status based on ```diagPin``` (```true``` is ok).

```
uint8_t brake(int brakePower);
```
Brake motor. brakePower=0 (or negative) same as setSpeed(0) - free run. If brakePower is positive, the motor will brake faster. A value of 400 is equivalent to full brake. Returns motor status based on ```diagPin``` (```true``` is ok).

```
uint8_t status(); 
```
Returns motor status based on ```diagPin``` (```true``` is ok, ```false``` means overloaded). Always returns ```true``` when ```diagPin``` is set to -1.

```
int motorCurrent();
```
Returns the value of ```analogRead()``` for the ```csPin```. This is a value proportional to the motor current. See above.

```
int speed;
```
This variable contains the current speed setting.

## Example sketches provided

Example sketches:

- **Single**: Controls a single motor (sketch as shown above).
- **Dual**: Controls two motors, including an illustration how to implement vehicle turning.
