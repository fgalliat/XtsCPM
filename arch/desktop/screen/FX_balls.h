#ifndef __FX_balls__
#define __FX_balls__

// Arduino .cos(rad)

// in degrees !!!!!
double lastAngleX, lastAngleY, lastAngleZ;
double angleX=0.0, angleY=0.0, angleZ=0.0;
uint16_t _b_color = 1; // WHITE

void _b_setupBCK();
void _b_loopBCK();

bool _ball_inited = false;
void runNativeBackGroundBalls(uint16_t _angleX,uint16_t _angleY,uint16_t _angleZ,uint16_t _color) {
  if ( !_ball_inited ) {
    _b_setupBCK();
    _ball_inited = true;
  }

  angleX = (double)_angleX;
  angleY = (double)_angleY;
  angleZ = (double)_angleZ;
  _b_color  = _color;

  _b_loopBCK();
}


/*
 Rotey Ball Demo

  from https://github.com/tobozo/Rotatey_Balls/blob/master/Rotatey_Balls.ino

*/


void calcRotation();
void spherePlot();
void meshPlot();
void vectorRotateXYZ(double angle, int axe);
void zSortPoints();
void cubeloop();


// =====================================
int calcRotationCounter = 0;

// MPU6050 - emulation

void calcRotation() {

/*
  if ( (calcRotationCounter % 3) == 0 ) {
    angleX += 5.0;
  } else if ( (calcRotationCounter % 3) == 1 ) {
    angleY += 5.0;
  } else if ( (calcRotationCounter % 3) == 2 ) {
    angleZ += 5.0;
  }
*/
    angleY += 5.0;
    angleZ = 90.0;
    angleX += 5.0;


  if(angleX>180.0) angleX-=360.0;
  if(angleX<-180.0) angleX+=360.0;
  if(angleY>180.0) angleY-=360.0;
  if(angleY<-180.0) angleY+=360.0;
  if(angleZ>180.0) angleZ-=360.0;
  if(angleZ<-180.0) angleZ+=360.0;

  calcRotationCounter++;
}



// =====================================
/*
Serial serial("/dev/ttyS1");
BridgedScreen screen( &serial );


#define CLR_GREEN 6
#define CLR_BLACK 0
#define CLR_WHITE 1
*/
#define CLR_PINK  8
// =====================================

/*
  could try to store actions to make a circle "sprite"
  then recall only this "sprite" @X,Y

*/


void _b_setupBCK() {

  // Serial.begin(115200);

  // mpu_init(sdaPin, sclPin);// sda, scl
  // mpu_calibrate();

  // store initial position
  calcRotation(); // read from MPU
  lastAngleX = angleX;
  lastAngleY = angleY;
  lastAngleZ = angleZ;

  printf("Starting screen\n");

  // display.init();
  // //display.flipScreenVertically(); // do this if you need to mess things up
  // display.clear();   // clears the screen and buffer
  // display.display();
  // display.setColor(WHITE);
  mcu.getScreen()->clear();

  //xTaskCreatePinnedToCore(loop1, "loop1", 4096, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
  
}

void _b_loopBCK() {

  //calcRotation(); // read from MPU

#if DEBUG == true
  printf("angle{X,Y,Z}= ");
  printf(angleX);
  printf("\t,");
  printf(angleY);
  printf("\t,");
  printf(angleZ);
  printf("\t || acc_{x,y,z}= ");
  printf(acc_x);
  printf("\t,");
  printf(acc_y);
  printf("\t,");
  printf(acc_z);
  printf("\n");
#endif  

  // ------- BLITT 0 ?????
  //screen.cls();
  cubeloop();
  //fps(1);
  //msOverlay();
  // display.display(); 
  // ------- BLITT 2 ?????
  
  // store last position
  lastAngleX = angleX;
  lastAngleY = angleY;
  lastAngleZ = angleZ;
 
}


/*
int main(int argc, char* argv[] ) {
  screen.cls();

  setup();

  for (int i=0; i < 50; i++) {
    loop();
  }

  // screen.drawBPP("/BLAST.BPP", 0, 0);
  // screen.drawPCT("/CARO.PCT", 160, 90);

  // screen.drawPCTSprite("/CARO.PCT", 160, 0, 64, 64, 0, 0);
  // screen.drawPCTSprite("", 160+64+4, 0, 64, 64, 0, 0);

  // screen.println("Hello world from LCD controller !"); // BEWARE max 30 chars
  // screen.print(128);
  // screen.println("");
  // screen.print(3.14f);
  // screen.println("");

  // screen.drawRect(10,10,20,20,1,8); // PINK FLAT RECT
  // screen.drawLine(10,10,10+20,1+20,7); // DARKGREEN LINE
  // //screen.drawCircle(10,10+30,10,1,5); // LIGHTGREEN FLAT CIRCLE -- doesn't show
  // screen.drawCircle(10,10+30,10,0,5); // LIGHTGREEN FLAT CIRCLE

  // screen.drawTriangle(100,10, 100+20,10+30, 100,10+60  ,1,5); // LIGHTGREEN FLAT TRIANGLE


}
*/

