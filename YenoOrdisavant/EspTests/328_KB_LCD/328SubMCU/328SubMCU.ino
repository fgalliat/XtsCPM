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
#include <SoftwareSerial.h>  // http://arduino.cc/en/Reference/softwareSerial

#define SF_RX  10  // Rx Pin
#define SF_TX  11  // Tx Pin
SoftwareSerial bridge(SF_RX, SF_TX);  // (Rx, Tx) 

#define LED_PIN 13


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
  // lcd.home ();                   // go home
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

int pollKeyb() {
  char customKey = customKeypad.getKey();
  
  if (customKey != NO_KEY){
    // Serial.println(customKey);
    // lcd.setCursor(16,3);
    // lcd.print( (char)customKey );
    return customKey;
  }

  return -1;
}


//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);


  // Serial.begin(115200); // Used for messages and the C array generator
  Serial.begin(9600); // Used for messages and the C array generator

  bridge.begin(9600);
  bridge.listen();

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

// ===========================================
// ============ Serial Port Routines =========
#define PORT_NONE 0
#define PORT_HARD 1
#define PORT_SOFT 2
char line[20+1];

int serReadLine(int port) {
  memset(line, 0x00, 20+1);
  if ( port == PORT_HARD ) { return Serial.readBytesUntil('\n', line, 20); }
  return bridge.readBytesUntil('\n', line, 20);
}

int serRead(int port) {
  if ( port == PORT_HARD ) { return Serial.read(); }
  return bridge.read();
}

int serWrite(int port, char ch) {
  if ( port == PORT_HARD ) { return Serial.write( (int)ch ); }
  return bridge.write( (int)ch );
}

int serWrite(int port, char* chs) {
  if ( port == PORT_HARD ) { Serial.print( chs ); Serial.write( (uint8_t)0x00 ); }
  bridge.print( chs ); bridge.write( (uint8_t)0x00 );
}

int probePort() {
  if ( Serial.available() > 0 ) { return PORT_HARD; }
  if ( bridge.available() > 0 ) { return PORT_SOFT; }
  return PORT_NONE;
}

// ===========================================
// ==== Keyboard buffer routines =============
#define KEYB_BUFF_LEN 16

char kBuff[KEYB_BUFF_LEN+1] = {
  // 0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16/0
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// ===========================================

void loop()
{
  if ( loopCpt == -1 ) {
    // first time
    loopCpt = 0;
    lcd.clear(); lcd.home();
    digitalWrite(LED_PIN, LOW);
  }


  int port = probePort();


  if ( port != PORT_NONE ) {
    char chr = serRead(port);

    // == LCD Control ==
    if ( chr == 'C' ) {
      // cls
      lcd.clear(); lcd.home();

      return; // skip key reading ?
    } else if ( chr == 'P' ) {
      // print
      int len = serReadLine(port);
      lcd.print(line);

      return; // ...
    } else if ( chr == 'c' ) {
      // setCursor(x,y)
      int x = serRead(port); // [0..3]
      int y = serRead(port); // [0..19]
      lcd.setCursor(x,y);

      return; // ...
    }
    // == KEYB Control ==
    else if ( chr == 'K' ) {
      // clear Key buffer
      memset( kBuff, 0x00, KEYB_BUFF_LEN+1 );

      return; // ...
    } else if ( chr == 'k' ) {
      // return kBuff content
      serWrite( port, kBuff );
      memset( kBuff, 0x00, KEYB_BUFF_LEN+1 );

      return; // ...
    }
    // == LED Control ==
    else if ( chr == 'L' ) {
      digitalWrite(LED_PIN, HIGH);

      return;
    }
    else if ( chr == 'l' ) {
      digitalWrite(LED_PIN, LOW);

      return;
    }

  }

  int currentBufferLen = strlen( kBuff );
  if ( currentBufferLen >= KEYB_BUFF_LEN ) {
    // Keyboard buffer overflow
    // stop here ...
    return;
  }

  // ~56msec
  int k = pollKeyb();
  if ( k == -1 ) { return; }

  kBuff[ currentBufferLen ] = (char)k;

  // t0 = millis();
  // pollKeyb();
  // t1 = millis();
  // t = t1 - t0;
  // if ( t > maxT ) { maxT = t; }
  // if ( t < minT ) { minT = t; }
  // times[ loopCpt ] = t;

  // loopCpt++;
  // if ( loopCpt >= SAMPLE_LEN ) {
  //   loopCpt = 0;
  //   avg = 0;
  //   for(int i=0; i < SAMPLE_LEN; i++) {
  //     avg += times[i];
  //   }
  //   avg = (long) ((double)avg / (double)SAMPLE_LEN);
  // }
  // char msg[20+1]; memset(msg, 0x00, 20+1);

  // lcd.home(); memset(msg, 0x20, 20);
  // sprintf(msg, "AVG:%lu", avg); // %ld -> long signed / %lu long unsigned
  // lcd.print(msg); lcd.setCursor(0, 1); memset(msg, 0x20, 20);
  // sprintf(msg, "MIN:%lu", minT);
  // lcd.print(msg); lcd.setCursor(0, 2);  memset(msg, 0x20, 20);
  // sprintf(msg, "MAX:%lu", maxT);
  // lcd.print(msg); lcd.setCursor(0, 3);  memset(msg, 0x20, 20);

  // minT = 10000;
  // maxT = 0;
}

//====================================================================================

