/**
 * 328P 3.3v ProMini -- working w/ arduino IDE
 * 
 * Xtase - fgalliat @Sept2019
 * 
 * YAEL SubMCU
 * 
 * 30 KB Sktech
 * 2 KB RAM
 *
 *
 * KeybLCD#1
 * 
 * 328 pinout
 * 
 * 
 */

//====================================================================================
//                                    Settings
//====================================================================================

//====================================================================================
//                                    LCD 20x4
//====================================================================================
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

void setupLCD() {
  lcd.begin(20,4);

  lcd.setBacklight(LOW);
  lcd.clear();
//   lcd.home ();                   // go home
  lcd.setCursor ( 0, 0 );
}

//====================================================================================
//                                    Keyboard
//====================================================================================
#include "Keypad_MC17.h"
#include <Keypad.h>        // from Arduino's libs
#include <Wire.h>          // from Arduino's libs

#define KEYB_I2CADDR 0x20

const byte K_ROWS = 8; // eight rows
const byte K_COLS = 8; // eight columns
//define the cymbols on the buttons of the keypads
char hexaKeys[K_ROWS][K_COLS] = {
  {'0', '1', '2', '3', '4', '5', '6', '7' }, // 1
  {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' }, // 2 
  {'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p' }, // 3
  {'q', 'r', 's', 't', 'u', 'v', 'w', 'x' }, // 4
  {'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F' }, // 5
  {'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N' }, // 6
  {'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V' }, // 7
  {'W', 'X', 'Y', 'Z', '&', '#', '(', ')' }, // 8
};
byte rowPins[K_ROWS] = {7, 6, 5, 4, 3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[K_COLS] = {15, 14, 13, 12, 11, 10, 9, 8}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad_MC17 customKeypad( makeKeymap(hexaKeys), rowPins, colPins, K_ROWS, K_COLS, KEYB_I2CADDR ); 

void setupKeyb() {
    customKeypad.begin();
}

void pollKeyb() {
  char customKey = customKeypad.getKey();
  
  if (customKey != NO_KEY){
    Serial.println(customKey);
    lcd.setCursor(16,3);
    lcd.print( (char)customKey );
  }
}


//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  // Serial.begin(115200); // Used for messages and the C array generator
  Serial.begin(9600); // Used for messages and the C array generator

  setupLCD();
  setupKeyb();

  // aux screen
  lcd.setCursor(0,0);
  lcd.print("== Xtase @Aug2019 ==");
  lcd.setCursor(0,1);
  //         12345678901234567890
  lcd.print("OrdiSavant new YATL");
  lcd.setCursor(0,2);
  lcd.print("Layout...");
  lcd.setCursor(0,3);
  lcd.print("Have fun !");

}

//====================================================================================
//                                    Loop
//====================================================================================
int numImg = 0;

#define SAMPLE_LEN 100
long times[SAMPLE_LEN];
long maxT = 0;
long minT = 10000;
long t0,t1,t,avg;

int loopCpt = -1;

void loop()
{
  if ( loopCpt == -1 ) {
    // first time
    loopCpt = 0;
    lcd.clear();
  }

  t0 = millis();
  pollKeyb();
  t1 = millis();
  t = t1 - t0;
  if ( t > maxT ) { maxT = t; }
  if ( t < minT ) { minT = t; }
  times[ loopCpt ] = t;

  loopCpt++;
  if ( loopCpt >= SAMPLE_LEN ) {
    loopCpt = 0;
    avg = 0;
    for(int i=0; i < SAMPLE_LEN; i++) {
      avg += times[i];
    }
    avg = (long) ((double)avg / (double)SAMPLE_LEN);
  }
  char msg[20+1]; memset(msg, 0x00, 20+1);

  lcd.home(); memset(msg, 0x20, 20);
  sprintf(msg, "AVG:%lu", avg); // %ld -> long signed / %lu long unsigned
  lcd.print(msg); lcd.setCursor(0, 1); memset(msg, 0x20, 20);
  sprintf(msg, "MIN:%lu", minT);
  lcd.print(msg); lcd.setCursor(0, 2);  memset(msg, 0x20, 20);
  sprintf(msg, "MAX:%lu", maxT);
  lcd.print(msg); lcd.setCursor(0, 3);  memset(msg, 0x20, 20);

  minT = 10000;
  maxT = 0;
}

//====================================================================================

