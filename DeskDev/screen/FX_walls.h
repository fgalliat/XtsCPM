#ifndef __FX_WALLS__
#define __FX_WALLS__


void w_setupBCK();
void w_loopBCK();

uint8_t w_scene[32];

bool _wall_inited = false;
void runNativeBackGroundWalls(uint8_t* scene) {
  if ( !_wall_inited ) {
    w_setupBCK();
    _wall_inited = true;
  }

  memcpy(w_scene, scene, 32);

  w_loopBCK();
}



// =====================================
/*
Serial serial("/dev/ttyS1");
BridgedScreen screen( &serial );
#define CLR_PINK  8
#define CLR_GREEN 6
#define CLR_BLACK 0
#define CLR_WHITE 1
*/
// =====================================

void drawWallScene();



void w_setupBCK() {
}

void w_loopBCK() {
 //screen.blitt(0);
 //if (mode ==1) mcu.getScreen()->clear();
 drawWallScene();
 //screen.blitt(2);
}

void drawRect(int x, int y, int w, int h, int light) {
  int mode = 1;
  int color = 1;
  
  if ( light == 0 ) {
    color = 4; // dark gray
    mode = 1;
  } else if ( light == 1 ) {
    color = 3;
    mode = 1;
  } else if ( light == 2 ) {
    color = 2; // light gray
    mode = 1;
  } else if ( light == 3 ) {
    color = 1; // white
    mode = 1;
  }  
  
  mcu.getScreen()->drawRect( x, y, w, h, mode, color );
}

void drawStrip(int i, int h, int light) {
  drawRect( 5+(i*10), (SCREEN_HEIGHT-h)/2, 10, h, light );
}

void drawWallScene() {
  int bckAttribs = w_scene[0];
  int wllAttribs = w_scene[1];
  
  // STILL TODO : draw floor & sky
  //screen.cls();
  mcu.getScreen()->drawRect(5, 0, 300, 120, 1, 9); // light-blue sky
  mcu.getScreen()->drawRect(5, 120, 300, 120, 1, 5); // light-green floor
  
  uint8_t wall;
  for(int i=0; i < 30; i++) {
    wall = w_scene[2+i];

    int height = (int) ((float) ((wall >> 2) % 64) * 3.80f);
    int attr   = wall % 4;

    drawStrip( i, height, attr );
  }
}




#endif
