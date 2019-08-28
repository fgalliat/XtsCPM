/**
 * 
 * MCP23017 8x8 OrdiSavant Keyboard
 * 
 * Xtase - fgalliat @ Aug 2019
 * 
 */


/* file CustomKeypad_MC17 Feb 2/13
||@file CustomKeypad.pde
|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates changing the keypad size and key values.
|| #
    Use with I2C i/o G. D. (Joe) Young Feb 28/12

    Use with MCP23008 I2C i/o G. D. (Joe) Young Jul 29/12
    Use with MCP23016 I2C i/o G. D. (Joe) Young Feb 2/13
    Use with MCP23017 I2C i/o G. D. (Joe) Young May 19/14
*/
#include "Keypad_MC17.h"
#include <Keypad.h>        // from Arduino's libs
#include <Wire.h>          // from Arduino's libs

// #define I2CADDR 0x24
#define I2CADDR 0x20

const byte ROWS = 8; // eight rows
const byte COLS = 8; // eight columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'0','1','2','3','4', '5', '6', '7' }, // 1
  {'a','b','c','d','e', 'f', 'g', 'h' }, // 2 
  {'i','j','k','l','m', 'n', 'o', 'p' }, // 3
  {'q','r','s','t','u', 'v', 'w', 'x' }, // 4
  {'y','z','A','B','C', 'D', 'E', 'F' }, // 5
  {'G','H','I','J','K', 'L', 'M', 'N' }, // 6
  {'O','P','Q','R','S', 'T', 'U', 'V' }, // 7
  {'W','X','Y','Z','&', '#', '(', ')' }, // 8
};
byte rowPins[ROWS] = {7, 6, 5, 4, 3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {15, 14, 13, 12, 11, 10, 9, 8}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad_MC17 customKeypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS, I2CADDR); 

void setup(){
//  Wire.begin( );
  customKeypad.begin( );        // GDY120705
  Serial.begin(9600);
}
  
void loop(){
  char customKey = customKeypad.getKey();
  
  if (customKey != NO_KEY){
    Serial.println(customKey);
  }
}
