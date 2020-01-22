#ifndef __XTS_BDOS_H_
#define __XTS_BDOS_H_ 1

 /**
  * Bdos extensions
  * 
  * 
  * Xtase - fgalliat @ May2019
  * 
  */

  // #include "Arduino.h"

  // forward symbols
  uint8_t mp3BdosCall(int32 value);
  uint8_t subMCUBdosCall(int32 value);

  // from TurboPascal 3 strings are 255 long max
  // & starts @ 1 ( 'cause @0 contains length)
  int getPascalStringFromRam(int32 addr, char* dest, int maxLen) {
      memset(dest, 0x00, maxLen);
      uint8_t *F = (uint8_t*)_RamSysAddr(addr);

      uint8_t len = F[0]; // seems to be init len (not strlen)
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
            yatl.getScreen()->consoleColorSet(); 
         } else {
            // set as old monochrome LCD (no backlight) 
            yatl.getScreen()->consoleColorSet( rgb(126, 155, 125), rgb(69,80,110), rgb(108-30,120-30,195-30) );
         }
     } else if ( a0 == 1 ) {
         // set the console font size & cols mode
         if ( a1 == 0 ) {
            // to reset
            yatl.getScreen()->consoleSetMode(LCD_CONSOLE_80_COLS, true);
         } else {
            // set as 53 cols (big font)
            yatl.getScreen()->consoleSetMode(LCD_CONSOLE_40_COLS, true);
         }
     }
     return 0;
  }

  int32 BdosTest229(int32 value) {
    yatl.dbug("/===== BDos 229 call =====\\");

    yatl.getScreen()->grabbSpritesOfSize( (char*)"SPRITE1.BMP", 0, 0, 32, 32);
    sprites[0].drawClip(0,20);
    sprites[1].drawClip(50,20);
    sprites[2].drawClip(100,20);
    sprites[3].drawClip(150,20);

    yatl.getScreen()->cleanSprites();

    sprites[0].setBounds(0,1,19,19);
    sprites[1].setBounds(40,1,19,19);
    sprites[2].setBounds(40,20,19,19);

    sprites[3].setBounds(62,1,31,28); // FILE
    sprites[4].setBounds(95,1,31,28); // FOLDER

    yatl.getScreen()->grabbSprites( (char*)"SPRITE1.BMP", 0, 0);

    yatl.getScreen()->fillRect(0, 120, 80+19, 64+19, 65535);
    sprites[0].drawClip(0,120);
    sprites[1].drawClip(80,120);
    sprites[2].drawClip(80,120+64);
    sprites[3].drawClip(0+10,120+20+10);
    sprites[4].drawClip(0+10+32,120+20+10);

    // char test[32+1];
    // int len = getStringFromRam(value, test, 32+1);
    // Serial.print("TP3 len= ");Serial.print(len);Serial.print("\n");
    // int i=1;
    // Serial.print("OpT ");Serial.print( (int)test[i++] );Serial.print("\n"); // 1
    // Serial.print("Sht ");Serial.print( (int)test[i++] );Serial.print("\n");
    // Serial.print("Fll ");Serial.print( (int)test[i++] );Serial.print("\n");

    // Serial.print("cb0 ");Serial.print( (int)test[i++] );Serial.print("\n"); // 4
    // Serial.print("cb1 ");Serial.print( (int)test[i++] );Serial.print("\n");

    // Serial.print("xb0 ");Serial.print( (int)test[i++] );Serial.print("\n"); // 6
    // Serial.print("xb1 ");Serial.print( (int)test[i++] );Serial.print("\n");

    // Serial.print("yb0 ");Serial.print( (int)test[i++] );Serial.print("\n"); // 8
    // Serial.print("yb1 ");Serial.print( (int)test[i++] );Serial.print("\n");

    // Serial.print("wb0 ");Serial.print( (int)test[i++] );Serial.print("\n"); // 10
    // Serial.print("wb1 ");Serial.print( (int)test[i++] );Serial.print("\n");

    // Serial.print("hb0 ");Serial.print( (int)test[i++] );Serial.print("\n"); // 12
    // Serial.print("hb1 ");Serial.print( (int)test[i++] );Serial.print("\n");
    return 0;
  }

  int32 drawRoutine(char* test) {
    // then draw it !!!!
    uint8_t OpType = test[1];
    uint8_t shapeType = test[2];
    uint8_t fillType = test[3]; // 0 draw / 1 fill

    uint16_t color = (uint16_t) ((uint8_t)test[4] << 8) + (uint8_t)test[5];
    color = mapColor( color );

    if ( OpType == 0x7F ) {
      // drawShapes
      uint16_t x = (uint16_t) ((uint8_t)test[6] << 8) + (uint8_t)test[7];
      uint16_t y = (uint16_t) ((uint8_t)test[8] << 8) + (uint8_t)test[9];
      if ( shapeType == 0x01 ) {
        // Shape : rectangle
        uint16_t w = (uint16_t) ((uint8_t)test[10] << 8) + (uint8_t)test[11];
        uint16_t h = (uint16_t) ((uint8_t)test[12] << 8) + (uint8_t)test[13];
        if ( fillType == 0x00 ) {
          // draw outlines
          yatl.getScreen()->drawRect( x, y, w, h, color );
        } else if ( fillType == 0x01 ) {
          // fills the rect

// if ( y > 220 ) {
// char str[64]; sprintf(str,"drawRect(%d,%d,%d,%d,%d)", x, y, w, h, color);
// yatl.dbug( (const char*)str );
// }

          yatl.getScreen()->fillRect( x, y, w, h, color );
        }
      } else if ( shapeType == 0x02 ) {
        // Shape : circle
        uint16_t r = (uint16_t)((uint8_t)test[10] << 8) + (uint8_t)test[11];
        if ( fillType == 0x00 ) {
          // draw outlines
          yatl.getScreen()->drawCircle( x,y,r, color );
        } else {
          yatl.getScreen()->fillCircle( x,y,r, color );
        }
      } else if ( shapeType == 0x03 ) {
        // Shape : line
        uint16_t x2 = (uint16_t) ((uint8_t)test[10] << 8) + (uint8_t)test[11];
        uint16_t y2 = (uint16_t) ((uint8_t)test[12] << 8) + (uint8_t)test[13];
        yatl.getScreen()->drawLine( x, y, x2, y2, color );
      } 
    } else if ( OpType == 0x80 ) {
      // manage Sprite
      uint16_t x = (uint16_t)((uint8_t)test[6] << 8) + (uint8_t)test[7];
      uint16_t y = (uint16_t)((uint8_t)test[8] << 8) + (uint8_t)test[9];

      if ( shapeType == 0x01 ) {
        // define sprite

        uint16_t w = (uint16_t)((uint8_t)test[10] << 8) + (uint8_t)test[11];
        uint16_t h = (uint16_t)((uint8_t)test[12] << 8) + (uint8_t)test[13];

        uint8_t num = test[14];

        sprites[num].setBounds( x, y, w, h );
      } else if ( shapeType == 0x02 ) {
        // draw sprite
        uint8_t num = test[10];

        sprites[num].drawClip( x, y );
      }

    }

    return 0;
  }

  extern void drawImgFromPAK(char* filename, int x, int y, int numInPak);

  char v_memseg[256];
  int32 XtsBdosCall(uint8 regNum, int32 value) {
    if ( regNum == 225 ) {

      memset( v_memseg, 0x00, 256 );
      getStringFromRam(value, v_memseg, 256);


      if ( (unsigned char)v_memseg[1] >= 0x7F ) {
        return drawRoutine( v_memseg );
      }

      char test[256];
      memset( test, 0x00, 256 );
      memcpy( &test[0], &v_memseg[0], 256 );


      // yatl.dbug("/===== BDos PString call =====\\");
      // yatl.dbug(test);

      upper(test);

      if ( endsWith(test, (char*)".BMP") ) {
        if ( test[0] == '!' ) {
          // yatl.dbug("|  Wanna grabb a BMP SpriteBoard |");
          yatl.getScreen()->cleanSprites();
          yatl.getScreen()->grabbSprites( &test[1], 0, 0 );
        } else {
          // yatl.dbug("|  Wanna draw a BMP wallpaper |");
          yatl.getScreen()->drawWallpaper( test );
        }

      } else if ( endsWith(test, (char*)".PAK") ) {
        // yatl.dbug("|  Wanna draw a PAK image |");

        int numImg = (int)test[0]-1; // 1 based
        int x = -1;
        int y = -1; // centered

        char* filename = &test[1];
        int tmp, lastTmp;
        if ( (tmp = indexOf( test, ',', 1 )) > -1 ) {
          char xx[ tmp - 1 ]; // -1 cf frameNum#
          memcpy(xx, &test[1], tmp-1);
          x = atoi(xx);
          lastTmp = tmp;
          tmp = indexOf( test, ',', tmp+1 );
          if ( tmp > -1 ) {
            char yy[ tmp - lastTmp ];
            memcpy(yy, &test[lastTmp+1], tmp-lastTmp);
            y = atoi(yy);
          }
          filename = &test[tmp+1];
        }

        char msg[128];
        sprintf(msg, "PAK:%s @%d,%d (%s)", filename, x, y, &test[1] );
        // yatl.dbug(msg);

        drawImgFromPAK( yatl.getFS()->getAssetsFileEntry( filename ), x, y, numImg );

      } else if ( endsWith(test, (char*)".PCT") ) {
        yatl.dbug("|  Wanna draw a PCT wallpaper |");
        yatl.dbug("|  NYI                        |");
      } else if ( endsWith(test, (char*)".BPP") ) {
        yatl.dbug("|  Wanna draw a BPP wallpaper |");
        yatl.dbug("|  NYI                        |");
      } else {
        yatl.dbug("| Wanna draw a ");
        yatl.dbug( test );
        printf( "[ %d, %d ]", (int)test[0], (int)test[1] );
        yatl.dbug(" -type wallpaper? |");
      }

      // yatl.dbug("\\===== BDos PString call =====/");
    } else if ( regNum == 226 ) {
     return xbdos_console(value);
    } else if ( regNum == 227 ) {
     return mp3BdosCall(value);
    } else if ( regNum == 228 ) {
      return subMCUBdosCall(value);
    } else if ( regNum == 229 ) {
      yatl.dbug( "BdosCall 229 NYI => Test Mode" );
      BdosTest229(value);
    }
    
    return 0;
  }

  // ==============] Deep Hardware Control [==========
  uint8_t subMCUBdosCall(int32 value) {
      // Serial.println("bridge Bdos call");
      uint8_t hiB = HIGH_REGISTER(value);
      if ( hiB == 0 ) {
        // yatl.getSubMCU()->cleanBuffer();
        uint8_t volts = (uint8_t)( yatl.getPWRManager()->getVoltage() * 256.0f / 5.0f );
        return volts;
      } else if ( hiB == 1 ) {
        yatl.getPWRManager()->reset();
      } else if ( hiB == 2 ) {
        yatl.getPWRManager()->deepSleep();
      } else if ( hiB == 3 ) {
        uint8_t loB = LOW_REGISTER(value);
        yatl.getLEDs()->mask(loB);
        // bool r = false;
        // bool g = false;
        // bool b = false;
        // if ( (loB & 128) == 128 ) { r = true; }
        // if ( (loB & 64) == 64 ) { r = true; }
        // if ( (loB & 32) == 32 ) { r = true; }
        // if ( (loB & 16) == 16 ) { r = true; }
        // if ( (loB & 8) == 8 ) { r = true; }
        // if ( (loB & 4) == 4 ) { r = true; }
        // if ( (loB & 2) == 2 ) { g = true; }
        // if ( (loB & 1) == 1 ) { b = true; }
        // yatl.getLEDs()->red(r);
        // yatl.getLEDs()->green(g);
        // yatl.getLEDs()->blue(b);
      } else if ( hiB == 4 ) {
        return yatl.getFS()->downloadFromSerial() ? 1 : 0;
      } else if ( hiB == 5 ) {
        return yatl.getFS()->downloadFromSubMcu() ? 1 : 0;
      } // Wifi Device calls -> 64+
      else if ( hiB == 64 ) {
        // Start the telnet server in APmode
        // See later for better

        // just to ensure WiFi will run...
        // Serial.println("Wasting IP");
        yatl.getWiFi()->getIP();
        // Serial.println("Closing WiFi");
        yatl.getWiFi()->close();

        // Serial.println("Opening WiFi APmode");
        int ok = yatl.getWiFi()->beginAP();
        if ( ok <= 0 ) {
          _puts("(!!) Wifi has not started !\n");
          return 0;
        } else {
          _puts("(ii) Wifi has started :");
          _puts( (const char*) yatl.getWiFi()->getIP() );
          _puts(" !\n");
        }
        ok = yatl.getWiFi()->startTelnetd();
        if ( ok <= 0 ) {
          _puts("(!!) Telnetd has not started !\n");
          return 0;
        } else {
          _puts("(ii) Telnetd has started :23 !\n");
        }

        return 1;
      }

      return 0;
  }

  // ==============] mp3 Hardware Control [==========
  uint8_t mp3BdosCall(int32 value) {
      // yatl.dbug("mp3 Bdos call");

      uint8_t a0 = HIGH_REGISTER(value);
      uint8_t a1 = LOW_REGISTER(value);

      if ( a0 >= (1 << 6) ) {
         // 11000000 -> 11 play+loop -> 64(10)
         // still 16000 addressable songs
         bool loopMode = a0 >= (1 << 7);

         if ( a0 >= 128 ) { a0 -= 128; }

         a0 -= 64;
         int trkNum = (a0<<8) + a1;

         if ( loopMode ) { yatl.getMusicPlayer()->loop(trkNum); }
         else { yatl.getMusicPlayer()->play(trkNum); }
      } else if (a0 == 0x00) {
          yatl.getMusicPlayer()->stop();
      } else if (a0 == 0x01) {
          yatl.getMusicPlayer()->pause();
      } else if (a0 == 0x02) {
          yatl.getMusicPlayer()->next();
      } else if (a0 == 0x03) {
          yatl.getMusicPlayer()->prev();
      } else if (a0 == 0x04) {
          yatl.getMusicPlayer()->volume( a1 );
      } else if (a0 == 0x05) {
          // for now : just for demo
          yatl.getMusicPlayer()->play( 65 );
      } else if (a0 == 0x06) {
          return yatl.getMusicPlayer()->isPlaying() ? 1 : 0;
      }

    return 0;
  }


#endif