#ifndef YATL_H_
#define YATL_H_ 1

/**
 * Xtase YATL Hardware interface
 * Xtase - fgalliat @Apr2019
 */

  #include "xts_yatl_settings.h"


  #define y_dbug(a) Serial.println(a)

  // ===============================================

  // Xtase run-time Handler
  #define USE_XTS_HDL 1
  bool ledState = false;
  void xts_hdl() {
    // run-time handler function

    // digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    // ledState = !ledState;
  }

  void setupISR();
  bool keybReady = false;


  void setupYatl() {
    bool ok = yatl.setup();
    if ( !ok ) {
      yatl.warn("Something wrong in init !");
      Serial.begin(115200);
      while(true) {
        Serial.println("GENERAL ERROR");
        delay(1000);
      }
    }
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

    #ifdef HAS_KEYBOARD
      #if KEYB_ISR_POLL
      #warning "TO LOOK AT : ISR & Kbd polling"
      // #ifdef LAYOUT_MOBIGO
      //   if ( !KB_AUTO_POLL && keybReady ) {
      //     kbd.poll();
      //   }
      // #endif
      yatl.getKeyboard()->poll();
      #endif
    #endif

    if ( nextISR == ISR_MODE_NONE ) {
      // ...
    } else if ( nextISR == ISR_MODE_PLAY_BUZZER ) {
      // doesn't really work well 
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
    //Timer1.initialize(50000); // 50ms 
    Timer1.attachInterrupt(timer_IRQ);
  }

  // =======================================

#endif