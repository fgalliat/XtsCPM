
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


//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  // Serial.begin(115200); // Used for messages and the C array generator
  Serial.begin(9600); // Used for messages and the C array generator

  // Initialise the SD library before the TFT so the chip select gets set
  if (!SD.begin(SD_CS)) {
    Serial.println("Initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");

  // Now initialise the TFT
  tft.begin();
  tft.setRotation(2);  // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_BLACK);
}

//====================================================================================
//                                    Loop
//====================================================================================
int numImg = 0;
void loop()
{
  tft.fillScreen(random(0xFFFF));
  // drawBmp("/parrot.bmp", 0, 0);//, 16);
  // drawBmp("/z/0/girl.bmp", 0, 0);

  // "/z/0/game1-1.pak"
  tft.setRotation(1);
  drawImgFromPAK("/z/0/game1-1.pak", 0, 0, numImg);

  if ( numImg++ >= 5 ) { numImg=0; } 

  delay(4000);
}
//====================================================================================

