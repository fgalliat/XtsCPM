#ifndef YATL_H_
#define YATL_H_ 1

/**
 * Xtase YATL Hardware interface
 * Xtase - fgalliat @Apr2019
 */

  #define YATL_PLATFORM 1

  #define HAS_BUILTIN_LCD 1

  #define HAS_SUB_MCU 1

  // 13 is used by SPI LCD
  #define LED_PIN 14

  #define SPEAKER_PIN 6

  #define y_dbug(a) Serial.println(a)

  // ===============================================
  #ifdef HAS_SUB_MCU

   #include "xts_submcu.h"

  #endif
  // ===============================================


  void setupArduinoScreen();

  // Xtase run-time Handler
  #define USE_XTS_HDL 1
  bool ledState = false;
  void xts_hdl() {
    // run-time handler function
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    ledState = !ledState;
  }

  #define HAS_BUILTIN_BUZZER 1
  void setupArduinoSpeaker();

  void setupISR();

  void setupYatl() {
    setupArduinoScreen();
    setupArduinoSpeaker();
    setupISR();
    #ifdef HAS_SUB_MCU
      setupBridge();
    #endif
  }


  // @@@@@@@@@@@@@ ISR Routines @@@@@@@@@@@@@
  #include <TimerOne.h>

  bool inIRQ=false;
  int irqCpt=0;


  #define ISR_MODE_NONE        0
  #define ISR_MODE_PLAY_BUZZER 1

  volatile char buzzerTuneStr[ 256 ];
  void _doPlay(char* tune);

  // TODO : use real stacks !!!!
  uint8_t nextISR = ISR_MODE_NONE;
  void ISR_push(uint8_t nextISROpCode = ISR_MODE_NONE) {
    nextISR = nextISROpCode;
  }



  void timer_IRQ(){
    if (inIRQ) { return; }
    inIRQ = true;

    // if ( irqCpt > 1 ) {
    //     screenSync();
    // }
    if ( nextISR == ISR_MODE_NONE ) {
      // ...
    } else if ( nextISR == ISR_MODE_PLAY_BUZZER ) {
      Serial.println( "ISR PUSHED : ISR_MODE_PLAY_BUZZER" );
      nextISR = ISR_MODE_NONE;
      _doPlay( (char*)buzzerTuneStr );
    } else {
      // ...
    }

    irqCpt++;

    inIRQ = false;
  }


  void setupISR() {
    Timer1.initialize(350000); // 350ms 
    Timer1.attachInterrupt(timer_IRQ);
  }

  // =======================================

  // ex. MONKEY.T5K -> /Z/0/MONKEY.T5K
  // spe DISK for assets : "Z:"
  // 'cause CP/M supports ONLY up to P:

  const int _fullyQualifiedFileNameSize = 5 + (8+1+3) + 1;
  char _assetEntry[ _fullyQualifiedFileNameSize ];

  // not ThreadSafe !
  char* getAssetsFileEntry(char* assetName) {
    memset(_assetEntry, 0x00, _fullyQualifiedFileNameSize);
    strcpy( _assetEntry, "/Z/0/" );
    strcat( _assetEntry, assetName );
    return _assetEntry;
  }

  // =======================================
  extern void drawBmp(char* filename, bool screenRotate);

  void yatlSDinited(bool inited) {
    // drawBmp( getAssetsFileEntry( (char*)"girl.BMP"), true );
  }



  // =======================================


#endif