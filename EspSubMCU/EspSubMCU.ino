/**
 * Esp8266 - ESP12 as SubMCU replacement
 * 
 * Xtase - fgalliat @May 2019
 * 
 * 
 * Le croquis utilise 277344 octets (26%) de l'espace de stockage de programmes. Le maximum est de 1044464 octets.
 * Les variables globales utilisent 27416 octets (33%) de mémoire dynamique, ce qui laisse 54504 octets pour les variables locales. Le maximum est de 81920 octets.
 */

#include <ESP8266WiFi.h>

#include <algorithm> // std::min

// =============] YATL [===================
#include "xts_yatl_settings.h"


// =============] Devices [================

// NodeMCU led GPIO16
// ESP led GPIO2


#define LED 2
bool ledState = false;

void led(bool state) {
    digitalWrite(LED, state ? HIGH : LOW);
    ledState = state;
}

void toogleLed() {
    led( !ledState );
}

#define LED2 16
void led2(bool state) {
    digitalWrite(LED2, state ? HIGH : LOW);
}

#define HAS_KEYB 1
#define HAS_MP3  1


// =============] I2C [=================

// infos
// D1 & D2 (A4 & A5) because SX1509 lib does Wire.begin() w/ specifing SDA,SCL pins ...

#include <Wire.h>

#ifdef HAS_KEYB
    #include <SparkFunSX1509.h> // Include SX1509 library
    const byte SX1509_ADDRESS = 0x3E;  // SX1509 I2C address
    SX1509 io;
    
    #ifdef KEYB_MOBIGO
        #warning "-= Has Keyboard =-"

        #include "xts_dev_MobigoKeyboard.h"
        #define KB_AUTO_POLL false
        // #define KB_AUTO_POLL true
        MobigoKeyboard kbd(&io, KB_AUTO_POLL);

        #define keyboard0 kbd

        bool setupKeyboard() {
            if (!io.begin(SX1509_ADDRESS))
            {
                Serial.println("Failed to communicate.");
                // while (1) ; // If we fail to communicate, loop forever.
                return false;
            }
            delay(300);
            
            kbd.setup(LED2, LED2, LED2);
            return true;
        }
    #endif
#endif

// =============] UART [================

#include <SoftwareSerial.h>
SoftwareSerial SerialX(14,12);
#define mp3Serial SerialX

// =============] MP3 [=================
#ifdef HAS_MP3
 #include <DFRobotDFPlayerMini.h>
 DFRobotDFPlayerMini myDFPlayer;
 #define SerialMP3 SerialX
 bool mp3InPause = false;
#endif

// ===========] Joypad [===============

uint8_t joypadX = 127;
uint8_t joypadY = 127;
uint8_t joypadB1 = 0;
uint8_t joypadB2 = 0;

void setupJoypad() {

}

void pollJoypad() {

}

void debugJoypad() {
    Serial.println("DBUGing Joypad");
}

// ===========] Bridge [===============
// Set the Brigde Serial port
#define serialBridge Serial
// #define serialBridge ats_serialBridge

int _avail() { return serialBridge.available(); }
int _read() { return serialBridge.read(); }
void _send(char ch) { serialBridge.write(ch); }

bool mp3ok = false;
bool kbdok = false;

void setup() {
    pinMode(LED, OUTPUT);  led(false);
    pinMode(LED2, OUTPUT); led2(false);

    // Serial.begin(BAUD_SERIAL);
    // Serial.setRxBufferSize(RXBUFFERSIZE);

    setupJoypad();

    Serial.begin(115200);
    serialBridge.begin(115200);


   // + system info
   // > logging info

   // ===== Ms ChatPad for XBOX 360 =====
   #ifdef HAS_KEYB
    kbdok = setupKeyboard();
   #else
    kbdok = false; // keyboard is NOK
   #endif

   // ===== DFPlayer mini MP3 =====
   #ifdef HAS_MP3
    SerialMP3.begin(9600);
    if (!myDFPlayer.begin(SerialMP3)) {
        Serial.println(F("> Unable to begin:"));
        Serial.println(F("> 1.Please recheck the connection!"));
        Serial.println(F("> 2.Please insert the SD card!"));
        mp3ok = false;
    }
    else {
        mp3ok = true;
        myDFPlayer.volume(20);  //Set volume value. From 0 to 30
        // myDFPlayer.play(1);  //Play the first mp3
    }
   #else
    mp3ok = false;
   #endif


   led(true); delay(300);
   led(!true); delay(300);
   led(true); delay(300);
   led(!true); delay(300);
   Serial.println("> Ready to work");
   serialBridge.println("> Ready to work");
   led2(false);
}

