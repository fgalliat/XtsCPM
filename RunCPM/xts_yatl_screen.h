#ifndef YATL_SCREEN_H_
#define YATL_SCREEN_H_ 1

/**
 * Xtase YATL Screen interface
 * Xtase - fgalliat @Apr2019
 */

#ifdef USE_BUILTIN_LCD

    // tty console emulation
    #define LCD_MODE_CONSOLE 1

#define LCD_TINYFONT 1

#ifdef LCD_TINYFONT
   // tiny 3x5 monospaced ? TTY_FONT_HEIGHT
   #include "font_pzim3x5.h"
#endif


    #include "SPI.h"
    #include "ILI9341_t3n.h"
    // w/ that lib BOTH CS & DC needs to besome CS pins
    #define _TFT_DC  20
    #define _TFT_CS  15
    // #define TFT_SCK 14 // doesn't work !!!!
    #define TFT_SCK 13

    #define TFT_MOSI 11
    #define TFT_MISO 12
    #define TFT_RST  17
    #define TFT_LED  16

    ILI9341_t3n tft = ILI9341_t3n(_TFT_CS, _TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO);

    // in pixels
    #define TFT_WIDTH 320
    #define TFT_HEIGHT 240


    #ifdef LCD_MODE_CONSOLE
      // char bounds
      #ifdef LCD_TINYFONT
         #define TTY_FONT_WIDTH 4
         #define TTY_FONT_HEIGHT 6
      #else
         #define TTY_FONT_WIDTH 6
         #define TTY_FONT_HEIGHT 8
      #endif
      // 53 - 80 if tinyFont
      #define TTY_CON_WIDTH  ( TFT_WIDTH / TTY_FONT_WIDTH )
      // 30 - 40 if tinyFont
      #define TTY_CON_HEIGHT ( TFT_HEIGHT / TTY_FONT_HEIGHT )
      // 1590 - 3200 if tinyFont
      #define TTY_CON_SIZE ( TTY_CON_WIDTH * TTY_CON_HEIGHT )

      #define COLORED_CONSOLE 1

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

      #define LCD_CONSOLE_40_COLS 0x00
      #define LCD_CONSOLE_80_COLS 0x01
      #ifdef LCD_TINYFONT
        #define LCD_CONSOLE_DEF_COLS LCD_CONSOLE_80_COLS
      #else
        #define LCD_CONSOLE_DEF_COLS LCD_CONSOLE_40_COLS
      #endif
      int consoleMode = LCD_CONSOLE_DEF_COLS;
      int consoleCurrentFontHeight = -1;
      int consoleCurrentFontWidth  = -1;

      // one of LCD_CONSOLE_xx_COLS
      void _setConsoleMode(int mode) {
         #ifndef LCD_TINYFONT
           if ( mode == LCD_CONSOLE_80_COLS ) {
              mode = LCD_CONSOLE_40_COLS;
           }
         #endif

         consoleMode = mode;
         if ( mode == LCD_CONSOLE_80_COLS ) {
            consoleCurrentFontHeight = 5+1;
            consoleCurrentFontWidth  = 3+1;
            #ifdef LCD_TINYFONT
               // type : ILI9341_t3_font_t 
               tft.setFont( Pixelzim_8 );
            #else
               tft.setFont();
            #endif
         } else {
            tft.setFont();
            consoleCurrentFontHeight = 8;
            consoleCurrentFontWidth  = 6;
         }
      }

      void _consoleSetCursor(int col, int row) {
         consoleCursorX = col;
         consoleCursorY = row;
         tft.setCursor(consoleCursorX * consoleCurrentFontWidth, consoleCursorY * consoleCurrentFontHeight);
      }

      void _consoleFill(char ch, bool resetCursor=true) {
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

      void consoleCls(bool clearDisplay=true) {
         _consoleFill(0x00, true);
         if (clearDisplay) { tft.fillScreen(ILI9341_BLACK); }
         _consoleSetCursor(0,0);
      }

      char _c_line[ ttyConsoleWidth + 1 ];

      void _consoleRenderOneLine(int row, bool clearArea=true) {
         if ( ttyConsoleFrame[ (row*ttyConsoleWidth)+0 ] == 0x00) {
            // do clear the area ????
            if ( clearArea ) {
               tft.fillRect(0, row*consoleCurrentFontHeight, TFT_WIDTH, consoleCurrentFontHeight, ILI9341_BLACK);
            }
         }

         memset(_c_line, 0x00, ttyConsoleWidth+1);
         memcpy(_c_line, &ttyConsoleFrame[ (row*ttyConsoleWidth)+0 ], ttyConsoleWidth );

         #ifdef COLORED_CONSOLE
            // beware if not clearDisplay
            tft.setCursor(0, row * consoleCurrentFontHeight);
            int c=0;
            while ( _c_line[c] != 0x00 ) {
               if ( ttyConsoleAttrs[ (row*ttyConsoleWidth)+c ] == 0x01 ) {
                  tft.setTextColor( ILI9341_GREEN );
               } else if ( ttyConsoleAttrs[ (row*ttyConsoleWidth)+c ] == 0x02 ) {
                  tft.setTextColor( ILI9341_YELLOW );
               } else {
                  tft.setTextColor( ILI9341_WHITE );
               }
            
               tft.write( _c_line[c] );

               c++;
               
               #ifdef LCD_TINYFONT
                  // to reduce space between letters
                  tft.setCursor(c * consoleCurrentFontWidth, row * consoleCurrentFontHeight);
               #endif
            }
         #else
            // beware if not clearDisplay
            tft.setCursor(0, row * consoleCurrentFontHeight);
            tft.print( line );
         #endif


      }

      void consoleRenderFull(bool clearDisplay=true) {
         if (clearDisplay) { tft.fillScreen(ILI9341_BLACK); }
         consoleCursorX = 0;
         for(int y=0; y < ttyConsoleHeight; y++) {
            consoleCursorY = y;
            _consoleRenderOneLine(consoleCursorY, false);
         }
      }

      void consoleRenderPartial() {
         y_dbug("XTS: CON: RenderPartial TODO");
      }

      void _toggleConsoleMode(bool rerender=true) {
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
         // y_dbug("XTS: CON: ScrollUp TODO");
         // ?????
         // tft.setScroll( ttyConsoleWidth * TTY_FONT_HEIGHT );
         /*
         try to use native text scroller -> too slow
         consoleCursorY = ttyConsoleHeight - 0;
         tft.setCursor(0, consoleCursorY * TTY_FONT_HEIGHT);
         tft.println("COUCOU");
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
      void _eraseTillEOL(bool clearArea=true) {
         if ( clearArea ) {
            tft.fillRect(consoleCursorX*consoleCurrentFontWidth, consoleCursorY*consoleCurrentFontHeight, TFT_WIDTH, consoleCurrentFontHeight, ILI9341_BLACK);
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

      void consoleWrite(char ch) {
         // use spe char to toggle console mode for now
         // 7F is 127 (console seems tobe 127 limited)
         if ( ch == 0x7F ) { _toggleConsoleMode(); return; }

         if ( ch == '\r' ) { return; }

         if ( ch == '\n' ) { 
            consoleCursorX = 0;
            consoleCursorY++;
            if ( consoleCursorY >= ttyConsoleHeight ) {
               _scrollUp();
            }
            return; 
         }

         // is generally used as '\b'+' '+'\b' so no need to render it
         if ( ch == '\b' ) { 
            consoleCursorX--;
            if ( consoleCursorX < 0 ) {
               consoleCursorY--;
               if ( consoleCursorY < 0 ) {
                  consoleCursorY = 0;
               }
            }
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

         } else if ( ch == 26 ) { 
            // seems to be the CLS escape sequence
            // Serial.println("Esc:26 ????");
            consoleCls();
            return;
         } else if ( ch == 7 ) { 
            beep();
            return;
         }

         if ( __escapeChar ) {
            if ( __escapeChar0 == 0x00 ) {
               curTextAttr = 0x00;
               __escapeChar0 = ch;
            } else if ( __escapeChar1 == 0x00 ) {
               __escapeChar1 = ch;
               Serial.print( "Esc:" );
               //Serial.println( (char)__escapeChar1 );
               Serial.print( (char)__escapeChar1 );

               if ( __escapeChar1 == 'C' ) { curTextAttr = 0x00; }
               else if ( __escapeChar1 == 'B' ) { curTextAttr = 0x01; }
               else if ( __escapeChar1 == '[' ) { __escapeSeq = true; }
               else { curTextAttr = 0x00; }
            } else if ( __escapeChar2 == 0x00 && !__escapeSeq ) {
               __escapeChar2 = ch;
               Serial.print( (char)__escapeChar2 );
            } else {
               
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
                        }
                     } else if ( slen == 2 ) {
                        if ( ch == 'J' ) {
                           if ( vt100seq[0] == '2' ) {
                              // ^[2J
                              consoleCls();
                              return;
                           }
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
                        }
                     }

                     Serial.println( vt100seq );

                  } else {
                     vt100seq[ strlen(vt100seq) ] = ch;
                     // Serial.print( ch );
                  }

               } else {
                  __escapeChar = false;
                  Serial.println(  );
               }
            }
         }

         if ( __escapeChar ) {
            return;
         }


         #ifdef COLORED_CONSOLE
           ttyConsoleAttrs[ ( consoleCursorY * ttyConsoleWidth ) + consoleCursorX ] = curTextAttr;
         #endif
         ttyConsoleFrame[ ( consoleCursorY * ttyConsoleWidth ) + consoleCursorX ] = ch;

         // direct render
         tft.setCursor(consoleCursorX * consoleCurrentFontWidth, consoleCursorY * consoleCurrentFontHeight);
         if ( ch == ' ' ) {
            // render spaces Cf '\b'
            tft.fillRect(consoleCursorX * consoleCurrentFontWidth, consoleCursorY * consoleCurrentFontHeight, consoleCurrentFontWidth, consoleCurrentFontHeight, ILI9341_BLACK);
         } else {

            if ( ch < 32 || ch >= 127 ) {
               Serial.write(ch);
               Serial.write(' ');
               Serial.print( (int)ch );
               Serial.write('/');
            }

            if ( curTextAttr == 0x01 ) { tft.setTextColor( ILI9341_GREEN );  }
            else if ( curTextAttr == 0x02 ) { tft.setTextColor( ILI9341_YELLOW ); }
            else if ( curTextAttr == 0x00 ) { tft.setTextColor( ILI9341_WHITE ); }

            // tft.write( __escapeChar1 ); // disp Esc char

            tft.write( ch );
            __escapeChar1 = 0x00;
         }

         consoleCursorX++;
         if ( consoleCursorX >= ttyConsoleWidth ) {
            consoleCursorY++;
            if ( consoleCursorY >= ttyConsoleHeight ) {
               _scrollUp();
            }
         }
      }

      void _consoleTest() {
         _consoleFill('A');
         consoleRenderFull();
         _consoleFill('b');
         consoleRenderFull();
         _consoleFill('X');
         consoleRenderFull();
         _consoleFill('M');
         consoleRenderFull();

         _scrollUp();
         _scrollUp();
         _scrollUp();

         consoleWrite('H');
         consoleWrite('e');
         consoleWrite('l');
         consoleWrite('l');
         consoleWrite('o');
         consoleWrite(' ');
         consoleWrite('W');
         consoleWrite('l');
         consoleWrite('d');

         consoleWrite('\n');

         consoleWrite('H');
         consoleWrite('e');
         consoleWrite('l');
         consoleWrite('l');
         consoleWrite('o');
         consoleWrite(' ');
         consoleWrite('W');
         consoleWrite('l');
         consoleWrite('d');
      }

    #endif


#endif

 class Screen {
     public:
       Screen() { ; }
       ~Screen() { ; }

       void cls() {
#ifndef USE_BUILTIN_LCD
           y_dbug( "XTS: Screen cls" );
#else
         #ifdef LCD_MODE_CONSOLE
           consoleCls();
         #else
           tft.fillScreen(ILI9341_BLACK);
           tft.setCursor(0, 0);
         #endif
#endif
       }

       void write(char ch) { 
          #ifdef LCD_MODE_CONSOLE
            consoleWrite(ch);
          #else
            tft.write( ch ); 
          #endif
       }

       void println(const char* str) { this->println( (char*)str ); }

       void println(char* str) {
#ifndef USE_BUILTIN_LCD
           y_dbug( "XTS: -> \\" );
           y_dbug( str );
           y_dbug( "XTS: -> /" );
#else
          #ifdef LCD_MODE_CONSOLE
          	while (*str)
              consoleWrite(*(str++));
              consoleWrite('\n');
          #else
           tft.print(str);
           tft.print('\n');
          #endif
#endif
       }

       int getWidth() { return 320; }
       int getHeight() { return 240; }

       int getTTYWidth() { return this->getWidth() / 6; } // 53
       int getTTYHeight() { return this->getHeight() / 8; } // 30

       // in TTY char unit (not pixels)
       void setCursor(int x, int y) {
#ifndef USE_BUILTIN_LCD
#else
         tft.setCursor(x*6, (y+0)*8); 
#endif
       }

 };





 // declared in xts_yatl.h
 void setupArduinoScreen() {
#ifndef USE_BUILTIN_LCD
      y_dbug("XTS: setupArduinoScreen -> TODO");
#else
   // ensure TFT backlight
   pinMode(TFT_LED, OUTPUT);
   digitalWrite(TFT_LED, HIGH);    

   tft.begin();
   tft.setRotation(3);

   #ifndef LCD_MODE_CONSOLE
   //tft.setScrollTextArea(0, 0, 53*6, 30*8);
   //tft.setTextWrap( true );
   tft.enableScroll();
   tft.setScrollTextArea(0,0,53*6, 30*8);
   // tft.setScrollBackgroundColor(ILI9341_GREEN);
   #endif

   _setConsoleMode( LCD_CONSOLE_DEF_COLS );
   //_setConsoleMode( LCD_CONSOLE_40_COLS );

   tft.fillScreen(ILI9341_BLACK);
   tft.setTextColor(ILI9341_YELLOW);
   tft.setTextSize(2);
   tft.println("Teensy 3.6 -YATL- Booting");

   tft.setTextSize(1);
   tft.setTextColor(ILI9341_WHITE);
#endif
 }

 Screen screen;

#endif