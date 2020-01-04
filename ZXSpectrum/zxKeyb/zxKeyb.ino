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
int altKeyFlag;

// Define the row and column pins
// Arduino micro pins
byte colPins[NUM_COLS] = {8, 9, 10, 11, 12};
byte rowPins[NUM_ROWS] = {7, 6, 4, 5, 3, 2, 1, 0};


#define CAP_KEY 0xFF 
#define SYM_KEY 0xFE 

// row 4 seems to be mis wired ...
char keyMapDef[NUM_COLS][NUM_ROWS] = {
  // 0    1    2    3    4    5    6     7
  { '5', 't', '6', 'g', '?', 'v', 'h',  'b' },
  { '4', 'r', '7', 'f', '?', 'c', 'j',  'n' },
  { '3', 'e', '8', 'd', '?', 'x', 'k',  'm' },
  { '2', 'w', '9', 's', '?', 'z', 'l', 0xfe }, // 7,3 Symb SHIFT
  { '1', 'q', '0', 'a', '?',0xff,'\n',  ' ' }  // 5,4 Cap SHIFT
};

// // char* to handle '<>' or '<=' sequences
// const char* keyMapDefGraph[NUM_COLS][NUM_ROWS] = {
//   { "%", "$", "#", "@", "!" } //...
// };



// 8 rows - input
// 5 cols - ouputs

void setup() {
  // Set all pins as inputs and activate pull-ups
  // 0 to 4
  for (byte c = 0 ; c < NUM_COLS ; c++) {
        pinMode(colPins[c], INPUT_PULLUP);
    // pinMode(colPins[c], OUTPUT);
    // digitalWrite(colPins[c], HIGH);
    
    for (byte r = 0 ; r < NUM_ROWS ; r++) {
      debounceCount[r][c] = 0;
    }
  }
  
  // Set all pins as inputs
  // 0 to 7
  for (byte r = 0 ; r < NUM_ROWS ; r++) {
    // pinMode(rowPins[r], INPUT);
    // digitalWrite(rowPins[r], HIGH);
    // pinMode(rowPins[r], INPUT_PULLUP);
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }
  
  // Function key is NOT pressed
  altKeyFlag = ALT_KEY_OFF;
  
  // Initialise the keyboard
  // EN layout
  Keyboard.begin();  
}

// rows 8
// cols 5
byte cols[NUM_COLS];

const byte bits[8] = {1,2,4,8,16,32,64,128};

void loop() {
  bool shifted = false;
  bool keyPressed = false;
  
  for (byte r = 0 ; r < NUM_ROWS ; r++) {
    digitalWrite(rowPins[r], LOW);

      // 0 to 4
      for (byte c = 0 ; c < NUM_COLS ; c++) { 

        // if (digitalRead(rowPins[r]) == LOW) {
        if (digitalRead(colPins[c]) == LOW) {
          // Increase the debounce count
          debounceCount[r][c]++;
        }
    } // for row
    digitalWrite(rowPins[r], HIGH);
  } // for col


  char found = 0x00;
  bool cap = false;
  bool sym = false;

  for(int c=0; c < NUM_COLS; c++) {
    for(int r=0; r < NUM_ROWS; r++) {
      if (debounceCount[r][c] > 0) {

        char defMapKey = keyMapDef[c][r];

        if ( defMapKey < 127 ) {
          found = defMapKey;
        } else {
          if ( defMapKey == CAP_KEY ) { cap = true; }
          if ( defMapKey == SYM_KEY ) { sym = true; }
        } 
        
        debounceCount[r][c] = 0;
      }
    }
  }

  if ( found > 0x00 ) {
      Keyboard.print( found );
      if ( cap ) { Keyboard.print( " SHIFT" ); }
      if ( sym ) { Keyboard.print( " SYMB" ); }
      Keyboard.println(' ');
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
