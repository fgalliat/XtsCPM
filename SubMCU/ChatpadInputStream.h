/**
 * 
 * Input Stream interface (see also Arduino::Stream class)
 * 
 * Xtase - fgalliat @Dec2018
 * 
 * 
 */

#ifndef __CP_INPUTSTREAM_H__
#define __CP_INPUTSTREAM_H__ 1

#define CHATPAD_FR 1

#include "./InputStream.h"

// #include <Chatpad.h>
#include "XtsChatpad.h"

// static Chatpad pad;
#define KEYPAD_BUFF_LEN 128
static char keyBuff[KEYPAD_BUFF_LEN + 1];

static bool __k_reading = false;

static bool _keyPeopleDown = false;

static void __print_keys(Chatpad &pad, Chatpad::keycode_t code,
                         Chatpad::eventtype_t type)
{

    char chs[2];
    chs[1] = 0x00;

    if (pad.isPeopleDown())
    {
        // digitalWrite(14, HIGH);
        chs[0] = 0x04; //Ctrl-D
        strcat(keyBuff, chs);
        // _keyPeopleDown = true;
        // digitalWrite(14, LOW);
        // _keyPeopleDown = false;
        return;
    } else {
        // digitalWrite(14, LOW);
        // _keyPeopleDown = false;
    }

    if (type == Chatpad::Down)
    {

        chs[0] = pad.toAscii(code);
        if (chs[0] != 0)
        {
            //Serial.print(a);

            // beware w/ '\b'
            // manage Ctrl-C (0x03)
            if (chs[0] == 'c' && pad.isGreenSquareDown())
            {
                chs[0] = 0x03; //Ctrl-C
            }
#ifdef CHATPAD_FR
            // -== French ChatPad ==-
            if (chs[0] == 'm')
            {
                chs[0] = ',';
            }
            else if (chs[0] == 'q')
            {
                chs[0] = 'a';
            }
            else if (chs[0] == 'w')
            {
                chs[0] = 'z';
            }
            else if (chs[0] == 'a')
            {
                chs[0] = 'q';
            }
            else if (chs[0] == 'z')
            {
                chs[0] = 'w';
            }
            else if (chs[0] == ',')
            {
                chs[0] = 'm';
            }

            if (pad.isOrangeCircleDown())
            {
                if (chs[0] == 'r')
                {
                    chs[0] = '"';
                }
                else if (chs[0] == 't')
                {
                    chs[0] = '(';
                }
                else if (chs[0] == 'p')
                {
                    chs[0] = ')';
                }
                else if (chs[0] == 'f')
                {
                    chs[0] = '{';
                }
                else if (chs[0] == 'g')
                {
                    chs[0] = '}';
                }
                else if (chs[0] == '.')
                {
                    chs[0] = ';';
                }
                else if (chs[0] == 'o')
                {
                    chs[0] = '@';
                }
            }
            else if (pad.isGreenSquareDown())
            {
                if (chs[0] == 'v')
                {
                    chs[0] = '=';
                }
                else if (chs[0] == 'b')
                {
                    chs[0] = '+';
                }
                else if (chs[0] == ',')
                {
                    chs[0] = '!';
                }
                else if (chs[0] == 'm')
                {
                    chs[0] = '*';
                }
                else if (chs[0] == 'k')
                {
                    chs[0] = '/';
                }
                else if (chs[0] == 'l')
                {
                    chs[0] = '$';
                }
                else if (chs[0] == '.')
                {
                    chs[0] = ':';
                }
            }
#endif

            // Serial.print(chs[0]);

            strcat(keyBuff, chs);
        } else {
            if ( code == 55 ) {
                // key Left
                chs[0] = 0x05; //Ctrl-E
                strcat(keyBuff, chs);
            } else if ( code == 51 ) {
                // key Right
                chs[0] = 0x06; //Ctrl-F
                strcat(keyBuff, chs);
            }
        }
    }
}

class ChatpadInputStream : public InputStream
{
  private:
    HardwareSerial _serial;
    Chatpad pad;
    bool autoPoll = true;

  public:
    ChatpadInputStream(HardwareSerial &serial)
    {
        this->_serial = serial;
        this->init(true);
    }

    void init(bool reinitBuffer=true) {
        pad.init(_serial, __print_keys);
        if (reinitBuffer) memset(keyBuff, 0x00, KEYPAD_BUFF_LEN + 1);
        __k_reading = false;
    }

    void disableAutoPoll() {
        this->autoPoll = false;
    }

    void poll()
    {
        if (__k_reading)
        {
            return;
        }
        __k_reading = true;
        pad.poll();
        __k_reading = false;
    }

    int available()
    {
        if (this->autoPoll) pad.poll(); // because millis() can't be used inside an ISR
        return strlen(keyBuff);
    }

    int read()
    {
        int avail = available();
        if (avail <= 0)
        {
            return -1;
        }
        // read 1st then shift buff
        char ch = keyBuff[0];
        // memmove(keyBuff + 0, keyBuff + 1, 1); // TODO : check that !!
        memmove( &keyBuff[0], &keyBuff[1], avail-1); // OK that works
        keyBuff[avail-1] = 0x00;
        return ch;
    }

    bool isPeopleDown()
    {
        pad.poll();
        bool result =  _keyPeopleDown;
        // _keyPeopleDown = false;
        // digitalWrite(14, LOW);
        return result;
    }

    // w/o an ECHO : useless !
    char *readLine()
    {
        int avail = available();
        if (avail <= 0)
        {
            return NULL;
        }
        // look for '\n' according to chatpad.cpp

        bool found = false;
        int i = 0;
        for (; i < avail; i++)
        {
            if (keyBuff[i] == '\n')
            {
                found = true;
                break;
            }
        }
        if (!found && i >= KEYPAD_BUFF_LEN)
        {
            found = true;
            i = KEYPAD_BUFF_LEN;
        }

        if (!found)
        {
            return NULL;
        }

        static char line[KEYPAD_BUFF_LEN + 1];
        memset(line, 0x00, KEYPAD_BUFF_LEN + 1);
        memcpy(line, keyBuff, i);
        memmove(keyBuff + 0, keyBuff + i, i);
        for (int e = avail - i; e <= KEYPAD_BUFF_LEN; e++)
        {
            keyBuff[e] = 0x00;
        }
        return line;
    }
};

#endif