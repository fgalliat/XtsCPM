#if defined __IMXRT1062__

 #include "Arduino.h"
 #include "xts_yat4l.h"

/**
 * Xtase - fgalliat @Nov2019
 * 
 * YAT4L routines Impl.
 * 
 * part of XtsCPM project
 */

  #include "Adafruit_ILI9486_Teensy.h"
  #include <SPI.h>

 // for pin definitions, please refer to the header file
  Adafruit_ILI9486_Teensy tft;


  #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
  extern SdFat SD;




  void xts_hdl() {
      // Xtase runtime handler
  }

  // ==================

    // ===================================================================================
    //                                   WiFi
    // ===================================================================================

    // impl. include
    #include "xts_yat4l_dev_wifi_esp_at.h"

    // ===================================================================================
    //                                   Music
    // ===================================================================================


    void yat4l_mp3Play(int trackNum) {}
    void yat4l_mp3Loop(int trackNum) {}
    void yat4l_mp3Vol(int volume) {}
    void yat4l_mp3Pause() {}
    void yat4l_mp3Stop() {}
    void yat4l_mp3Next() {}
    void yat4l_mp3Prev() {}
    bool yat4l_mp3IsPlaying() {return false;}



    // ===================================================================================
    //                                   Buzzer
    // ===================================================================================
    bool BUZZER_MUTE = false;

    void yat4l_buzzer_init() {
      pinMode(BUZZER_PIN, OUTPUT);
      digitalWrite(BUZZER_PIN, LOW);
    }

    void yat4l_buzzer_tone(int freq, int duration) {
      if ( !BUZZER_MUTE ) {
        tone(BUZZER_PIN, freq, duration);
      }
    }
    void yat4l_buzzer_noTone() {
      noTone(BUZZER_PIN);
    }

    // void yat4l_buzzer_beep(int freq, int duration) {}
    // void yat4l_buzzer_playTuneString(char* sequence) {}
    // bool yat4l_buzzer_playTuneFile(const char* tuneStreamName) {return true;}


    //====================================================================================
    //                                    Led
    //====================================================================================

    void led(bool state, bool fastMode) {
        if ( LED_BUILTIN_PIN > 0 ) {
            digitalWrite(LED_BUILTIN_PIN, state ? HIGH : LOW);
        }

        if (fastMode) { return; } 

        // if ( state ) { bridgeSerial.write('L'); }
        // else  { bridgeSerial.write('l'); }
        delay(1);
    }

    void drive_led(bool state) {
        led(state, true);
    }

    // ===================================================================================
    //                                   FileSystem
    // ===================================================================================

    const int _fullyQualifiedFileNameSize = 1+5 + (8+1+3) + 1;
    char _assetEntry[ _fullyQualifiedFileNameSize ];

    // not ThreadSafe !
    char* yat4l_fs_getAssetsFileEntry(char* assetName) {
        if ( assetName == NULL || strlen(assetName) <= 0 ) { yat4l_dbug("NULL filename"); return NULL; }
        memset(_assetEntry, 0x00, _fullyQualifiedFileNameSize);

        if ( assetName[1] == ':' ) {
            // ex. "Y:IMG.PAK"
            // sprintf( _assetEntry, "/%c/0/%s", assetName[0], &assetName[2] );
            sprintf( _assetEntry, "%c/0/%s", assetName[0], &assetName[2] );
        } else {
            // sprintf( _assetEntry, "/Z/0/%s", &assetName[0] );
            sprintf( _assetEntry, "Z/0/%s", &assetName[0] );
        }

        return _assetEntry;
    }

    bool yat4l_fs_downloadFromSerial() { 
        while( Serial.available() ) { Serial.read(); delay(2); }
        yat4l_warn("Download in progress");
        Serial.println("+OK");
        while( !Serial.available() ) { delay(2); }
        // for now : file has to be like "/C/0/XTSDEMO.PAS"
        int tlen = 0;
        char txt[128+1]; 
        char name[64+1]; memset(name, 0x00, 64); tlen = Serial.readBytesUntil(0x0A, name, 64);
        if ( tlen <= 0 ) {
            sprintf(txt, "Downloading %s (error)", name);
            yat4l_warn((const char*)txt);
            Serial.println("Download not ready");
            Serial.println(name);
            Serial.println("-OK");
            return false;
        }

        // Cf CPM may padd the original file
        File f = SD.open(name, O_CREAT | O_WRITE);
        if ( !f ) {
          Serial.println("-OK");
          return false;    
        }
        f.remove();
        f.close();
        // Cf CPM may padd the original file

        f = SD.open(name, O_CREAT | O_WRITE);
        if ( !f ) {
          Serial.println("-OK");
          return false;    
        }

        Serial.println("+OK");
        while( !Serial.available() ) { delay(2); }
        char sizeStr[12+1]; memset(sizeStr, 0x00, 12); tlen = Serial.readBytesUntil(0x0A, sizeStr, 12);
        long size = atol(sizeStr);
        sprintf(txt, "Downloading %s (%ld bytes)", name, size);
        yat4l_warn((const char*)txt);
        char packet[128+1];
        Serial.println("+OK");
        for(int readed=0; readed < size;) {
            while( !Serial.available() ) { delay(2); }
            int packetLen = Serial.readBytes( packet, 128 );
            f.write(packet, packetLen);
            f.flush();
            readed += packetLen;
        }
        f.close();
        yat4l_warn("-EOF-");
        yat4l_buzzer_beep();
        return true;
    }

    // ==================
  void yat4l_led(bool state, bool fastMode) { led(state, fastMode); }

    // see xts_yat4l_dev_console.cpp
    extern void consoleCls(bool clearDisplay);
    extern void _setConsoleMode(int mode);

    bool yat4l_setup() {
        const int SD_CS = 0;
        pinMode( SD_CS, OUTPUT );
        digitalWrite( SD_CS, HIGH );

  if ( SUBMCU_READY_PIN > 0 ) {
    pinMode(SUBMCU_READY_PIN, INPUT);
  }

  if ( LED_BUILTIN_PIN > 0 ) {
   pinMode(LED_BUILTIN_PIN, OUTPUT);
   digitalWrite(LED_BUILTIN_PIN, LOW);
  }


        Serial.begin(115200);
        bool ok = yat4l_wifi_setup();


        // SD is init by YatlCPM.ino
    
        yat4l_buzzer_init();

  // Now initialise the TFT
        SPI.begin();
        tft.begin();
  tft.setRotation(DEFAULT_TFT_ROTATION);  // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(CLR_BLACK);

  _setConsoleMode(1); // compute 80 cols mode


        return true;
    }


    // ===================================================================================
    //                                   TFT Screen
    // ===================================================================================

    void yat4l_tft_cls() { tft.fillScreen(CLR_BLACK); tft.setCursor(0,0); }
    void yat4l_tft_setCursor(int col, int row) { tft.setCursor(col,row); }

    #include "xts_yat4l_soft_drawBMP.h"
    #include "xts_yat4l_soft_drawPAK.h"

    void yat4l_tft_drawBMP(char* filename, int x, int y) { 
        tft.setRotation(DEFAULT_TFT_ROTATION == 1 ? 2 : 0);
        // tft.setRotation(DEFAULT_TFT_ROTATION == 1 ? 0 : 2);

        int swap = x;
        x = y;
        y = swap;
        #if MODE_4INCH
        y += ( 480 - 320 );
        #endif

        drawBmp(filename, x, y);
        tft.setRotation(DEFAULT_TFT_ROTATION);
    }
    void yat4l_tft_drawPAK(char* filename, int x, int y, int imgNum) { drawImgFromPAK(filename, x, y, imgNum); }



    void yat4l_tft_drawRect(int x, int y, int w, int h, uint16_t color) { tft.drawRect(x, y, w, h, color); }
    void yat4l_tft_fillRect(int x, int y, int w, int h, uint16_t color) { tft.fillRect(x, y, w, h, color); }
    void yat4l_tft_drawCircle(int x, int y, int radius, uint16_t color) { tft.drawCircle(x, y, radius, color); }
    void yat4l_tft_fillCircle(int x, int y, int radius, uint16_t color) { tft.fillCircle(x, y, radius, color); }
    void yat4l_tft_drawLine(int x, int y, int x2, int y2, uint16_t color) { tft.drawLine(x, y, x2, y2, color); }


  // ===========================================
  // ===========================================

  void drawTextBox(const char* title, const char* msg, uint16_t color) {
      // tft.fillRect( 20, 20, TFT_WIDTH-40, TFT_HEIGHT-40, mapColor(color) );
      tft.fillRect( 20, 20, TFT_WIDTH-40, TFT_HEIGHT-40, color );
      tft.drawRect( 20, 20, TFT_WIDTH-40, TFT_HEIGHT-40, CLR_WHITE );
      tft.setTextColor(CLR_WHITE);
      tft.setCursor( 40, 30 );
      tft.print( title );
      tft.setCursor( 40, 30+30 );
      tft.print( msg );
  }


  void yat4l_dbug(char* str) { 
    Serial.print("(ii) "); Serial.println(str); 
  }

  void yat4l_dbug(const char* str) { yat4l_dbug( (char*)str ); }

  void yat4l_warn(char* str) { 
    // no serial write : because used while Serial copy
    // Serial.print("(!!) "); Serial.println(str); 
    drawTextBox("WARNING", (const char*)str, CLR_CYAN);
  }

  void yat4l_warn(const char* str) { yat4l_warn( (char*)str ); }

  void yat4l_error(char* str) { 
    // no serial write : because used while Serial copy
    drawTextBox("ERROR", (const char*)str, CLR_RED);
  }

  void yat4l_error(const char* str) { yat4l_error( (char*)str ); }

  // ===========================================

    void softReset() {
        // Teensy 4.0 specify 
        // but beware w/ some breakout boards

        // semms more like halt than reboot
        // SCB_AIRCR = 0x05FA0004;
    }

  void yat4l_reboot() { Serial.println("TEENSY4 rebooting"); softReset(); }
  void yat4l_halt() { Serial.println("TEENSY4 shutting down"); }

#endif