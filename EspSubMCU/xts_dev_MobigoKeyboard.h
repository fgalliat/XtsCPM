/**
 * Xtase - fgalliat @May 2019
 * 
 * Vtech Mobigo Keyboard decoding API headers
 * uses SX1509 breakout board (from Sparkfun)
 * 
 * @ this time : only the key [ ? ] is missing....
 * 
 * Arduino UNO
 * SDA : A4
 * SCL : A5
 * 
 */

#ifndef __MOBIGO_KBD_H__
#define __MOBIGO_KBD_H__

#include <SparkFunSX1509.h> // Include SX1509 library

// NB Vs BeGin
#define KB_ROWS_NB 5
#define KB_ROWS_BG 0

// #define KB_COLS_NB 10
// #define KB_COLS_BG 6
#define KB_COLS_NB 9
#define KB_COLS_BG 7

  // for AZERTY layout
  static const char regularMap[KB_ROWS_NB][KB_COLS_NB] = {
    { 0x00, 'a', 0x00, 0x00, 0x00, ' ', 0x00, 0x00, 0x00 },
    { 'm', 0x00, 'w', 'x', 'c', 'v', 'b', 'n', '\b' },
    { 'l', 'q', 's', 'd', 'f', 'g', 'h', 'j', 'k' },
    { 'p', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o' },
    { 0x00, 0x00, 0x00, 0x00, 0x00, '\n', 0x00, 0x00, 0x00 },
  };

  static const char shiftMap[KB_ROWS_NB][KB_COLS_NB] = {
    { 0x00, 'A', 0x00, 0x00, 0x00, ' ', 0x00, 0x00, 0x00 },
    { 'M', 0x00, 'W', 'X', 'C', 'V', 'B', 'N', '\b' },
    { 'L', 'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K' },
    { 'P', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O' },
    { 0x00, 0x00, 0x00, 0x00, 0x00, '\n', 0x00, 0x00, 0x00 },
  };

  static const char numsMap[KB_ROWS_NB][KB_COLS_NB] = {
    { 0x00, '1', 0x00, 0x00, 0x00, ' ', 0x00, 0x00, 0x00 },
    { '\b', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { '0', '2', '3', '4', '5', '6', '7', '8', '9' },
    { 0x00, 0x00, 0x00, 0x00, 0x00, '\n', 0x00, 0x00, 0x00 },
  };

  // 0x1B => 27 Esc
  // 0x09 =>  9 Tab
  static const char symbolMap[KB_ROWS_NB][KB_COLS_NB] = {
    { 0x00, 'a', 0x00, 0x00, 0x1B, ' ', 0x00, 0x00, 0x00 },
    { 'm', 0x00, 'w', 'x', 'c', '?', '.', ':', '\b' },
    { 'l', 'q', 's', 'd', 'f', 'g', 'h', 'j', 'k' },
    { 'p', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o' },
    { 0x00, 0x00, 0x00, 0x00, 0x09, '\n', 0x00, 0x00, 0x00 },
  };

  static const char ctrlMap[KB_ROWS_NB][KB_COLS_NB] = {
    { 0x00, (char)1, 0x00, 0x00, 0x00, ' ', 0x00, 0x00, 0x00 },
    { (char)13, 0x00, (char)23, (char)24, (char)3, (char)22, (char)2, (char)14, '\b' },
    { (char)12, (char)17, (char)19, (char)4, (char)6, (char)7, (char)8, (char)10, (char)11 },
    { (char)16, (char)26, (char)5, (char)18, (char)20, (char)25, (char)21, (char)9, (char)15 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, '\n', 0x00, 0x00, 0x00 },
  };

  #define KEYB_BUFF_LEN 16

  class MobigoKeyboard
  {
  private:
      SX1509* io;
      
      char _keyBuff[KEYB_BUFF_LEN+1];
      void flushBuffer() { memset(_keyBuff, 0x00, KEYB_BUFF_LEN+1); }

      bool _kbdShift = false;
      bool _kbdNums  = false;
      bool _kbdSymbs = false;

      bool _autoPoll = false;

      int led0=-1;
      int led1=-1;
      int led2=-1;

      void led(int num, bool state);

      void setShift(bool state) { this->_kbdShift = state; this->led(0, state); }
      void setNums(bool state)  { this->_kbdNums  = state; this->led(1, state); }
      void setSymb(bool state)  { this->_kbdSymbs = state; this->led(2, state); }
      void setCtrl(bool state)  { this->setNums(state); this->setSymb(state); }
      bool isShift() { return this->_kbdShift; }
      bool isNums()  { return this->_kbdNums && !this->_kbdSymbs; }
      bool isSymb()  { return !this->_kbdNums && this->_kbdSymbs; }
      bool isCtrl()  { return this->_kbdNums && this->_kbdSymbs; }

      void activateRow(int row);
      void deactivateRow(int row);
      void deactivateAllRows();

      bool isColPressed(int col);

      bool isKeyPressed(int row, int col);

      char getKeychar(int row, int col);

  public:
      MobigoKeyboard(SX1509* gpio, bool autoPoll=false);
      ~MobigoKeyboard();

      bool gpio_ok = false;

      void setup(int led0=-1, int led1=-1, int led2=-1);
      void reboot();

      void setAutoPoll(bool _auto);

      void poll();
      int available();
      int read();

      // for another device that emulates Keyb (ex. telnetd)
      void injectChar(char ch);
      void injectStr(const char* str);
  };
  
  
  


#endif