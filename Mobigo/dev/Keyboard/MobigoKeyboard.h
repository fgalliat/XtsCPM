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

  // for ABCDEF layout
  static const char regularMap[KB_ROWS_NB][KB_COLS_NB] = {
    { 0x00, 'a', 0x00, 0x00, 0x00, ' ', 0x00, 0x00, 0x00 },
    { '\b', 0x00, 't', 'u', 'v', 'w', 'x', 'y', 'z' },
    { 's', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' },
    { 'j', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i' },
    { 0x00, 0x00, 0x00, 0x00, 0x00, '\n', 0x00, 0x00, 0x00 },
  };

  static const char shiftMap[KB_ROWS_NB][KB_COLS_NB] = {
    { 0x00, 'A', 0x00, 0x00, 0x00, ' ', 0x00, 0x00, 0x00 },
    { '\b', 0x00, 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' },
    { 'S', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R' },
    { 'J', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I' },
    { 0x00, 0x00, 0x00, 0x00, 0x00, '\n', 0x00, 0x00, 0x00 },
  };

  static const char numsMap[KB_ROWS_NB][KB_COLS_NB] = {
    { 0x00, '1', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { '\b', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { '0', '2', '3', '4', '5', '6', '7', '8', '9' },
    { 0x00, 0x00, 0x00, 0x00, 0x00, '\n', 0x00, 0x00, 0x00 },
  };

  static const char symbolMap[KB_ROWS_NB][KB_COLS_NB] = {
    { 0x00, 'a', 0x00, 0x00, 0x00, ' ', 0x00, 0x00, 0x00 },
    { '\b', 0x00, 't', 'u', 'v', 'w', 'x', '.', ':' },
    { 's', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' },
    { 'j', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i' },
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

      void activateRow(int row);
      void deactivateRow(int row);
      void deactivateAllRows();

      bool isColPressed(int col);

      bool isKeyPressed(int row, int col);

      char getKeychar(int row, int col);

  public:
      MobigoKeyboard(SX1509* gpio);
      ~MobigoKeyboard();

      void setup();

      void poll();
      int available();
      int read();
  };
  
  
  


#endif