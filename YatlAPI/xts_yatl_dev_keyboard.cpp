// just for devl purposes
#define ARDUINO 1


#ifdef ARDUINO
 /**
 * Xtase - fgalliat @Jun2019
 * 
 * implementaion of Yatl hardware API (Arduino Version)
 * Keyboard over SubMCU impl.
 * 
 */

  #include "Arduino.h"

  #include "xts_yatl_settings.h"
  #include "xts_yatl_api.h"
  extern Yatl yatl;

    const int keybBufferLen = 64; 
    char keybBuffer[keybBufferLen+1];

    void initKeyb() {
      memset(keybBuffer, 0x00, keybBufferLen+1);
    }
    bool firstKeybUse = true;

    bool YatlKeyboard::setup() { initKeyb(); return true; }

    uint8_t kbMap(uint8_t code) {
      if ( code == '\n' ) { code = '\r'; }
      return code;
    }


    void YatlKeyboard::poll() {
        if ( firstKeybUse ) {
            // clean the buffer garbage ....
            while ( this->yatl->getSubMCU()->available() > 0 ) {
                this->yatl->getSubMCU()->read();
            }
            firstKeybUse = false;
        }

        if ( strlen( keybBuffer ) >= keybBufferLen ) {
            return;
        }

        this->yatl->getSubMCU()->send('k');
        char tmp[32+1];
        memset(tmp, 0x00, 32+1);
        int tlen = this->yatl->getSubMCU()->readUntil(0x00, tmp, 32);
        if ( tlen == 0 ) { return; }
        // beware w/ overflow...
        // but len == 0 should not occur...
        strcat(keybBuffer, tmp);
    }

    int YatlKeyboard::available() {
        int tlen = strlen( keybBuffer );
        if ( tlen == 0 ) {
            this->poll();
        }
        return tlen;
    }

    int YatlKeyboard::read() {
        int len = this->available();
        if ( len > 0 ) {
            char ch = keybBuffer[0];
            memmove(&keybBuffer[0], &keybBuffer[1], len-1);
            keybBuffer[len-1] = 0x00;
            return (int) kbMap(ch);
        } else {
            // kbPoll();
            len = this->available(); // will do poll
            if ( len == 0 ) {
                return -1; // no byte to read
            }
            char ch = keybBuffer[0];
            memmove(&keybBuffer[0], &keybBuffer[1], len-1);
            keybBuffer[len-1] = 0x00;
            return (int) kbMap(ch);
        }
    }


#endif