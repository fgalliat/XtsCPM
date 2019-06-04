#ifndef _YATL_SETTINGS_H_
#define _YATL_SETTINGS_H_ 1
/**
 * Xtase - fgalliat @Jun2019
 * 
 * General Settings Headers
 * 
 */

    #define YATL_PLATFORM 1

    #define BUILTIN_LED    14
    #define BUILTIN_BUZZER 6

    #define BRIDGE_MCU_SERIAL Serial1
    #define BRIDGE_MCU_BAUDS  115200


  // to move away...
  static char charUpCase(char ch) {
      if ( ch >= 'a' && ch <= 'z' ) {
          return ch - 'a' + 'A';
      }
      return ch;
  }

#endif