int zoom = 2;

uint16_t screenWidth  = 128*zoom;
uint16_t screenHeight = 64*zoom;

//uint8_t centerX = 64;
//uint8_t centerY = 32;
uint8_t centerX = 160; // BEWARE uint8_t
uint8_t centerY = 120;



#define DEBUG false

// fps counter
unsigned int fpsall = 30;

const double halfC = M_PI / 180;

// Overall scale and perspective distance
uint8_t sZ = 4, scale = 16*zoom;
// screen center coordinates (calculated from screen dimensions)

typedef struct {
    double x;
    double y;
    double z;
} Coord3DSet;

typedef struct {
    double x;
    double y;
} Coord2DSet;

typedef struct {
    uint16_t id1;
    uint16_t id2;
} Lines;  


/* https://codepen.io/ge1doot/pen/grWrLe */

static Coord3DSet CubePoints3DArray[21] = {
  {  1,  1,  1 },
  {  1,  1, -1 },
  {  1, -1,  1 },
  {  1, -1, -1 },
  { -1,  1,  1 },
  { -1,  1, -1 },
  { -1, -1,  1 },
  { -1, -1, -1 },

  {  1,  1,  0 },
  {  1,  0,  1 },
  {  0,  1,  1 },

  {  -1,  1,  0 },
  {  -1,  0,  1 },
  {  0,  -1,  1 },

  {  1,  -1,  0 },
  {  1,  0,  -1 },
  {  0,  1,  -1 },

  {  -1,  -1,  0 },
  {  -1,  0,  -1 },
  {  0,  -1,  -1 },

  {0, 0, 0}
  
};

static Coord3DSet CubePoints2DArray[21] = {
  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },

  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },
  { 0,0 },
  
  { 0,0 },
  { 0,0 },
  { 0,0 },

  { 0,0 }
};

static Lines LinesArray[12] = {
  { 0, 1 },
  { 0, 2 },
  { 0, 4 },
  { 1, 3 },
  { 1, 5 },
  { 2, 3 },
  { 2, 6 },
  { 3, 7 },
  { 4, 5 },
  { 4, 6 },
  { 5, 7 },
  { 6, 7 }
/*
  { 1, 4 },
  { 2, 3 },
  { 1, 6 },
  { 2, 5 },
  { 2, 8 },
  { 6, 4 },
  { 4, 7 },
  { 3, 8 },
  { 1, 7 },
  { 3, 5 },
  { 5, 8 },
  { 7, 6 }
 */
  
};

// used for sorting points by depth
uint16_t zsortedpoints[21] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

uint16_t totalpoints = sizeof(CubePoints3DArray) / sizeof(CubePoints3DArray[0]);
uint16_t totallines = sizeof(LinesArray) / sizeof(LinesArray[0]);


/*
const int maxScale = 64;
const int redZone = 5;
const int sampleSize = 1024; // initially 1024
const int sampleWindow = 20; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
uint16_t a0value;
*/



// pseudo CLS
void pcls() {
  // BEWARE w/ negative coords
  mcu.getScreen()->drawRect(centerX-(screenWidth/2)-20, centerY-(screenHeight/2)-20, screenWidth+40, screenHeight+40, 1, 0); 
}


void cubeloop() {
  float diffAngleX, diffAngleY, diffAngleZ;

  diffAngleX = lastAngleX - angleX;
  diffAngleY = lastAngleY - angleY;
  diffAngleZ = lastAngleZ - angleZ;

  vectorRotateXYZ((double)(diffAngleY+0.1)*halfC, 1); // X
  vectorRotateXYZ((double)(diffAngleX+0.1)*halfC, 2); // Y
  vectorRotateXYZ((double)diffAngleZ*halfC, 3); // Z

  bool blitt = true;


  zSortPoints();

//  if (blitt) { screen.blitt(0); }
  //screen.drawRect(0, 0, 128, 64, 1, 0); // pseudo CLS
  pcls();
  // meshPlot();
  spherePlot();

//  if (blitt) { screen.blitt(2); }

}


