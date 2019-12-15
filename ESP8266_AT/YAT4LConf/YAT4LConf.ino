#include <SdFat.h>

#define SD_CONFIG SdSpiConfig(0, DEDICATED_SPI, SD_SCK_MHZ(50), &SPI1)
SdFat SD;

#include "xts_string.h"

#include "xts_io.h"

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
  Serial.println("");
  Serial.println("A. ERASE config");
  Serial.println("B. SHOW config");
  Serial.println("C. setHomeConfig(..)");
  Serial.println("D. addSSID(.., ..)");
  Serial.println("");
  char* line = _kbReadLine();

  if ( strlen(line) == 1 ) {
      char ch = line[0];
           if ( ch == 'A' ) { __ERASE_WIFI_CONF(); }
      else if ( ch == 'B' ) { __DBUG_WIFI_CONF(); }
      else if ( ch == 'C' ) { 
          Serial.println("Home SSID ?");
          line = _kbReadLine();
          char ssid[64+1]; memset(ssid, 0x00, 64+1);
          sprintf(ssid, "%s", line);
          Serial.println("Home LOCAL ?");
          line = _kbReadLine();
          char homeLocal[32+1]; memset(homeLocal, 0x00, 32+1);
          sprintf(homeLocal, "%s", line);
          Serial.println("Home REMOTE ?");
          line = _kbReadLine();
          char homeRemote[32+1]; memset(homeRemote, 0x00, 32+1);
          sprintf(homeRemote, "%s", line);

          if ( strlen(ssid) > 0 && strlen(homeLocal) > 0 && strlen(homeRemote) > 0 ) {
              bool ok = yat4l_wifi_setHomeConfig(ssid, homeLocal, homeRemote);
              Serial.print("Home Config Saved : ");
              Serial.println( ok );
          } else {
              Serial.println("Invalid Home Config");
          }

      }
      else if ( ch == 'D' ) { 
          Serial.println("New SSID ?");
          line = _kbReadLine();
          char ssid[64+1]; memset(ssid, 0x00, 64+1);
          sprintf(ssid, "%s", line);
          Serial.println("PSK ?");
          line = _kbReadLine();
          char psk[32+1]; memset(psk, 0x00, 32+1);
          sprintf(psk, "%s", line);
          if ( strlen(ssid) > 0 ) {
              bool ok = yat4l_wifi_addWifiPSK(ssid, psk);
              Serial.print("New WIFI Config Saved : ");
              Serial.println( ok );
          } else {
              Serial.println("Invalid New Config");
          }

      }
  }

  delay(100);
}