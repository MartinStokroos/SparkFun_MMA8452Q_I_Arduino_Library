/*
  Library for the MMA8452Q
  By: Jim Lindblom and Andrea DeVore
  SparkFun Electronics

  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/14587

  This sketch uses the SparkFun_MMA8452Q library to initialize
  the accelerometer and stream raw x, y, z, acceleration
  values from it.

  Hardware hookup:
  Arduino --------------- MMA8452Q Breakout
    3.3V  ---------------     3.3V
    GND   ---------------     GND
  SDA (A4) --\/330 Ohm\/--    SDA
  SCL (A5) --\/330 Ohm\/--    SCL

  The MMA8452Q is a 3.3V max sensor, so you'll need to do some
  level-shifting between the Arduino and the breakout. Series
  resistors on the SDA and SCL lines should do the trick.

  License: This code is public domain, but if you see me
  (or any other SparkFun employee) at the local, and you've
  found our code helpful, please buy us a round (Beerware
  license).

  Distributed as is; no warranty given.

  ########################################################
  
  Modified hardware hookup for using the data ready interrupt by M. Stokroos, November 18, 2022

  Hardware hookup:
  Arduino --------------- MMA8452Q Breakout
    3.3V  ---------------     3.3V
    GND   ---------------     GND
  SDA (A4) --\/330 Ohm\/--    SDA
  SCL (A5) --\/330 Ohm\/--    SCL
  INT (D2) ---------------    INT1

*/


#include "Arduino.h"
#include <Wire.h>                 // Must include Wire library for I2C
#include "SparkFun_MMA8452Q.h"    // Click here to get the library: http://librarymanager/All#SparkFun_MMA8452Q

const byte interruptPin = 2;
bool drdy = false;
short ax, ay, az;

MMA8452Q accel;                   // create instance of the MMA8452 class

void setup() {
  Serial.begin(115200);
  //Serial.println("MMA8452Q Raw Data Reading Code!"); // This string messes up the Arduino plotter
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), dataRdy, FALLING);
  Wire.begin();

  if (accel.begin() == false) {
    Serial.println("Not Connected. Please check connections and read the hookup guide.");
    while (1);
  }

  /* Default scale is ±2g (full-scale range)
     Set scale using SCALE_2G, SCALE_4G, SCALE_8G
     Sets scale to ±2g, 4g, or 8g respectively
  */
  accel.setScale(SCALE_2G);

  /* Default output data rate (ODR) is 800 Hz (fastest)
     Set data rate using ODR_800, ODR_400, ODR_200,
     ODR_100, ODR_50, ODR_12, ODR_6, ODR_1
     Sets data rate to 800, 400, 200, 100, 50, 12.5,
     6.25, or 1.56 Hz respectively
     See data sheet for relationship between voltage
     and ODR (pg. 7)
https://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/MMA8452Q-rev8.1.pdf */
  accel.setDataRate(ODR_100);

  /* enable Interrupt output for Data Ready on pin INT1 or INT2 */
  accel.enDataIrq(INT_1);

  /* Set to the lowest cutoff frequency */
  accel.setFilter(HPASS_0);
}

void loop() {
    if ( drdy && accel.dataIrq() ) {    // Wait for INT from accelerometer. Checking dataIrq will hold the program for a short time,
                                        // but is needed to make sure the INT was triggered by INT_DRDY. The check on dataIrq
                                        // could be left out if INT_DRDY is the only active interrupt.
    
    digitalWrite(LED_BUILTIN, true);    // For debugging purpose. Check on loop overrun at the highest data rates.
    // Raw of acceleration of x, y, and z directions
    ax = accel.getX(); // Must read x,y,z to clear the interrupt.
    Serial.print(ax);
    Serial.print("\t");
    ay = accel.getY();
    Serial.print(ay);
    Serial.print("\t");
    az = accel.getZ();
    Serial.print(az); // Can print only one channel a time at ODR_400 / 115200Bd.
    Serial.println();
    drdy = false; // Clear interrupt flag
    digitalWrite(LED_BUILTIN, false);
    }
}


void dataRdy() {
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED for debugging purpose.
    drdy = true;
}


