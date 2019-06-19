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

#include "xts_yatl_settings.h"
#ifdef LAYOUT_MOBIGO

#include "xts_dev_MobigoKeyboard.h"

// higher speed R/W for SX1509 
// #define HARDCORE_SX_READ 1
#define HARDCORE_SX_READ 0

// #define INV_LOGIC 0
#define INV_LOGIC 1

#ifdef HARDCORE_SX_READ
  #include <Wire.h> // Include the I2C library (required)
  #define REG_DATA_B				0x10	//	RegDataB Data register _ I/O[15_8] (Bank B) 1111 1111*
  #define deviceAddress 0x3E

  #define FIX_MODE 1

  // writeWord(byte registerAddress, ungisnged int writeValue)
  //	This function writes a two-byte word to registerAddress and registerAddress + 1
  //	- the upper byte of writeValue is written to registerAddress
  //		- the lower byte of writeValue is written to registerAddress + 1
  //	- No return value.
  void SX1509_writeWord(byte registerAddress, unsigned int writeValue)
  {
    byte msb, lsb;
    msb = ((writeValue & 0xFF00) >> 8);
    lsb = (writeValue & 0x00FF);
    Wire.beginTransmission(deviceAddress);
    Wire.write(registerAddress);
    Wire.write(msb);
    Wire.write(lsb);
    Wire.endTransmission();	
  }

  // readWord(byte registerAddress)
  //	This function will read a two-byte word beginning at registerAddress
  //	- A 16-bit unsigned int will be returned.
  //		- The msb of the return value will contain the value read from registerAddress
  //		- The lsb of the return value will contain the value read from registerAddress + 1
  unsigned int SX1509_readWord(byte registerAddress)
  {
    unsigned int readValue;
    unsigned int msb, lsb;
    unsigned int timeout = RECEIVE_TIMEOUT_VALUE * 2;

    Wire.beginTransmission(deviceAddress);
    Wire.write(registerAddress);
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, (byte) 2);

    while ((Wire.available() < 2) && (timeout != 0))
      timeout--;
      
    if (timeout == 0)
      return 0;
    
    msb = (Wire.read() & 0x00FF) << 8;
    lsb = (Wire.read() & 0x00FF);
    readValue = msb | lsb;

    return readValue;
  }

  extern MobigoKeyboard kbd;

  unsigned int SX_readBanks() {
    unsigned int banks = SX1509_readWord(REG_DATA_B);
    #if FIX_MODE
      if ( !kbd.gpio_ok ) { return 0; }

      // A bit dirty for sure !
      delay(1);
      unsigned int banksSURE = SX1509_readWord(REG_DATA_B);

      if ( banks == banksSURE && banks != 0 ) {
        return banks;
      }

      delay(1);
      unsigned int banksSURE2 = SX1509_readWord(REG_DATA_B);

      bool b0=false,b1=false,b2=false;

      if ( banks != 0 ) { b0 = true; }
      if ( banksSURE != 0 ) { b1 = true; }
      if ( banksSURE2 != 0 ) { b2 = true; }

      if ( (b0 && b2) && (banks == banksSURE2) ) {
        return banks;
      }

      if ( (b1 && b2) && (banksSURE == banksSURE2) ) {
        return banksSURE;
      }

      return b2 ? banksSURE2 : (b1 ? banksSURE : banks);

    #endif

    return banks;
  }

  // U have to ensure that Ur pin is really an INPUT !!!!
  byte SX_readPin(unsigned int banksValue, byte pin) {
    if ( !kbd.gpio_ok ) { return 1; }

    if (banksValue & (1<<pin))
			return 1;
    return 0; 
  }

  // U have to ensure that Ur pins are really an OUTPUTs !!!!
  void SX_writeBanks(unsigned int curRegData) {
    if ( !kbd.gpio_ok ) { return; }
		SX1509_writeWord(REG_DATA_B, curRegData);
  }

  // U have to ensure that Ur pin is really an OUTPUT !!!!
  void SX_writePin(byte pin, byte highLow) {
    // the gain is of @least one transaction
    unsigned int tempRegData = SX_readBanks();
		if (highLow)	tempRegData |= (1<<pin);
		else			tempRegData &= ~(1<<pin);
		SX_writeBanks(tempRegData);
  }

  

  // U have to ensure that Ur pin is really an OUTPUT !!!!
  void SX_writeBank(unsigned int curRegData, byte pin, byte highLow) {
    // the gain is of @least two transaction
		if (highLow)	curRegData |= (1<<pin);
		else			curRegData &= ~(1<<pin);
		SX_writeBanks(curRegData);
  }

  // U have to ensure that Ur pin is really an OUTPUT !!!!
  unsigned int SX_alterBanks(unsigned int curRegData, byte pin, byte highLow) {
    if (highLow)	curRegData |= (1<<pin);
		else			curRegData &= ~(1<<pin);
    return curRegData;
  }

  

