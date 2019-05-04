/**
 * SubMCU - Lower Layout for YATL
 * 
 * Designed to handle
 *  - MS Chatpad as builtin keyboard [v]
 *  - builtin Joypad & BTNs
 *  - ESP01 as WiFi adapter
 *  - DFPlayer as MP3 Player
 *  - ?Battery Voltmeter?
 *  - ?RGB Led?
 *  - Bridge to Upper Layout
 * 
 * Xtase - fgalliat @May2019
 */

#include <DFRobotDFPlayerMini.h>

#include "ChatpadInputStream.h"

ChatpadInputStream keyboard0(Serial1);

#define LED 13
void led(bool state) { digitalWrite( LED, state ? HIGH : LOW ); }

int _avail() { return Serial.available(); }
int _read() { return Serial.read(); }
void _send(char ch) { Serial.write(ch); }

void setup() {
   pinMode( LED, OUTPUT );
   digitalWrite( LED, LOW );

   Serial.begin(115200);

   keyboard0.init();
   // keyboard0.disableAutoPoll();
   delay(300); // to let keyboard enough time to init...

   led(true);
   Serial.println("> Ready to work");
}

int loopCounter = 0;

void loop() {
    // if ( keyboard0.available() > 0 ) {
    //     Serial.write('K');
    //     Serial.write( keyboard0.read() );
    // }
    keyboard0.poll();

    if ( _avail() ) {
        char ch = _read();
        if ( ch == 'k' ) {
            if ( !keyboard0.available() ) { _send(0x00); }
            else {
                while( keyboard0.available() >= 1 ) { _send( keyboard0.read() ); }
            }
        } else {
            Serial.print("> Command ");
            Serial.print(ch);
            Serial.println(" is NYI ");
        }
    }


    delay(5);
    loopCounter %= 50;
    // if ( loopCounter == 0 ) { Serial.print('.'); }
}