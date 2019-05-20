/**
 * Xtase - fgalliat @May 2019
 * 
 * Vtech Mobigo Keyboard decoding API
 * uses SX1509 breakout board (from Sparkfun)
 * 
 * 5 rows [0..4]
 * 9 cols [7..15]
 * 1 NC 
 * 
 */


#include "MobigoKeyboard.h"


  MobigoKeyboard::MobigoKeyboard(SX1509* gpio)
  {
      this->io = gpio;
  }
  
  MobigoKeyboard::~MobigoKeyboard()
  {
  }

  void MobigoKeyboard::setup() {
    int i=0;
    for(; i < KB_COLS_NB; i++) {
      this->io->pinMode( KB_COLS_BG+i, INPUT );
    }

    i=0;
    for(; i < KB_ROWS_NB; i++) {
      this->io->pinMode( KB_ROWS_BG+i, OUTPUT );
      this->io->digitalWrite(KB_ROWS_BG+i, LOW);
    }

    this->flushBuffer();

    delay(300);
  }

  int MobigoKeyboard::available() {
    return strlen( this->_keyBuff );
  }

  int MobigoKeyboard::read() {
    int avail = strlen( this->_keyBuff );
    if ( avail <= 0 ) { return -1; }

    char ch = this->_keyBuff[0];
    memmove( &(this->_keyBuff[0]), &(this->_keyBuff[1]), avail-1);
    this->_keyBuff[avail-1] = 0x00;

    return (int)ch;
  }

  long lastPollTime = -1L;
  bool lastTimeKeyReleased = true;

  void MobigoKeyboard::poll() {
      this->deactivateAllRows();

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

    // read the metaKeys
    this->_kbdShift = this->isKeyPressed(1,1);
    this->_kbdNums  = this->isKeyPressed(0,2);
    this->_kbdSymbs = this->isKeyPressed(0,3);

    bool oneFoundOnKbd = false;
    bool oneFoundOnRow = false;

    // read char
    for(int row=0; row < KB_ROWS_NB; row++) {
      this->activateRow(row);
      oneFoundOnRow = false;

      for(int col=0; col < KB_COLS_NB; col++) {
        if ( this->isColPressed(col) ) {
          oneFoundOnKbd = true;
          oneFoundOnRow = true;
          char ch = this->getKeychar(row, col);
          if ( ch != 0x00 ) {
            int avail = strlen(this->_keyBuff);
            if ( avail <= KEYB_BUFF_LEN ) {
              strcat(this->_keyBuff, &ch);
            } else {
              // Overflow
            }
          }
        }
      }

      this->deactivateRow(row);
      if ( oneFoundOnRow ) { break; }
    }

    lastTimeKeyReleased = !oneFoundOnKbd;
    lastPollTime = millis();
  }

  void MobigoKeyboard::activateRow(int row) {
      this->io->digitalWrite(KB_ROWS_BG+row, HIGH);
      delay(2);
  }

  void MobigoKeyboard::deactivateRow(int row) {
      this->io->digitalWrite(KB_ROWS_BG+row, LOW);
  }

  void MobigoKeyboard::deactivateAllRows() {
      for(int i=0; i < KB_ROWS_NB; i++) { this->deactivateRow(i); }
  }

  bool MobigoKeyboard::isColPressed(int col) {
      return this->io->digitalRead( KB_COLS_BG+col ) == HIGH;
  }

  bool MobigoKeyboard::isKeyPressed(int row, int col) {
    this->activateRow(row);
    bool ok = this->isColPressed(col);
    this->deactivateRow(row);
    return ok;
  }

  char MobigoKeyboard::getKeychar(int row, int col) {
    // if ( _kbdShift ) { return shiftedMap[row][col]; }
    return regularMap[row][col];
  }
