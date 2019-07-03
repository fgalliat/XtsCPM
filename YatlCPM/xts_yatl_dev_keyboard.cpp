// just for devl purposes
//#define ARDUINO 1


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

    extern bool keybLocked;

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
        if ( keybLocked ) { return; }
        if ( firstKeybUse ) {
            // clean the buffer garbage ....
            while ( this->yatl->getSubMCU()->available() > 0 ) {
                this->yatl->getSubMCU()->read();
            }
            firstKeybUse = false;
        }

        int initBuffLen = strlen( keybBuffer );
        if ( initBuffLen >= keybBufferLen ) {
            return;
        }

        this->yatl->getSubMCU()->send('k');
        char tmp[32+1];
        memset(tmp, 0x00, 32+1);
        int tlen = this->yatl->getSubMCU()->readUntil(0x00, tmp, 32);
        if ( tlen == 0 ) { return; }
        // beware w/ overflow...
        // but len == 0 should not occur...
        
        if ( initBuffLen + tlen >= keybBufferLen ) {
            // some overflowed chars will be lost
            for(int i=0; i < tlen; i++) {
                keybBuffer[ initBuffLen + i ] = tmp[i];
            }
            keybBuffer[ initBuffLen + tlen ] = 0x00;
        } else {
            strcat(keybBuffer, tmp);
        }
    }

    int YatlKeyboard::available(bool autopoll) {
        if ( keybLocked ) { return 0; }

        int tlen = strlen( keybBuffer );
        if ( tlen == 0 ) {
            if ( autopoll ) {
              this->poll();
            }
        }
        return tlen;
    }

    void displayCharMap() {
		const char* msg = 
		      " a+ z- e* r/ t\\ y< u> i_ o= p@ \n"\
              "\n"\
		"        q$ s& d# f{ g} h[ j] k( l) m? \n"\
              "\n"\
		"        w\" x' c; v, b. n:";
		yatl.getScreen()->drawTextBox( "-= Char Map =-", msg, 6 ); // purple
	}

	int handleControlChars(char ch) {
		if ( ch == 0xFF ) {
			displayCharMap();
			return -1;
		}
		return (int)kbMap(ch);
	}

    int YatlKeyboard::read() {
        if ( keybLocked ) { return -1; }

        int len = this->available(false);
        if ( len > 0 ) {
            char ch = keybBuffer[0];
            memmove(&keybBuffer[0], &keybBuffer[1], len-1);
            keybBuffer[len-1] = 0x00;
            return handleControlChars(ch);
        } else {
            // kbPoll();
            len = this->available(); // will do poll
            if ( len <= 0 ) {
                return -1; // no byte to read
            }
            char ch = keybBuffer[0];
            if ( len >= 1 ) {
                memmove(&keybBuffer[0], &keybBuffer[1], len-1);
                keybBuffer[len-1] = 0x00;
            }
            return handleControlChars(ch);
        }
    }


#endif
