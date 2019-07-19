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
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <time.h>
#include <math.h>


#include "../WiredScreen.h"
#include "../FX_starfield.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

    static int fbfd = 0;
    static struct fb_var_screeninfo vinfo;
    static struct fb_fix_screeninfo finfo;
    static long int screensize = 0;
    static char *fbp = 0;
    static int x = 0, y = 0;
    static long int location = 0;


    WiredScreen::WiredScreen() {
        srand( time( NULL ) );
    }
    WiredScreen::~WiredScreen() {
    }

    bool WiredScreen::init(int arg) {
        char fbDesc[16];
        sprintf( fbDesc, "/dev/fb%d", arg );
    
        // Open the file for reading and writing
        fbfd = open(fbDesc, O_RDWR);
        if (fbfd == -1) {
            perror("Error: cannot open framebuffer device");
            exit(1);
        }
        printf("The framebuffer device was opened successfully.\n");

        // Get fixed screen information
        if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
            perror("Error reading fixed information");
            exit(2);
        }

        // Get variable screen information
        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
            perror("Error reading variable information");
            exit(3);
        }

        printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

        // Figure out the size of the screen in bytes
        //  screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

        // Calculate the size of the screen in bytes 
        screensize = vinfo.xres_virtual * vinfo.yres_virtual * (vinfo.bits_per_pixel / 8);
    
        // Map the device to memory
        fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
        //if ((int)fbp == -1) {
        if ( !fbp ) {
            perror("Error: failed to map framebuffer device to memory");
            exit(4);
        }
        printf("The framebuffer device was mapped to memory successfully.\n");


        return true;
    }

    void WiredScreen::close() {
        munmap(fbp, screensize);
        ::close(fbfd);
    }

    void WiredScreen::blitt(uint8_t mode) {
        // int len = 2; txBuff[0] = SIG_SCR_BLITT; txBuff[1] = mode; 
	    // serial->write( txBuff, len );
	    // flushRX(serial);
    }

    void WiredScreen::cls() {
        this->drawRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,1,0);
    }

	int ttyX = 0, ttyY = 0;

    // int nb of TTY chars
    void WiredScreen::setCursor(int x, int y) {
        ttyX = x;
        ttyY = y;
    }

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
    	    	this->DrawChar( ch , ttyX*6, ttyY*8, CLR_WHITE);
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
    }
    
    void WiredScreen::println(char* str) {
        this->print(str);
        this->print((char*)"\n");
    }


	void WiredScreen::dispStr(char* str, int x, int y, uint16_t color) {
    	if ( str == NULL ) { return; }
    	int len = strlen( str );
    	char ch;
    	for(int i=0; i < len; i++) {
    		if ( x >= SCREEN_WIDTH ) { break; }
    		ch = str[i];
    		if (! ( ch == '\n' ) || ( ch == '\r' ) || ( ch == '\b' ) ) {
    	    	this->DrawChar( ch , x, y, color);
    		}
    		x += 6;
    	}
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
	  const char* DATA_PWD = "/vm_mnt/data"; // no trailing slash !
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
    }
    
    void WiredScreen::drawBitmapImg(int x, int y, uint8_t* raster, int color) {
    	int sx = screenOffsetX + x;
	    int sy = screenOffsetY + y;
	
	    int width = 128;
	    int height = 64;
	
	    unsigned char c;
	
	    // TODO : lock blitt
	
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
    }
    // --------
    
    #define MEM_RAST_HEIGHT  128
    #define MEM_RAST_WIDTH   160
    #define MEM_RAST_LEN_u16 MEM_RAST_WIDTH * MEM_RAST_HEIGHT
    #define MEM_RAST_LEN_u8  MEM_RAST_LEN_u16 * 2

    #define PCT_HEADER_LEN 7
    static uint16_t color_picturebuff[ MEM_RAST_LEN_u16 ];

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
	          w = ((uint16_t)header[3]*256) + ((uint16_t)header[4]);
	          h = ((uint16_t)header[5]*256) + ((uint16_t)header[6]);
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
	//printf("PCT.3 %d x %d \n", w, h);
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
    }
    
    // even on ARM cpu -- to check
    #define INTEL_MODE 1

	void WiredScreen::drawRGB16(int x, int y, int width, int height, uint16_t* raster) {
		uint16_t color, c16b;
        for (int yy = 0; yy < height; yy++)
        {
        	
        	location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                       (y +yy +vinfo.yoffset) * finfo.line_length;
        	
        	//memcpy( *(fbp+location), raster[(yy * width) + 0], width*2);
        	for (int xx = 0; xx < width; xx++) {
        		color = raster[(yy * width) + (xx)];
        		*((unsigned short int*)(fbp + location)) = color;
        		location+=2;
        	}
        	
        	/*
            for (int xx = 0; xx < width; xx++)
            {
                color = raster[(yy * width) + (xx)];

			      //#ifdef INTEL_MODE
			      //  // Intel endian ?
			      //  color = (color%256)*256 + color/256;
			      //#endif

                //drawPixel(x + xx, y + yy, color );
                //*((unsigned short int*)(fbp + location)) = color;
                 
            }
            */
        }
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

			      #ifdef INTEL_MODE
			        // Intel endian ?
			        color = (color%256)*256 + color/256;
			      #endif
			
			      int _r = (unsigned int)((color >> 11) * (255/31) /* % (unsigned char)0xF8*/ );
			      int _g = (unsigned int)(( ((color) >> 5) % (unsigned char)0x40) * (255/63) /*% (unsigned char)0xFC*/);
			      int _b = (unsigned int)(color % (unsigned char)0x20) * (255/31);

				c16b = rgb16( _r, _g, _b );

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
	          w = ((uint16_t)header[3]*256) + ((uint16_t)header[4]);
	          h = ((uint16_t)header[5]*256) + ((uint16_t)header[6]);
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

    void WiredScreen::drawPixel(int x, int y, uint16_t color) {
        if ( x >= SCREEN_WIDTH || x < 0 || y >= SCREEN_HEIGHT || y < 0 ) {
            return;
        }

        location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                   (y+vinfo.yoffset) * finfo.line_length;
        #ifdef MODE32BPP
            // if (vinfo.bits_per_pixel == 32) {
                *(fbp + location) = 100;        // Some blue
                *(fbp + location + 1) = (15+(x-50)/2)%255;     // A little green
                *(fbp + location + 2) = (200-(y-50)/5)%255;    // A lot of red
                *(fbp + location + 3) = 0;      // No transparency
        #else
            // 16BPP -- ILI9341 current case
            // int b = rand()%32;
            // //int g = ((x-50)/6)%32;     // A little green
            // int g = rand()%32;
            // int r = (31-(y-50)/16)%32;    // A lot of red
            // unsigned short int t = r<<11 | g << 5 | b;


            // since 18/09/2018
            if ( color > 0 && color < 16 ) {
                     if ( color == 0 ) { color = CLR_BLACK; }
                else if ( color == 1 ) { color = CLR_WHITE; }
                else if ( color == 2 ) { color = CLR_LIGHTGRAY; }
                else if ( color == 3 ) { color = CLR_GRAY; }
                else if ( color == 4 ) { color = CLR_DARKGRAY; }

                else if ( color == 5 ) { color = CLR_LIGHTGREEN; }
                else if ( color == 6 ) { color = CLR_GREEN; }
                else if ( color == 7 ) { color = CLR_DARKGREEN; }

                else if ( color ==  8 ) { color = CLR_PINK; }
                else if ( color ==  9 ) { color = CLR_CYAN; }
                else if ( color == 10 ) { color = CLR_RED; }
                else if ( color == 11 ) { color = CLR_GREEN; }
                else if ( color == 12 ) { color = CLR_BLUE; }
                
            }

            unsigned short int t = color;
            *((unsigned short int*)(fbp + location)) = t;
        #endif
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
    }
    
    void WiredScreen::drawRect(int x, int y, int w, int h, uint8_t mode, uint16_t color) {
        // TODO : better
        if ( x >= SCREEN_WIDTH || x < 0 || y >= SCREEN_HEIGHT || y < 0 ) {
            return;
        }

        if ( mode == MODE_FILL ) {
	        for(int yy=0; yy<h; yy++) {
	            for(int xx=0; xx<w; xx++) {
	                this->drawPixel(x+xx,y+yy, color);
	            }
	        }
        } else {
        	for(int xx=0; xx<w; xx++) {
	          this->drawPixel(x+xx,y+0, color);
	          this->drawPixel(x+xx,y+h-1, color);
	        }
	        
	        for(int yy=0; yy<h; yy++) {
	          this->drawPixel(x+0,y+yy, color);
	          this->drawPixel(x+w-1,y+yy, color);
	        }
        }

    }
    void WiredScreen::drawCircle(int x, int y, int radius, uint8_t mode, uint16_t color) {
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
    }
    void WiredScreen::drawTriangle(int x, int y, int x2, int y2, int x3, int y3, uint8_t mode, uint16_t color) {
    	// TODO : fill mode
    	
        this->drawLine(x,y,x2,y2, color);
        this->drawLine(x2,y2,x3,y3, color);
        this->drawLine(x3,y3,x,y, color);
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
    }


    void WiredScreen::drawAnimatedBackground(int mode, int m1, int m2, int m3, int m4) {
    	
    	if ( mode == 1 ) {
    		drawStarFieldFrame(this, m1);
    	}
    	
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
    }


// =============================================
// =============================================
// =============================================

//if defined char range 0x20-0x7F otherwise 0x20-0xFF
#define FONT_END7F
#define FONT_START (0x20) //first character

#define FONT_WIDTH (6)
#define FONT_HEIGHT (8)

const uint8_t font_PGM[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x20
        0x04, 0x0E, 0x0E, 0x04, 0x04, 0x00, 0x04, 0x00, // 0x21
        0x1B, 0x1B, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x22
        0x00, 0x0A, 0x1F, 0x0A, 0x0A, 0x1F, 0x0A, 0x00, // 0x23
        0x08, 0x0E, 0x10, 0x0C, 0x02, 0x1C, 0x04, 0x00, // 0x24
        0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13, 0x00, // 0x25
        0x08, 0x14, 0x14, 0x08, 0x15, 0x12, 0x0D, 0x00, // 0x26
        0x0C, 0x0C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x27
        0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x00, // 0x28
        0x08, 0x04, 0x04, 0x04, 0x04, 0x04, 0x08, 0x00, // 0x29
        0x00, 0x0A, 0x0E, 0x1F, 0x0E, 0x0A, 0x00, 0x00, // 0x2A
        0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00, 0x00, // 0x2B
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x08, // 0x2C
        0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, // 0x2D
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00, // 0x2E
        0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00, // 0x2F
        0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E, 0x00, // 0x30
        0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0x31
        0x0E, 0x11, 0x01, 0x06, 0x08, 0x10, 0x1F, 0x00, // 0x32
        0x0E, 0x11, 0x01, 0x0E, 0x01, 0x11, 0x0E, 0x00, // 0x33
        0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02, 0x00, // 0x34
        0x1F, 0x10, 0x10, 0x1E, 0x01, 0x11, 0x0E, 0x00, // 0x35
        0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E, 0x00, // 0x36
        0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08, 0x00, // 0x37
        0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E, 0x00, // 0x38
        0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C, 0x00, // 0x39
        0x00, 0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00, // 0x3A
        0x00, 0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x08, // 0x3B
        0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02, 0x00, // 0x3C
        0x00, 0x00, 0x1F, 0x00, 0x00, 0x1F, 0x00, 0x00, // 0x3D
        0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x00, // 0x3E
        0x0E, 0x11, 0x01, 0x06, 0x04, 0x00, 0x04, 0x00, // 0x3F
        0x0E, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0E, 0x00, // 0x40
        0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x00, // 0x41
        0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E, 0x00, // 0x42
        0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E, 0x00, // 0x43
        0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E, 0x00, // 0x44
        0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F, 0x00, // 0x45
        0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10, 0x00, // 0x46
        0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F, 0x00, // 0x47
        0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11, 0x00, // 0x48
        0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0x49
        0x01, 0x01, 0x01, 0x01, 0x11, 0x11, 0x0E, 0x00, // 0x4A
        0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11, 0x00, // 0x4B
        0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, // 0x4C
        0x11, 0x1B, 0x15, 0x11, 0x11, 0x11, 0x11, 0x00, // 0x4D
        0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11, 0x00, // 0x4E
        0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00, // 0x4F
        0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10, 0x00, // 0x50
        0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D, 0x00, // 0x51
        0x1E, 0x11, 0x11, 0x1E, 0x12, 0x11, 0x11, 0x00, // 0x52
        0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E, 0x00, // 0x53
        0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, // 0x54
        0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00, // 0x55
        0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04, 0x00, // 0x56
        0x11, 0x11, 0x15, 0x15, 0x15, 0x15, 0x0A, 0x00, // 0x57
        0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11, 0x00, // 0x58
        0x11, 0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x00, // 0x59
        0x1E, 0x02, 0x04, 0x08, 0x10, 0x10, 0x1E, 0x00, // 0x5A
        0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0E, 0x00, // 0x5B
        0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, // 0x5C
        0x0E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0E, 0x00, // 0x5D
        0x04, 0x0A, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x5E
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, // 0x5F
        0x0C, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x60
        0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0x61
        0x10, 0x10, 0x1E, 0x11, 0x11, 0x11, 0x1E, 0x00, // 0x62
        0x00, 0x00, 0x0E, 0x11, 0x10, 0x11, 0x0E, 0x00, // 0x63
        0x01, 0x01, 0x0F, 0x11, 0x11, 0x11, 0x0F, 0x00, // 0x64
        0x00, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E, 0x00, // 0x65
        0x06, 0x08, 0x08, 0x1E, 0x08, 0x08, 0x08, 0x00, // 0x66
        0x00, 0x00, 0x0F, 0x11, 0x11, 0x0F, 0x01, 0x0E, // 0x67
        0x10, 0x10, 0x1C, 0x12, 0x12, 0x12, 0x12, 0x00, // 0x68
        0x04, 0x00, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0x69
        0x02, 0x00, 0x06, 0x02, 0x02, 0x02, 0x12, 0x0C, // 0x6A
        0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12, 0x00, // 0x6B
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0x6C
        0x00, 0x00, 0x1A, 0x15, 0x15, 0x11, 0x11, 0x00, // 0x6D
        0x00, 0x00, 0x1C, 0x12, 0x12, 0x12, 0x12, 0x00, // 0x6E
        0x00, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E, 0x00, // 0x6F
        0x00, 0x00, 0x1E, 0x11, 0x11, 0x11, 0x1E, 0x10, // 0x70
        0x00, 0x00, 0x0F, 0x11, 0x11, 0x11, 0x0F, 0x01, // 0x71
        0x00, 0x00, 0x16, 0x09, 0x08, 0x08, 0x1C, 0x00, // 0x72
        0x00, 0x00, 0x0E, 0x10, 0x0E, 0x01, 0x0E, 0x00, // 0x73
        0x00, 0x08, 0x1E, 0x08, 0x08, 0x0A, 0x04, 0x00, // 0x74
        0x00, 0x00, 0x12, 0x12, 0x12, 0x16, 0x0A, 0x00, // 0x75
        0x00, 0x00, 0x11, 0x11, 0x11, 0x0A, 0x04, 0x00, // 0x76
        0x00, 0x00, 0x11, 0x11, 0x15, 0x1F, 0x0A, 0x00, // 0x77
        0x00, 0x00, 0x12, 0x12, 0x0C, 0x12, 0x12, 0x00, // 0x78
        0x00, 0x00, 0x12, 0x12, 0x12, 0x0E, 0x04, 0x18, // 0x79
        0x00, 0x00, 0x1E, 0x02, 0x0C, 0x10, 0x1E, 0x00, // 0x7A
        0x06, 0x08, 0x08, 0x18, 0x08, 0x08, 0x06, 0x00, // 0x7B
        0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04, 0x00, // 0x7C
        0x0C, 0x02, 0x02, 0x03, 0x02, 0x02, 0x0C, 0x00, // 0x7D
        0x0A, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x7E
        0x04, 0x0E, 0x1B, 0x11, 0x11, 0x1F, 0x00, 0x00, // 0x7F
#ifndef FONT_END7F
        0x0E, 0x11, 0x10, 0x10, 0x11, 0x0E, 0x04, 0x0C, // 0x80
        0x12, 0x00, 0x12, 0x12, 0x12, 0x16, 0x0A, 0x00, // 0x81
        0x03, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E, 0x00, // 0x82
        0x0E, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0x83
        0x0A, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0x84
        0x0C, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0x85
        0x0E, 0x0A, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0x86
        0x00, 0x0E, 0x11, 0x10, 0x11, 0x0E, 0x04, 0x0C, // 0x87
        0x0E, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E, 0x00, // 0x88
        0x0A, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E, 0x00, // 0x89
        0x0C, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E, 0x00, // 0x8A
        0x0A, 0x00, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0x8B
        0x0E, 0x00, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0x8C
        0x08, 0x00, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0x8D
        0x0A, 0x00, 0x04, 0x0A, 0x11, 0x1F, 0x11, 0x00, // 0x8E
        0x0E, 0x0A, 0x0E, 0x1B, 0x11, 0x1F, 0x11, 0x00, // 0x8F
        0x03, 0x00, 0x1F, 0x10, 0x1E, 0x10, 0x1F, 0x00, // 0x90
        0x00, 0x00, 0x1E, 0x05, 0x1F, 0x14, 0x0F, 0x00, // 0x91
        0x0F, 0x14, 0x14, 0x1F, 0x14, 0x14, 0x17, 0x00, // 0x92
        0x0E, 0x00, 0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0x93
        0x0A, 0x00, 0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0x94
        0x18, 0x00, 0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0x95
        0x0E, 0x00, 0x12, 0x12, 0x12, 0x16, 0x0A, 0x00, // 0x96
        0x18, 0x00, 0x12, 0x12, 0x12, 0x16, 0x0A, 0x00, // 0x97
        0x0A, 0x00, 0x12, 0x12, 0x12, 0x0E, 0x04, 0x18, // 0x98
        0x12, 0x0C, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0x99
        0x0A, 0x00, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0x9A
        0x00, 0x00, 0x01, 0x0E, 0x16, 0x1A, 0x1C, 0x20, // 0x9B
        0x06, 0x09, 0x08, 0x1E, 0x08, 0x09, 0x17, 0x00, // 0x9C
        0x0F, 0x13, 0x15, 0x15, 0x15, 0x19, 0x1E, 0x00, // 0x9D
        0x00, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x00, 0x00, // 0x9E
        0x02, 0x05, 0x04, 0x0E, 0x04, 0x04, 0x14, 0x08, // 0x9F
        0x06, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0xA0
        0x06, 0x00, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0xA1
        0x06, 0x00, 0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xA2
        0x06, 0x00, 0x12, 0x12, 0x12, 0x16, 0x0A, 0x00, // 0xA3
        0x0A, 0x14, 0x00, 0x1C, 0x12, 0x12, 0x12, 0x00, // 0xA4
        0x0A, 0x14, 0x00, 0x12, 0x1A, 0x16, 0x12, 0x00, // 0xA5
        0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, 0x0F, 0x00, // 0xA6
        0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, 0x1E, 0x00, // 0xA7
        0x04, 0x00, 0x04, 0x0C, 0x10, 0x11, 0x0E, 0x00, // 0xA8
        0x1E, 0x25, 0x2B, 0x2D, 0x2B, 0x21, 0x1E, 0x00, // 0xA9
        0x00, 0x00, 0x3F, 0x01, 0x01, 0x00, 0x00, 0x00, // 0xAA
        0x10, 0x12, 0x14, 0x0E, 0x11, 0x02, 0x07, 0x00, // 0xAB
        0x10, 0x12, 0x14, 0x0B, 0x15, 0x07, 0x01, 0x00, // 0xAC
        0x04, 0x00, 0x04, 0x04, 0x0E, 0x0E, 0x04, 0x00, // 0xAD
        0x00, 0x00, 0x09, 0x12, 0x09, 0x00, 0x00, 0x00, // 0xAE
        0x00, 0x00, 0x12, 0x09, 0x12, 0x00, 0x00, 0x00, // 0xAF
        0x15, 0x00, 0x2A, 0x00, 0x15, 0x00, 0x2A, 0x00, // 0xB0
        0x15, 0x2A, 0x15, 0x2A, 0x15, 0x2A, 0x15, 0x2A, // 0xB1
        0x2A, 0x3F, 0x15, 0x3F, 0x2A, 0x3F, 0x15, 0x3F, // 0xB2
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, // 0xB3
        0x04, 0x04, 0x04, 0x3C, 0x04, 0x04, 0x04, 0x04, // 0xB4
        0x06, 0x00, 0x04, 0x0A, 0x11, 0x1F, 0x11, 0x00, // 0xB5
        0x0E, 0x00, 0x04, 0x0A, 0x11, 0x1F, 0x11, 0x00, // 0xB6
        0x0C, 0x00, 0x04, 0x0A, 0x11, 0x1F, 0x11, 0x00, // 0xB7
        0x1E, 0x21, 0x2D, 0x29, 0x2D, 0x21, 0x1E, 0x00, // 0xB8
        0x14, 0x34, 0x04, 0x34, 0x14, 0x14, 0x14, 0x14, // 0xB9
        0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, // 0xBA
        0x00, 0x3C, 0x04, 0x34, 0x14, 0x14, 0x14, 0x14, // 0xBB
        0x14, 0x34, 0x04, 0x3C, 0x00, 0x00, 0x00, 0x00, // 0xBC
        0x00, 0x04, 0x0E, 0x10, 0x10, 0x0E, 0x04, 0x00, // 0xBD
        0x11, 0x0A, 0x04, 0x1F, 0x04, 0x1F, 0x04, 0x00, // 0xBE
        0x00, 0x00, 0x00, 0x3C, 0x04, 0x04, 0x04, 0x04, // 0xBF
        0x04, 0x04, 0x04, 0x07, 0x00, 0x00, 0x00, 0x00, // 0xC0
        0x04, 0x04, 0x04, 0x3F, 0x00, 0x00, 0x00, 0x00, // 0xC1
        0x00, 0x00, 0x00, 0x3F, 0x04, 0x04, 0x04, 0x04, // 0xC2
        0x04, 0x04, 0x04, 0x07, 0x04, 0x04, 0x04, 0x04, // 0xC3
        0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, // 0xC4
        0x04, 0x04, 0x04, 0x3F, 0x04, 0x04, 0x04, 0x04, // 0xC5
        0x05, 0x0A, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0xC6
        0x05, 0x0A, 0x04, 0x0A, 0x11, 0x1F, 0x11, 0x00, // 0xC7
        0x14, 0x17, 0x10, 0x1F, 0x00, 0x00, 0x00, 0x00, // 0xC8
        0x00, 0x1F, 0x10, 0x17, 0x14, 0x14, 0x14, 0x14, // 0xC9
        0x14, 0x37, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, // 0xCA
        0x00, 0x3F, 0x00, 0x37, 0x14, 0x14, 0x14, 0x14, // 0xCB
        0x14, 0x17, 0x10, 0x17, 0x14, 0x14, 0x14, 0x14, // 0xCC
        0x00, 0x3F, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, // 0xCD
        0x14, 0x37, 0x00, 0x37, 0x14, 0x14, 0x14, 0x14, // 0xCE
        0x11, 0x0E, 0x11, 0x11, 0x11, 0x0E, 0x11, 0x00, // 0xCF
        0x0C, 0x10, 0x08, 0x04, 0x0E, 0x12, 0x0C, 0x00, // 0xD0
        0x0E, 0x09, 0x09, 0x1D, 0x09, 0x09, 0x0E, 0x00, // 0xD1
        0x0E, 0x00, 0x1F, 0x10, 0x1E, 0x10, 0x1F, 0x00, // 0xD2
        0x0A, 0x00, 0x1F, 0x10, 0x1E, 0x10, 0x1F, 0x00, // 0xD3
        0x0C, 0x00, 0x1F, 0x10, 0x1E, 0x10, 0x1F, 0x00, // 0xD4
        0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xD5
        0x06, 0x00, 0x0E, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0xD6
        0x0E, 0x00, 0x0E, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0xD7
        0x0A, 0x00, 0x0E, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0xD8
        0x04, 0x04, 0x04, 0x3C, 0x00, 0x00, 0x00, 0x00, // 0xD9
        0x00, 0x00, 0x00, 0x07, 0x04, 0x04, 0x04, 0x04, // 0xDA
        0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, // 0xDB
        0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x3F, // 0xDC
        0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04, 0x00, // 0xDD
        0x0C, 0x00, 0x0E, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0xDE
        0x3F, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, // 0xDF
        0x06, 0x0C, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xE0
        0x00, 0x1C, 0x12, 0x1C, 0x12, 0x12, 0x1C, 0x10, // 0xE1
        0x0E, 0x0C, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xE2
        0x18, 0x0C, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xE3
        0x0A, 0x14, 0x00, 0x0C, 0x12, 0x12, 0x0C, 0x00, // 0xE4
        0x0A, 0x14, 0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xE5
        0x00, 0x00, 0x12, 0x12, 0x12, 0x1C, 0x10, 0x10, // 0xE6
        0x00, 0x18, 0x10, 0x1C, 0x12, 0x1C, 0x10, 0x18, // 0xE7
        0x18, 0x10, 0x1C, 0x12, 0x12, 0x1C, 0x10, 0x18, // 0xE8
        0x06, 0x00, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xE9
        0x0E, 0x00, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xEA
        0x18, 0x00, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xEB
        0x06, 0x00, 0x12, 0x12, 0x12, 0x0E, 0x04, 0x18, // 0xEC
        0x06, 0x00, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x00, // 0xED
        0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xEE
        0x0C, 0x0C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xEF
        0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, // 0xF0
        0x00, 0x04, 0x0E, 0x04, 0x00, 0x0E, 0x00, 0x00, // 0xF1
        0x00, 0x00, 0x1F, 0x00, 0x00, 0x1F, 0x00, 0x00, // 0xF2
        0x30, 0x1A, 0x34, 0x0B, 0x15, 0x07, 0x01, 0x00, // 0xF3
        0x0F, 0x15, 0x15, 0x0D, 0x05, 0x05, 0x05, 0x00, // 0xF4
        0x0E, 0x11, 0x0C, 0x0A, 0x06, 0x11, 0x0E, 0x00, // 0xF5
        0x00, 0x04, 0x00, 0x1F, 0x00, 0x04, 0x00, 0x00, // 0xF6
        0x00, 0x00, 0x00, 0x0E, 0x06, 0x00, 0x00, 0x00, // 0xF7
        0x0C, 0x12, 0x12, 0x0C, 0x00, 0x00, 0x00, 0x00, // 0xF8
        0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, // 0xF9
        0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, // 0xFA
        0x08, 0x18, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, // 0xFB
        0x1C, 0x08, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, // 0xFC
        0x18, 0x04, 0x08, 0x1C, 0x00, 0x00, 0x00, 0x00, // 0xFD
        0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00, // 0xFE
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 0xFF
#endif
};

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
  ptr = &font_PGM[pos];
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






