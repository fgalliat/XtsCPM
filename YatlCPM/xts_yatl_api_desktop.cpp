// just for devl purposes
// #define DESKTOP 1


#ifndef ARDUINO
 /**
 * Xtase - fgalliat @Jul2019
 * 
 * implementation of Yatl hardware API (Desktop Version)
 * 
 * 
 */

    #warning "-= Yatl API Desktop Mode =-"
    #ifdef ARDUINO
    #include "Arduino.h"
    #else 
    #include "Desktop.h"
    #endif

    // #include "xts_string.h"

    uint16_t rgb(uint8_t r,uint8_t g,uint8_t b) { return (uint16_t)( (( r *31/255 )<<11) | (( g *63/255 )<<5) | ( b *31/255 ) );}

    // @@@@@@@@@@@@@@@@@@
    #include "../arch/desktop/screen/WiredScreen.h"
    WiredScreen sdlScreen;
    // @@@@@@@@@@@@@@@@@@

    #include "xts_yatl_settings.h"
    #include "xts_yatl_api.h"

    #include "xts_yatl_dev_console.h"

    // ::::::::::::::::::::::::::::::::::::::
    bool keybLocked = false;

    void YatlBuzzer::beep(int freq, int dur) {
        printf("beep(x,y)\n");
    }

    void YatlBuzzer::playTuneString(const char* str) {
        printf("playTuneString(...)\n");
    }

    bool YatlBuzzer::playTuneFile(const char* file) {
        printf("playTuneFile(...)\n");
        return true;
    }

    void YatlBuzzer::setup() {
    }

    bool YatlScreen::setup() {
        bool ok = sdlScreen.init();
        if ( !ok ) { return ok; }
        _setConsoleMode(LCD_CONSOLE_DEF_COLS);
        return ok;
    }

    bool YatlKeyboard::setup() {
        return false;
    }

    // ============== Draw routines ==================

    void YatlScreen::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
      sdlScreen.drawRect(x, y, w, h, MODE_DRAW, color);
    }
    void YatlScreen::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
      sdlScreen.drawRect(x, y, w, h, MODE_FILL, color);
    }
    void YatlScreen::drawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color) {
      sdlScreen.drawCircle(x, y, r, MODE_DRAW, color);
    }
    void YatlScreen::fillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color) {
      sdlScreen.drawCircle(x, y, r, MODE_FILL, color);
    }

    void YatlScreen::drawLine(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t color) {
      sdlScreen.drawLine(x, y, x2, y2, color);
    }

  void YatlScreen::drawWallpaper(char* assetName) {
      char* fileName = this->yatl->getFS()->getAssetsFileEntry( assetName );
      drawBmp(fileName, true);
  }

  void YatlScreen::cleanSprites() { 
     //::cleanSprites(); 
     this->yatl->dbug("cleanSprites NYI");
  }
  void YatlScreen::grabbSpritesOfSize(char* imageName, int offsetX, int offsetY, int width, int height) {
     //::grabbSpritesOfSize(imageName, offsetX, offsetY, width, height);
     this->yatl->dbug("grabbSpritesOfSize NYI");
  }
  void YatlScreen::grabbSprites(char* imageName, int offsetX, int offsetY) {
     //::grabbSprites(imageName, offsetX, offsetY);
     this->yatl->dbug("grabbSprites NYI");
  }


    // ============== Image routines ==================

