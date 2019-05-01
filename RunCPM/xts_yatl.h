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

  // I'm now able to send 255 long String to BDos Hook !!!!!
  void testXtsBdosCall(int interruptNum, int32 addr) {
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
  }


  // =======================================
  extern void drawBmp(char* filename, bool screenRotate);

  void yatlSDinited(bool inited) {
    drawBmp("/Z/0/girl.bmp", true);
  }


#endif