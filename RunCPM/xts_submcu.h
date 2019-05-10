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

  #define serialBridge Serial1
  bool bridgeLocked = false;

  void bridge_write(uint8_t ch) { serialBridge.write(ch); }
  int bridge_readUntil(uint8_t until, char* dest, int maxLen) {
      // TODO : write it
  return 0;
  }

  void initKeyb() {
      memset(keybBuffer, 0x00, keybBufferLen);
  }

  void setupBridge() {
      serialBridge.begin(115200);
      initKeyb();
  }

  // ====================================
  void playMp3(int num) {
      // if ( bridgeLocked ) ... wait or queue
      // no matter : has no return value
      uint8_t d0 = num >> 8;
      uint8_t d1 = num % 256;
      serialBridge.write('p');
      serialBridge.write('p');
      serialBridge.write(d0);
      serialBridge.write(d1);
  }
  void loopMp3(int num) {
      uint8_t d0 = num >> 8;
      uint8_t d1 = num % 256;
      serialBridge.write('p');
      serialBridge.write('l');
      serialBridge.write(d0);
      serialBridge.write(d1);
  }
  void volumeMp3(int num) {
      uint8_t d1 = num % 256;
      serialBridge.write('p');
      serialBridge.write('V');
      serialBridge.write(d1);
  }
  void pauseMp3() {
      serialBridge.write('p');
      serialBridge.write('P');
  }
  void stopMp3() {
      serialBridge.write('p');
      serialBridge.write('s');
  }
  void nextMp3() {
      serialBridge.write('p');
      serialBridge.write('n');
  }
  void prevMp3() {
      serialBridge.write('p');
      serialBridge.write('v');
  }

  uint8_t mp3BdosCall(int32 value) {
      Serial.println("mp3 Bdos call");
      // int trckNum += (128+64) << 8

      uint8_t a0 = HIGH_REGISTER(value);
      uint8_t a1 = LOW_REGISTER(value);

      if ( a0 >= (1 << 6) ) {
         // 11000000 -> 11 play+loop -> 64(10)
         // still 16000 addressable songs
         bool loopMode = a0 >= (1 << 7);

         if ( a0 >= 128 ) { a0 -= 128; }

         a0 -= 64;
         int trkNum = (a0<<8) + a1;

if ( loopMode ) Serial.println("mp3 LOOP");
Serial.println("mp3 play");
Serial.println(trkNum);

         if ( loopMode ) { loopMp3(trkNum); }
         else { playMp3(trkNum); }
      } else if (a0 == 0x00) {
Serial.println("mp3 stop");
          stopMp3();
      } else if (a0 == 0x01) {
Serial.println("mp3 pause");
          pauseMp3();
      } else if (a0 == 0x02) {
          nextMp3();
      } else if (a0 == 0x03) {
          prevMp3();
      } else if (a0 == 0x04) {
          volumeMp3( a1 );
      } else if (a0 == 0x05) {
Serial.println("mp3 demo");          
          // for now : just for demo
          playMp3( 65 );
      }

    return 0;
  }


  // ====================================


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
          if ( strlen(tmp) == 0 ) { return -1; }
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