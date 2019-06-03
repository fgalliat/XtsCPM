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

    yatl.dbug("Wait for SubMcu ready....");
    yatl.delay(3000);
    yatl.getMusicPlayer()->play(65);
}

void loop() {
    Serial.print("Voltage : ");
    Serial.println( yatl.getPWRManager()->getVoltage() );
    yatl.delay(200);
}