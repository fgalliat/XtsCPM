#include <iostream>

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
// #include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <time.h>
#include <math.h>

#include <SDL2/SDL.h>


#include "../WiredScreen.h"

#ifndef ZOOM
  // #define ZOOM 1
  #define ZOOM 2
#endif


#ifndef MODE_480
  #define MODE_480 1
#endif


#if MODE_480
int SCREEN_WIDTH = 480;
int SCREEN_HEIGHT = 320;
#else
int SCREEN_WIDTH = 320;
int SCREEN_HEIGHT = 240;
#endif

// experimental for now 
bool screenRotated = false; // for Bitmap WallPaper

// =====] Fonts [=====]
#define TINYFONT 1

#define FONT_TINY 0
#define FONT_REGULAR 1

    const int ttyFontWidthRegular = 6;
    const int ttyFontHeightRegular = 8;
    const int ttyFontWidthTiny = 4;
    const int ttyFontHeightTiny = 6;

#if TINYFONT
    int currentFont = FONT_TINY;
    int ttyFontWidth = ttyFontWidthTiny;
    int ttyFontHeight = ttyFontHeightTiny;
#else
    int currentFont = FONT_REGULAR;
    int ttyFontWidth = ttyFontWidthRegular;
    int ttyFontHeight = ttyFontHeightRegular;
#endif



void WiredScreen::screenRotate(bool portrait) {
    screenRotated = portrait;
    SCREEN_WIDTH = portrait ? 240 : 320;
    SCREEN_HEIGHT = portrait ? 320 : 240;
}




#ifndef SDL_WINDOW_ALWAYS_ON_TOP
  // for old versions of SDL2
  #define SDL_WINDOW_ALWAYS_ON_TOP 0x00008000
#endif

/*
 ~ DESKTOP impl. (SDL2 for ariettaG25 Xts-subSystem)
 
*/

    int y = 0, x = 0;
    Uint32 *pixels = NULL;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Surface *surface;

    SDL_Color clWHITE;
    SDL_Color clBLACK;

    int txtColor = 1;

    SDL_Event event;

// ========================================
int _k_KEYS = 0;

void __doBlitt();


void WiredScreen::dispStr(char* str, int x, int y, unsigned short color) {

  this->setCursorPx(x,y);
  this->setTextColor(color);
  //this->print(str);
  int t = strlen(str);
  for(int i=0; i < t; i++) { this->write(str[i]); }

}


void *_xts_blittThread(void *argument){
    while( true ) {
        __doBlitt();
        SDL_Delay(100);
    }
}


