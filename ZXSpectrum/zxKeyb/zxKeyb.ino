/**
 * 
 * ZX Spectrum+ 8x5 keyboard decoding
 * 
 * found @ : https://smittytone.wordpress.com/2014/02/16/the-sinclair-zx81-a-raspberry-pi-retro-restyle-part-1/
 * was initialy for a regular ZX Spectrum keyboard
 * 
 * 
 * for a 32u4 board
 * used an Arduino MICRO board
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

#define SHIFT_COL 4
#define SHIFT_ROW 5

// #define DEBOUNCE_VALUE 250
#define DEBOUNCE_VALUE 150
#define REPEAT_DELAY 500

#define ALT_KEY_ON 255
#define ALT_KEY_OFF 0

// Global Variables
int debounceCount[NUM_ROWS][NUM_COLS];
int debounceTotal[NUM_ROWS][NUM_COLS];
int altKeyFlag;

// Define the row and column pins
// Arduino micro pins
byte colPins[NUM_COLS] = {8, 9, 10, 11, 12};
byte rowPins[NUM_ROWS] = {7, 6, 5, 4, 3, 2, 1, 0};

#define LED 13

#define CAP_KEY 0xFF 
#define SYM_KEY 0xFE 
#define CAPLOCK_KEY 0xFD 
#define CTRL_KEY 0xFC 

// Ctrl is a toggleKey -> use a led for that
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
  {  KEY_F1,        'R', 'F', KEY_UP_ARROW,   'U', 'C', 'J',  'N' },
  { CTRL_KEY,       'E', 'D', KEY_RIGHT_ARROW,'I', 'X', 'K',  'M' },
  { CAPLOCK_KEY,    'W', 'S', KEY_TAB,        'O', 'Z', 'L',  0xfe}, // 7,3 Symb SHIFT
  { KEY_TAB,        'Q', 'A', KEY_BACKSPACE,  'P',0xff,'\n',  0x03}  // 5,4 Cap SHIFT // 0x03 BREAK / CtrlC
};

const char* keyMapSymb[NUM_COLS][NUM_ROWS] = {
  // 0    1    2    3    4    5    6     7
  { "%",  ">", "}",  "&",  "[", "/", "^",  "*" },
  { "$",  "<", "{",  "'",  "]", "?", "-",  "," },
  { "#", ">=", "\\", "(",  "@", "X", "+",  "." },
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


// 8 rows - ouputs
// 5 cols - inputs

// #define COLS_AS_INPUT 1
#define COLS_AS_INPUT 0

#define INPUT_TYPE INPUT_PULLUP
// #define INPUT_TYPE INPUT

#define ACTIVE LOW
#define INACTIVE HIGH

void led(bool state) {
  digitalWrite(LED, state ? HIGH : LOW);
}

void setup() {

  pinMode(LED, OUTPUT);
  led(false);

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
  
  // Function key is NOT pressed
  altKeyFlag = ALT_KEY_OFF;
  
  // Initialise the keyboard
  // EN layout
  Keyboard.begin();  

  led(false);
}

// rows 8
// cols 5

bool capLock = false;

bool scanKey(byte* d0, int d0Size,byte* d1, int d1Size, bool rotated) {
  bool keyPressed = false;
  for(int i=0; i < 5; i++) {
  for (byte y = 0 ; y < d0Size ; y++) {
    pinMode(d0[y], OUTPUT);
    digitalWrite(d0[y], ACTIVE);
      // 0 to 4
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

    // if ( found >= '0' && found <= 'z' ) { Keyboard.write( found );  }
    // else { Keyboard.press( found ); delay(5); Keyboard.release( found ); }
  }
  else { Keyboard.write( found ); }
}


bool capLocked = false;
bool ctrl = false;

void loop() {
  bool keyPressed = false;

  keyPressed = scanKey(rowPins, NUM_ROWS, colPins, NUM_COLS, false);

  if ( !keyPressed ) {
    for (byte c = 0 ; c < NUM_COLS ; c++) {
      for (byte r = 0 ; r < NUM_ROWS ; r++) {
        debounceTotal[r][c] = 0;
      }
    }

    return;
  }

  // if found something : rotate 90deg then re-scan
  // Cf key combos ....
  keyPressed = scanKey(colPins, NUM_COLS, rowPins, NUM_ROWS, true);

  char found = 0x00;
  bool cap = false;
  bool sym = false;
  int pressedR=-1, pressedC=-1;
  int debounce = 0;

  // auto apply CAP LOCK if needed
  cap |= capLock;

  for(int c=0; c < NUM_COLS; c++) {
    for(int r=0; r < NUM_ROWS; r++) {
      if (debounceCount[r][c] > 0) {
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

  char* strRepr = (char*)"Oups";

  if ( found != 0x00 ) {

    #define DEBOUNCE_VALUE_XTS 15
    #define DEBOUNCE_REPEAT_XTS 30

    if ( debounce >= DEBOUNCE_VALUE_XTS ) {

      if ( ctrl ) {
        found = keyMapCtrl[pressedC][pressedR];
        ctrl = false; // auto release after char
      } else if ( cap ) {
        if ( found == CAPLOCK_KEY ) { capLock = !capLock; led(capLock); found = 0x00; } // CAPLOCK Key
        else if ( found == CTRL_KEY ) { ctrl = true; found = 0x00; } // EDIT Key
        else found = keyMapCap[pressedC][pressedR];
      } else if ( sym ) {
        strRepr = (char*)keyMapSymb[pressedC][pressedR];
      }
    }

    if (found == 0x00) { return; }

    if ( debounce == DEBOUNCE_VALUE_XTS ) {
      sendChar( found, strRepr, cap, sym, ctrl );
    } else if ( debounce >= DEBOUNCE_REPEAT_XTS ) {
      if ( (debounce % DEBOUNCE_REPEAT_XTS) == 0 ) {
        sendChar( found, strRepr, cap, sym, ctrl );
      }
    }
  }

}

// long lastKey = -1;

// #define DBUG 1

// void pressKey(byte r, byte c, bool shifted)
// {  

//   if ( lastKey == -1 ) { lastKey = millis(); }
//   else if ( millis() - lastKey >= 500 ) {
//     #if DBUG
//       Keyboard.write(KEY_RETURN);
//     #endif
//     lastKey = millis();
//   }


//   // Send the keypress
//   byte key = shifted ? keyMapShifted[r][c] : keyMap[r][c];

//   if (key == KEY_F5)
//   {
//     // If the Function key pressed (Shift + New Line)
    
//     altKeyFlag = ALT_KEY_ON;
//     key = 0;
//     debounceCount[r][c] = 0;
//   }
  
//   if (altKeyFlag == ALT_KEY_ON)
//   {
//     // Get the Alt key pressed after Function has been selected
    
//     key = keyMapAlt[r][c];
//     altKeyFlag = ALT_KEY_OFF;
//   }

//   // send the key
  
//   if (key > 0) Keyboard.write(key);
//   #if DBUG
//     Keyboard.print( " [" );
//     Keyboard.print( r );
//     Keyboard.print( 'x' );
//     Keyboard.print( c );
//     Keyboard.println( "]" );
//   #endif
// }
