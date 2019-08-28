/**
 * FullDemo#1
 * 
 * ESP32 + ILI9341 Screen + SDCard + MCP23017 Keyboard decoder
 * 
 */


//====================================================================================
//                                  Libraries
//====================================================================================

#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Call up the TFT library
#include <TFT_eSPI.h> // Hardware-specific library for ESP8266

// Invoke TFT library this will set the TFT chip select high
TFT_eSPI tft = TFT_eSPI();


// ==== Wiring =====
#define OWN_SPI_CS   5
#define OWN_SPI_DC   15
#define OWN_SPI_MOSI 23
#define OWN_SPI_CLK  18
#define OWN_SPI_MISO 19

#define TFT_CS OWN_SPI_CS
#define SD_CS 4 // SD chip select

#include "drawPAK.h"
#include "drawBMP.h"

#define DEFAULT_ROTATION 1

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
  {'0','1','2','3','4', '5', '6', '7' }, // 1
  {'a','b','c','d','e', 'f', 'g', 'h' }, // 2 
  {'i','j','k','l','m', 'n', 'o', 'p' }, // 3
  {'q','r','s','t','u', 'v', 'w', 'x' }, // 4
  {'y','z','A','B','C', 'D', 'E', 'F' }, // 5
  {'G','H','I','J','K', 'L', 'M', 'N' }, // 6
  {'O','P','Q','R','S', 'T', 'U', 'V' }, // 7
  {'W','X','Y','Z','&', '#', '(', ')' }, // 8
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
    tft.print( (char)customKey );
  }
}

//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  // Serial.begin(115200); // Used for messages and the C array generator
  Serial.begin(9600); // Used for messages and the C array generator

  setupKeyb();

  // Initialise the SD library before the TFT so the chip select gets set
  if (!SD.begin(SD_CS)) {
    Serial.println("Initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");

  // Now initialise the TFT
  tft.begin();
  tft.setRotation(DEFAULT_ROTATION);  // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_BLACK);

  // Draw Wallpaper
  tft.setRotation(2);
  tft.fillScreen(random(0xFFFF));
  drawBmp("/Z/0/GIRL.BMP", 0, 0);
  tft.setRotation(DEFAULT_ROTATION);

  tft.println("Screen OK");
  tft.println("SDCard OK");
  tft.println("Checking Keyboard : ");

}

//====================================================================================
//                                    Loop
//====================================================================================
int numImg = 0;
void loop()
{
  // tft.fillScreen(random(0xFFFF));
  // // drawBmp("/parrot.bmp", 0, 0);//, 16);
  // drawBmp("/Z/0/GIRL.BMP", 0, 0);

  // tft.setRotation(1);
  // drawImgFromPAK("/z/0/game1-1.pak", 0, 0, numImg);

  // if ( numImg++ >= 5 ) { numImg=0; } 
  // delay(4000);

  pollKeyb();
}
//====================================================================================