void *_xts_keyThread(void *argument){
  
  while( true ) {

        SDL_WaitEvent(&event); // beware w/ wait !!!
        //SDL_PollEvent(&event); // check that

        if ( event.type == SDL_QUIT ) {
            // ...
            exit(0);
        }

        // TODO : better : manage muliple keys
        if ( event.type == SDL_KEYUP ) {
            _k_KEYS = 0;
        }

        if ( event.type == SDL_KEYDOWN ) {
            _k_KEYS = 0;
            switch (event.key.keysym.sym)
            {
                case SDLK_LEFT:  _k_KEYS = SDLK_LEFT; break;
                case SDLK_RIGHT: _k_KEYS = SDLK_RIGHT; break;
                case SDLK_UP:    _k_KEYS = SDLK_UP; break;
                case SDLK_DOWN:  _k_KEYS = SDLK_DOWN; break;

                case SDLK_s:  _k_KEYS = SDLK_s; break;
                case SDLK_d:  _k_KEYS = SDLK_d; break;
                case SDLK_f:  _k_KEYS = SDLK_f; break;
            }
        }

  }
  
}
// ========================================



    // exported to FakeGPIO
    int pollKey() {

        int result = 0;

        result = _k_KEYS;
        /*

        //SDL_WaitEvent(&event); // beware w/ wait !!!
        SDL_PollEvent(&event); // check that

        if ( event.type == SDL_QUIT ) {
            // ...
            exit(0);
        }

        if ( event.type == SDL_KEYDOWN ) {
            switch (event.key.keysym.sym)
            {
                case SDLK_LEFT:  result = SDLK_LEFT; break;
                case SDLK_RIGHT: result = SDLK_RIGHT; break;
                case SDLK_UP:    result = SDLK_UP; break;
                case SDLK_DOWN:  result = SDLK_DOWN; break;

                case SDLK_s:  result = SDLK_s; break;
                case SDLK_d:  result = SDLK_d; break;
                case SDLK_f:  result = SDLK_f; break;
            }
        }
        */

        return result;
    }

    bool _BLITT_LOCKED = false;

    // called from 'manual' blitt
    void doBlitt() {
    }

    // called from Thread
    void __doBlitt() {
        if (_BLITT_LOCKED) { 
            // char ch = '!'; putchar(ch); putchar(ch); putchar(ch); putchar(ch); putchar(ch); 
            return; 
        }
        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // char ch = '$'; putchar(ch); putchar(ch); putchar(ch); putchar(ch); putchar(ch);
    }

    WiredScreen::WiredScreen() {
        srand( time( NULL ) );
    }
    WiredScreen::~WiredScreen() {
    }

    bool WiredScreen::init(int arg) {
        window = SDL_CreateWindow("ILI9341", 0, 0, SCREEN_WIDTH * ZOOM, SCREEN_HEIGHT * ZOOM, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS);
        if (window == NULL)
        {
            std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            exit(1);
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == NULL)
        {
            std::cout << "Failed to create renderer : " << SDL_GetError();
            exit(1);
        }

        surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH * ZOOM, SCREEN_HEIGHT * ZOOM,
                                       32,
                                       0,
                                       0,
                                       0,
                                       0);

        if (surface == NULL)
        {
            printf("Could not create surface !!!!\n");
            exit(1);
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

        // white
        clWHITE.r = 255;
        clWHITE.g = 255;
        clWHITE.b = 255;

        clBLACK.r = 0;
        clBLACK.g = 0;
        clBLACK.b = 0;

        pthread_t thread1;
        pthread_t thread2;

        int i1 = pthread_create( &thread1, NULL, _xts_keyThread, (void*) NULL);
        int i2 = pthread_create( &thread2, NULL, _xts_blittThread, (void*) NULL);
        // pthread_join(thread1,NULL);

        return true;
    }

    void WiredScreen::close() {
        SDL_DestroyTexture(texture);
        SDL_DestroyWindow(window);

        SDL_Quit(); // TODO : check that !!!
    }

    // ===============================================

    void WiredScreen::blitt(uint8_t mode) {}

    void WiredScreen::cls(uint16_t bgColor) {
        this->drawRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,1,bgColor);

        doBlitt();
    }

	int ttyX = 0, ttyY = 0;

    // int nb of TTY chars
    void WiredScreen::setCursor(int x, int y) {
        ttyX = x;
        ttyY = y;
    }

    // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    int cursorXpx = 0;
    int cursorYpx = 0;

    int fontWidth = 4;
    int fontHeight = 6;

    void WiredScreen::setCursorPx(int x, int y) {
        cursorXpx = x;
        cursorYpx = y;
    }

    uint16_t textColor = CLR_WHITE;
    void WiredScreen::setTextColor(uint16_t fgColor) {
        textColor = fgColor;
    }

    void WiredScreen::write(char ch) {
        this->DrawChar( ch , cursorXpx, cursorYpx, textColor);
        cursorXpx += fontWidth;
        if ( cursorXpx >= SCREEN_WIDTH ) {
            cursorXpx = 0;
            cursorYpx += fontHeight;
        }
        doBlitt();
    }

    // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    void WiredScreen::print(int val) {
        char txt[17+1];
        sprintf( txt, "%d", val );
        this->print( txt );
    }
    void WiredScreen::print(float val) {
        char txt[17+1];
        //sprintf( txt, "%f", val );
        sprintf( txt, "%g", val ); // %g -> auto-precision format
        this->print( txt );
    }
    
    void WiredScreen::print(char* str) {
    	if ( str == NULL ) { return; }
    	int len = strlen( str );
    	char ch;
    	for(int i=0; i < len; i++) {
    		ch = str[i];
    		if (! ( ch == '\n' ) || ( ch == '\r' ) || ( ch == '\b' ) ) {
    	    	this->DrawChar( ch , ttyX*ttyFontWidth, ttyY*ttyFontHeight, CLR_WHITE);
    		}
    		
    		if ( ch == '\r' ) {}
    		else if ( ch == '\b' ) { ttyX--; }
    		else if ( ch == '\n' ) { ttyY++; ttyX = 0; }
    		else { ttyX++; }
    		
    		if ( ttyX >= 52 ) {
    			ttyY++; ttyX = 0;
    		}
    		
    		if ( ttyY >= 30 ) {
    			this->cls();
    			ttyY = 0;
    			ttyX = 0;
    		}
    	}

        doBlitt();
    }
    
    void WiredScreen::println(char* str) {
        this->print(str);
        this->print((char*)"\n");
    }

	// --------
	#define SCREEN_MODE_128 0
	#define SCREEN_MODE_160 1
	#define SCREEN_MODE_320 2
	
	
	
	int __screenMode = SCREEN_MODE_320;
	int screenOffsetX = 0;
	int screenOffsetY = 0;
	
	  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	  // destMode is SCREEN_MODE_320
	  void WiredScreen::drawPixShaded(int x, int y, uint16_t color, int fromMode ) {
	    int sx = 0; // screenOffsetX;
	    int sy = 0; // screenOffsetY;
	
	    if ( __screenMode != SCREEN_MODE_320 ) {
	      this->drawPixel(screenOffsetX+x, screenOffsetY+y, color);
	      return;
	    }
	
	    if ( fromMode == SCREEN_MODE_128 ) {
	      sx = (320-(128*2))/2;
	      sy = (240-(64*3))/2;
	      x*=2; int w=2;
	      y*=3; int h=3;
	      //display.fillRect(sx+x, sy+y, w, h, color);
	      this->drawRect(sx+x, sy+y, w, h, MODE_FILL, color);
	      
	      
	    } else if ( fromMode == SCREEN_MODE_160 ) {
	      sx = (320-(160*2))/2;
	      sy = (240-(128*2))/2; // check if not overflow (256 Vs 240)
	      x*=2; int w=2;
	      y*=2; int h=2;
	      //display.fillRect(sx+x, sy+y, w, h, color);
	      this->drawRect(sx+x, sy+y, w, h, MODE_FILL, color);
	      
	    } else if ( fromMode == SCREEN_MODE_320 ) {
	      //display.fillRect(sx+x, sy+y, 1, 1, color);
	      this->drawRect(sx+x, sy+y, 1, 1, MODE_FILL, color);
	    } 
	
	  }
	  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	  
	  void error(char* str) { printf("(EE) "); printf(str); printf("\n"); }
	  void error(const char* str) { error( (char*)str ); }
	  
	  void warn(char* str) { printf("(!!) "); printf(str); printf("\n"); }
	  void warn(const char* str) { error( (char*)str ); }
	  
	  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	  //                      123456789012
      #ifdef MY_PC
	  const char* DATA_PWD = "/vm_mnt/dat2"; // no trailing slash !
      #else
      const char* DATA_PWD = "/vm_mnt/data"; // no trailing slash !
      #endif
	  const int DATA_PWD_len = 12; // strlen(DATA_PWD);
	  char entryName[ DATA_PWD_len + 1+ 8+1+3 +1]; // 1+ '/' -- 8+1+3 'TOTO.BAD' -- +1 '0x00'
	
	
	// ex. filename : "/BLAST.BPP"
	char* getAssetEntryName(char* filename) {
	    strcpy(entryName, DATA_PWD);
	    strcat(entryName, filename);
	return entryName;
	}
	
	
	// name : "/BLAST.BPP"
	// default pixShader mode -> x,y ignored
    void WiredScreen::drawBPP(char* filename, int x, int y) {

		// 128 * 64 * 1
		#define BPP_SIZE 1024

		static uint8_t* raster = NULL;
		if ( raster == NULL ) {
			raster = (uint8_t*)malloc( BPP_SIZE );
		}

		if ( filename == NULL && raster == NULL ) {
			error( "BPP buffer empty !" );
			return;
		}

		if ( filename != NULL ) {
	        // ==============================
	        char* entryName = getAssetEntryName(filename);
	        // =============================
	        
	        FILE* f = fopen(entryName, "r");
	      	if ( !f ) { error("(BPP) File not ready"); return; }
	      	fseek(f, 0, SEEK_SET);
	      	int readed = fread( raster, 1, BPP_SIZE, f );
	      	fclose(f);
	      	
	      	// =============================
	      	
	      	if ( readed != BPP_SIZE ) { warn("(BPP) beware : file may be corrupted"); }
		}
        
        // pixshader mode
        x=0; y=0;
        this->drawBitmapImg(x, y, raster, CLR_WHITE);

        doBlitt();
    }
    
    void WiredScreen::drawBitmapImg(int x, int y, uint8_t* raster, int color) {
    	int sx = screenOffsetX + x;
	    int sy = screenOffsetY + y;
	
	    int width = 128;
	    int height = 64;
	
	    unsigned char c;
	
	    // TODO : lock blitt
	
        _BLITT_LOCKED = true;

	    //this->drawRect(sx, sy, 128, 64, 1, 0);
	    this->cls();
	    //drawPixShadedRect(x, y, 128, 64, CLR_BLACK, SCREEN_MODE_128 );
	
	    for (int yy = 0; yy < height; yy++) {
	      for (int xx = 0; xx < width; xx++) {
	        c = (raster[(yy * (width / 8)) + (xx / 8)] >> (7 - ((xx) % 8))) % 2;
	        if (c == 0x00) {
	        }
	        else {
	            // _oled_display->drawPixel(sx + xx, sy + yy, CLR_WHITE);
	            drawPixShaded(xx, yy, CLR_WHITE, SCREEN_MODE_128 );
	        }
	      }
	    }

        _BLITT_LOCKED = false;
    }
    // --------
    
    #define MEM_RAST_HEIGHT  128
    #define MEM_RAST_WIDTH   160
    #define MEM_RAST_LEN_u16 MEM_RAST_WIDTH * MEM_RAST_HEIGHT
    #define MEM_RAST_LEN_u8  MEM_RAST_LEN_u16 * 2

    #define PCT_HEADER_LEN 7
    static uint16_t color_picturebuff[ MEM_RAST_LEN_u16 ];

    // even on ARM cpu -- to check
    #define INTEL_MODE 1

	// filename ex. "/CARO.PCT" --or-- null to recall last one
    void WiredScreen::drawPCT(char* filename, int x, int y) {
    	static int w=-1, h=-1; // img size
    	static char header[PCT_HEADER_LEN];
        
        if ( filename != NULL ) {
          // ==============================
	      char* entryName = getAssetEntryName(filename);
	      // =============================
	        
          FILE* f = fopen(entryName, "r");
	      if ( !f ) { error("(PCT) File not ready"); return; }
	      fseek(f, 0, SEEK_SET);
	      int readed = fread( header, 1, PCT_HEADER_LEN, f );
	
	      if ( header[0] == '6' && header[1] == '4' && header[2] == 'K' ) {
	          w = ((uint8_t)header[3]*256) + ((uint8_t)header[4]);
	          h = ((uint8_t)header[5]*256) + ((uint8_t)header[6]);
              // printf("PCT.2.5 %d x %d \n", (uint8_t)header[4], (uint8_t)header[6]);
	      } else {
	          warn( "(PCT) Wrong PCT header" );
	          //mcu->print( (int)header[0] );
	          //mcu->print( ',' );
	          //mcu->print( (int)header[1] );
	          //mcu->print( ',' );
	          //mcu->print( (int)header[2] );
	          //mcu->println( "");
	          // mcu->println( header );
	      }
	      // printf("PCT.3 %d x %d \n", w, h);
	      // Serial.print("A.2 "); Serial.print(w); Serial.print('x');Serial.print(h);Serial.println("");
	      if( w <= 0 || h <= 0 ) {
	        error("Wrong size ");
	        //mcu->print(w);
	        //mcu->print("x");
	        //mcu->print(h);
	        //mcu->println("");
	        fclose(f);
	        return;
	      }

	      int scanZoneSize = w*MEM_RAST_HEIGHT*2; // *2 -> u16
	      int startX = screenOffsetX+x;
	      int startY = screenOffsetY+y;

	      int yy = 0;
	      while( true ) { 
	        // BEWARE : @ this time : h need to be 128
	        readed = fread( (uint8_t*)color_picturebuff, 1, scanZoneSize, f );
	        this->drawColoredImg(startX, startY+yy, w, MEM_RAST_HEIGHT, color_picturebuff);
	        yy += MEM_RAST_HEIGHT;
	        if ( yy + (MEM_RAST_HEIGHT) > h ) { break; }
	      }
	
	      fclose(f);
        } else {
		    // recall last MEM_RAST area
		    int scanZoneSize = w*MEM_RAST_HEIGHT*2; // *2 -> u16
		    int startX = screenOffsetX+x;
		    int startY = screenOffsetY+y;
        	this->drawColoredImg(x, y, w, h, color_picturebuff);
        }

        doBlitt();
    }
    


	void WiredScreen::drawRGB16(int x, int y, int width, int height, uint16_t* raster) {
		uint16_t color, c16b;
        for (int yy = 0; yy < height; yy++)
        {
        	/*
        	location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                       (y +yy +vinfo.yoffset) * finfo.line_length;
        	
        	//memcpy( *(fbp+location), raster[(yy * width) + 0], width*2);
        	for (int xx = 0; xx < width; xx++) {
        		color = raster[(yy * width) + (xx)];
        		*((unsigned short int*)(fbp + location)) = color;
        		location+=2;
        	}
            */
        	
        	
            for (int xx = 0; xx < width; xx++)
            {
                color = raster[(yy * width) + (xx)];

			      //#ifdef INTEL_MODE
			      //  // Intel endian ?
			      //  color = (color%256)*256 + color/256;
			      //#endif

                drawPixel(x + xx, y + yy, color );
            }
            
        }

        doBlitt();
	}

    
    void WiredScreen::drawColoredImg(int x, int y, int width, int height, uint16_t* picBuff) {
    	// int width = scanSize / height / 2;
        // width = scanSize; //???
        uint16_t color, c16b;

        for (int yy = 0; yy < height; yy++)
        {
            for (int xx = 0; xx < width; xx++)
            {
                color = picBuff[(yy * width) + (xx)];
                // 'color' is 565 colored (for ILI9341 raw)

			    //   #ifdef INTEL_MODE
			    //     // Intel endian ?
			    //     color = (color%256)*256 + color/256;
			    //   #endif
			
			    //   int _r = (unsigned int)((color >> 11) * (255/31) /* % (unsigned char)0xF8*/ );
			    //   int _g = (unsigned int)(( ((color) >> 5) % (unsigned char)0x40) * (255/63) /*% (unsigned char)0xFC*/);
			    //   int _b = (unsigned int)(color % (unsigned char)0x20) * (255/31);

				// c16b = rgb16( _r, _g, _b );

                c16b = color;

                drawPixel(x + xx, y + yy, c16b );
            }
        }
    }
    
    void WiredScreen::drawPCTSprite(char* filename, int dx, int dy, int dw, int dh, int sx, int sy) {
    	static int w=-1, h=-1; // img size
    	static char header[PCT_HEADER_LEN];

	    if ( sx < 0 ) { sx = 0; }
	    if ( sy < 0 ) { sy = 0; }
        
        if ( filename != NULL ) {
          // ==============================
	      char* entryName = getAssetEntryName(filename);
	      // =============================
	        
          FILE* f = fopen(entryName, "r");
	      if ( !f ) { error("(PCTSpr) File not ready"); return; }
	      fseek(f, 0, SEEK_SET);
	      int readed = fread( header, 1, PCT_HEADER_LEN, f );
	
	      if ( header[0] == '6' && header[1] == '4' && header[2] == 'K' ) {
	          w = ((uint8_t)header[3]*256) + ((uint8_t)header[4]);
	          h = ((uint8_t)header[5]*256) + ((uint8_t)header[6]);
	      } else {
	          warn( "(PCTSpr) Wrong PCT header" );
	      }

	      if( w <= 0 || h <= 0 ) {
	        error("Wrong size ");

	        fclose(f);
	        return;
	      }

	      int scanZoneSize = w*h*2; // *2 -> u16
	      readed = fread( (uint8_t*)color_picturebuff, 1, scanZoneSize, f );
	      fclose(f);
	      
	      if ( dw < 0 ) { dw = w; }
          if ( dh < 0 ) { dh = h; }
	      this->drawColoredSprite(dx, dy, dw, dh, sx, sy, w, h, color_picturebuff);
        } else {
		    // recall last MEM_RAST area
		    if ( dw < 0 ) { dw = w; }
            if ( dh < 0 ) { dh = h; }
        	this->drawColoredSprite(dx, dy, dw, dh, sx, sy, w, h, color_picturebuff);
        }

        doBlitt();
    }
    
    void WiredScreen::drawColoredSprite(int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, uint16_t* color_picturebuff) {
        //#ifndef COMPUTER
	    static int lastOffset=-1,lastW=-1,lastH=-1;
	    // BEWARE : mem overflow !!!!!
	    // 32x64 seems to be the max @ this time
	    // static uint16_t subImage[ dw*dh ];
	    static uint16_t subImage[ 64*64 ];
	    //#else
	    //int lastOffset=-1,lastW=-1,lastH=-1;
	    //uint16_t subImage[ 64*64 ];
	    //#endif
	
	    int startOffset = ( sy * dw ) + sx;
	    int offset = startOffset;
	
	    if (! ( lastOffset == offset && lastW == dw && lastH == dh ) ) {
	      for(int yy=0; yy < dh; yy++) {
	        // // TODO : find faster way !
	        // for(int xx=0; xx < dw; xx++) {
	        //   subImage[(yy*dw)+xx] = color_picturebuff[offset+xx];
	        // }
	        // cf Intel Endian ??? -- seems no diff
	        memcpy( &subImage[(yy*dw)], &color_picturebuff[offset], dw*sizeof(uint16_t) );
	
	        offset = ( (sy+yy) * sw ) + sx;
	      }
	    }
	
	    dx += screenOffsetX;
	    dy += screenOffsetY;
	
	    //display.pushImage(dx, dy, dw, dh, subImage);
	    this->drawColoredImg(dx, dy, dw, dh, subImage);
    }

    bool _DBUG = false;
    #define INTEL_MODE 1

    void __FillColorRect(int x, int y, int w, int h, uint16_t color) {

        // TODO : some work !!!!!
        SDL_Rect r;
        r.x = x;
        r.y = y;
        r.w = w;
        r.h = h;

        SDL_Color _color;

        // make as an optional param
        bool forceDirectColor = true;

        if ( forceDirectColor || color > 8 ) {
            #ifdef INTEL_MODE
                // Intel endian ?
//                color = (color%256)*256 + color/256;
            #endif

            int _r = (unsigned int)((color >> 11) * (255/31) /* % (unsigned char)0xF8*/ );
            int _g = (unsigned int)(( ((color) >> 5) % (unsigned char)0x40) * (255/63) /*% (unsigned char)0xFC*/);
            int _b = (unsigned int)(color % (unsigned char)0x20) * (255/31);

                if (_DBUG) {
                    printf("r=%d g=%d b=%d \n", _r, _g, _b);
                }
                _color.r = _r;
                _color.g = _g;
                _color.b = _b;
        } else {
            if ( color == 2 ) {
                // LIGHTGREY
                _color.r = 200;
                _color.g = 200;
                _color.b = 200;
            } else if ( color == 3 ) {
                // GREY
                _color.r = 128;
                _color.g = 128;
                _color.b = 128;
            } else if ( color == 4 ) {
                // DARKGREY
                _color.r = 50;
                _color.g = 50;
                _color.b = 50;
            } else if ( color == 5 ) {
                // LIGHTGREEN
                _color.r = 150;
                _color.g = 255;
                _color.b = 150;
            } else if ( color == 6 ) {
                // GREEN
                _color.r = 80;
                _color.g = 255;
                _color.b = 80;
            } else if ( color == 7 ) {
                // DARKGREEN
                _color.r = 0;
                _color.g = 150;
                _color.b = 0;
            } else if ( color == 8 ) {
                // PINK
                _color.r = 249;
                _color.g = 13;
                _color.b = 234;
            } 
        }


        // SDL_Color usedColor = color == 0x00 ? clBLACK : !forceDirectColor && color == 0x01 ? clWHITE : _color;
        SDL_Color usedColor = forceDirectColor ? _color : color == 0x00 ? clBLACK : _color;

        // draw a rect
        SDL_FillRect(surface, &r, SDL_MapRGB(surface->format, usedColor.r, usedColor.g, usedColor.b));
    }


    void WiredScreen::drawPixel(int x, int y, uint16_t color) {
        if ( screenRotated ) {
            __FillColorRect( y, x, 1, 1, color);
        } else {
            __FillColorRect( x, y, 1, 1, color);
        }
        // ??? doBlitt(); ???
    }

    int  min(int a,int b) { return a < b ? a : b; }
    int  max(int a,int b) { return a > b ? a : b; }
    int  abs(int a)   { return a < 0 ? -a : a; }
    void swap(int& a, int& b) { int swp=a; a=b; b=swp; }
    
    // there was a bug here between '*' & '/'
    #define icos(a) cos((double)a * 3.141596 / 180.0)
	#define isin(a) sin((double)a * 3.141596 / 180.0)
    
    void WiredScreen::drawLine(int x1, int y1, int x2, int y2, uint16_t color) {
        if (y1 == y2) {
            if (y1 < 0 || y1 >= SCREEN_HEIGHT) {
                return;
            }
            if (x2 < x1) {
                int swap = x2;
                x2 = x1;
                x1 = swap;
            }

            int e0 = max(x1, 0);
            int e1 = min(x2, SCREEN_WIDTH - 1);

            for (int i = e0; i <= e1; i++){
                this->drawPixel(i, y1, color);
            }
        }
        else if (x1 == x2) {
            if (x1 < 0 || x1 >= SCREEN_WIDTH) {
                return;
            }
            if (y2 < y1) {
                int swap = y2;
                y2 = y1;
                y1 = swap;
            }

            int e0 = max(y1, 0);
            int e1 = min(y2, SCREEN_HEIGHT - 1);

            for (int i = e0; i <= e1; i++) {
                this->drawPixel(x1, i, color);
            }
        }
        else {

            int x0 = x1;
            int y0 = y1;
            x1 = x2;
            y1 = y2;
            int16_t steep = abs(y1 - y0) > abs(x1 - x0);
            if (steep) {
                swap(x0, y0);
                swap(x1, y1);
            }

            if (x0 > x1) {
                swap(x0, x1);
                swap(y0, y1);
            }

            int16_t dx, dy;
            dx = x1 - x0;
            dy = abs(y1 - y0);

            int16_t err = dx / 2;
            int16_t ystep;

            if (y0 < y1) {
                ystep = 1;
            }
            else {
                ystep = -1;
            }

            for (; x0 <= x1; x0++) {
                if (steep) {
                    this->drawPixel(y0, x0, color);
                }
                else {
                    this->drawPixel(x0, y0, color);
                }
                err -= dy;
                if (err < 0) {
                    y0 += ystep;
                    err += dx;
                }
            }
        }

        doBlitt();
    }
    
    void WiredScreen::drawRect(int x, int y, int w, int h, uint8_t mode, uint16_t color) {
        if ( mode == MODE_FILL ) {
            __FillColorRect( x, y, w, h, color);
        } else {
            _BLITT_LOCKED = true;
            this->drawLine(x,y, x+w-1, y, color);
            this->drawLine(x+w-1,y, x+w-1, y+h-1, color);
            this->drawLine(x+w-1,y+h-1, x, y+h-1, color);
            this->drawLine(x,y+h-1, x, y, color);
            _BLITT_LOCKED = false;
        }

        doBlitt();
    }
    void WiredScreen::drawCircle(int x, int y, int radius, uint8_t mode, uint16_t color) {
        _BLITT_LOCKED = true;

        for (int i = 0; i < 90; i++) {
            int xx = (int)((double)radius * icos(i));
            int yy = (int)((double)radius * isin(i));
            if ( mode == MODE_FILL ) {
            	this->drawLine(x - xx, y + yy, x + xx, y + yy, color);
            	this->drawLine(x - xx, y - yy, x + xx, y - yy, color);
            } else {
	            this->drawPixel(x + xx, y + yy, color);
	            this->drawPixel(x - xx, y + yy, color);
	            this->drawPixel(x + xx, y - yy, color);
	            this->drawPixel(x - xx, y - yy, color);
            }
        }

        _BLITT_LOCKED = false;

        doBlitt();
    }
    void WiredScreen::drawTriangle(int x, int y, int x2, int y2, int x3, int y3, uint8_t mode, uint16_t color) {
    	// TODO : fill mode
    	
        this->drawLine(x,y,x2,y2, color);
        this->drawLine(x2,y2,x3,y3, color);
        this->drawLine(x3,y3,x,y, color);

        doBlitt();
    }

    void WiredScreen::drawBall(int x, int y, int radius, uint16_t color) {
        // int len = 1; txBuff[0] = 0x51; 
        // serial->write( txBuff, len );
        
        // len = 2; txBuff[0] = (uint8_t)( x/256 ); txBuff[1] = (uint8_t)( x%256 );  // X
        // serial->write( txBuff, len );

        // len = 2; txBuff[0] = (uint8_t)( y/256 ); txBuff[1] = (uint8_t)( y%256 );  // Y
        // serial->write( txBuff, len );

        // len = 2; txBuff[0] = (uint8_t)( radius/256 ); txBuff[1] = (uint8_t)( radius%256 );  // radius
        // serial->write( txBuff, len );

        // len = 2; txBuff[0] = (uint8_t)( color/256 ); txBuff[1] = (uint8_t)( color%256 );  // COLOR
        // serial->write( txBuff, len );

        // flushRX(serial);

        doBlitt();
    }


    void WiredScreen::drawAnimatedBackground(int mode, int m1, int m2, int m3, int m4) {
        // int len = 1; txBuff[0] = 0x61; 
        // serial->write( txBuff, len );
        
        // len = 1; txBuff[0] = mode; 
        // serial->write( txBuff, len );
        
        // len = 2; txBuff[0] = (uint8_t)( m1/256 ); txBuff[1] = (uint8_t)( m1%256 );  // X
        // serial->write( txBuff, len );

        // len = 2; txBuff[0] = (uint8_t)( m2/256 ); txBuff[1] = (uint8_t)( m2%256 );  // X
        // serial->write( txBuff, len );

        // len = 2; txBuff[0] = (uint8_t)( m3/256 ); txBuff[1] = (uint8_t)( m3%256 );  // X
        // serial->write( txBuff, len );

        // len = 2; txBuff[0] = (uint8_t)( m4/256 ); txBuff[1] = (uint8_t)( m4%256 );  // X
        // serial->write( txBuff, len );

        // flushRX(serial);      

        doBlitt();
    }

    void WiredScreen::drawAnimatedBackground(int mode, uint8_t* scene, int sceneLen) {
        // int len = 1; txBuff[0] = 0x61; 
        // serial->write( txBuff, len );
        
        // len = 1; txBuff[0] = mode; 
        // serial->write( txBuff, len );
        
        // len = sceneLen;
        // serial->write( scene, len );

        // len = 1; txBuff[0] = 0x00;
        // serial->write( txBuff, len );

        // flushRX(serial);      

        doBlitt();
    }


