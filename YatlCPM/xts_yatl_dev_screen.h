#ifndef _XTS_DEV_SCREEN_H_
#define _XTS_DEV_SCREEN_H_ 1
 /**
 * Xtase - fgalliat @Jun2019
 * 
 * implementaion of Yatl hardware API (Arduino Version)
 * Screen over SPI impl.
 * 
 */
  #include "Arduino.h"

  // #include "xts_string.h"
  extern char charUpCase(char ch);

  #include "xts_yatl_settings.h"
  #include "xts_yatl_api.h"
  extern Yatl yatl;

    #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
    extern SdFatSdio SD;

  #define SPRITES_SUPPORT 1

  #if SPRITES_SUPPORT
    #define SPRITE_AREA_WIDTH 160
    #define SPRITE_AREA_HEIGHT 120
    #define SPRITE_AREA_SIZE (SPRITE_AREA_WIDTH*SPRITE_AREA_HEIGHT)
    uint16_t spriteArea[ SPRITE_AREA_SIZE ];

    int spriteInstanceCounter = 0; 
    int lastAddr = 0;

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
         
         void drawClip(int x, int h);
    };

    #define NB_SPRITES 15
    Sprite sprites[NB_SPRITES];

    void cleanSprites();

    void grabbSpritesOfSize(char* imageName, int offsetX=0, int offsetY=0, int width=32, int height=32);

  #endif


#endif