/**
 * 
 * ZX Spectrum+ 8x5 keyboard decoding
 * 
 * found @ : https://smittytone.wordpress.com/2014/02/16/the-sinclair-zx81-a-raspberry-pi-retro-restyle-part-1/
 * was initialy for a regular ZX Spectrum keyboard
 * 
 * 
 * for a 32u4 board
 * used an Arduino MICRO
 * 
 * 
 * Xtase - fgalliat @ Jan2020
 */

// ZX81 USB Keyboard for Leonardo
// (c) Dave Curran
// 2013-04-27
// Modified with Function keys by Tony Smith
// 2014-02-15

#include <Keyboard.h>


#define NUM_ROWS 8
#define NUM_COLS 5

#define DEBOUNCE_VALUE_XTS 15
#define DEBOUNCE_REPEAT_XTS 30

#define DELAY_AFTER_KEYSTROKE 40

// Global Variables
int debounceCount[NUM_ROWS][NUM_COLS];
int debounceTotal[NUM_ROWS][NUM_COLS];

// Define the row and column pins
// Arduino micro pins
byte colPins[NUM_COLS] = {8, 9, 10, 11, 12};
byte rowPins[NUM_ROWS] = {7, 6, 5, 4, 3, 2, 1, 0};

#define LED 13
#define LED2 14

#define CAP_KEY 0xFF 
#define SYM_KEY 0xFE 
#define CAPLOCK_KEY 0xFD 
#define CTRL_KEY 0xFC 

// Ctrl is a toggleAutoOffKey -> use a led for that
// CapLOCK is a toggleKey -> use a led for that too

unsigned char keyMapDef[NUM_COLS][NUM_ROWS] = {
  // 0    1    2    3    4    5    6     7
  { '5', 't', 'g', '6', 'y', 'v', 'h',  'b' },
  { '4', 'r', 'f', '7', 'u', 'c', 'j',  'n' },
  { '3', 'e', 'd', '8', 'i', 'x', 'k',  'm' },
  { '2', 'w', 's', '9', 'o', 'z', 'l', 0xfe }, // 7,3 Symb SHIFT
  { '1', 'q', 'a', '0', 'p',0xff,'\n',  ' ' }  // 5,4 Cap SHIFT
};

unsigned char keyMapCap[NUM_COLS][NUM_ROWS] = {
  { KEY_LEFT_ARROW, 'T', 'G', KEY_DOWN_ARROW, 'Y', 'V', 'H',  'B' },
  // INV VIDEO
  {         KEY_F1, 'R', 'F', KEY_UP_ARROW,   'U', 'C', 'J',  'N' },
  // TRUE VIDEO
  {       KEY_ESC , 'E', 'D', KEY_RIGHT_ARROW,'I', 'X', 'K',  'M' },
  // CAP LOCK                   GRAPH
  {    CAPLOCK_KEY, 'W', 'S', KEY_TAB,        'O', 'Z', 'L',  0xfe}, // 7,3 Symb SHIFT
  // EDIT                       DEL
  {       CTRL_KEY, 'Q', 'A', KEY_BACKSPACE,  'P',0xff,'\n',  0x03}  // 5,4 Cap SHIFT // 0x03 BREAK / CtrlC
};

const char* keyMapSymb[NUM_COLS][NUM_ROWS] = {
  // 0    1    2    3    4    5    6     7
  { "%",  ">", "}",  "&",  "[", "/", "^",  "*" },
  { "$",  "<", "{",  "'",  "]", "?", "-",  "," },
  { "#", ">=", "\\", "(",  "@", "~", "+",  "." },
  { "@", "<>", "|",  ")",  ";", ":", "=", "??" }, // 7,3 Symb SHIFT
  { "!", "<=", '~',  "_", "\"","??","\n",  " " }  // 5,4 Cap SHIFT
};

// [EXTEND MODE] key (cap+sym)
unsigned char keyMapCtrl[NUM_COLS][NUM_ROWS] = {
  { 0x00, 20,  7, 0x00, 25,  22,   8,   2 },
  { 0x00, 18,  6, 0x00, 21,   3,  10,  14 },
  { 0x00,  5,  4, 0x00,  9,  24,  11,  13 },
  { 0x00, 23, 19, 0x00, 15,  26,  12, 0x00}, // 7,3 Symb SHIFT
  { 0x00, 17,  1, 0x00, 16,0x00,0x00, 0x00}  // 5,4 Cap SHIFT // 0x03 BREAK / CtrlC
};

// #define COLS_AS_INPUT 1
#define COLS_AS_INPUT 0

#define INPUT_TYPE INPUT_PULLUP
// #define INPUT_TYPE INPUT

#define ACTIVE LOW
#define INACTIVE HIGH

void led(bool state) {
  digitalWrite(LED, state ? HIGH : LOW);
}

void led2(bool state) {
  digitalWrite(LED2, state ? HIGH : LOW);
}

