#ifndef _XTS_SETTINGS_H_
#define _XTS_SETTINGS_H_ 1

 /**
  * Yatl general Settings
  * 
  * 
  * Xtase - fgalliat @ May2019
  */
  #include "Arduino.h"


  #define YATL_PLATFORM 1

  #define HAS_BUILTIN_LCD 1
  #define USE_BUILTIN_LCD 1
  #define HAS_KEYBOARD 1
  
  #define KEYB_ISR_POLL 0

  // 13 is used by SPI LCD
  #define LED_PIN 14
  #define SPEAKER_PIN 6

   #define BUILTIN_LED    LED_PIN
   #define BUILTIN_BUZZER SPEAKER_PIN

   #define BRIDGE_MCU_SERIAL Serial1
   #define BRIDGE_MCU_BAUDS  115200

#endif