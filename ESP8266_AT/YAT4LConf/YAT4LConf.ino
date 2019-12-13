#include <SdFat.h>

#define SD_CONFIG SdSpiConfig(0, DEDICATED_SPI, SD_SCK_MHZ(50), &SPI1)
SdFat SD;

#include "xts_string.h"

#include "xs_yat4l_impl.h"

#include "xts_yat4l_dev_fs.h"
#include "xts_yat4l_soft_wifi.h"


void setup() {
    Serial.begin(115200);

    while( !Serial ) {
        delay(100);
    }

    if ( !SD.begin( SD_CONFIG ) ) {
        Serial.println("SD init failed!");
        while(true) {
            delay(10000);
        }
    }
    Serial.println("SD init OK!");

    char* wifiConfFile = yat4l_fs_getAssetsFileEntry("wifi.psk");
    Serial.println( wifiConfFile );

    const int confBuffLen = 2048; // env. 10 x (64 + 128) entries max
    char confBuff[confBuffLen];

    int read = yat4l_fs_readTextFile( wifiConfFile, confBuff, confBuffLen );
    Serial.print( "Read from config : " );
    Serial.println( read );

    if ( read < 0 ) {
        char* confContent = "xxxxxx:192.168.1.___:___.dyndns-free.com\n"
                            "xxxxxx:myPASS\n"
                            "yyyyyy:myOthPASS\n";

        Serial.println( confContent );

        read = yat4l_fs_writeTextFile( wifiConfFile, confContent, confBuffLen );
        Serial.print( "Written from config : " );
        Serial.println( read );

    }


    Serial.print("SSID : "); Serial.println( yat4l_wifi_getSSID() );
    Serial.print("HOME : "); Serial.println( yat4l_wifi_getHomeServer() );

}


void loop() {
  delay(1000);
}