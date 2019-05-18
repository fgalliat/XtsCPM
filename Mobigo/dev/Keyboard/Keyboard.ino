/**
 * Xtase - fgalliat @May 2019
 * 
 * Vtech Mobigo I - Keyboard
 * Control w/ an SX1509
 * 
 * Arduino UNO
 * SDA : A4
 * SCL : A5
 */

#include <Wire.h> // Include the I2C library (required)
#include <SparkFunSX1509.h> // Include SX1509 library

// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte SX1509_ADDRESS = 0x3E;  // SX1509 I2C address
SX1509 io; // Create an SX1509 object to be used throughout

// #define KEY_ROWS 4 // Number of rows in the keypad matrix
// #define KEY_COLS 3 // Number of columns in the keypad matrix

// // keyMap maps row/column combinations to characters:
// char keyMap[KEY_ROWS][KEY_COLS] = {
// { '1', '2', '3'},
// { '4', '5', '6'},
// { '7', '8', '9'},
// { '*', '0', '#'}};

const byte ARDUINO_INTERRUPT_PIN = 2;
// NB Vs BeGin
#define ROWS_NB 5
#define ROWS_BG 0

// #define COLS_NB 10
// #define COLS_BG 6
#define COLS_NB 9
#define COLS_BG 7

void setup() 
{
  // Serial is used to display the keypad button pressed:
  Serial.begin(9600);
  // Call io.begin(<address>) to initialize the SX1509. If it
  // successfully communicates, it'll return 1.
  if (!io.begin(SX1509_ADDRESS))
  {
    Serial.println("Failed to communicate.");
    while (1) ; // If we fail to communicate, loop forever.
  }
  delay(300);

  int i=0;
  for(; i < COLS_NB; i++) {
    // io.pinMode( i, INPUT_PULLUP );
    io.pinMode( COLS_BG+i, INPUT );
  }


  i=0;
  for(; i < ROWS_NB; i++) {
    io.pinMode( ROWS_BG+i, OUTPUT );
    io.digitalWrite(ROWS_BG+i, LOW);
  }

  delay(300);

  // Set up the Arduino interrupt pin as an input w/ 
  // internal pull-up. (The SX1509 interrupt is active-low.)
  pinMode(ARDUINO_INTERRUPT_PIN, INPUT_PULLUP);
}

int scanLine(int row) {
    io.digitalWrite(ROWS_BG+row, HIGH);
    delay(2);
    int result = -1;
    int col = 0;
    // col = 1; // ignore 1st col for now
    for(; col < COLS_NB; col++) {
        int c = io.digitalRead( COLS_BG+col ) == HIGH ? 1 : 0;
        if (c > 0) { result = col; break; }
    }
    io.digitalWrite(ROWS_BG+row, LOW);
    delay(2);
    return result;
}


char disp[COLS_NB+1];


void dispRow(int row) {
    int res = scanLine(row);

    if (res < 0) return;

    memset(disp, ' ', COLS_NB);
    if ( res > -1 ) { disp[res] = '#'; }
    Serial.print( row );
    Serial.print( " | " );
    Serial.print( disp );
    Serial.print( " | " );
    Serial.println( res );
}

void loop() 
{
    disp[16] = 0x00;

    for(int row=0; row < ROWS_NB; row++) {
        dispRow(row);
    }

//   // If the SX1509 INT pin goes low, a keypad button has
//   // been pressed:
//   if (digitalRead(ARDUINO_INTERRUPT_PIN) == LOW)

  delay(1);  // Gives releaseCountMax a more intuitive unit
}
