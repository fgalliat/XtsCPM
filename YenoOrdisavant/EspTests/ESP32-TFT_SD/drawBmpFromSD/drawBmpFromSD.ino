/**
 * ESP32 working w/ arduino IDE
 * 
 * Additional board url : https://dl.espressif.com/dl/package_esp32_index.json
 * goto Board Manager
 * choose ESP32 by espressif
 * 
 * DOIT ESP32 DEVKIT V1
 * 
 * 1.3 MB Sktech
 * 320 KB RAM
 * 
 */

#include <SPI.h>

#include <TFT_eSPI.h>

#include "FS.h"
#include "SD.h"
#define SDSPEED 27000000


// ============ SPI ===================

// Cf TFT_eSPI wiring
#define OWN_SPI_CS   5
#define OWN_SPI_DC   15
#define OWN_SPI_MOSI 23
#define OWN_SPI_CLK  18
#define OWN_SPI_MISO 19

#define TFT_CS   OWN_SPI_CS
#define SDC_CS 4

SPIClass spiSD(HSPI);

void setupSPI() {
  pinMode(TFT_CS, OUTPUT);
  pinMode(SDC_CS, OUTPUT);

  // disable all devices
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(SDC_CS, HIGH);

  spiSD.begin(OWN_SPI_CLK,OWN_SPI_MISO,OWN_SPI_MOSI,OWN_SPI_DC);//SCK,MISO,MOSI,ss
}

void disableScreenSPI() {
      digitalWrite(TFT_CS, HIGH);
      digitalWrite(SDC_CS, LOW);
}

void enableScreenSPI() {
      digitalWrite(TFT_CS, LOW);
      digitalWrite(SDC_CS, HIGH);
}

void enableSdCardSPI() {
      disableScreenSPI();
}

void disableSdCardSPI() {
      enableScreenSPI();
}

// ======== Devices ===================

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

// ======== Software ===================

// "/Z/0/GIRL.BMP"
void drawBmp(char* path);

void readFile(fs::FS &fs, const char * path);

void setup() {
  //Serial.begin(115200);
  Serial.begin(9600);

  Serial.println("wait a char...");
    while( Serial.available() <= 0 ) {
        delay(150);
    }
    Serial.read();

  Serial.println("init SPI");
    setupSPI();

  Serial.println("init SdCard");
    // w. TFT_eSPI : must be init before
    enableSdCardSPI();
    if(!SD.begin( SDC_CS, spiSD, SDSPEED)){
        Serial.println("Card Mount Failed");
        return;
    }
    
    Serial.println("Card Mount OK");


  Serial.println("init Screen");
    enableScreenSPI();
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

    delay(100); // mandatory for endOfScreen/beginOfSd


    enableSdCardSPI();
    Serial.println("let's paint something");
    // drawBmp("/Z/0/GIRL.BMP");
    // readFile(SD, "/foo.txt");
    listDir(SD, "/", 0);

}

void loop()
{

  delay(150);

}

// =========== FileSystem support ====

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}


void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

// =========== Bmp File Support ======

  #define BUFFPIXEL 80

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

void _drawBmp(char* fn, int x, int y);
void drawBmp(char* path) {
   bool screenRotate = true;
   if ( screenRotate ) { tft.setRotation(0); }  
   _drawBmp(path, 0, 0);
   if ( screenRotate ) { tft.setRotation(1); }  
}

   void _drawBmp(char* filename, int x, int y) {
      if ( filename == NULL || strlen(filename) <= 0 || strlen(filename) >= 32 ) {
         Serial.println("(WW) Wrong BMP filename !");
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
      uint32_t pos = 0;//, startTime = millis();

      uint16_t awColors[320]; // hold colors for one row at a time...

      if ((x >= tft.width()) || (y >= tft.height()))
         return;

      // Serial.println();
      // Serial.print(F("Loading image '"));
      // Serial.print(filename);
      // Serial.println('\'');

      enableSdCardSPI();

      // Open requested file on SD card
      if (!(bmpFile = SD.open(filename)))
      {
         Serial.println("File not found");
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
               
               // TFT_eSPI specific
            //    tft.writeRect(0, row, w, 1, awColors);
               enableScreenSPI();
               tft.pushRect(0, row, w, 1, awColors);
               enableSdCardSPI();

            } // end scanline
            // long timeElapsed = millis() - startTime;
            // Serial.print(F("Loaded in "));
            // Serial.print(timeElapsed);
            // Serial.println(" ms");
            } // end goodBmp
         }
      }

      bmpFile.close();
      if (!goodBmp) {
         Serial.println("BMP format not recognized.");
      }

   }





