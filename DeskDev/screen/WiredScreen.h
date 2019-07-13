#ifndef __BRIDGED_SCREEN_H_
#define __BRIDGED_SCREEN_H_ 1

#include <stdint.h>

#define rgb16(r,g,b) { (uint16_t)( (( r *31/255 )<<11) | (( g *63/255 )<<5) | ( b *31/255 ) ) }

#define CLR_CYAN  rgb16(0,255,255)

#define CLR_PINK  rgb16(255,0,150)
#define CLR_RED   rgb16(255,0,0)
#define CLR_GREEN rgb16(0,255,0)
#define CLR_BLUE  rgb16(0,0,255)
#define CLR_BLACK 0
#define CLR_WHITE rgb16(255,255,255)

#define CLR_LIGHTGRAY rgb16(200,200,200)
#define CLR_GRAY      rgb16(128,128,128)
#define CLR_DARKGRAY  rgb16(50,50,50)

#define CLR_LIGHTGREEN rgb16(150,255,150)
#define CLR_DARKGREEN  rgb16(0,150,0)



//#define CLR_WHITE 0xFFFF

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define MODE_FILL 1
#define MODE_DRAW 0
  // ____________________________________________
  #define TEXT_OVERWRITE 0x00 // w/ BG
  #define TEXT_INCRUST   0x01 // w/o BG
  

  #define SIG_MCU_RESET       0x01
  #define SIG_MCU_MASTER_SYNC 0x02 // Query
  #define SIG_MCU_SLAVE_SYNC  0x03 // Answer
  #define SIG_MCU_UPLOAD_BDG  0x04 // Upload via Bridge
  #define SIG_MCU_DIR2        0x05 // DIR onMCU#2
  #define SIG_MCU_UPLOAD_SER  0x06 // Upload via Main Serial line

  #define SIG_MP3_PLAY  0x11
  #define SIG_MP3_PAUSE 0x12
  #define SIG_MP3_NEXT  0x13
  #define SIG_MP3_PREV  0x14
  #define SIG_MP3_VOL   0x15
  #define SIG_MP3_STOP  0x16

  #define SIG_SCR_MODE          0x21
  #define SIG_SCR_CLEAR         0x22
  #define SIG_SCR_CURSOR        0x23 // TTY cursor
  #define SIG_SCR_COLOR         0x24 // Text + Shapes ?
  #define SIG_SCR_BLITT         0x25
  #define SIG_SCR_TXTMODE       0x26
  // ____________________________________________

  class WiredScreen {
      private:
      	void drawBitmapImg(int x, int y, uint8_t* raster, int color);
      	void drawColoredImg(int x, int y, int w, int h, uint16_t* raster);
      	void drawColoredSprite(int x, int y, int w, int h, int sx, int sy, int sw, int sh, uint16_t* raster);
      	void drawPixShaded(int x, int y, uint16_t color, int fromMode );
      	
      	int_least16_t drawChar(int_least16_t x, int_least16_t y, char c, uint_least16_t color, uint_least16_t bg, uint_least8_t size);
      	void DrawChar(char c, uint16_t x, uint16_t y, uint16_t color);
      
      public:
        WiredScreen();
        ~WiredScreen();

        bool init();
        void close();

        void cls();
        void setCursor(int x, int y);

        void print(int val);
        void print(float val);
        void print(char* str);
        void println(char* str);
        void dispStr(char* str, int x, int y, uint16_t color=CLR_WHITE);

        void drawBPP(char* name, int x, int y);
        void drawPCT(char* name, int x, int y);
        void drawPCTSprite(char* name, int x, int y, int w, int h, int sx, int sy);
        
        void drawRGB16(int x, int y, int w, int h, uint16_t* raster);

        void drawPixel(int x, int y, uint16_t color=1);
        void drawLine(int x, int y, int x2, int y2, uint16_t color=1);
        void drawRect(int x, int y, int w, int h, uint8_t mode=0, uint16_t color=1);
        void drawCircle(int x, int y, int radius, uint8_t mode=0, uint16_t color=1);
        void drawTriangle(int x, int y, int x2, int y2, int x3, int y3, uint8_t mode=0, uint16_t color=1);

        void drawBall(int x, int y, int radius, uint16_t color=1);

        void drawAnimatedBackground(int mode, int m1, int m2, int m3, int m4);
        void drawAnimatedBackground(int mode, uint8_t* scene, int sceneLen);

        void blitt(uint8_t mode);
        // setMode(int mode);
  };


#endif