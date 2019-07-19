#ifndef __FX_STARFIELD___
#define __FX_STARFIELD___ 1

void setupBCK(WiredScreen* screen);
void loopBCK(WiredScreen* screen);

int mode = 1; // screen redraw mode -> 1: cls
float speed = 0.5f;
uint16_t color = 1; // WHITE

bool _star_inited = false;
// void runNativeBackGroundStars(uint16_t starsNb,uint16_t _speed,uint16_t _mode,uint16_t _color) {
//   if ( !_star_inited ) {
//     setupBCK();
//     _star_inited = true;
//   }

//   // starsNb is ignored @this time
//   speed = (float)_speed / 200.0f; // in percent --> 100 => 0.5f
//   mode  = _mode;
//   color = _color;

//   loopBCK();
// }

// Arduino .cos(rad)

static bool firstRND=true;

float rnd(float d1) {
 if (firstRND) {
   firstRND = false;
    srand( time(NULL) );
 }

 return (float)( (rand() % 10000) / 10000.0 * (double)d1 );
// return (float)( ( random(10000) ) / 10000.0 * (double)d1 );
}



void drawStarFieldFrame(WiredScreen* screen, int mode=1);




void setupBCK(WiredScreen* screen) {
}

void loopBCK(WiredScreen* screen) {
 //screen.blitt(0);
 //if (mode ==1) screen->cls();
 drawStarFieldFrame(screen, 0);
 //screen.blitt(2);
}


// ----------------------------------------------------------------------

typedef struct {
    float x;
    float y;
    float z;
} Star;

// TO-LOOK
const int size=50;

int zMax = 5;


int width = SCREEN_WIDTH;
int height = SCREEN_HEIGHT;

bool inited = false;

static Star starfield[ size ];

/*
 have to define 2 exec modes : 1 uses CLS
 then another that just erase previous drawn star
*/

void createStar(int i);
void initStars();

void drawStarFieldFrame(WiredScreen* screen, int mode) {
  if ( !inited ) {
    initStars();
    inited = true;
  }
  
  if (mode ==1) screen->cls();
  
  for(int i=0; i < size; i++) {
    if ( starfield[i].z <= speed ) {
      createStar(i);
    } else {
      starfield[i].z -= speed;
    }
    
    int x = (int)( (float)(width / 2) + starfield[i].x / starfield[i].z );
    int y = (int)( (float)(height / 2) + starfield[i].y / starfield[i].z );
    
    if ( x<0 || y<0 || x>=width || y>=height ) {
      createStar(i);
    } else {
      int zy = (int)starfield[i].z;
      if ( zy < 1 ) { zy = 1; }
      int zz = (zMax / zy );
      if (zz < 1) { zz=1; }
      int xx = x-(zz/2); if ( xx < 0 ) { xx=0; }
      int yy = y-(zz/2); if ( yy < 0 ) { yy=0; }
      
      //screen.drawRect( xx, yy, zz,zz, 1, 1 );
      screen->drawRect( xx, yy, zz,zz, 1, color );
    }
  }
  
}

void initStars() {
 width = SCREEN_WIDTH;
 height = SCREEN_HEIGHT;

  for(int i=0; i < size; i++) {
    createStar(i);
    starfield[i].z = rnd( zMax );
  }
}

void createStar(int i) {
  // starfield[i] = new ....
  starfield[i].x = rnd(2*width) - width;
  starfield[i].y = rnd(2*height) - height;
  starfield[i].z = zMax;
}

#endif