// filename is "/Z/0/packXXX.pak"
   // reads&display image #numInPak of packed image from filename
   void drawImgFromPAK(char* filename, int x, int y, int numInPak) {
       yatl.dbug("drawImgFromPAK NYI");
   }

    #define BUFFPIXEL 80

   // TODO : move away
   // uint16_t read16(FILE &f) {
   uint16_t read16(FILE* f) {
      uint16_t result;
    //   ((uint8_t *)&result)[0] = f.read(); // LSB
      ((uint8_t *)&result)[0] = fgetc(f); // LSB
      ((uint8_t *)&result)[1] = fgetc(f); // MSB
      return result;
   }

   uint32_t read32(FILE* f) {
      uint32_t result;
      ((uint8_t *)&result)[0] = fgetc(f); // LSB
      ((uint8_t *)&result)[1] = fgetc(f);
      ((uint8_t *)&result)[2] = fgetc(f);
      ((uint8_t *)&result)[3] = fgetc(f); // MSB
      return result;
   }

    void _screenRotate(bool portrait) {
        sdlScreen.screenRotate(portrait);
    }

      void _drawBmp(char* filename, int x, int y) {
          if ( filename == NULL || strlen(filename) <= 0 || strlen(filename) >= 32 ) {
         yatl.dbug("(WW) Wrong BMP filename !");
         return;
      }
 
      //File bmpFile;
      FILE* bmpFile;

      int bmpWidth, bmpHeight;             // W+H in pixels
      uint8_t bmpDepth;                    // Bit depth (currently must be 24)
      uint32_t bmpImageoffset;             // Start of image data in file
      uint32_t rowSize;                    // Not always = bmpWidth; may have padding
      uint8_t sdbuffer[3 * BUFFPIXEL];     // pixel buffer (R+G+B per pixel)
      uint16_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
      bool goodBmp = false;             // Set to true on valid header parse
      bool flip = true;                 // BMP is stored bottom-to-top
      int w, h, row, col;
      uint8_t r, g, b;
      uint32_t pos = 0;//, startTime = millis();

      uint16_t awColors[320]; // hold colors for one row at a time...

      if ((x >= SCREEN_WIDTH) || (y >= SCREEN_HEIGHT)) {
          yatl.dbug("COORDS TOO BIG FOR SCREEN");
         return;
      }

      // Serial.println();
      // Serial.print(F("Loading image '"));
      // Serial.print(filename);
      // Serial.println('\'');

      // Open requested file on SD card
      // if (!(bmpFile = SD.open(filename)))
      if (!(bmpFile = fopen(filename, "r")))
      {
         yatl.dbug("File not found : ");
         yatl.dbug(filename);
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
            if ((x + w - 1) >= SCREEN_WIDTH)
               w = SCREEN_WIDTH - x;
            if ((y + h - 1) >= SCREEN_HEIGHT)
               h = SCREEN_HEIGHT - y;

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
            //    if (bmpFile.position() != pos) { // Need seek?
               if ( ftell(bmpFile) != pos) { // Need seek?
                  //bmpFile.seek(pos);
                  fseek(bmpFile, pos, SEEK_SET);
                  buffidx = sizeof(sdbuffer); // Force buffer reload
               }

               for (col = 0; col < w; col++) { // For each pixel...
                  // Time to read more pixel data?
                  if (buffidx >= sizeof(sdbuffer)) { // Indeed
                    //bmpFile.read(sdbuffer, sizeof(sdbuffer));
                    fread(sdbuffer, 1, sizeof(sdbuffer), bmpFile);
                    buffidx = 0; // Set index to beginning
                  }

                  // Convert pixel from BMP to TFT format, push to display
                  b = sdbuffer[buffidx++];
                  g = sdbuffer[buffidx++];
                  r = sdbuffer[buffidx++];
                  awColors[col] = rgb(r, g, b);
               } // end pixel
               __fillPixelRect(0, row, w, 1, awColors);
            } // end scanline
            // long timeElapsed = millis() - startTime;
            // Serial.print(F("Loaded in "));
            // Serial.print(timeElapsed);
            // Serial.println(" ms");
            } // end goodBmp
         }
      }

    //   bmpFile.close();
      fclose(bmpFile);
      if (!goodBmp) {
         yatl.dbug("BMP format not recognized.");
      }

      }

    // ============== Console routines ==================

      void _feedSprites(char* filename, int x, int y) {
          yatl.dbug("_feedSprites() NYI");
      }

      void __setFont(int fontMode) {
          yatl.dbug("SET Font NYI");
      }
      void __fillPixelRect(int x, int y, int w, int h, uint16_t* raster) {
          sdlScreen.drawRGB16(x,y,w,h,raster);
      }

      void __setCursor(int xPx, int yPx) {
          sdlScreen.setCursorPx(xPx, yPx);
      }

      void __setTextColor(uint16_t color) {
          sdlScreen.setTextColor(color);
      }

      void __fillRect(int x, int y, int w, int h, uint16_t bgColor) {
          sdlScreen.drawRect(x,y,w,h,MODE_FILL, bgColor);
      }

      void __write1char(char ch) {
        sdlScreen.write(ch);  
      }

      void __clearScreen(uint16_t bgColor) {
        sdlScreen.cls(bgColor);
      }


    void YatlScreen::cls() {
        consoleCls();
    }

    void YatlScreen::write(char ch) {
        consoleWrite(ch);
    }

    // void YatlScreen::println(const char* str) {
    //     printf("'%s' \n", str);
    //     sdlScreen.println((char*)str);
    // }

    // void YatlScreen::println(char* str) {
    //     printf("'%s' \n", str);
    //     sdlScreen.println(str);
    // }
    // ============== Console routines ==================

    void YatlScreen::drawTextBox(char const* title, char const* text, unsigned short color) {
        printf("TXT BOX '%s' \n", text);
    }
    // ::::::::::::::::::::::::::::::::::::::


    // #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
    // extern SdFatSdio SD;

    extern Yatl yatl;
    YatlSubMCU _subMcu(&yatl);
    YatlBuzzer _buzzer(&yatl);
    YatlPWRManager _pwrManager(&yatl);
    YatlLEDs _leds(&yatl);
    YatlWiFi _wifi(&yatl);
    YatlMusicPlayer _mp3(&yatl);
    YatlFS _fs(&yatl);
    YatlKeyboard _keyb(&yatl);
    YatlScreen _screen(&yatl);

    Yatl::Yatl() {
        this->subMcu = &_subMcu;
        this->buzzer = &_buzzer;
        this->pwrManager = &_pwrManager;
        this->leds = &_leds;
        this->wifi = &_wifi;
        this->mp3 = &_mp3;
        this->fs = &_fs;
        this->keyb = &_keyb;
        this->screen = &_screen;
    }

    Yatl::~Yatl() {
    }

    bool screenReady = false;
    bool screenDBUG = !true;

    bool Yatl::setup() {
        // Serial.begin(115200);
        this->dbug("Yatl setuping ...");

        pinMode( BUILTIN_LED, OUTPUT );
        this->led(false);

        this->buzzer->setup();

        bool ok;
        ok = this->getScreen()->setup();
        screenReady = ok;
        if (!ok) { this->warn("Screen not ready !"); }

        ok = this->getFS()->setup();
        if (!ok) { this->warn("SD not ready !"); }

        ok = this->subMcu->setup();
        if (!ok) { this->warn("SubMCU Setup Failed"); }

        ok = this->keyb->setup();
        if (!ok) { this->warn("Keyb Setup Failed"); }

        this->delay(200);

        return true;
    }

    void Yatl::dbug(const char* str) { 
        // Serial.println(str); 
        printf( "%s\n", str );

        if ( screenDBUG && screenReady ) {
            this->getScreen()->println( str );
        }
    }
    void Yatl::dbug(const char* str, int val) { 
        char msg[128+1]; sprintf(msg, "%s %d", str, val);
        // Serial.println(msg); 
        printf( "%s\n", str );

        if ( screenDBUG && screenReady ) {
            this->getScreen()->println( msg );
        }
    }
    void Yatl::dbug(const char* str, float val) { 
        char msg[128+1]; sprintf(msg, "%s %g", str, val);
        //Serial.println(msg); 
        printf( "%s\n", str );

        if ( screenDBUG && screenReady ) {
            this->getScreen()->println( msg );
        }
    }

    void Yatl::delay(long time) {
        ::delay(time);
    }

    void Yatl::beep() {
        this->getBuzzer()->beep(440, 100);
    }

    void Yatl::led(bool state) {
        digitalWrite(BUILTIN_LED, state ? HIGH : LOW);
    }

    void Yatl::blink(int times) {
        this->led(false);
        for(int i=0; i < times; i++) {
            this->led(true);
            this->delay(200);
            this->led(false);
            this->delay(200);
        }
        this->led(false);
    }

    void Yatl::alert(const char* msg) {
        if ( screenReady ) {
            this->getScreen()->drawTextBox( "ALERT", msg, 2 ); // red
        } else {
            this->blink(5);
            printf("*****************************\n");
            printf("*  ALERT                    *\n");
            printf("* \n");
            printf("%s\n",msg);
            printf("*****************************\n");
        }
        this->buzzer->beep(440, 200);
    }
    void Yatl::warn(const char* msg) {
        if ( screenReady ) {
            this->getScreen()->drawTextBox( "WARNING", msg, 7 ); // cyan
        } else {
            this->blink(5);
            printf("*****************************\n");
            printf("*  WARNING                  *\n");
            printf("* ");
            printf("%s\n",msg);
            printf("*****************************\n");
        }
    }

    // ===============] FileSystem [===========
    // ex. MONKEY.T5K -> /Z/0/MONKEY.T5K
    // spe DISK for assets : "Z:"
    // 'cause CP/M supports ONLY up to P:

    const int _fullyQualifiedFileNameSize = 1+5 + (8+1+3) + 1;
    char _assetEntry[ _fullyQualifiedFileNameSize ];

    // SdFatSdio SD;

    bool YatlFS::setup() {
        // return SD.begin();
        return true;
    }

    // not ThreadSafe !
    char* YatlFS::getAssetsFileEntry(char* assetName) {
        memset(_assetEntry, 0x00, _fullyQualifiedFileNameSize);
        strcpy( _assetEntry, "./Z/0/" );
        strcat( _assetEntry, assetName );
        return _assetEntry;
    }

    bool YatlFS::downloadFromSerial() {
        // while( Serial.available() ) { Serial.read(); delay(2); }
        // this->yatl->warn("Download in progress");
        // Serial.println("+OK");
        // while( !Serial.available() ) { delay(2); }
        // // for now : file has to be like "/C/0/XTSDEMO.PAS"
        // int tlen = 0;
        // char txt[128+1]; 
        // char name[64+1]; memset(name, 0x00, 64); tlen = Serial.readBytesUntil(0x0A, name, 64);
        // if ( tlen <= 0 ) {
        //     sprintf(txt, "Downloading %s (error)", name);
        //     this->yatl->warn((const char*)txt);
        //     Serial.println("Download not ready");
        //     Serial.println(name);
        //     Serial.println("-OK");
        //     return false;
        // }

        // // Cf CPM may padd the original file
        // File f = SD.open(name, O_CREAT | O_WRITE);
        // if ( !f ) {
        //   Serial.println("-OK");
        //   return false;    
        // }
        // f.remove();
        // f.close();
        // // Cf CPM may padd the original file

        // f = SD.open(name, O_CREAT | O_WRITE);
        // if ( !f ) {
        //   Serial.println("-OK");
        //   return false;    
        // }

        // Serial.println("+OK");
        // while( !Serial.available() ) { delay(2); }
        // char sizeStr[12+1]; memset(sizeStr, 0x00, 12); tlen = Serial.readBytesUntil(0x0A, sizeStr, 12);
        // long size = atol(sizeStr);
        // sprintf(txt, "Downloading %s (%ld bytes)", name, size);
        // this->yatl->warn((const char*)txt);
        // char packet[128+1];
        // Serial.println("+OK");
        // for(int readed=0; readed < size;) {
        //     while( !Serial.available() ) { delay(2); }
        //     int packetLen = Serial.readBytes( packet, 128 );
        //     f.write(packet, packetLen);
        //     f.flush();
        //     readed += packetLen;
        // }
        // f.close();
        // this->yatl->warn("-EOF-");
        // this->yatl->beep();
        // return true;
        return false;
    }

    bool YatlFS::downloadFromSubMcu() {
//         this->yatl->getSubMCU()->lock();
//         this->yatl->getSubMCU()->cleanBuffer();


//         while( this->yatl->getSubMCU()->available() ) { this->yatl->getSubMCU()->read(); delay(2); }
//         this->yatl->warn("Download in progress");
//         this->yatl->getSubMCU()->println("+OK");
// Serial.println("+OK");
//         // while( !this->yatl->getSubMCU()->available() ) { delay(2); }
//         // while( this->yatl->getSubMCU()->available() ) { this->yatl->getSubMCU()->read(); delay(2); }

//         // for now : file has to be like "/C/0/XTSDEMO.PAS"
//         int tlen = 0;
//         char txt[128+1]; 
//         // while( !this->yatl->getSubMCU()->available() ) { delay(2); }
//         char name[64+1]; memset(name, 0x00, 64); tlen = this->yatl->getSubMCU()->readBytesUntil(0x0A, name, 64);
// Serial.print("Name : ");
// Serial.println(name);
//         if ( tlen <= 0 ) {
//             sprintf(txt, "Downloading %s (error)", name);
//             this->yatl->warn((const char*)txt);
//             // Serial.println("Download not ready");
//             // Serial.println(name);
//             // Serial.println("-OK");
//             this->yatl->getSubMCU()->unlock();
//             return false;
//         }

//         sprintf(txt, "Downloading %s (0x01)", name);
//         this->yatl->warn((const char*)txt);

//         // Cf CPM may padd the original file
//         File f = SD.open(name, O_CREAT | O_WRITE);
//         if ( !f ) {
//           this->yatl->getSubMCU()->println("-OK");
//           this->yatl->getSubMCU()->unlock();
//           return false;    
//         }
//         f.remove();
//         f.close();
//         // Cf CPM may padd the original file

//         f = SD.open(name, O_CREAT | O_WRITE);
//         if ( !f ) {
//             sprintf(txt, "Failed to open %s (0x02)", name);
//             this->yatl->warn((const char*)txt);
//             this->yatl->getSubMCU()->println("-OK");
//             this->yatl->getSubMCU()->unlock();
//           return false;    
//         }
// Serial.println("+OK");
//         this->yatl->getSubMCU()->println("+OK");
//         sprintf(txt, "wait for fileLen %s (0x03)", name);
//         this->yatl->warn((const char*)txt);
//         while( !this->yatl->getSubMCU()->available() ) { delay(2); }
//         char sizeStr[12+1]; memset(sizeStr, 0x00, 12); tlen = this->yatl->getSubMCU()->readBytesUntil(0x0A, sizeStr, 12);
// Serial.print("Size : ");
// Serial.println(sizeStr);
//         long size = atol(sizeStr);
//         sprintf(txt, "Downloading %s (%ld bytes)", name, size);
//         this->yatl->warn((const char*)txt);
//         char packet[128+1];
// Serial.println("+OK");
//         this->yatl->getSubMCU()->println("+OK");
//         for(int readed=0; readed < size;) {
//             while( !this->yatl->getSubMCU()->available() ) { delay(2); }
//             int packetLen = this->yatl->getSubMCU()->readBytes( packet, 128 );
//             f.write(packet, packetLen);
//             f.flush();
//             readed += packetLen;
//         }
//         f.close();
// Serial.println("-EOF-");
//         this->yatl->getSubMCU()->unlock();
//         this->yatl->warn("-EOF-");
//         this->yatl->beep();
//         return true;
        return false;
    }

    // ===============] SubMCU [===============
    bool YatlSubMCU::setup() {
        // BRIDGE_MCU_SERIAL.begin( BRIDGE_MCU_BAUDS );

        // send 'i' wait for response ....
        // will see later Cf synchro.

        return true;
    }

    void YatlSubMCU::reboot(bool waitFor) {
        // this->send('r');
        if (waitFor) { delay(1000); }
    }

    void YatlSubMCU::flush() { /*BRIDGE_MCU_SERIAL.flush(); */ }
    void YatlSubMCU::send(char ch) { /* BRIDGE_MCU_SERIAL.write( ch ); */ }
    void YatlSubMCU::send(const char* str) { /* BRIDGE_MCU_SERIAL.print( str ); */ }
    void YatlSubMCU::send(char* str) { /* BRIDGE_MCU_SERIAL.print( str ); */ }

    void YatlSubMCU::println(const char* str) { /* BRIDGE_MCU_SERIAL.println( str ); */ }

    extern bool keybLocked;

    void YatlSubMCU::lock() {
        keybLocked = true;
    }

    void YatlSubMCU::unlock() {
        keybLocked = false;
    }

    int YatlSubMCU::available() { return 0;/*BRIDGE_MCU_SERIAL.available(); */ }
    int YatlSubMCU::read() { 
        // int ret = BRIDGE_MCU_SERIAL.read(); 
        int ret = -1;
        return ret;
    }

    int YatlSubMCU::readUntil(uint8_t until, char* dest, int maxLen) {
    //   int readed = BRIDGE_MCU_SERIAL.readBytesUntil((char)until, dest, maxLen);
    int readed = 0;
      return readed;
    }

    int YatlSubMCU::readBytes(char* dest, int maxLen) {
    //   int readed = BRIDGE_MCU_SERIAL.readBytes(dest, maxLen);
    int readed = 0;
      return readed;
    }

    #define MAX_SUBMCU_LINE_LEN 255
    char lastSubMCULine[MAX_SUBMCU_LINE_LEN+1];
    char* YatlSubMCU::readLine() {
        memset(lastSubMCULine, 0x00, MAX_SUBMCU_LINE_LEN+1);
        // BRIDGE_MCU_SERIAL.readBytesUntil( '\n', lastSubMCULine, MAX_SUBMCU_LINE_LEN );
        return lastSubMCULine;
    }

    char* YatlSubMCU::waitForNonEmptyLine(unsigned long timeout) {
//         if ( timeout == 0 ) { timeout = 10*1000; } // 10sec
//         unsigned long began = millis();

memset(lastSubMCULine, 0x00, MAX_SUBMCU_LINE_LEN+1);
  uint8_t idx = 0;
//   char c;
//   do
//   {
//     while (BRIDGE_MCU_SERIAL.available() == 0) {
//         delay(1);
//         if (millis()-began >= timeout) { keybLocked = false; return lastSubMCULine; }
//     } // wait for a char this causes the blocking
//     c = BRIDGE_MCU_SERIAL.read();
//     lastSubMCULine[idx++] = c;

//     if (millis()-began >= timeout) { keybLocked = false; return lastSubMCULine; }
//   }
//   while (c != '\n' && c != '\r'); 
  lastSubMCULine[idx] = 0;

       return lastSubMCULine;
    }

    void YatlSubMCU::cleanBuffer() {
        while( this->available() ) { this->read(); }
    }

    // ===============] PWR [=================

    float YatlPWRManager::getVoltage() {
        // this->yatl->getSubMCU()->send('v'); this->yatl->getSubMCU()->flush();
        // float volt = -1.0f;
        // volt = atof( this->yatl->getSubMCU()->waitForNonEmptyLine() );
        float volt = 3.0f;
        return volt;
    }

    #define RESTART_ADDR 0xE000ED0C
    #define READ_RESTART() (*(volatile uint32_t *)RESTART_ADDR)
    #define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))
    void softReset() {
        // WRITE_RESTART(0x5FA0004);
        printf("Reset to Impl. \n");
    }


    // wholeSystem -> BOTH Main & SubMCU
    void YatlPWRManager::reset(bool wholeSystem) { 
        this->yatl->warn("Rebooting !");
        this->yatl->getSubMCU()->reboot(!false);
        softReset();
    }

    void YatlPWRManager::deepSleep(bool wholeSystem) { /*TODO*/ this->yatl->warn("DEEPSLEEP NYI"); this->yatl->getSubMCU()->send("h"); }

    // ==============] LEDs [==================

    void YatlLEDs::red(bool state) { this->yatl->getSubMCU()->send( state ? "l1xx" : "l0xx" ); }
    void YatlLEDs::green(bool state) { this->yatl->getSubMCU()->send( state ? "lx1x" : "lx0x" ); }
    void YatlLEDs::blue(bool state) { this->yatl->getSubMCU()->send( state ? "lxx1" : "lxx0" ); }
    void YatlLEDs::builtin(bool state) { this->yatl->led(state); }

    // ==============] WiFi [==================

    bool YatlWiFi::beginSTA() { 
        this->yatl->getSubMCU()->cleanBuffer();
        this->yatl->getSubMCU()->send("wcs");
        char* resp = this->yatl->getSubMCU()->waitForNonEmptyLine();
        ::delay(200);
        // Serial.println("----------->");
        // Serial.println(resp);
        // Serial.println("----------->");
        return strlen(resp) > 0 && resp[0] == '+';
    }
    bool YatlWiFi::beginAP() { 
        this->yatl->getSubMCU()->cleanBuffer();
        this->yatl->getSubMCU()->send("wca");
        char* resp = this->yatl->getSubMCU()->waitForNonEmptyLine();
        // Serial.print("-> WIFI : ");Serial.print((const char*)resp);Serial.print("\n");
        return strlen(resp) > 0 && resp[0] == '+';
    }

    void YatlWiFi::close() { 
        this->yatl->getSubMCU()->cleanBuffer();
        this->yatl->getSubMCU()->send("ws"); 
        this->yatl->getSubMCU()->waitForNonEmptyLine(); 
    }

    char* YatlWiFi::getIP() { this->yatl->getSubMCU()->send("wi"); return this->yatl->getSubMCU()->waitForNonEmptyLine(); }
    char* YatlWiFi::getSSID() { this->yatl->getSubMCU()->send("we"); return this->yatl->getSubMCU()->waitForNonEmptyLine(); }

    bool YatlWiFi::startTelnetd() {
        this->yatl->getSubMCU()->cleanBuffer();
        this->yatl->getSubMCU()->send("wto");
        char* resp = this->yatl->getSubMCU()->waitForNonEmptyLine();
        // Serial.print("-> TELNET : ");Serial.print((const char*)resp);Serial.print("\n");
        return strlen(resp) > 0 && resp[0] == '+';
    }
    bool YatlWiFi::stopTelnetd() {
        this->yatl->getSubMCU()->cleanBuffer();
        this->yatl->getSubMCU()->send("wto");
        char* resp = this->yatl->getSubMCU()->waitForNonEmptyLine();
        return true;
    }
    // char* YatlWiFi::wget(char* url);

    // ==============] MP3 [==================

    void YatlMusicPlayer::play(int trackNum) { 
        uint8_t d0 = trackNum / 256;
        uint8_t d1 = trackNum % 256;
        this->yatl->getSubMCU()->send("pp");
        this->yatl->getSubMCU()->send(d0);
        this->yatl->getSubMCU()->send(d1);
    }
    void YatlMusicPlayer::loop(int trackNum) {
        uint8_t d0 = trackNum / 256;
        uint8_t d1 = trackNum % 256;
        this->yatl->getSubMCU()->send("pl");
        this->yatl->getSubMCU()->send(d0);
        this->yatl->getSubMCU()->send(d1);
    }
    void YatlMusicPlayer::stop() { this->yatl->getSubMCU()->send("ps"); }
    void YatlMusicPlayer::pause() { this->yatl->getSubMCU()->send("pP"); }
    void YatlMusicPlayer::next() { this->yatl->getSubMCU()->send("pn"); }
    void YatlMusicPlayer::prev() { this->yatl->getSubMCU()->send("pv"); }
    void YatlMusicPlayer::volume(uint8_t vol) {
        this->yatl->getSubMCU()->send("pV");
        this->yatl->getSubMCU()->send(vol%256);
    }

#endif
