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

  void xts_hdl() {
      // Xtase runtime handler
  }

  // ==================

    // ===================================================================================
    //                                   WiFi
    // ===================================================================================

    char* yat4l_wifi_getIP() { return "0.0.0.0"; }
    char* yat4l_wifi_getSSID() { return "NotConnetcted"; }

    bool yat4l_wifi_close() { return true; }
    bool yat4l_wifi_beginAP() { return false; }
    bool yat4l_wifi_startTelnetd() { return false; }

    bool yat4l_wifi_loop() { ; }

    void yat4l_wifi_telnetd_broadcast(char ch)  { ; }
    int  yat4l_wifi_telnetd_available()  { return 0; }
    int  yat4l_wifi_telnetd_read() { return -1; }

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
    void yat4l_buzzer_init() {}
    void yat4l_buzzer_tone(int freq, int duration) {}
    void yat4l_buzzer_noTone() {}
    void yat4l_buzzer_playTuneString(char* sequence) {}
    bool yat4l_buzzer_playTuneFile(const char* tuneStreamName) {return true;}
    void yat4l_buzzer_beep(int freq, int duration) {}


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

    bool yat4l_fs_downloadFromSerial() { Serial.println("-- REQUEST FOR SERIAL UPLOAD --"); return false; }

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

    void yat4l_tft_drawBMP(char* filename, int x, int y) { Serial.print("drawBMP : "); Serial.println(filename); }
    void yat4l_tft_drawPAK(char* filename, int x, int y, int imgNum) { Serial.print("drawPAK : "); Serial.println(filename); }

    void yat4l_tft_drawRect(int x, int y, int w, int h, uint16_t color) { tft.drawRect(x, y, w, h, color); }
    void yat4l_tft_fillRect(int x, int y, int w, int h, uint16_t color) { tft.fillRect(x, y, w, h, color); }
    void yat4l_tft_drawCircle(int x, int y, int radius, uint16_t color) { tft.drawCircle(x, y, radius, color); }
    void yat4l_tft_fillCircle(int x, int y, int radius, uint16_t color) { tft.fillCircle(x, y, radius, color); }
    void yat4l_tft_drawLine(int x, int y, int x2, int y2, uint16_t color) { tft.drawLine(x, y, x2, y2, color); }


  // ===========================================
  // ===========================================

  void yat4l_dbug(char* str) { 
    Serial.print("(ii) "); Serial.println(str); 
  }

  void yat4l_dbug(const char* str) { yat4l_dbug( (char*)str ); }

  void yat4l_warn(char* str) { 
    // no serial write : because used while Serial copy
    // Serial.print("(!!) "); Serial.println(str); 
  }

  void yat4l_warn(const char* str) { yat4l_warn( (char*)str ); }

  // ===========================================

  void yat4l_reboot() { Serial.println("TEENSY4 rebooting"); }
  void yat4l_halt() { Serial.println("TEENSY4 shutting down"); }

#endif