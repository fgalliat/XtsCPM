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
  int getPascalStringFromRam(int32 addr, char* dest, int maxLen) {
      memset(dest, 0x00, maxLen);
      uint8_t *F = (uint8_t*)_RamSysAddr(addr);

      uint8_t len = F[0]; // maybe not if always 205
      memcpy(dest, &F[1], min(len,maxLen) );

      return len;
  } 

  int getStringFromRam(int32 addr, char* dest, int maxLen) {
      return getPascalStringFromRam(addr, dest, maxLen);
  }

  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  int32 xbdos_console(int32 value) {
     int a0 = HIGH_REGISTER( value );
     int a1 = LOW_REGISTER( value ); 

     if ( a0 == 0 ) {
         // set the console colorSet
         if ( a1 == 0 ) {
            // to reset
            consoleColorSet(); 
         } else {
            // set as old monochrome LCD (no backlight) 
            consoleColorSet( rgb(126, 155, 125), rgb(69,80,110), rgb(108-30,120-30,195-30) );
         }
     } else if ( a0 == 1 ) {
         // set the console font size & cols mode
         if ( a1 == 0 ) {
            // to reset
            _setConsoleMode(LCD_CONSOLE_80_COLS);
            consoleRenderFull();
         } else {
            // set as 53 cols (big font)
            _setConsoleMode(LCD_CONSOLE_40_COLS);
            consoleRenderFull();
         }
     }
     return 0;
  }

  int32 BdosTest229(int32 value) {
    Serial.println("/===== BDos 229 call =====\\");
    char test[32+1];
    int len = getStringFromRam(value, test, 32+1);
    Serial.print("TP3 len= ");Serial.print(len);Serial.print("\n");
    int i=1;
    Serial.print("OpT ");Serial.print( (int)test[i++] );Serial.print("\n"); // 1
    Serial.print("Sht ");Serial.print( (int)test[i++] );Serial.print("\n");
    Serial.print("Fll ");Serial.print( (int)test[i++] );Serial.print("\n");

    Serial.print("cb0 ");Serial.print( (int)test[i++] );Serial.print("\n"); // 4
    Serial.print("cb1 ");Serial.print( (int)test[i++] );Serial.print("\n");

    Serial.print("xb0 ");Serial.print( (int)test[i++] );Serial.print("\n"); // 6
    Serial.print("xb1 ");Serial.print( (int)test[i++] );Serial.print("\n");

    Serial.print("yb0 ");Serial.print( (int)test[i++] );Serial.print("\n"); // 8
    Serial.print("yb1 ");Serial.print( (int)test[i++] );Serial.print("\n");

    Serial.print("wb0 ");Serial.print( (int)test[i++] );Serial.print("\n"); // 10
    Serial.print("wb1 ");Serial.print( (int)test[i++] );Serial.print("\n");

    Serial.print("hb0 ");Serial.print( (int)test[i++] );Serial.print("\n"); // 12
    Serial.print("hb1 ");Serial.print( (int)test[i++] );Serial.print("\n");

    // then draw it !!!!
    uint8_t OpType = test[1];
    uint8_t shapeType = test[2];
    uint8_t fillType = test[3]; // 0 draw / 1 fill

    uint16_t color = ((uint16_t)test[4] << 8) + test[5];

    if ( OpType == 0x7F ) {
      // drawShapes
      if ( shapeType == 0x01 ) {
        // Shape : rectangle
        if ( fillType == 0x00 ) {
          // draw outlines
          uint16_t x = ((uint16_t)test[6] << 8) + test[7];
          uint16_t y = ((uint16_t)test[8] << 8) + test[9];
          uint16_t w = ((uint16_t)test[10] << 8) + test[11];
          uint16_t h = ((uint16_t)test[12] << 8) + test[13];

          tft.drawRect( x, y, w, h, color );
        } else if ( fillType == 0x01 ) {
          // fills the rect
          uint16_t x = ((uint16_t)test[6] << 8) + test[7];
          uint16_t y = ((uint16_t)test[8] << 8) + test[9];
          uint16_t w = ((uint16_t)test[10] << 8) + test[11];
          uint16_t h = ((uint16_t)test[12] << 8) + test[13];

          tft.fillRect( x, y, w, h, color );
        }
      }
    }

    return 0;
  }


  int32 XtsBdosCall(uint8 regNum, int32 value) {
    if ( regNum == 225 ) {

      char test[256];
      getStringFromRam(value, test, 256);

      Serial.println("/===== BDos PString call =====\\");
      Serial.println(test);

      upper(test);

      if ( endsWith(test, (char*)".BMP") ) {
        Serial.println("|  Wanna draw a BMP wallpaper |");
        drawBmp( getAssetsFileEntry( test ), true );
      } else if ( endsWith(test, (char*)".PCT") ) {
        Serial.println("|  Wanna draw a PCT wallpaper |");
        Serial.println("|  NYI                        |");
      } else if ( endsWith(test, (char*)".BPP") ) {
        Serial.println("|  Wanna draw a BPP wallpaper |");
        Serial.println("|  NYI                        |");
      } else {
        Serial.print("| Wanna draw a ");
        Serial.print( test );
        Serial.println(" wallpaper? |");
      }

      Serial.println("\\===== BDos PString call =====/");
    } else if ( regNum == 226 ) {
     return xbdos_console(value);
    } else if ( regNum == 227 ) {
     return mp3BdosCall(value);
    } else if ( regNum == 228 ) {
      Serial.println( "BdosCall 228 NYI" );
    } else if ( regNum == 229 ) {
      BdosTest229(value);
    }
    
    return 0;
  }




#endif