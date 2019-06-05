// just for devl purposes
#define ARDUINO 1


#ifdef ARDUINO
 /**
 * Xtase - fgalliat @Jun2019
 * 
 * implementaion of Yatl hardware API (Arduino Version)
 * Screen over SPI impl.
 * 
 */

  #include "Arduino.h"

  #include "xts_yatl_settings.h"
  #include "xts_yatl_api.h"
  extern Yatl yatl;

    #include "SPI.h"
    #include "ILI9341_t3n.h"
    // w/ that lib BOTH CS & DC needs to besome CS pins
    #define _TFT_DC  20
    #define _TFT_CS  15
    // #define TFT_SCK 14 // doesn't work !!!!
    #define TFT_SCK 13

    #define TFT_MOSI 11
    #define TFT_MISO 12
    #define TFT_RST  17
    #define TFT_LED  16

    ILI9341_t3n tft = ILI9341_t3n(_TFT_CS, _TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO);

    // in pixels
    #define TFT_WIDTH 320
    #define TFT_HEIGHT 240

    #define TFT_ROTATION 1


  bool YatlScreen::setup() {
      // ensure TFT backlight
      pinMode(TFT_LED, OUTPUT);
      digitalWrite(TFT_LED, HIGH);    

      tft.begin();
      tft.setRotation(TFT_ROTATION);

      tft.fillScreen(ILI9341_BLACK);
      tft.setTextSize(1);
      tft.setTextColor(ILI9341_WHITE);

      tft.println("Teensy 3.6 -YATL- Booting");

      return true;
  }

#endif