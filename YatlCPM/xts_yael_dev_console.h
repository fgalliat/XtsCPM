 /**
 * Xtase - fgalliat @Sept2019
 * 
 * implementaion of Yatl software VT-Ext console (Arduino & cpp Version)
 * VT100 + Ext protocols & routines
 * 
 * for YAEL Layout
 * 
 */
  #warning "-= Yael API Desktop Mode =-"
    #ifdef ARDUINO
    #include "Arduino.h"
    #else 
    #include "Desktop.h"
    #endif

  // #include "xts_string.h"
  extern char charUpCase(char ch);


  // #include "xts_yatl_settings.h"
  // #include "xts_yatl_api.h"
  // extern Yatl yatl;
  #include "xts_yael.h"

    // #include "xts_yael_dev_screen.h"

  
  // forward symbols
  void _setConsoleMode(int mode);

  void drawBmp(char* filename, bool screenRotate=true);

  // === TFT direct routines ============
   #define FONT_NORMAL 0
   #define FONT_TINY 1

      void __setTextColor(uint16_t color);
      void __setFont(int fontMode=0);
      void __setCursor(int xPx, int yPx);
      void __clearScreen(uint16_t bgColor);
      void __fillRect(int x, int y, int w, int h, uint16_t bgColor);
      void __fillPixelRect(int x, int y, int w, int h, uint16_t* bgColor);
      void __write1char(char ch); // tft.write(ch)

  // ==========] Console Routines [==========

   // playInParallel Not Supported
   void playVTMusic(char* tuneStr, bool playInParallel = false);

   #define TFT_WIDTH 320
   #define TFT_HEIGHT 240

   // one of LCD_CONSOLE_xx_COLS
   void _setConsoleMode(int mode);

   void _consoleSetCursor(int col, int row);

   void _consoleFill(char ch, bool resetCursor=true);

   uint16_t mapColor(uint16_t color);
   
   void consoleColorSet(uint16_t bg=CLR_BLACK, uint16_t fg=CLR_WHITE, uint16_t acc=CLR_GREEN);

   void consoleCls(bool clearDisplay=true);

   void _consoleRenderOneLine(int row, bool clearArea=true);

   void consoleRenderFull(bool clearDisplay=true);

   void _toggleConsoleMode(bool rerender=true);

   void _scrollUp();

   // erase from current position to EndOfLine
   void _eraseTillEOL(bool clearArea=true);

   void drawGlyphSpace(uint16_t color, uint16_t color2);

   void hideCursor();

   void showCursor();

   void consoleWrite(char ch);



  // ==========] Draw Bmp Routines [==========
   void _drawBmp(char* filename, int x, int y);
   void drawBmp(char* filename, bool screenRotate);

    void cleanSprites();

   // will takes only 160x120 px of bmp file
    void _feedSprites(char* filename, int x, int y);


    void grabbSprites(char* imageName, int offsetX, int offsetY);

    void grabbSpritesOfSize(char* imageName, int offsetX, int offsetY, int width, int height);

   // filename is "/Z/0/packXXX.pak"
   // reads&display image #numInPak of packed image from filename
   void drawImgFromPAK(char* filename, int x, int y, int numInPak);