#endif

  MobigoKeyboard::MobigoKeyboard(SX1509* gpio, bool autoPoll)
  {
      this->io = gpio;
      
      this->io->reset(false);
      delay(200);
      this->io->begin();
      delay(50);

      this->io->debounceTime(2);
      this->setAutoPoll(autoPoll);

      // Cf SX could not respond, then uses telnetd
      // so needs a cleaned buffer
      this->flushBuffer();
  }
  
  MobigoKeyboard::~MobigoKeyboard()
  {
  }

  void MobigoKeyboard::reboot() {
    bool hard = false;
    this->io->reset(hard);
  }

  void MobigoKeyboard::led(int num, bool state) {
    if ( num == 0 && led0 > -1 ) { digitalWrite(led0, state ? HIGH : LOW); }
    if ( num == 1 && led1 > -1 ) { digitalWrite(led1, state ? HIGH : LOW); }
    if ( num == 2 && led2 > -1 ) { digitalWrite(led2, state ? HIGH : LOW); }
  }

  void MobigoKeyboard::setup(int led0, int led1, int led2) {
    this->led0 = led0;
    this->led1 = led1;
    this->led2 = led2;
    int i=0;
    for(; i < KB_COLS_NB; i++) {
      #if INV_LOGIC
        this->io->pinMode( KB_COLS_BG+i, INPUT_PULLUP );
      #else
        this->io->pinMode( KB_COLS_BG+i, INPUT );
      #endif
    }

    i=0;
    for(; i < KB_ROWS_NB; i++) {
      this->io->pinMode( KB_ROWS_BG+i, OUTPUT );
      #if INV_LOGIC
        this->io->digitalWrite(KB_ROWS_BG+i, HIGH);
      #else
        this->io->digitalWrite(KB_ROWS_BG+i, LOW);
      #endif
    }

    this->flushBuffer();
    this->deactivateAllRows();

    delay(300);
  }

  int MobigoKeyboard::available() {
    if ( this->_autoPoll ) { this->poll(); }

    return strlen( this->_keyBuff );
  }

  int MobigoKeyboard::read() {
    if ( this->_autoPoll ) { this->poll(); }

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
    #if not HARDCORE_SX_READ
    this->deactivateAllRows();
    #endif

    if ( lastPollTime == -1L ) {
      lastPollTime = millis();
    } else {
      if ( millis() - lastPollTime < 10 ) { return; }

      // filter + auto repeat
      if ( !lastTimeKeyReleased ) { 
        lastTimeKeyReleased = ( millis() - lastPollTime > 250 ); 
        return; 
      }
    }

    // read the metaKeys
    // remanant style

    this->deactivateAllRows();
    bool loclShift = this->isKeyPressed(1,1);
    #if HARDCORE_SX_READ
      this->deactivateAllRows();
      activateRow(0);
      unsigned int tbank = SX_readBanks();
      bool loclNums  = SX_readPin(tbank, KB_COLS_BG+2);
      bool loclSymbs = SX_readPin(tbank, KB_COLS_BG+3);
      // deactivateRow(0);
      this->deactivateAllRows();
    #else
      bool loclNums  = this->isKeyPressed(0,2);
      bool loclSymbs = this->isKeyPressed(0,3);
    #endif

    bool eject = false;

    if ( loclShift ) {
      this->setShift( !this->_kbdShift );
      eject = true;
    }
    if ( loclNums ) {
      this->setNums( !this->_kbdNums );
      eject = true;
    }
    if ( loclSymbs ) {
      this->setSymb( !this->_kbdSymbs );
      eject = true;
    }

    bool oneFoundOnKbd = false;
    bool oneFoundOnRow = false;


    if ( eject ) { 
      oneFoundOnKbd = true;
    } else { 
      // read char
      this->deactivateAllRows();
      for(int row=0; row < KB_ROWS_NB; row++) {
        this->activateRow(row);
        oneFoundOnRow = false;

        #if HARDCORE_SX_READ
          unsigned int banks = SX_readBanks();
        #endif

        for(int col=0; col < KB_COLS_NB; col++) {
          #if HARDCORE_SX_READ
          if ( SX_readPin(banks, col+KB_COLS_BG) ) {
          #else
          if ( this->isColPressed(col) ) {
          #endif
            oneFoundOnKbd = true;
            oneFoundOnRow = true;
            char ch = this->getKeychar(row, col);
            if ( ch != 0x00 ) {
              this->injectChar(ch);
            }
          }
        }

        this->deactivateRow(row);
        if ( oneFoundOnRow ) { break; }
      }

      if ( oneFoundOnKbd ) {
        // Cf remanant style meta keys
        this->setShift(false);
        this->setNums(false);
        this->setSymb(false);
        this->setCtrl(false);
      }

    }

    lastTimeKeyReleased = !oneFoundOnKbd;
    lastPollTime = millis();
  }

  // for another device that emulates Keyb (ex. telnetd)
  void MobigoKeyboard::injectChar(char ch) {
    int avail = strlen(this->_keyBuff);
    if ( avail <= KEYB_BUFF_LEN ) {
      // strcat(this->_keyBuff, &ch);
      this->_keyBuff[avail] = ch;
      this->_keyBuff[avail+1] = 0x00;
    } else {
      // Overflow
    }
  }

  void MobigoKeyboard::injectStr(const char* str) {
    int tlen = strlen(str);
    for(int i=0; i < tlen; i++) { this->injectChar(str[i]); }
  }

  void MobigoKeyboard::activateRow(int row) {
    #if HARDCORE_SX_READ
      SX_writePin(KB_ROWS_BG+row, HIGH);
      delay(2);
    #else
      #if INV_LOGIC
        this->io->digitalWrite(KB_ROWS_BG+row, LOW);
      #else
        this->io->digitalWrite(KB_ROWS_BG+row, HIGH);
      #endif
      // delay(1);
    #endif
  }

  void MobigoKeyboard::deactivateRow(int row) {
    #if HARDCORE_SX_READ
      SX_writePin(KB_ROWS_BG+row, LOW);
      delay(1);
    #else
      #if INV_LOGIC
        this->io->digitalWrite(KB_ROWS_BG+row, HIGH);
      #else
        this->io->digitalWrite(KB_ROWS_BG+row, LOW);
      #endif
    #endif
  }

  void MobigoKeyboard::deactivateAllRows() {
    #if HARDCORE_SX_READ
      unsigned int banks = SX_readBanks();
      for(int i=0; i < KB_ROWS_NB; i++) {
        banks = SX_alterBanks(banks, KB_ROWS_BG+i, LOW);
      }
      SX_writeBanks(banks);
    #else
      for(int i=0; i < KB_ROWS_NB; i++) { this->deactivateRow(i); }
    #endif
  }

  bool MobigoKeyboard::isColPressed(int col) {
    #if HARDCORE_SX_READ
          // gain is 1 I2C transaction (no inputModePin test)
          unsigned int banks = SX_readBanks();
          return SX_readPin(banks, KB_COLS_BG+col) == HIGH;
    #else
      #if INV_LOGIC
        return this->io->digitalRead( KB_COLS_BG+col ) == LOW;
      #else
        return this->io->digitalRead( KB_COLS_BG+col ) == HIGH;
      #endif
    #endif
  }

  bool MobigoKeyboard::isKeyPressed(int row, int col) {
    this->activateRow(row);
    bool ok = this->isColPressed(col);
    this->deactivateRow(row);
    return ok;
  }

  char MobigoKeyboard::getKeychar(int row, int col) {
    if ( this->isNums() ) { return numsMap[row][col]; }
    else if ( this->isSymb() ) { return symbolMap[row][col]; }
    else if ( this->isShift() ) { return shiftMap[row][col]; }
    else if ( this->isCtrl() ) { 
      // Serial.print("Ctrl"); 
      // Serial.println( regularMap[row][col] );
      return ctrlMap[row][col]; 
    }
    return regularMap[row][col];
  }

  void MobigoKeyboard::setAutoPoll(bool _auto) {
    this->_autoPoll = _auto;
  }

#endif