// =============================================
// =============================================


// ==== Fonts ==================================

void WiredScreen::setFont(int ftSize) {
    #if MODE_480
      ftSize = FONT_REGULAR;
    #endif
    currentFont = ftSize;
    if ( ftSize == FONT_TINY ) {
        ttyFontWidth = ttyFontWidthTiny;
        ttyFontHeight = ttyFontHeightTiny;
    } else {
        currentFont = FONT_REGULAR;
        ttyFontWidth = ttyFontWidthRegular;
        ttyFontHeight = ttyFontHeightRegular;
    }
}

const uint8_t* getFont() {
    if ( currentFont == FONT_TINY ) { return tiny_font_PGM; }
    return big_font_PGM;
}

// for text !!
#define lcd_width SCREEN_WIDTH
#define lcd_height SCREEN_HEIGHT

int_least16_t WiredScreen::drawChar(int_least16_t x, int_least16_t y, char c, uint_least16_t color, uint_least16_t bg, uint_least8_t size) {
  int_least16_t ret, pos;
#if FONT_WIDTH <= 8
  uint_least8_t data, mask;
#elif FONT_WIDTH <= 16
  uint_least16_t data, mask;
#elif FONT_WIDTH <= 32
  uint_least32_t data, mask;
#endif
  uint_least8_t i, j, width, height;
  //const PROGMEM uint8_t *ptr;
  const uint8_t *ptr;

#if FONT_WIDTH <= 8
  pos = (c - FONT_START) * (8 * FONT_HEIGHT / 8);
#elif FONT_WIDTH <= 16
  pos = (c - FONT_START) * (16 * FONT_HEIGHT / 8);
#elif FONT_WIDTH <= 32
  pos = (c - FONT_START) * (32 * FONT_HEIGHT / 8);
#endif
//   ptr = &font_PGM[pos];
  ptr = &(getFont()[pos]);
  width = FONT_WIDTH;
  height = FONT_HEIGHT;
  size = size & 0x7F;

  if (size <= 1)
  {
    ret = x + width;

    if ((ret - 1) >= lcd_width)
    {
      return lcd_width + 1;
    }
    else if ((y + height - 1) >= lcd_height)
    {
      return lcd_width + 1;
    }

// char msg[64];
//         sprintf(msg, "A w:%d h:%d ret:%d, lw%d\n", width, height, ret, lcd_width);
//         for(int i=0; i < strlen(msg); i++) {
//             putchar(msg[i]);
//         }

    //setArea(x, y, (x+width-1), (y+height-1));

    //drawStart();
    int xx = 0, yy = 0;
    for (; height != 0; height--)
    {
      // #if FONT_WIDTH <= 8
      //   data = pgm_read_byte(ptr); ptr+=1;
      data = *ptr;
      ptr += 1;
      // #elif FONT_WIDTH <= 16
      //       data = pgm_read_word(ptr); ptr+=2;
      // #elif FONT_WIDTH <= 32
      //       data = pgm_read_dword(ptr); ptr+=4;
      // #endif
      for (mask = (1 << (width - 1)); mask != 0; mask >>= 1)
      {
        if (data & mask)
        {
          //draw(color);
          this->drawPixel(x + xx, y + yy, color);
        }
        else
        {
          //draw(bg);
          // not for now as real SSD1306 does
          // __DrawPixel(x + xx, y + yy, bg);
        }
        xx++;
      }
      yy++;
      xx = 0;
    }
    //drawStop();
  }
  //   else
  //   {
  //     ret = x+(width*size);
  //     if((ret-1) >= lcd_width)
  //     {
  //       return lcd_width+1;
  //     }
  //     else if((y+(height*size)-1) >= lcd_height)
  //     {
  //       return lcd_width+1;
  //     }

  //     setArea(x, y, (x+(width*size)-1), (y+(height*size)-1));

  //     drawStart();
  //     for(; height!=0; height--)
  //     {
  // #if FONT_WIDTH <= 8
  //       data = pgm_read_byte(ptr); ptr+=1;
  // #elif FONT_WIDTH <= 16
  //       data = pgm_read_word(ptr); ptr+=2;
  // #elif FONT_WIDTH <= 32
  //       data = pgm_read_dword(ptr); ptr+=4;
  // #endif
  //       for(i=size; i!=0; i--)
  //       {
  //         for(mask=(1<<(width-1)); mask!=0; mask>>=1)
  //         {
  //           if(data & mask)
  //           {
  //             for(j=size; j!=0; j--)
  //             {
  //               draw(color);
  //             }
  //           }
  //           else
  //           {
  //             for(j=size; j!=0; j--)
  //             {
  //               draw(bg);
  //             }
  //           }
  //         }
  //       }
  //     }
  //     //drawStop();
  //   }

  return ret;
}

void WiredScreen::DrawChar(char c, uint16_t x, uint16_t y, uint16_t color) {
  this->drawChar(x, y, c, color, CLR_BLACK, 1);
}










