/**
 * Xtase - fgalliat @May 2019
 * 
 * Vtech Mobigo I - Keyboard
 * Control w/ an SX1509
 * 
 * @ this time : only the key [ ? ] is missing....
 * 
 * Arduino UNO
 * SDA : A4
 * SCL : A5
 */

#include <Wire.h> // Include the I2C library (required)
#include <SparkFunSX1509.h> // Include SX1509 library

// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte SX1509_ADDRESS = 0x3E;  // SX1509 I2C address
SX1509 io; // Create an SX1509 object to be used throughout

const byte ARDUINO_INTERRUPT_PIN = 2;

  // ========================================
  #include "MobigoKeyboard.h"
  MobigoKeyboard kbd(&io, true);

void setup() 
{
  Serial.begin(9600);
  if (!io.begin(SX1509_ADDRESS))
  {
    Serial.println("Failed to communicate.");
    while (1) ; // If we fail to communicate, loop forever.
  }
  delay(300);

  kbd.setup();

  // Set up the Arduino interrupt pin as an input w/ 
  // internal pull-up. (The SX1509 interrupt is active-low.)
  pinMode(ARDUINO_INTERRUPT_PIN, INPUT_PULLUP);
}

void loop() 
{
  // kbd.poll();

  while ( kbd.available() > 0 ) {
    Serial.print( (char)kbd.read() );
  }


//   // If the SX1509 INT pin goes low, a keypad button has
//   // been pressed:
//   if (digitalRead(ARDUINO_INTERRUPT_PIN) == LOW)

  delay(1);  // Gives releaseCountMax a more intuitive unit
}
