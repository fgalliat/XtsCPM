#include "xts_yat4l_dev_wifi_esp_at.h"

void setup() {

    Serial.begin(115200);

    // while( !Serial ) { ; }

    Serial.println( "Setup.." );

    Serial.println( "WifiSetup.." );
    yat4l_wifi_setup();

    if ( true ) {
        Serial.println( "Waiting.." );
        delay(5000);
        Serial.println( "Waited.." );
    }


    Serial.println( "WifiInit.." );
    yat4l_wifi_init();

    Serial.println( "Done.." );
}


bool first = true;
void loop() {
    if (first) Serial.println( "loop.." );
    first = false;
    delay(1000);
}