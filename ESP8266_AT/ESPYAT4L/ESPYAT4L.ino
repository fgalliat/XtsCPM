#include "xts_yat4l_dev_wifi_esp_at.h"

void setup() {

    Serial.begin(115200);
    Serial.println( "Setup.." );

    Serial.println( "WifiSetup.." );
    yat4l_wifi_setup();

    Serial.println( "WifiInit.." );
    yat4l_wifi_init();

    Serial.println( "Done.." );
}



void loop() {
    Serial.println( "loop.." );
    delay(1000);
}