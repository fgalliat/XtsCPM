#ifndef __XTS_BDOS_H_
#define __XTS_BDOS_H_ 1

 /**
  * Bdos extensions
  * 
  * 
  * Xtase - fgalliat @ May2019
  * 
  */

  #include "Arduino.h"

  // from TurboPascal 3 strings are 255 long max
  // & starts @ 1 ( 'cause @0 contains length)
  char* getPascalStringFromRam(int32 addr, char* dest, int maxLen) {
      memset(dest, 0x00, maxLen);
      uint8_t *F = (uint8_t*)_RamSysAddr(addr);

      uint8_t len = F[0];
      memcpy(dest, &F[1], len);

      return dest;
  } 

  char* getStringFromRam(int32 addr, char* dest, int maxLen) {
      return getPascalStringFromRam(addr, dest, maxLen);
  }

  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  int32 XtsBdosCall(uint8 regNum, int32 value) {
    if ( regNum == 225 ) {

      char test[256];
      getStringFromRam(value, test, 256);

      Serial.println("/===== BDos PString call =====\\");
      Serial.println(test);
      Serial.println("\\===== BDos PString call =====/");

      drawBmp( getAssetsFileEntry( test ), true );
    } else if ( regNum == 226 ) {
      int a0 = HIGH_REGISTER( value );
      int a1 = LOW_REGISTER( value );
      // use a0 & a1 .... for subOp. dispatch

      if ( value == 0 ) {
        // to reset
        consoleColorSet();
      } else {
        // set as old monochrome LCD (no backlight) 
        consoleColorSet( rgb(126, 155, 125), rgb(69,80,110), rgb(108-30,120-30,195-30) );
      }
    }
    
    return 0;
  }




#endif