int loopCounter = 0;

void _yield() {
    // deadlock when not using WiFi
    // prevent from deadlock when using WiFi
    yield();
}

void loop() {
    pollJoypad();

    #ifdef HAS_KEYB
     if (!KB_AUTO_POLL) { keyboard0.poll(); }
    #endif

    if ( _avail() ) {
        char ch = _read();

        if ( ch == 'i' ) {
            if (!mp3ok) Serial.println("+m:NOK");
            else Serial.println("+m:OK");
            if (!kbdok) Serial.println("+k:NOK");
            else Serial.println("+k:OK");
        } else if ( ch == 'k' ) {
            #ifdef HAS_KEYB
                if ( !keyboard0.available() ) { 
                    // Serial.println("> <Empty Buffer>"); 
                    _send(0x00); 
                }
                else {
                    while( keyboard0.available() >= 1 ) { _send( keyboard0.read() ); _yield(); }
                    _send(0x00);
                }
            #else
                _send(0x00);
            #endif
        } else if ( ch == 'm' ) {
            // MP3 subCommands to dispatch
            #ifdef HAS_MP3
              myDFPlayer.play(65); // THEC64-MENU-Theme
            #endif
        } else if ( ch == 'p' ) {
            #ifdef HAS_MP3
              while( _avail() <= 0 ) { delay(2); }
              ch = _read();
              if ( ch == 'p' ) {
                  // play an MP3
                  while( _avail() <= 0 ) { delay(2); }
                  ch = _read();
                  uint8_t d0 = ch;
                  while( _avail() <= 0 ) { delay(2); }
                  ch = _read();
                  uint8_t d1 = ch;
                  int trackNumber = (d0<<8)+d1;
                  myDFPlayer.play(trackNumber);
              } else if ( ch == 'l' ) {
                  // loop an MP3
                  while( _avail() <= 0 ) { delay(2); }
                  ch = _read();
                  uint8_t d0 = ch;
                  while( _avail() <= 0 ) { delay(2); }
                  ch = _read();
                  uint8_t d1 = ch;
                  int trackNumber = (d0<<8)+d1;
                  myDFPlayer.loop(trackNumber);
              } else if ( ch == 'n' ) {
                  myDFPlayer.next();
              } else if ( ch == 'v' ) {
                  myDFPlayer.previous();
              } else if ( ch == 'P' ) {
                  if ( mp3InPause ) {
                      myDFPlayer.start();
                      mp3InPause = false;
                  } else {
                    myDFPlayer.pause();
                    mp3InPause = true;
                  }
              } else if ( ch == 's' ) {
                  myDFPlayer.stop();
              } else if ( ch == 'V' ) {
                  while( _avail() <= 0 ) { delay(2); }
                  ch = _read();
                  myDFPlayer.volume(ch);
              }
            #endif
        } else if ( ch == 'j' ) {
            debugJoypad();

            _send( joypadX );
            _send( joypadY );
            _send( joypadB1 );
            _send( joypadB2 );
        } else if ( ch == '\n' || ch == '\r' ) {
            // may be some dusty end of line due to terminal 
            // that was used
        } else {
            Serial.print("> Command ");
            Serial.print(ch);
            Serial.println(" is NYI ");
        }

        _yield();
    }


    loopCounter++;
    loopCounter %= 200;
    if ( loopCounter == 0 ) { 
        // Serial.print('.'); 
        #ifdef HAS_KEYB
          // due to a frequent read bug ...
          // keyboard0.init(false);
          // now done only if error handled
        #endif
    }
    delay(2);

    _yield();
}