/**
 * Xtase - fgalliat @Jun2019
 * 
 * a test for Yatl hardware API
 * 
 * 
 */

#include "xts_yatl_api.h"

Yatl yatl;


// bool muteMode = false;
bool muteMode = true;

void setup() {
    if ( !yatl.setup() ) {
        Serial.begin(115200);
        Serial.println("Yatl failed to initialize !");
        while( true ) { delay(2000); }
    }

    yatl.getSubMCU()->reboot(); // reboot SubMCU

    yatl.getSubMCU()->cleanBuffer();

    yatl.getBuzzer()->beep(440, 200);

    yatl.getScreen()->drawWallpaper( (char*) "girl.bmp");

    yatl.dbug("Wait for SubMcu ready....");
    yatl.delay(3000);
    if ( !muteMode ) {
        yatl.getMusicPlayer()->play(60);
        yatl.delay(700);
        yatl.getMusicPlayer()->next();
        yatl.getMusicPlayer()->volume(15);
    }

    // // remember that SubMCU might not be reseted ...
    // yatl.getWiFi()->close();
    // delay(500);
    // yatl.getSubMCU()->cleanBuffer();

    bool ok;
    // yatl.dbug("Wait for WiFi STA....");
    // ok = yatl.getWiFi()->beginSTA();
    yatl.dbug("Wait for WiFi AP....");
    ok = yatl.getWiFi()->beginAP();

    if (ok) {
        Serial.println("WiFi OK");
    } else {
        Serial.println("WiFi Failed");
    }

    if ( !muteMode ) {
        yatl.getMusicPlayer()->next();
        yatl.getBuzzer()->playTuneFile("mario.t53");
    }

    for(int i=0; i < 10; i++) {
        yatl.getSubMCU()->cleanBuffer();
        delay(100);
    }
}

int loopCpt = 0;

void loop() {
    yatl.getKeyboard()->poll();

    if ( Serial.available() ) {
        char ch = (char)Serial.read();
        if ( ch == 'r' ) {
            yatl.getPWRManager()->reset();
        }
    }

    bool atLeastOneKey = false;
    while ( yatl.getKeyboard()->available() ) {
        atLeastOneKey = true;
        char ch = (char)yatl.getKeyboard()->read();
        Serial.write( ch );
    }
    if ( atLeastOneKey ) {
        Serial.write( '\n' );
    }

    if ( !atLeastOneKey ) {
        Serial.print("Voltage : ");
        Serial.println( yatl.getPWRManager()->getVoltage() );
        yatl.delay(100);

        Serial.println( yatl.getWiFi()->getIP() );
        yatl.delay(100);
        Serial.println( yatl.getWiFi()->getSSID() );

        yatl.delay(200);
    }

    int led = loopCpt % 3;
    if ( led == 0 ) { yatl.getLEDs()->red(true); yatl.getLEDs()->green(false); yatl.getLEDs()->blue(false); }
    if ( led == 1 ) { yatl.getLEDs()->green(true); yatl.getLEDs()->red(false); yatl.getLEDs()->blue(false); }
    if ( led == 2 ) { yatl.getLEDs()->blue(true); yatl.getLEDs()->green(false); yatl.getLEDs()->red(false); }

    loopCpt++;
}