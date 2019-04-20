#ifndef YATL_SCREEN_H_
#define YATL_SCREEN_H_ 1

/**
 * Xtase YATL Screen interface
 * Xtase - fgalliat @Apr2019
 */

#ifdef USE_BUILTIN_LCD

    // tty console emulation
    #define LCD_MODE_CONSOLE 1

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
      #define TTY_FONT_WIDTH 6
      #define TTY_FONT_HEIGHT 8
      // 53
      #define TTY_CON_WIDTH  ( TFT_WIDTH / TTY_FONT_WIDTH )
      // 30
      #define TTY_CON_HEIGHT ( TFT_HEIGHT / TTY_FONT_HEIGHT )
      // 1590
      #define TTY_CON_SIZE ( TTY_CON_WIDTH * TTY_CON_HEIGHT )

      char ttyConsoleFrame[ TTY_CON_SIZE ];
      // just to prevent from recomputing each time
      const int ttyConsoleFrameSize = TTY_CON_SIZE;
      const int ttyConsoleWidth     = TTY_CON_WIDTH;
      const int ttyConsoleHeight    = TTY_CON_HEIGHT;

      int consoleCursorX = 0;
      int consoleCursorY = 0;

      void _consoleFill(char ch, bool resetCursor=true) {
         memset(ttyConsoleFrame, 0x00, ttyConsoleFrameSize);
         if (resetCursor) {
           consoleCursorX = 0;
           consoleCursorY = 0;
         }
      }

      void consoleCls() {
         _consoleFill(0x00, true);
      }

      void consoleWrite(char ch) {
         ttyConsoleFrame[ ( consoleCursorY * ttyConsoleWidth ) + consoleCursorX ] = ch;
      }

      void consoleRenderFull(bool clearDisplay=true) {
         if (clearDisplay) { tft.fillScreen(ILI9341_BLACK); }
         tft.setCursor(0, 0);
         
         for(int y=0; y < ttyConsoleHeight; y++) {
            consoleCursorX = 0;
            consoleCursorY = y;
            tft.setCursor(0, consoleCursorY * TTY_FONT_HEIGHT);
            tft.print( &ttyConsoleFrame[ (y*ttyConsoleWidth)+0 ] );
         }
      }

      void consoleRenderPartial() {
         y_dbug("XTS: CON: RenderPartial TODO");
      }

      void _scrollUp() {
         y_dbug("XTS: CON: ScrollUp TODO");
      }

      void _consoleTest() {
         _consoleFill('A');
         consoleRenderFull();
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
           tft.fillScreen(ILI9341_BLACK);
           tft.setCursor(0, 0);
#endif
       }

       void write(char ch) { tft.write( ch ); }

       void println(const char* str) { this->println( (char*)str ); }

       void println(char* str) {
#ifndef USE_BUILTIN_LCD
           y_dbug( "XTS: -> \\" );
           y_dbug( str );
           y_dbug( "XTS: -> /" );
#else
           tft.print(str);
           tft.print('\n');
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

   //tft.setScrollTextArea(0, 0, 53*6, 30*8);
   //tft.setTextWrap( true );
   tft.enableScroll();
   tft.setScrollTextArea(0,0,53*6, 30*8);
   // tft.setScrollBackgroundColor(ILI9341_GREEN);

   tft.fillScreen(ILI9341_BLACK);
   tft.setTextColor(ILI9341_YELLOW);
   tft.setTextSize(2);
   tft.println("Waiting for Arduino Serial Monitor...");

   tft.setTextSize(1);
   tft.setTextColor(ILI9341_WHITE);
#endif
 }

 Screen screen;

#endif