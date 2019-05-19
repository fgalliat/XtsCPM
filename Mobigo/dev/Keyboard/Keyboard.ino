/**
 * Xtase - fgalliat @May 2019
 * 
 * Vtech Mobigo I - Keyboard
 * Control w/ an SX1509
 * 
 * @ this time : only the key [ ? ] is missing....
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

const byte ARDUINO_INTERRUPT_PIN = 2;
// NB Vs BeGin
#define ROWS_NB 5
#define ROWS_BG 0

// #define COLS_NB 10
// #define COLS_BG 6
#define COLS_NB 9
#define COLS_BG 7

// #define TEMP_CODE 1
#ifdef TEMP_CODE
#endif

  // for ABCDEF layout
  const char regularMap[ROWS_NB][COLS_NB] = {
    { 0x00, 'a', 0x00, 0x00, 0x00, ' ', 0x00, 0x00, 0x00 },
    { '\b', 0x00, 't', 'u', 'v', 'w', 'x', 'y', 'z' },
    { 's', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' },
    { 'j', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i' },
    { 0x00, 0x00, 0x00, 0x00, 0x00, '\n', 0x00, 0x00, 0x00 },
  };

  #define KEYB_BUFF_LEN 16
  char _keyBuff[KEYB_BUFF_LEN+1];
  void flushBuffer() {
    memset(_keyBuff, 0x00, KEYB_BUFF_LEN+1);
  }

  bool _kbdShift = false;
  bool _kbdNums  = false;
  bool _kbdSymbs = false;

  // #define getKeyAddr(row, col) { return (COLS_NB * row) + col; }

  void _activateRow(int row) {
    io.digitalWrite(ROWS_BG+row, HIGH);
    delay(2);
  }

  void _deactivateRow(int row) {
    io.digitalWrite(ROWS_BG+row, LOW);
    // delay(2);
  }

  bool _isColPressed(int col) {
    return io.digitalRead( COLS_BG+col ) == HIGH;
  }

  bool _isKeyPressed(int row, int col) {
    _activateRow(row);
    bool ok = _isColPressed(col);
    _deactivateRow(row);
    return ok;
  }

  void _deactivateAllRows() {
    for(int i=0; i < ROWS_NB; i++) { _deactivateRow(i); }
  }

  long lastPollTime = -1L;
  bool lastTimeKeyReleased = true;

  void _doPoll() {
    _deactivateAllRows();

    if ( lastPollTime == -1L ) {
      lastPollTime = millis();
    } else {
      if ( millis() - lastPollTime < 50 ) { return; }

      // filter + auto repeat
      if ( !lastTimeKeyReleased ) { 
        lastTimeKeyReleased = ( millis() - lastPollTime > 300 ); 
        return; 
      }
    }

    // read the metas
    _kbdShift = _isKeyPressed(1,1);
    _kbdNums  = _isKeyPressed(0,2);
    _kbdSymbs = _isKeyPressed(0,3);

    bool oneFoundOnKbd = false;
    bool oneFoundOnRow = false;

    // read char
    for(int row=0; row < ROWS_NB; row++) {
      _activateRow(row);
      oneFoundOnRow = false;

      for(int col=0; col < COLS_NB; col++) {
        if ( _isColPressed(col) ) {
          oneFoundOnKbd = true;
          oneFoundOnRow = true;
          char ch = getKeychar(row, col);
          if ( ch != 0x00 ) {
            int avail = strlen(_keyBuff);
            if ( avail <= KEYB_BUFF_LEN ) {
              strcat(_keyBuff, &ch);
            } else {
              // Overflow
            }
          }
        }
      }

      _deactivateRow(row);
      if ( oneFoundOnRow ) { break; }
    }

    lastTimeKeyReleased = !oneFoundOnKbd;
    lastPollTime = millis();
  }

  // ========================================

  char getKeychar(int row, int col) {
    // if ( _kbdShift ) { return shiftedMap[row][col]; }
    return regularMap[row][col];
  } 

  void pollKbd() {
    _doPoll();
  }

  int availableKbd() {
    return strlen( _keyBuff );
  }

  int readKbd() {
    int avail = strlen( _keyBuff );
    if ( avail <= 0 ) { return -1; }

    char ch = _keyBuff[0];
    memmove( &_keyBuff[0], &_keyBuff[1], avail-1); // OK that works
    _keyBuff[avail-1] = 0x00;

    return (int)ch;
  }

  void setupKbd() {
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
  }


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

  setupKbd();

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
    Serial.print( res );
    Serial.print( " | (" );
    Serial.print( regularMap[row][res] );
    Serial.print( ")" );
    Serial.println();
}

void loop() 
{
  /*
    disp[16] = 0x00;
    for(int row=0; row < ROWS_NB; row++) {
        dispRow(row);
    }
  */
  pollKbd();

  if ( availableKbd() > 0 ) {
    Serial.print( (char)readKbd() );
  }


//   // If the SX1509 INT pin goes low, a keypad button has
//   // been pressed:
//   if (digitalRead(ARDUINO_INTERRUPT_PIN) == LOW)

  delay(1);  // Gives releaseCountMax a more intuitive unit
}
