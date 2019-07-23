#ifndef ARDUINO
/**
 * Xtase - fgalliat @Jul2019
 * 
 * implementaion of Yatl software VT-Ext console (Arduino & cpp Version)
 * VT100 + Ext protocols & routines
 * 
 */

#include "xts_yatl_dev_console.h"

  
//   // forward symbols
//   void _setConsoleMode(int mode);

//   void drawBmp(char* filename, bool screenRotate);



  // === yatl additional routines ============
   // void YatlScreen::write(char ch) {
   //    consoleWrite(ch);
   // }
//   void YatlScreen::cls() {
//      consoleCls(true);
//   }


   void YatlScreen::consoleColorSet(uint16_t bg, uint16_t fg, uint16_t acc) {
      ::consoleColorSet(bg, fg, acc);
   }

   void YatlScreen::consoleSetMode(uint8_t columnMode, bool rerenderFull) {
      ::_setConsoleMode(columnMode);
      if (rerenderFull) ::consoleRenderFull();
   }

   void YatlScreen::println(const char* str) {
      this->println( (char*)str );
   }

   void YatlScreen::println(char* str) {
      while (*str)
              consoleWrite(*(str++));
      consoleWrite('\n');
   }



  // === TFT direct routines ============
   #define FONT_NORMAL 0
   #define FONT_TINY 1

      void __setTextColor(uint16_t color);
      void __setFont(int fontMode);
      void __setCursor(int xPx, int yPx);
      void __clearScreen(uint16_t bgColor);
      void __fillRect(int x, int y, int w, int h, uint16_t bgColor);
      void __fillPixelRect(int x, int y, int w, int h, uint16_t* bgColor);
      void __write1char(char ch); // tft.write(ch)

  // ==========] Console Routines [==========

   void _doPlay(char* sequence) {
      int slen = strlen( sequence );

      // Serial.println("Playing :");
      // Serial.println(sequence);

      if ( slen <= 2 ) { 
         yatl.getBuzzer()->playTuneString(sequence); 
         return; 
      }

      char ch = sequence[ slen-1  ];
      ch = charUpCase(ch);

      if ( (ch == '3' || ch == 'K') && sequence[ slen-2  ] == '5' ) {
         yatl.getBuzzer()->playTuneFile(sequence);
      }  else {
         yatl.getBuzzer()->playTuneString(sequence);
      }
   }

   // playInParallel Not Supported
   void playVTMusic(char* tuneStr, bool playInParallel) {
      if ( strlen( tuneStr ) <= 0 ) { return; }
      _doPlay(tuneStr);
   }

   #define TTY_FONT_WIDTH 4
   #define TTY_FONT_HEIGHT 6
  
   // 53 - 80 if tinyFont
   #define TTY_CON_WIDTH  ( TFT_WIDTH / TTY_FONT_WIDTH )
   // 30 - 40 if tinyFont
   #define TTY_CON_HEIGHT ( TFT_HEIGHT / TTY_FONT_HEIGHT )
   // 1590 - 3200 if tinyFont
   #define TTY_CON_SIZE ( TTY_CON_WIDTH * TTY_CON_HEIGHT )

   #define COLORED_CONSOLE 1
   #define LCD_TINYFONT 1

   #ifdef COLORED_CONSOLE
      char ttyConsoleAttrs[ TTY_CON_SIZE ];
      char curTextAttr = 0x00;
   #endif

   char ttyConsoleFrame[ TTY_CON_SIZE ];
   // just to prevent from recomputing each time
   const int ttyConsoleFrameSize = TTY_CON_SIZE;
   const int ttyConsoleWidth     = TTY_CON_WIDTH;
   const int ttyConsoleHeight    = TTY_CON_HEIGHT;

   int consoleCursorX = 0;
   int consoleCursorY = 0;

   int consoleMode = LCD_CONSOLE_DEF_COLS;
   int consoleCurrentFontHeight = -1;
   int consoleCurrentFontWidth  = -1;

   // one of LCD_CONSOLE_xx_COLS
   void _setConsoleMode(int mode) {
      consoleMode = mode;
      if ( mode == LCD_CONSOLE_80_COLS ) {
         consoleCurrentFontHeight = 5+1;
         consoleCurrentFontWidth  = 3+1;
         __setFont( FONT_TINY );
      } else {
         __setFont( FONT_NORMAL );
         consoleCurrentFontHeight = 8;
         consoleCurrentFontWidth  = 6;
      }
   }

   void _consoleSetCursor(int col, int row) {
      consoleCursorX = col;
      consoleCursorY = row;
      __setCursor(consoleCursorX * consoleCurrentFontWidth, consoleCursorY * consoleCurrentFontHeight);
   }

   void _consoleFill(char ch, bool resetCursor) {
      #ifdef COLORED_CONSOLE
         curTextAttr = 0x00;
         memset(ttyConsoleAttrs, curTextAttr, ttyConsoleFrameSize);
      #endif
      memset(ttyConsoleFrame, ch, ttyConsoleFrameSize);
      if (resetCursor) {
         consoleCursorX = 0;
         consoleCursorY = 0;
      }

      /* - test purposes - 
      for(int i=0; i<ttyConsoleWidth; i++) {
         ttyConsoleFrame[i] = '0'+( i % 10 );
      }
      for(int i=0; i<ttyConsoleHeight; i++) {
         ttyConsoleFrame[(i*ttyConsoleWidth)+0] = '0'+( i % 10 );
      }
      */
   }

  

   uint16_t TTY_COLOR_BG = CLR_BLACK;
   uint16_t TTY_COLOR_FG = CLR_WHITE;
   uint16_t TTY_COLOR_ACCENT = CLR_GREEN;

   uint16_t mapColor(uint16_t color) {
      if ( color >= 16 ) { return color; }
      
      if ( color == 0 ) { return CLR_BLACK; }
      if ( color == 1 ) { return CLR_WHITE; }
      if ( color == 2 ) { return CLR_RED; }
      if ( color == 3 ) { return CLR_GREEN; }
      if ( color == 4 ) { return CLR_BLUE; }
      if ( color == 5 ) { return CLR_YELLOW; }
      if ( color == 6 ) { return CLR_PURPLE; }
      if ( color == 7 ) { return CLR_CYAN; }
      if ( color == 8 ) { return CLR_ORANGE; }
      if ( color == 9 ) { return CLR_MAGENTA; }

      return CLR_PINK;
   }


   void consoleColorSet(uint16_t bg, uint16_t fg, uint16_t acc) {
      TTY_COLOR_BG = bg;
      TTY_COLOR_FG = fg;
      TTY_COLOR_ACCENT = acc;
   }

   void consoleCls(bool clearDisplay) {
      _consoleFill(0x00, true);
      if (clearDisplay) { __clearScreen(TTY_COLOR_BG); }
      _consoleSetCursor(0,0);
   }

   char _c_line[ ttyConsoleWidth + 1 ];

   #define TTY_ATTR_NORMAL   0x00
   #define TTY_ATTR_ACCENT   0x01
   #define TTY_ATTR_ACCENT2  0x02
   #define TTY_ATTR_INVVIDEO 0x03

   void _consoleRenderOneLine(int row, bool clearArea) {
      if ( ttyConsoleFrame[ (row*ttyConsoleWidth)+0 ] == 0x00) {
         // do clear the area ????
         if ( clearArea ) {
            __fillRect(0, row*consoleCurrentFontHeight, TFT_WIDTH, consoleCurrentFontHeight, TTY_COLOR_BG);
         }
      }

      memset(_c_line, 0x00, ttyConsoleWidth+1);
      memcpy(_c_line, &ttyConsoleFrame[ (row*ttyConsoleWidth)+0 ], ttyConsoleWidth );

      #ifdef COLORED_CONSOLE
         // beware if not clearDisplay
         __setCursor(0, row * consoleCurrentFontHeight);
         int c=0;
         while ( _c_line[c] != 0x00 ) {
            bool invVideo = ttyConsoleAttrs[ (row*ttyConsoleWidth)+c ] == TTY_ATTR_INVVIDEO;

            if ( ttyConsoleAttrs[ (row*ttyConsoleWidth)+c ] == TTY_ATTR_ACCENT ) {
               __setTextColor( TTY_COLOR_ACCENT );
            } else if ( ttyConsoleAttrs[ (row*ttyConsoleWidth)+c ] == TTY_ATTR_ACCENT2 ) {
               __setTextColor( CLR_YELLOW );
            } else {
               __setTextColor( TTY_COLOR_FG );
            }
         
            if ( invVideo ) {
               __fillRect(consoleCursorX * consoleCurrentFontWidth, consoleCursorY * consoleCurrentFontHeight, consoleCurrentFontWidth, consoleCurrentFontHeight, TTY_COLOR_FG);
               __setTextColor( TTY_COLOR_BG );
            }

            __write1char( _c_line[c] );

            c++;
            
            #ifdef LCD_TINYFONT
               // to reduce space between letters
               __setCursor(c * consoleCurrentFontWidth, row * consoleCurrentFontHeight);
            #endif
         }
      #else
         // beware if not clearDisplay
         tft.setCursor(0, row * consoleCurrentFontHeight);
         tft.print( line );
      #endif
   }

   void consoleRenderFull(bool clearDisplay) {
      if (clearDisplay) { __clearScreen(TTY_COLOR_BG); }
      consoleCursorX = 0;
      for(int y=0; y < ttyConsoleHeight; y++) {
         consoleCursorY = y;
         _consoleRenderOneLine(consoleCursorY, false);
      }
   }

   void _toggleConsoleMode(bool rerender) {
      if ( consoleMode == LCD_CONSOLE_80_COLS ) {
         consoleMode = LCD_CONSOLE_40_COLS;
      } else {
         consoleMode = LCD_CONSOLE_80_COLS;
      }
      _setConsoleMode(consoleMode);
      if (rerender) {
         consoleRenderFull();
      }
   }

   void _scrollUp() {
      /*
      try to use native text scroller -> too slow
      */

      #ifdef COLORED_CONSOLE
         memmove( &ttyConsoleAttrs[ 0 ], &ttyConsoleAttrs[ ttyConsoleWidth ], ttyConsoleFrameSize - ttyConsoleWidth );
         memset( &ttyConsoleAttrs[ ttyConsoleFrameSize - ttyConsoleWidth ], 0x00, ttyConsoleWidth );
      #endif

      memmove( &ttyConsoleFrame[ 0 ], &ttyConsoleFrame[ ttyConsoleWidth ], ttyConsoleFrameSize - ttyConsoleWidth );
      memset( &ttyConsoleFrame[ ttyConsoleFrameSize - ttyConsoleWidth ], 0x00, ttyConsoleWidth );
      consoleCursorX = 0;
      consoleCursorY = ttyConsoleHeight - 1;
      consoleRenderFull();
   }

   // erase from current position to EndOfLine
   void _eraseTillEOL(bool clearArea) {
      if ( clearArea ) {
         __fillRect(consoleCursorX*consoleCurrentFontWidth, consoleCursorY*consoleCurrentFontHeight, TFT_WIDTH, consoleCurrentFontHeight, TTY_COLOR_BG);
      }
      int addr = ( consoleCursorY * ttyConsoleWidth) + consoleCursorX;
      int len = (ttyConsoleWidth-consoleCursorX);
      #ifdef COLORED_CONSOLE
         memset( &ttyConsoleAttrs[ addr ], 0x00, len );
      #endif
      memset( &ttyConsoleFrame[ addr ], 0x00, len );
   }

   bool __escapeChar = false;
   char __escapeChar0 = 0x00;
   char __escapeChar1 = 0x00;
   char __escapeChar2 = 0x00;

   bool __escapeSeq = false; // VT100 escape sequence
   char vt100seq[16+1];

   bool __escapeMSeq = false; // VT-MUSIC escape sequence
   #define _VT_MUSIC_LEN 64
   char vtMUSICseq[_VT_MUSIC_LEN+1];

   const bool drawCursor = true;

   void drawGlyphSpace(uint16_t color, uint16_t color2) {
      int xx = consoleCursorX * consoleCurrentFontWidth;
      int yy = consoleCursorY * consoleCurrentFontHeight;
      __fillRect(xx, yy, consoleCurrentFontWidth, consoleCurrentFontHeight, color);
      char ch = ttyConsoleFrame[ (consoleCursorY*ttyConsoleWidth)+consoleCursorX ];
      if ( ch == 0x00 || ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t' || ch == '\b' ) {
         return;
      }
      // TODO read consoleCharAttrs for color & style
      __setTextColor( color2 );
      __setCursor(xx, yy);
      __write1char( ch );
      if (color2 != TTY_COLOR_FG) __setTextColor( TTY_COLOR_FG );
   }

   bool cursorLocked = false;
   bool cursorShown = false;

   void hideCursor() {
      if (cursorLocked) { return; }
      if (!cursorShown) { return; }

      drawGlyphSpace(TTY_COLOR_BG, TTY_COLOR_FG);
      cursorShown = false;
   }

   void showCursor() {
      if (cursorLocked) { return; }
      if (cursorShown) { return; }

      drawGlyphSpace(TTY_COLOR_FG, TTY_COLOR_BG);
      cursorShown = true;
   }

   void consoleWrite(char ch) {
      // use spe char to toggle console mode for now
      // 7F is 127 (console seems tobe 127 limited)
      if ( ch == 0x7F ) { _toggleConsoleMode(); return; }

      if ( ch == '\r' ) { hideCursor(); return; }

      if ( ch == '\n' ) { 
         hideCursor();
         consoleCursorX = 0;
         consoleCursorY++;
         if ( consoleCursorY >= ttyConsoleHeight ) {
            _scrollUp();
         }
         // showCursor();
         return; 
      }

      // is generally used as '\b'+' '+'\b' so no need to render it
      if ( ch == '\b' ) { 
         hideCursor();
         consoleCursorX--;
         if ( consoleCursorX < 0 ) {
            consoleCursorY--;
            if ( consoleCursorY < 0 ) {
               consoleCursorY = 0;
            }
         }
         // showCursor();
         return; 
      }

      // beware w/ '\t' & esc seqs 
      // seems that '\t' is not used ?

      // VT100 escapes
      if ( ch == 27 ) { 
         // escape sequence ex. ^B1 ^C1
         __escapeChar = true;
         __escapeChar0 = 0x00;
         __escapeChar1 = 0x00;
         __escapeChar2 = 0x00;

         __escapeSeq = false;
         memset( vt100seq, 0x00, 16+1 );

         __escapeMSeq = false;

      } else if ( ch == 26 ) { 
         // seems to be the CLS escape sequence
         // Serial.println("Esc:26 ????");
         consoleCls();
         return;
      } else if ( ch == 7 ) { 
         yatl.beep();
         return;
      }

      if ( __escapeChar ) {
         if ( __escapeChar0 == 0x00 ) {
            curTextAttr = 0x00;
            __escapeChar0 = ch;
         } else if ( __escapeChar1 == 0x00 ) {
            __escapeChar1 = ch;
            // Serial.print( "Esc:" );
            // Serial.print( (char)__escapeChar1 );

            if ( __escapeChar1 == 'C' ) { curTextAttr = 0x00; }
            else if ( __escapeChar1 == 'B' ) { curTextAttr = 0x01; }
            else if ( __escapeChar1 == '[' ) { __escapeSeq = true; }
            else if ( __escapeChar1 == '$' ) { 
               // vt-MUSIC mode
               __escapeMSeq = true;
               memset(vtMUSICseq, 0x00, _VT_MUSIC_LEN+1); 
            }
            else { curTextAttr = 0x00; }
         } else if ( __escapeChar2 == 0x00 && !__escapeSeq && !__escapeMSeq ) {
            __escapeChar2 = ch;
            // Serial.print( (char)__escapeChar2 );
         } else {
            
            // VT-MUSIC SUPPORT
            // ex. ^$aac#d! => plays "AAC#D"
            if ( __escapeMSeq ) {

               // bool playInParallel = !false;
               // SEEMS doesn't work for now
               bool playInParallel = false;

               if ( ch == '!'  ) {
                  __escapeChar = false;
                  playVTMusic(vtMUSICseq, playInParallel);
                  return;
               } else {
                  int l = strlen(vtMUSICseq);
                  if ( l >= _VT_MUSIC_LEN ) {
                     __escapeChar = false;
                     playVTMusic(vtMUSICseq, playInParallel);
                     return;
                  }
                  vtMUSICseq[ l ] = ch;
               }
            } else 

            if ( __escapeSeq ) {
               if ( ch >= 'A' && ch <= 'z'  ) {
                  vt100seq[ strlen(vt100seq) ] = ch;
                  // Serial.println( ch );
                  __escapeChar = false;

                  int slen = strlen(vt100seq);
                  if ( slen == 1 ) {
                     if ( ch == 'K' ) {
                        // ^[K
                        _eraseTillEOL();
                        return;
                     } else if ( ch == 'H' ) {
                        // ^[H
                        // return to Home
                        _consoleSetCursor(0,0);
                        return;
                     } else {
                        // Serial.print("a.");
                        // Serial.println( vt100seq );
                     }
                  } else if ( slen == 2 ) {
                     if ( ch == 'J' ) {
                        if ( vt100seq[0] == '2' ) {
                           // ^[2J
                           consoleCls();
                           return;
                        }
                     } else {
                        // Serial.print("b.");
                        // Serial.println( vt100seq );
                     }
                  } else {
                     if ( ch == 'H' ) {
                        // ^[<row>;<col>H
                        // set location
                        int row = 0;
                        int col = 0;
                        char rowStr[8];
                        char colStr[8];
                        memset( rowStr, 0x00, 8 );
                        memset( colStr, 0x00, 8 );
                        char _ch;
                        int _i,_j=0;
                        for(_i=0; _i < slen; _i++) {
                           _ch = vt100seq[_i];
                           if ( _ch == ';' ) { break; }
                           rowStr[_i] = _ch;
                        }
                        _i++;
                        for(; _i < slen; _i++) {
                           _ch = vt100seq[_i];
                           if ( _ch == 'H' ) { break; }
                           colStr[_j++] = _ch;
                        }
                        col = atoi(colStr) - 1;
                        row = atoi(rowStr) - 1;
                        _consoleSetCursor(col,row);
                        return;
                     } else {
                        // Serial.print("c.");
                        // Serial.println( vt100seq );
                     }
                  }

               } else {
                  vt100seq[ strlen(vt100seq) ] = ch;
               }

            } else {
               __escapeChar = false;
               // Serial.print("d.");
               // Serial.println( vt100seq );
            }
         }
      }

      if ( __escapeChar ) {
         return;
      }


      int consoleAddr = ( consoleCursorY * ttyConsoleWidth ) + consoleCursorX;

      #ifdef COLORED_CONSOLE
         ttyConsoleAttrs[ consoleAddr ] = curTextAttr;
      #endif

      char previousChar = ttyConsoleFrame[ consoleAddr ];
      // assign new char
      ttyConsoleFrame[ consoleAddr ] = ch;

      // direct render
      hideCursor();
      __setCursor(consoleCursorX * consoleCurrentFontWidth, consoleCursorY * consoleCurrentFontHeight);
      if ( ch == ' ' ) {
         if ( drawCursor || ( ! (previousChar == 0x00 || previousChar == ' ') ) ) {
            // render spaces Cf '\b'
            hideCursor();
         }
      } else {

         // if ( ch < 32 || ch >= 127 ) {
         //    Serial.write(ch);
         //    Serial.write(' ');
         //    Serial.print( (int)ch );
         //    Serial.write('/');
         // }

         if ( curTextAttr == 0x01 ) { __setTextColor( TTY_COLOR_ACCENT );  }
         else if ( curTextAttr == 0x02 ) { __setTextColor( CLR_YELLOW ); }
         else if ( curTextAttr == 0x00 ) { __setTextColor( TTY_COLOR_FG ); }

         // tft.write( __escapeChar1 ); // disp Esc char

         if (drawCursor) {
            hideCursor();
         }

         __write1char( ch );
         __escapeChar1 = 0x00;
      }

      consoleCursorX++;
      if ( consoleCursorX >= ttyConsoleWidth ) {
         consoleCursorY++;
         if ( consoleCursorY >= ttyConsoleHeight ) {
            _scrollUp();
         }
      }

      // draw cursor
      if (drawCursor) {
         // showCursor();
      }
   }




  // ==========] Draw Bmp Routines [==========
   void _drawBmp(char* filename, int x, int y);

   void drawBmp(char* filename, bool screenRotate) {
      // if (TFT_ROTATION == 3) {
      //    if ( screenRotate ) tft.setRotation(0);
      // } else {
      //    if ( screenRotate ) tft.setRotation(2);
      // }
      _drawBmp( filename, 0, 0 );
      // if ( screenRotate ) tft.setRotation(TFT_ROTATION);
   }

   #define BUFFPIXEL 80

   void _drawBmp(char* filename, int x, int y);

   #if SPRITES_SUPPORT

   uint16_t spriteArea[ SPRITE_AREA_SIZE ];
   Sprite sprites[NB_SPRITES];

   int spriteInstanceCounter = 0; 
   int lastAddr = 0;

   void Sprite::drawClip(int x, int y) {
      if ( w < 0 || h < 0 ) { return; }
      if ( !isValid() ) { return; }

      uint16_t row[ w ];
      for(int i=0; i < h; i++) {
         if ( i+y >= TFT_HEIGHT ) { break; }
         // *2 cf uint16_t
         memcpy( &row[0], &spriteArea[ ( (this->y+i) * SPRITE_AREA_WIDTH )+this->x ], w*2 );
         __fillPixelRect(x, i+y, w, 1, row);
      }
   }
   
    void cleanSprites() {
       memset(spriteArea, 0, SPRITE_AREA_SIZE);
       spriteInstanceCounter = 0; 
       lastAddr = 0;
       for(int i=0; i < NB_SPRITES; i++) {
          sprites[i].invalid();
       }
    }

    void _feedSprites(char* filename, int x, int y);


    void grabbSprites(char* imageName, int offsetX, int offsetY) {
       char* fileName = yatl.getFS()->getAssetsFileEntry( imageName );
       _feedSprites(fileName, offsetX, offsetY);
    }

    void grabbSpritesOfSize(char* imageName, int offsetX, int offsetY, int width, int height) {

      //  char* fileName = this->yatl->getFS()->getAssetsFileEntry( imageName );
      char* fileName = yatl.getFS()->getAssetsFileEntry( imageName );

       cleanSprites();
       int nbW = 160/width;
       int nbH = 120/height;
       int howMany = nbW * nbH;
       if ( howMany > NB_SPRITES ) { howMany = NB_SPRITES; }
       int cpt = 0;
       for(int y=0; y < nbH; y++) {
         for(int x=0; x < nbW; x++) {
            sprites[cpt].x = x*width;
            sprites[cpt].y = y*height;
            sprites[cpt].w = width;
            sprites[cpt].h = height;
            cpt++;
            if ( cpt >= howMany ) { break; }
         }
       }
       _feedSprites(fileName, offsetX, offsetY);
    }



   // will takes only 160x120 px of bmp file
   void _feedSprites(char* filename, int x, int y);
   #endif

   // filename is "/Z/0/packXXX.pak"
   // reads&display image #numInPak of packed image from filename
   void drawImgFromPAK(char* filename, int x, int y, int numInPak);

#endif
