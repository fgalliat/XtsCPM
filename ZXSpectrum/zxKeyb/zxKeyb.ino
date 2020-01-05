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

// Keymap for normal use

byte keyMap[NUM_ROWS][NUM_COLS] =
{
  {'5', '4', '3', '2', '1'},  // 0
  {'t', 'r', 'e', 'w', 'q'},  // 1
  {'6', '7', '8', '9', '0'},  // 2 
  {'g', 'f', 'd', 's', 'a'},  // 3
  {'y', 'u', 'i', 'o', 'p'},  // 4 ->
  {'v', 'c', 'x', 'z', 0},    // 5
  {'h', 'j', 'k', 'l', KEY_RETURN}, // 6
  {'b', 'n', 'm', '.', ' '}   // 7
};

// Keymap if Shift is pressed

byte keyMapShifted[NUM_ROWS][NUM_COLS] =
{
  {KEY_LEFT_ARROW, '$', '\\', '@', KEY_ESC},
  {'T', 'R', 'E', 'W', 'Q'},
  {KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW, '!', KEY_BACKSPACE},
  {'G', 'F', 'D', 'S', 'A'},
  {'Y', 'U', 'I', 'O', 'P'},
  {'V', 'C', 'X', 'Z', 0},
  {'H', 'J', 'K', 'L', KEY_F5},
  {'B', 'N', 'M', ',', '#'} 
};

// Keymap if Function-Shift pressed
// NEXT key read should be from this table

byte keyMapAlt[NUM_ROWS][NUM_COLS] =
{
  {KEY_LEFT_ARROW, '}', '{', ']', '['},
  {'=', '$', '(', ')', '"'},
  {KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW, '!', KEY_BACKSPACE},
  {0, '_', '~', '|', '@'},
  {'>', '$', '(', ')', '"'},
  {'/', '?', ';', ':', 0},
  {'*', '-', '+', '=', KEY_RETURN},
  {'*', '<', '>', '\'', '#'}
};


// Global Variables
int debounceCount[NUM_ROWS][NUM_COLS];
int debounceTotal[NUM_ROWS][NUM_COLS];
int altKeyFlag;

// Define the row and column pins
// Arduino micro pins
byte colPins[NUM_COLS] = {8, 9, 10, 11, 12};
byte rowPins[NUM_ROWS] = {7, 6, 5, 4, 3, 2, 1, 0};


#define CAP_KEY 0xFF 
#define SYM_KEY 0xFE 

// row 4 seems to be mis wired ...
unsigned char keyMapDef[NUM_COLS][NUM_ROWS] = {
  // 0    1    2    3    4    5    6     7
  { '5', 't', 'g', '6', 'y', 'v', 'h',  'b' },
  { '4', 'r', 'f', '7', 'u', 'c', 'j',  'n' },
  { '3', 'e', 'd', '8', 'i', 'x', 'k',  'm' },
  { '2', 'w', 's', '9', 'o', 'z', 'l', 0xfe }, // 7,3 Symb SHIFT
  { '1', 'q', 'a', '0', 'p',0xff,'\n',  ' ' }  // 5,4 Cap SHIFT
};

// // char* to handle '<>' or '<=' sequences
// const char* keyMapDefGraph[NUM_COLS][NUM_ROWS] = {
//   { "%", "$", "#", "@", "!" } //...
// };



// 8 rows - ouputs
// 5 cols - inputs

// #define COLS_AS_INPUT 1
#define COLS_AS_INPUT 0

void setup() {
  // 0 to 4
  for (byte c = 0 ; c < NUM_COLS ; c++) {
    #if COLS_AS_INPUT
    pinMode(colPins[c], INPUT_PULLUP);
    #else
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c],HIGH);
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
    digitalWrite(rowPins[r], HIGH);
    #else
    pinMode(rowPins[r], INPUT_PULLUP);
    #endif
  }
  
  // Function key is NOT pressed
  altKeyFlag = ALT_KEY_OFF;
  
  // Initialise the keyboard
  // EN layout
  Keyboard.begin();  
}

// rows 8
// cols 5

void loop() {
  bool shifted = false;
  bool keyPressed = false;

for(int i=0; i < 3; i++) {
#if COLS_AS_INPUT  
  for (byte r = 0 ; r < NUM_ROWS ; r++) {
    digitalWrite(rowPins[r], LOW);
#else
  for (byte c = 0 ; c < NUM_COLS ; c++) {
    digitalWrite(colPins[c], LOW);
#endif    
      // 0 to 4
#if COLS_AS_INPUT      
      for (byte c = 0 ; c < NUM_COLS ; c++) { 
        if (digitalRead(colPins[c]) == LOW) {
#else
      for (byte r = 0 ; r < NUM_ROWS ; r++) { 
        if (digitalRead(rowPins[r]) == LOW) {
#endif
          // Increase the debounce count
          debounceCount[r][c]++;
          keyPressed = true;
        }
    } // for row
#if COLS_AS_INPUT    
    digitalWrite(rowPins[r], HIGH);
#else
    digitalWrite(colPins[c], HIGH);
#endif
  } // for col
}

if ( !keyPressed ) {

  for (byte c = 0 ; c < NUM_COLS ; c++) {
    for (byte r = 0 ; r < NUM_ROWS ; r++) {
      debounceTotal[r][c] = 0;
    }
  }

  // delay(3);
  return;
}

  char found = 0x00;
  bool cap = false;
  bool sym = false;
  int pressedR=-1, pressedC=-1;
  int debounce = 0;

  for(int c=0; c < NUM_COLS; c++) {
    for(int r=0; r < NUM_ROWS; r++) {
      if (debounceCount[r][c] > 0) {
          pressedR = r;
          pressedC = c;

        unsigned char defMapKey = keyMapDef[c][r];
        // Keyboard.print( (int)defMapKey );
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

  if ( found != 0x00 ) {

    #define DEBOUNCE_VALUE_XTS 5
    #define DEBOUNCE_REPEAT_XTS 20

    if ( debounce == DEBOUNCE_VALUE_XTS ) {
      Keyboard.print( found );

      Keyboard.print( "  " );
      Keyboard.print( debounce );
      // Keyboard.print( "  " );
      // Keyboard.print( pressedR );
      // Keyboard.print( ',' );
      // Keyboard.print( pressedC );

      if ( cap ) { Keyboard.print( " SHIFT" ); }
      if ( sym ) { Keyboard.print( " SYMB" ); }
      Keyboard.println(' ');
    }
  } else {
      // Keyboard.print( "  " );
      // Keyboard.print( pressedR );
      // Keyboard.print( ',' );
      // Keyboard.print( pressedC );
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
