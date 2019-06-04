/**
 * Xtase - fgalliat @Jun2019
 * 
 * a test for Yatl hardware API
 * 
 * 
 */

#include "xts_yatl_api.h"

Yatl yatl;


void setup() {
    if ( !yatl.setup() ) {
        Serial.begin(115200);
        Serial.println("Yatl failed to initialize !");
        while( true ) { delay(2000); }
    }

    // TODO : cleanBuffer
    while( yatl.getSubMCU()->available() ) { yatl.getSubMCU()->read(); }

    yatl.getBuzzer()->beep(440, 200);

    yatl.dbug("Wait for SubMcu ready....");
    yatl.delay(3000);
    yatl.getMusicPlayer()->play(60);
    yatl.delay(700);
    yatl.getMusicPlayer()->next();
    yatl.getMusicPlayer()->volume(15);

    // remember that SubMCU might not reseted ...
    yatl.getWiFi()->close();
    // TODO : cleanBuffer
    while( yatl.getSubMCU()->available() ) { yatl.getSubMCU()->read(); }

    yatl.dbug("Wait for WiFi STA....");
    yatl.getWiFi()->beginSTA();
    // yatl.dbug("Wait for WiFi AP....");
    // yatl.getWiFi()->beginAP();

    // TODO : cleanBuffer
    while( yatl.getSubMCU()->available() ) { yatl.getSubMCU()->read(); }

    yatl.getMusicPlayer()->next();

    yatl.getBuzzer()->playTuneFile("mario.t53");
}

int loopCpt = 0;

void loop() {
    Serial.print("Voltage : ");
    Serial.println( yatl.getPWRManager()->getVoltage() );
    yatl.delay(100);

    Serial.println( yatl.getWiFi()->getIP() );
    yatl.delay(100);
    Serial.println( yatl.getWiFi()->getSSID() );

    yatl.delay(200);

    int led = loopCpt % 3;
    if ( led == 0 ) { yatl.getLEDs()->red(true); yatl.getLEDs()->green(false); yatl.getLEDs()->blue(false); }
    if ( led == 1 ) { yatl.getLEDs()->green(true); yatl.getLEDs()->red(false); yatl.getLEDs()->blue(false); }
    if ( led == 2 ) { yatl.getLEDs()->blue(true); yatl.getLEDs()->green(false); yatl.getLEDs()->red(false); }

    loopCpt++;
}