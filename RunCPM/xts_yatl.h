#ifndef YATL_H_
#define YATL_H_ 1

/**
 * Xtase YATL Hardware interface
 * Xtase - fgalliat @Apr2019
 */

  #define YATL_PLATFORM 1

  #define HAS_BUILTIN_LCD 1

  // 13 is used by SPI LCD
  #define LED_PIN 14

  #define SPEAKER_PIN 6

  #define y_dbug(a) Serial.println(a)

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


  // I'm now able to send 255 long String to BDos Hook !!!!!
  void testXtsBdosCall(int interruptNum, int32 addr) {
    if ( interruptNum == 225 ) {

      uint8_t *F = (uint8_t*)_RamSysAddr(addr);
      char test[256];
      memset(test, 0x00, 256);

      // from TurboPascal 3 strings are 255 long max
      // & starts @ 1 ( 'cause @0 contains length)
      uint8_t len = F[0];
      memcpy(test, &F[1], len);

      Serial.println("/===== BDos String call =====\\");
      Serial.println(test);
      Serial.println("\\===== BDos String call =====/");

      drawBmp( getAssetsFileEntry( test ), true );
    } else if ( interruptNum == 226 ) {
      if ( addr == 0 ) { 
        // to reset
        consoleColorSet();
      } else {
        consoleColorSet( rgb(126, 155, 125), rgb(69,80,110), rgb(108-30,120-30,195-30) );
      }
    }
    
  }


#endif