void vectorRotateXYZ(double angle, int axe) {
  int8_t m1; // coords polarity
  double t1, t2;
  uint16_t i;
  for( i=0; i<totalpoints; i++ ) {
    switch(axe) {
      case 1: // X
        m1 = -1;
        t1 = CubePoints3DArray[i].y;
        t2 = CubePoints3DArray[i].z;
        CubePoints3DArray[i].y = t1*cos(angle)+(m1*t2)*sin(angle);
        CubePoints3DArray[i].z = (-m1*t1)*sin(angle)+t2*cos(angle);
      break;
      case 2: // Y
        m1 = 1;
        t1 = CubePoints3DArray[i].x;
        t2 = CubePoints3DArray[i].z;
        CubePoints3DArray[i].x = t1*cos(angle)+(m1*t2)*sin(angle);
        CubePoints3DArray[i].z = (-m1*t1)*sin(angle)+t2*cos(angle);
      break;
      case 3: // Z
        m1 = 1;
        t1 = CubePoints3DArray[i].x;
        t2 = CubePoints3DArray[i].y;
        CubePoints3DArray[i].x = t1*cos(angle)+(m1*t2)*sin(angle);
        CubePoints3DArray[i].y = (-m1*t1)*sin(angle)+t2*cos(angle);
      break;
    }
  }
}

/* sort xyz by z depth */
void zSortPoints() {
  bool swapped;
  uint16_t temp;
  float radius, nextradius;
  do {
    swapped = false;
    for(uint16_t i=0; i!=totalpoints-1; i++ ) {
      radius     = (-CubePoints3DArray[zsortedpoints[i]].z+3)*2;
      nextradius = (-CubePoints3DArray[zsortedpoints[i+1]].z+3)*2;
      
      if (radius > nextradius) {
        temp = zsortedpoints[i];
        zsortedpoints[i] = zsortedpoints[i + 1];
        zsortedpoints[i + 1] = temp;
        swapped = true;
      }
    }
  } while (swapped);
}


/* draw scaled spheres from background to foreground */
void spherePlot() {
  uint16_t i;
  int radius;
  int transid;

int step = 1;
//step=3;
int clr;

  for( i=0; i<totalpoints; i+=step ) {
    transid = zsortedpoints[i];
    CubePoints2DArray[transid].x = centerX + scale/(1+CubePoints3DArray[transid].z/sZ)*CubePoints3DArray[transid].x; 
    CubePoints2DArray[transid].y = centerY + scale/(1+CubePoints3DArray[transid].z/sZ)*CubePoints3DArray[transid].y;
    radius = (-CubePoints3DArray[transid].z+3)*2.5;
    
    radius *= zoom;
     // -or- screen.drawSceneBall( angleX, angleY, ...
     // DRAWBCK 1, angleX, angleY, angleZ, color
  
     // add DIRM cmd
    
     //clr = (i%8)+1;
     clr = _b_color;
     mcu.getScreen()->drawBall( CubePoints2DArray[transid].x, CubePoints2DArray[transid].y, radius, 1, clr );
  }

}

/* draw lines between given pairs of points */
void meshPlot() {
  uint16_t i;
  uint16_t id1, id2;
  for( i=0; i<totallines; i++ ) {
    id1 = LinesArray[i].id1;
    id2 = LinesArray[i].id2;
    mcu.getScreen()->drawLine(CubePoints2DArray[id1].x, CubePoints2DArray[id1].y, CubePoints2DArray[id2].x, CubePoints2DArray[id2].y, CLR_PINK);
  }
}



// static inline void fps(const int seconds){
//   // Create static variables so that the code and variables can
//   // all be declared inside a function
//   static unsigned long lastMillis;
//   static unsigned long frameCount;
//   static unsigned int framesPerSecond;
  
//   // It is best if we declare millis() only once
//   unsigned long now = millis();
//   frameCount ++;
//   if (now - lastMillis >= seconds * 1000) {
//     framesPerSecond = frameCount / seconds;
//     //Serial.println(framesPerSecond);
//     fpsall = framesPerSecond;
//     frameCount = 0;
//     lastMillis = now;
//   }
// }

// void msOverlay() {
//   //display.setTextAlignment(TEXT_ALIGN_RIGHT);
//   display.setFont(ArialMT_Plain_10);
// display.drawString(0, 10, String(fpsall)+"fps");
// }





#endif