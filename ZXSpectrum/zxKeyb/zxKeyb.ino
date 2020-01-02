/**
 * 
 * ZX Spectrum 8x5 keyboard decoding
 * 
 * found @ : https://smittytone.wordpress.com/2014/02/16/the-sinclair-zx81-a-raspberry-pi-retro-restyle-part-1/
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
  {'y', 'u', 'i', 'o', 'p'},  // 4
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

// byte colPins[NUM_COLS] = {13, 12, 11, 10, 9};
// byte rowPins[NUM_ROWS] = {7, 6, 5, 4, 3, 2, 1, 0};

// Arduino micro pins
byte colPins[NUM_COLS] = {8, 9, 10, 11, 12};
byte rowPins[NUM_ROWS] = {7, 6, 4, 5, 3, 2, 1, 0};

// SETUP

void setup()
{
  // Set all pins as inputs and activate pull-ups
  
  for (byte c = 0 ; c < NUM_COLS ; c++)
  {
    pinMode(colPins[c], INPUT);
    digitalWrite(colPins[c], HIGH);
    
    // Clear debounce counts
    
    for (byte r = 0 ; r < NUM_ROWS ; r++)
    {
      debounceCount[r][c] = 0;
    }
  }
  
  // Set all pins as inputs
  
  for (byte r = 0 ; r < NUM_ROWS ; r++)
  {
    pinMode(rowPins[r], INPUT);
  }
  
  // Function key is NOT pressed
  
  altKeyFlag = ALT_KEY_OFF;
  
  // Initialise the keyboard
  
  Keyboard.begin();  
}

// LOOP

void loop()
{
  bool shifted = false;
  bool keyPressed = false;
  
  // Check for the Shift key being pressed
  
  pinMode(rowPins[SHIFT_ROW], OUTPUT);
  
  if (digitalRead(colPins[SHIFT_COL]) == LOW) shifted = true;
  
  if (shifted == true && altKeyFlag == ALT_KEY_ON)
  {
    // NOP to prevent Function selection from autorepeating
  }
  else
  {
    pinMode(rowPins[SHIFT_ROW], INPUT);
    
    for (byte r = 0 ; r < NUM_ROWS ; r++)
    {
      // Run through the rows, turn them on
      
      pinMode(rowPins[r], OUTPUT);
      digitalWrite(rowPins[r], LOW);
      
      for (byte c = 0 ; c < NUM_COLS ; c++)
      { 
        if (digitalRead(colPins[c]) == LOW)
        {
          // Increase the debounce count
          
          debounceCount[r][c]++;
          
          // Has the switch been pressed continually for long enough?
          
          int count = debounceCount[r][c];
          if (count == DEBOUNCE_VALUE)
          {
            // First press
            
            keyPressed = true;
            pressKey(r, c, shifted);
          }
          else if (count > DEBOUNCE_VALUE)
          {
            // Check for repeats
            
            count -= DEBOUNCE_VALUE;
            if (count % REPEAT_DELAY == 0)
            {
              // Send repeat
              
              keyPressed = true;
              pressKey(r, c, shifted);
            }
          }
        }
        else
        {
          // Not pressed; reset debounce count
          
          debounceCount[r][c] = 0;
        }
      }
     
    // Turn the row back off
     
    pinMode(rowPins[r], INPUT);
    }
    
    digitalWrite(rowPins[SHIFT_ROW], LOW);
  }
}

void pressKey(byte r, byte c, bool shifted)
{  
  // Send the keypress
  
  byte key = shifted ? keyMapShifted[r][c] : keyMap[r][c];

  if (key == KEY_F5)
  {
    // If the Function key pressed (Shift + New Line)
    
    altKeyFlag = ALT_KEY_ON;
    key = 0;
    debounceCount[r][c] = 0;
  }
  
  if (altKeyFlag == ALT_KEY_ON)
  {
    // Get the Alt key pressed after Function has been selected
    
    key = keyMapAlt[r][c];
    altKeyFlag = ALT_KEY_OFF;
  }

  // send the key
  
  if (key > 0) Keyboard.write(key);
  // Keyboard.print( r );
  // Keyboard.print( 'x' );
  // Keyboard.println( c );
}