void setup() {
  pinMode(LED, OUTPUT);
  led(false);

  pinMode(LED2, OUTPUT);
  led2(false);

  led(true);
  // 0 to 4
  for (byte c = 0 ; c < NUM_COLS ; c++) {
    #if COLS_AS_INPUT
    pinMode(colPins[c], INPUT_TYPE);
    #else
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c],INACTIVE);
    #endif
    
    for (byte r = 0 ; r < NUM_ROWS ; r++) {
      debounceCount[r][c] = 0;
      debounceTotal[r][c] = 0;
    }
  }
  
  // 0 to 7
  for (byte r = 0 ; r < NUM_ROWS ; r++) {
    #if COLS_AS_INPUT
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], INACTIVE);
    #else
    pinMode(rowPins[r], INPUT_TYPE);
    #endif
  }
  
  // Initialise the keyboard
  // EN layout
  Keyboard.begin();  

  led(false);
}

bool capLock = false;
bool ctrlLock = false;

bool scanKey(byte* d0, int d0Size,byte* d1, int d1Size, bool rotated) {
  bool keyPressed = false;
  for(int i=0; i < 5; i++) {
  for (byte y = 0 ; y < d0Size ; y++) {
    pinMode(d0[y], OUTPUT);
    digitalWrite(d0[y], ACTIVE);
      for (byte x = 0 ; x < d1Size ; x++) { 
        pinMode(d1[x], INPUT_TYPE);

        if (digitalRead(d1[x]) == ACTIVE) {
          debounceCount[rotated ? x : y][rotated ? y : x]++;
          keyPressed = true;
        }
    } // for row
    digitalWrite(d0[y], INACTIVE);
  } // for col
}
return keyPressed;
}


void sendChar(unsigned char found, char* strRepr, bool cap, bool sym, bool ctrl) {
  if ( sym ) { Keyboard.print( strRepr ); }
  else if (ctrl || cap ) { 
    if ( found > 0 && found < 32 ) {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press( ('a'+found-1) );
      delay(5);
      Keyboard.release( ('a'+found-1) );
      Keyboard.release(KEY_LEFT_CTRL);
    } else if ( found > 0 ) {
      Keyboard.write( found );
    }
  }
  else { Keyboard.write( found ); }

  ctrlLock = false; // auto release after char
  led2(ctrlLock);

  delay(DELAY_AFTER_KEYSTROKE);
}




void loop() {
  bool keyPressed = false;

  keyPressed |= scanKey(rowPins, NUM_ROWS, colPins, NUM_COLS, false);

  // if found something : rotate 90deg then re-scan
  // Cf key combos ....
  keyPressed |= scanKey(colPins, NUM_COLS, rowPins, NUM_ROWS, true);

  unsigned char found = 0x00;
  bool cap = false;
  bool sym = false;
  int pressedR=-1, pressedC=-1;
  int debounce = 0;

  // auto apply CAP LOCK if needed
  cap |= capLock;

  for(int c=0; c < NUM_COLS; c++) {
    for(int r=0; r < NUM_ROWS; r++) {
      int count = debounceCount[r][c];
      if (count > 0) {
        unsigned char defMapKey = keyMapDef[c][r];
        if ( defMapKey < 0x7F ) {
          found = defMapKey;
          pressedR = r;
          pressedC = c;
          debounceTotal[r][c]++;
          debounce = debounceTotal[r][c];
        } else {
          // >> no ELSE cf CAP+SYM => EXT MODE key
          if ( defMapKey == CAP_KEY ) { cap = true; }
          if ( defMapKey == SYM_KEY ) { sym = true; }
        } 
        debounceCount[r][c] = 0;
      }
    }
  }

  if ( !keyPressed ) {
    for (byte c = 0 ; c < NUM_COLS ; c++) {
      for (byte r = 0 ; r < NUM_ROWS ; r++) {
        debounceTotal[r][c] = 0;
      }
    }

    return;
  }


  char* strRepr = (char*)"Oups";

  if ( found != 0x00 ) {

    if ( debounce >= DEBOUNCE_VALUE_XTS ) {

      if ( ctrlLock ) {
        found = keyMapCtrl[pressedC][pressedR];
      } else if ( cap ) {
        found = keyMapCap[pressedC][pressedR];
        if ( found == CAPLOCK_KEY ) { 
          // Keyboard.println("Cap"); 
          capLock = !capLock; led(capLock); found = 0x00; 
          delay(70);
        } // CAPLOCK Key
        else if ( found == CTRL_KEY ) { 
          // Keyboard.println("Ctr"); 
          ctrlLock = !ctrlLock; led2(ctrlLock); found = 0x00; 
          delay(70);
        } // EDIT Key
      } else if ( sym ) {
        strRepr = (char*)keyMapSymb[pressedC][pressedR];
      }
    }

    // auto apply CAP LOCK if needed
    cap |= capLock;

    if (found == 0x00) { return; }

    if ( debounce == DEBOUNCE_VALUE_XTS ) {
      sendChar( found, strRepr, cap, sym, ctrlLock );
    } else if ( debounce >= DEBOUNCE_REPEAT_XTS ) {
      if ( (debounce % DEBOUNCE_REPEAT_XTS) == 0 ) {
        sendChar( found, strRepr, cap, sym, ctrlLock );
      }
    }

  }

}
