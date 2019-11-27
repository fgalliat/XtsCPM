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

    // ====== Fs Routines =================

    const int _fullyQualifiedFileNameSize = 1+5 + (8+1+3) + 1;
    char _assetEntry[ _fullyQualifiedFileNameSize ];

    // not ThreadSafe !
    char* yael_fs_getAssetsFileEntry(char* assetName) {
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



    // void yat4l_tft_cls() {
    //     consoleCls(true);
    // }

void yat4l_tft_cls() { tft.fillScreen(CLR_BLACK); tft.setCursor(0,0); }
void yat4l_tft_setCursor(int col, int row) { tft.setCursor(col,row); }

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