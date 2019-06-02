# Arduino VNH3SP30 motor controller library
Arduino library for the VNH3SP30 or VNH3ASP30 motor controller.


## Getting Started

To install this library use the **Clone or download > Download ZIP** button on the repository home page and then install the library in your Arduino environment using **Sketch > Include Library > Add .ZIP Library...**

This library supports AVR based Arduino boards (Arduino MEGA, UNO, Nano, Micro, etc.).

### Prerequisites




### Example code (AVR)

This example is for any AVR Arduino board. Note: this example is for AVR based boards only as the esp32 library does not support the analogwrite() function used by the servo.h library.

```
#include <IBusBM.h>
#include <Servo.h>

IBusBM IBus;    // IBus object
Servo myservo;  // create servo object to control a servo

void setup() {
  IBus.begin(Serial);    // iBUS object connected to serial0 RX pin
  myservo.attach(9);     // attaches the servo on pin 9 to the servo object
}

void loop() {
  int val;
  val = IBus.readChannel(0); // get latest value for servo channel 1
  myservo.writeMicroseconds(val);   // sets the servo position 
  delay(20);
}

```


## Class member functions and data members

The VNH3SP30 class exposes the following functions:

```
- void begin(HardwareSerial& serial, int8_t timerid=0, int8_t rxPin=-1, int8_t txPin=-1);
```
This initializes the library f

```
uint8_t addSensor(uint8_t type); 
```
Defines new sensor of type "type", returns sensor number (first is number 1)

```
uint16_t readChannel(uint8_t channelNr);
```
Read the value of servo channel 0..9 corresponding with servo channels 1..10.

```
void setSensorMeasurement(uint8_t adr, uint16_t value);
```
Set value of sensor number adr to a given value (first sensor is number 1). The background process will send the value back through the receiver.

```
void loop();
```
Call the internal polling function (at least once per 1 ms) in case you disable the timer interrup. See below. If you have multiple instances of the IbusBM class, you only need to call this function for the first instance.

The IBusBM class exposes the following counters. Counters are 1 byte (value 0..255) and values can be used by your code to understand if new data is available. 

```
uint8_t cnt_rec; // count received number of servo messages from receiver
uint8_t cnt_poll; // count received number of sensor poll messages
uint8_t cnt_sensor; // count times a sensor value has been sent back
```

Counters can also be used to debug the hardware connections between the receiver and the Arduino board: If at least one sensor is defined, the RX pin will receive sensor poll messages. If the sensor is not able to "talk back" to the receiver the receiver will try to establish contact with the sensor every 7ms and the cnt_poll counter will increment. Only if the TX pin is correctly connected to the receiver, the cnt_sensor counter will increment and the cnt_poll value will stay the same.



## Example sketches provided

Example sketches:

- **Ibus2PWM**: converts iBUS data to Servo PWM format: Reads data from first servo channel and translates this to PWM signal to send to a normal servo (AVR version)
- **Ibus2PWM_ESP32**: converts iBUS data to Servo PWM format: Reads data from first servo channel and translates this to PWM signal to send to a normal servo (ESP32 version)
- **Ibus_singlemonitor**: monitor/debugger for receivers with a single iBUS pin (providing both servo and sensor data such as the Flysky X6B and Flysky FS-iA8X). Prints out servo channels to the standard serial output (PC debug window) and simulates 2 sensors with random values sent back to transmitter (as long as your receiver supports this). Requires Arduino board with 2 or more hardware serial ports (such as MEGA 2560)
- **Ibus_multimonitor**: monitor/debugger for receivers with a two separate iBUS pins (one for servo data and one for sensor data, such as the TGY-IA6B). Prints out servo channels to the standard serial output (PC debug window) and simulates 2 sensors with random values sent back to transmitter. Requires Arduino board with 3 or more hardware serial ports (such as MEGA 2560)
- **IBus_sensor**: simulate two telemetry sensors and send values back over the iBUS to the receiver to be shown in the display of your transmitter.
