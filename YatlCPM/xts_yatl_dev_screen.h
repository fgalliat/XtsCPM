#ifndef _XTS_DEV_SCREEN_H_
#define _XTS_DEV_SCREEN_H_ 1
 /**
 * Xtase - fgalliat @Jun2019
 * 
 * implementaion of Yatl hardware API (Arduino Version)
 * Screen over SPI impl.
 * 
 */
  #ifdef ARDUINO
  #include "Arduino.h"
  #else 
  #include "Desktop.h"
  #endif

  // #include "xts_string.h"
  extern char charUpCase(char ch);

  #include "xts_yatl_settings.h"
  #include "xts_yatl_api.h"
  

  #define SPRITES_SUPPORT 1

  #if SPRITES_SUPPORT
    #define SPRITE_AREA_WIDTH 160
    #define SPRITE_AREA_HEIGHT 120
    #define SPRITE_AREA_SIZE (SPRITE_AREA_WIDTH*SPRITE_AREA_HEIGHT)
    
    extern int spriteInstanceCounter;

    class Sprite {
       private:
         int idx;
         int addr;
       public:
         int x, y, w, h;

         Sprite() {
            this->invalid();
            this->idx = spriteInstanceCounter++;
            this->addr = -1;
         }
         ~Sprite() {}
         void setBounds(int x, int y, int w, int h) {
            this->x = x; this->y = y;
            this->w = w; this->h = h;
         }
         bool isValid() {
            return this->x > -1 && this->y > -1; 
         }
         void invalid() {
            this->x = -1;
            this->y = -1;
            this->addr = -1;
         }
         
         void drawClip(int x, int y);
    };

    #define NB_SPRITES 15
    extern Sprite sprites[NB_SPRITES];

    void cleanSprites();

    void grabbSpritesOfSize(char* imageName, int offsetX=0, int offsetY=0, int width=32, int height=32);
    void grabbSprites(char* imageName, int offsetX=0, int offsetY=0);

  #endif


#endif