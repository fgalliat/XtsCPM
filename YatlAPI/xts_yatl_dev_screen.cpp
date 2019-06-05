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

    #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
    extern SdFatSdio SD;


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

  void drawBmp(char* filename, bool screenRotate=true);

  void YatlScreen::drawWallpaper(char* assetName) {
      char* fileName = this->yatl->getFS()->getAssetsFileEntry( assetName );
      drawBmp(fileName, true);
  }

  void YatlScreen::write(char ch) {
      // TODO : console emulation
      tft.write(ch);
  }

  void YatlScreen::drawTextBox(const char* title, const char* msg) {
      tft.fillRect( 20, 20, TFT_WIDTH-40, TFT_HEIGHT-40, ILI9341_RED );
      tft.drawRect( 20, 20, TFT_WIDTH-40, TFT_HEIGHT-40, ILI9341_WHITE );
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor( 40, 30 );
      tft.print( title );
      tft.setCursor( 40, 30+30 );
      tft.print( msg );
  }

  // ==========] Draw Bmp Routines [==========
   void _drawBmp(char* filename, int x, int y);

   void drawBmp(char* filename, bool screenRotate) {
      if (TFT_ROTATION == 3) {
         if ( screenRotate ) tft.setRotation(0);
      } else {
         if ( screenRotate ) tft.setRotation(2);
      }
      _drawBmp( filename, 0, 0 );
      if ( screenRotate ) tft.setRotation(TFT_ROTATION);
   }

   // TODO : move away
   uint16_t read16(File &f) {
      uint16_t result;
      ((uint8_t *)&result)[0] = f.read(); // LSB
      ((uint8_t *)&result)[1] = f.read(); // MSB
      return result;
   }

   uint32_t read32(File &f) {
      uint32_t result;
      ((uint8_t *)&result)[0] = f.read(); // LSB
      ((uint8_t *)&result)[1] = f.read();
      ((uint8_t *)&result)[2] = f.read();
      ((uint8_t *)&result)[3] = f.read(); // MSB
      return result;
   }


   #define BUFFPIXEL 80

   void _drawBmp(char* filename, int x, int y) {
      if ( filename == NULL || strlen(filename) <= 0 || strlen(filename) >= 32 ) {
         yatl.dbug("(WW) Wrong BMP filename !");
         return;
      }
 
      File bmpFile;
      int bmpWidth, bmpHeight;             // W+H in pixels
      uint8_t bmpDepth;                    // Bit depth (currently must be 24)
      uint32_t bmpImageoffset;             // Start of image data in file
      uint32_t rowSize;                    // Not always = bmpWidth; may have padding
      uint8_t sdbuffer[3 * BUFFPIXEL];     // pixel buffer (R+G+B per pixel)
      uint16_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
      boolean goodBmp = false;             // Set to true on valid header parse
      boolean flip = true;                 // BMP is stored bottom-to-top
      int w, h, row, col;
      uint8_t r, g, b;
      uint32_t pos = 0, startTime = millis();

      uint16_t awColors[320]; // hold colors for one row at a time...

      if ((x >= tft.width()) || (y >= tft.height()))
         return;

      // Serial.println();
      // Serial.print(F("Loading image '"));
      // Serial.print(filename);
      // Serial.println('\'');

      // Open requested file on SD card
      if (!(bmpFile = SD.open(filename)))
      {
         yatl.dbug("File not found");
         return;
      }

      // Parse BMP header
      if (read16(bmpFile) == 0x4D42) { // BMP signature
         // Serial.print(F("File size: "));

         // need to be done @least once
         // Serial.println(read32(bmpFile));
         (void)read32(bmpFile);

         (void)read32(bmpFile);            // Read & ignore creator bytes
         bmpImageoffset = read32(bmpFile); // Start of image data
         // Serial.print(F("Image Offset: "));
         // Serial.println(bmpImageoffset, DEC);
         // Read DIB header
         // Serial.print(F("Header size: "));

         // need to be done @least once
         // Serial.println(read32(bmpFile));
         (void)read32(bmpFile);

         bmpWidth = read32(bmpFile);
         bmpHeight = read32(bmpFile);
         if (read16(bmpFile) == 1)     { // # planes -- must be '1'
            bmpDepth = read16(bmpFile); // bits per pixel
            // Serial.print(F("Bit Depth: "));
            // Serial.println(bmpDepth);
            if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
            goodBmp = true; // Supported BMP format -- proceed!
            // Serial.print(F("Image size: "));
            // Serial.print(bmpWidth);
            // Serial.print('x');
            // Serial.println(bmpHeight);

            // BMP rows are padded (if needed) to 4-byte boundary
            rowSize = (bmpWidth * 3 + 3) & ~3;

            // If bmpHeight is negative, image is in top-down order.
            // This is not canon but has been observed in the wild.
            if (bmpHeight < 0) {
               bmpHeight = -bmpHeight;
               flip = false;
            }

            // Crop area to be loaded
            w = bmpWidth;
            h = bmpHeight;
            if ((x + w - 1) >= tft.width())
               w = tft.width() - x;
            if ((y + h - 1) >= tft.height())
               h = tft.height() - y;

            for (row = 0; row < h; row++) { // For each scanline...

               // Seek to start of scan line.  It might seem labor-
               // intensive to be doing this on every line, but this
               // method covers a lot of gritty details like cropping
               // and scanline padding.  Also, the seek only takes
               // place if the file position actually needs to change
               // (avoids a lot of cluster math in SD library).
               if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
                  pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
               else // Bitmap is stored top-to-bottom
                  pos = bmpImageoffset + row * rowSize;
               if (bmpFile.position() != pos) { // Need seek?
                  bmpFile.seek(pos);
                  buffidx = sizeof(sdbuffer); // Force buffer reload
               }

               for (col = 0; col < w; col++) { // For each pixel...
                  // Time to read more pixel data?
                  if (buffidx >= sizeof(sdbuffer)) { // Indeed
                  bmpFile.read(sdbuffer, sizeof(sdbuffer));
                  buffidx = 0; // Set index to beginning
                  }

                  // Convert pixel from BMP to TFT format, push to display
                  b = sdbuffer[buffidx++];
                  g = sdbuffer[buffidx++];
                  r = sdbuffer[buffidx++];
                  awColors[col] = tft.color565(r, g, b);
               } // end pixel
               tft.writeRect(0, row, w, 1, awColors);
            } // end scanline
            long timeElapsed = millis() - startTime;
            // Serial.print(F("Loaded in "));
            // Serial.print(timeElapsed);
            // Serial.println(" ms");
            } // end goodBmp
         }
      }

      bmpFile.close();
      if (!goodBmp) {
         yatl.dbug("BMP format not recognized.");
      }

   }

#endif