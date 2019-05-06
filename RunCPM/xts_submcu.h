#ifndef _XTS_SUBMCU_H_
#define _XTS_SUBMCU_H_ 1

 /**
  * SubMCU bridge controller
  * 
  * 
  * Xtase - fgalliat @ May2019
  */
  #include "Arduino.h"

  const int keybBufferLen = 64; 
  char keybBuffer[keybBufferLen];

  void initKeyb() {
      memset(keybBuffer, 0x00, keybBufferLen);
  }

  int kbAvailable() {
      return strlen( keybBuffer );
  }

  int kbRead() {
      if ( kbAvailable() > 0 ) {
          int len = kbAvailable();
          char ch = keybBuffer[0];
          memmove(&keybBuffer[0], &keybBuffer[1], len-1);
          keybBuffer[len-1] = 0x00;
          return (int)ch;
      } else {
          // send('k') ....
          bridge_write('k');
          char tmp[32];
          bridge_readUntil(0x00, tmp, 32);
          if ( str_len(tmp) == 0 ) { return -1; }
          // beware w/ overflow...
          // but len == 0 should not occur...
          strcat(keybBuffer, tmp);
          
          int len = kbAvailable();
          char ch = keybBuffer[0];
          memmove(&keybBuffer[0], &keybBuffer[1], len-1);
          keybBuffer[len-1] = 0x00;
          return (int)ch;
      }
  }

#endif