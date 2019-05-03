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

void setup() {
   Serial.begin(115200);

//    while( !Serial )

   keyboard0.init();
   delay(300); // to let keyboard enough time to init...
}

int loopCounter = 0;

void loop() {
    if ( keyboard0.available() > 0 ) {
        // Serial.write('\n');
        Serial.write('K');
        Serial.write( keyboard0.read() );
    }

    delay(5);
    loopCounter %= 50;
    // if ( loopCounter == 0 ) { Serial.print('.'); }
}