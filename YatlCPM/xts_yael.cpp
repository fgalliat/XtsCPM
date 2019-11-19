#if defined ESP32

/**
 * Xtase - fgalliat @Sept2019
 * 
 * YAEL routines Impl.
 * 
 * ESP32 working w/ arduino IDE
 * 
 * Additional board url : https://dl.espressif.com/dl/package_esp32_index.json
 * goto Board Manager
 * choose ESP32 by espressif
 * 
 * DOIT ESP32 DEVKIT V1
 * 
 * for TFT-eSPI lib, (now see TFT4inch Settings how-to...)
 * ESP32 + ILI9486 Screen + SDCard + MCP23017 Keyboard decoder
 * // ===== Xtase Settings =====
 * #define TFT_MISO 19
 * #define TFT_MOSI 23
 * #define TFT_SCLK 18
 * #define TFT_CS    5  // Chip select control pin
 * #define TFT_DC   15  // Data Command control pin
 * #define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
 * 
 * #define SD_CS     4  // SdCard CHIP-SELECT
 * #define TS_CS     2  // TouchScreen CHIP-SELECT
 *
 * #define SD_CS     26 // REDEF for now
 * in SdFatConfig.h (of SdFatLib)
 *  If the symbol ENABLE_EXTENDED_TRANSFER_CLASS is nonzero, the class SdFatEX
 *  will be defined. If the symbol ENABLE_SOFTWARE_SPI_CLASS is also nonzero,
 *  the class SdFatSoftSpiEX will be defined.
 *  These classes used extended multi-block SD I/O for better performance.
 *  the SPI bus may not be shared with other devices in this mode.
 * 
 * 1.3 MB Sktech
 * 320 KB RAM
 *
 * SubMCU Bridge on RX2/TX2 (ProMini 328P 3.3v)
 * 
 * --------------------------
 * part of XtsCPM project
 */

#include "Arduino.h"

#include "xts_yael.h"

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void _yield() {
    // prevent from deadlock (specific to ESP8266)
    yield();
}

//====================================================================================
//                                    WiFi Managment
//====================================================================================

#define USE_WIFI 1

#if USE_WIFI
    // #include <ESP8266WiFi.h> // ESP-12 version (as ESP8266)
    #include <WiFi.h>

    #include <algorithm> // std::min

    #warning "WiFi Active code"

    bool ledState = false;

    void red(bool state) { ledState = state; yael_led(state); }
    void green(bool state) { /*yael_led(state);*/ }
    void blue(bool state) { /*yael_led(state);*/ }

    void blink(int times, bool fast=true) {
        long time = fast ? 100 : 400;
        for(int i=0; i < times; i++) {
            red(true);
            delay(time);
            red(false);
            delay(time);
        }
    }

    void toggleLed() {
        ledState = !ledState;
        red(ledState);
    } 

    #define WIFI_CONN_MODE_NONE 0
    #define WIFI_CONN_MODE_STA  1
    #define WIFI_CONN_MODE_AP   2

    #define WIFI_USE_MODE_NONE    0
    #define WIFI_USE_MODE_TELNETD 1
    #define WIFI_USE_MODE_WGET    2

    uint8_t wifiConnMode = WIFI_CONN_MODE_NONE;
    uint8_t wifiUseMode  = WIFI_USE_MODE_NONE;

    // should be in the .gitignore ...
    #include "ssid_psk.h"
    #ifndef STANB
        #define STANB 1
        const char* ssids[STANB] = {
            "your-ssid"
        };
        const char* pwds[STANB] = {
            "your-password"
        };
    #endif

    #ifndef AP_SSID
      #define AP_SSID "YaelAP"
      // must be at least 8 chars long
      #define AP_PSK  "yael1234"
    #endif

    // the telnet access password
    #ifndef TELNETPASSWD
      #define TELNETUSER   "root"
      #define TELNETPASSWD "yael"
    #endif

    #define STACK_PROTECTOR  512 // bytes

    //how many clients should be able to telnet to this ESP8266
    #define MAX_SRV_CLIENTS 2
    /*const*/ char* ssid = "None Yet";
    /*const*/ char* password = "None Yet";


// =============] Server [================
    const int port = 23;
    WiFiServer server(port);
    WiFiClient serverClients[MAX_SRV_CLIENTS];

    bool wifiStarted = false;
    bool telnetdStarted = false;

    bool isWiFiStarted() { return wifiStarted; }
    bool isTelnetdStarted() { return telnetdStarted; }

    bool startWiFi(bool staMode=true) {
        if ( wifiStarted ) {
            return false;
        }
        wifiStarted = false;
        wifiConnMode = WIFI_CONN_MODE_NONE;
        if ( staMode ) {
            WiFi.mode(WIFI_STA);
            bool foundAsta = false;
            int ssidIdx = 0;

            while( !foundAsta ) {

                ssid = (char*)ssids[ssidIdx];
                password = (char*)pwds[ssidIdx];

                WiFi.begin(ssid, password);
                red(true); green(false);
                int retry = 0;
                while (WiFi.status() != WL_CONNECTED) {
                    toggleLed();
                    delay(500);
                    if (retry > 6) { break; }
                    retry++;
                }

                if (WiFi.status() == WL_CONNECTED) {
                    foundAsta = true;
                    break;
                }

                ssidIdx++;
                if (ssidIdx >= STANB) {
                    return false;
                } 
            }

            red(false); green(true);
            wifiConnMode = WIFI_CONN_MODE_STA;
            wifiStarted = true;
        } else {
            WiFi.mode(WIFI_AP);
            delay(200);
            const char* _ssid = AP_SSID;
            // must be longer then 8 chars
            const char* _password = AP_PSK;
            red(true); green(false); 
            bool ok = WiFi.softAP(_ssid, _password);
            if ( !ok ) {
                return false;
            }
            red(false); green(true);
            wifiConnMode = WIFI_CONN_MODE_AP;
            wifiStarted = true;
        }
        return wifiStarted;
    }

    void stopWiFi() {
        if (wifiConnMode == WIFI_CONN_MODE_AP) {
            WiFi.softAPdisconnect();
        } // else ?
        WiFi.disconnect();
        wifiStarted = false;
        wifiConnMode = WIFI_CONN_MODE_NONE;
    }

    const int IPlen = 3+1+3+1+3+1+3;
    char ip[IPlen+1];
    char* getLocalIP() {
        memset(ip, 0x00, IPlen+1);
        if (wifiConnMode == WIFI_CONN_MODE_STA) {
            strcpy(ip, WiFi.localIP().toString().c_str() );
        } else if (wifiConnMode == WIFI_CONN_MODE_AP) {
            strcpy(ip, WiFi.softAPIP().toString().c_str() );
        } else {
            strcpy(ip, "0.0.0.0");
        }
        return ip;
    }

    char currentSsid[32+1];
    char* getSSID() {
        memset(currentSsid, 0x00, 32+1);
        if (wifiConnMode == WIFI_CONN_MODE_STA) {
            strcpy(currentSsid, ssid );
        } else if (wifiConnMode == WIFI_CONN_MODE_AP) {
            // TODO : finish that
            strcpy(currentSsid, AP_SSID );
        } else {
            strcpy(currentSsid, "None");
        }
        return currentSsid;
    }

    // ===] Server Mode[===

    bool startTelnetd() {
        if ( telnetdStarted ) { return false; }
        if ( !wifiStarted ) { return false; }
        telnetdStarted = false;
        wifiUseMode = WIFI_USE_MODE_NONE;
        server.begin();
        server.setNoDelay(true);
        telnetdStarted = true;
        wifiUseMode = WIFI_USE_MODE_TELNETD;
        return true;
    }

    void stopTelnetd() {
        if ( !telnetdStarted ) { return; }
        server.close();
        telnetdStarted = false;
        wifiUseMode = WIFI_USE_MODE_NONE;
    }  

    #define TELNET_MODE_NONE 0
    #define TELNET_MODE_KEYB 1 

    int telnetMode = TELNET_MODE_NONE;

    // ================= Protocol ==========================
    void telnet_client_connected(int num) {
       WiFiClient clt = serverClients[num];

       clt.println("***************************");
       clt.println("* Welcome to Yael System  *");
       clt.println("* Xtase-fgalliat @Nov2019 *");
       clt.println("***************************");
       clt.println("");
       clt.flush();
       while( clt.available() ) { clt.read(); }

       bool loginOK = false;
       bool passwOK = false;

       int tmp;

       clt.print("User : ");
       clt.flush(); 
       char login[32+1]; memset(login, 0x00, 32+1);
       while( !clt.available() ) { delay(2); yield(); }
       tmp = clt.readBytesUntil(0x0A, login, 32);
       if ( tmp > 0 && login[ tmp-1 ] == '\r' ) { login[ tmp-1 ] = 0x00; }
       login[tmp] = 0x00;
       

       if ( strcmp(login, TELNETUSER) == 0 ) {
         loginOK = true;
       }
       while( clt.available() ) { clt.read(); }

       clt.print("Password : ");
       clt.flush(); 
       char passd[32+1]; memset(passd, 0x00, 32+1);
       while( !clt.available() ) { delay(2); yield(); }
       tmp = clt.readBytesUntil(0x0A, passd, 32);
       if ( tmp > 0 && passd[ tmp-1 ] == '\r' ) { passd[ tmp-1 ] = 0x00; }
       passd[tmp] = 0x00;

       if ( (tmp=strcmp( (const char*)passd, TELNETPASSWD)) == 0 ) {
         passwOK = true;
       }
       while( clt.available() ) { clt.read(); }

       if ( !( loginOK && passwOK ) ) {
          char msg[64+1]; sprintf( msg, " Acess DENIED (%s/%s)", login, passd );
          clt.println(msg);
          clt.flush(); 
          clt.stop();
          return;
       }

       clt.println(" Acess GRANTED ");
       clt.flush();

    //    int rc;
    //    do {
    //      rc = telnet_client_menu(num);
    //      if ( rc == 0 && telnetMode == TELNET_MODE_NONE ) {
    //          continue;
    //      } else {
    //          break;
    //      }
    //    } while(true);

    }

    void broadcastToTelnetd(char ch) {
        if (!telnetdStarted) { return; }
        int i;
        for (i = 0; i < MAX_SRV_CLIENTS; i++) {
            if ( !serverClients[i] ) {
                continue;
            }

            serverClients[i].write( ch );
        }
    }

    int telnetdInputAvailable() {
        if (!telnetdStarted) { return 0; }
        int i;
        for (i = 0; i < MAX_SRV_CLIENTS; i++) {
            if ( !serverClients[i] ) {
                continue;
            }

            int k = serverClients[i].available();
            if ( k > 0 ) { return k; }
        }
        return 0;
    }

    int telnetdInputRead() {
        if (!telnetdStarted) { return -1; }
        int i;
        for (i = 0; i < MAX_SRV_CLIENTS; i++) {
            if ( !serverClients[i] ) {
                continue;
            }

            int k = serverClients[i].available();
            if ( k > 0 ) { return serverClients[i].read(); }
        }
        return -1;
    }


    void loopTelnetd() {
        if (!telnetdStarted) { return; }

        //check if there are any new clients
        if (server.hasClient()) {
            //find free/disconnected spot
            int i;
            for (i = 0; i < MAX_SRV_CLIENTS; i++)
            if (!serverClients[i]) { // equivalent to !serverClients[i].connected()
                serverClients[i] = server.available();
                serverClients[i].flush();
                // logger->print("New client: index ");
                // logger->println(i);
                blink(4);
                // telnetMode = TELNET_MODE_KEYB;
                telnetMode = TELNET_MODE_NONE;
                telnet_client_connected(i);
                break;
            }

            //no free/disconnected spot so reject
            if (i >= MAX_SRV_CLIENTS) {
                server.available().println("busy");
                // hints: server.available() is a WiFiClient with short-term scope
                // when out of scope, a WiFiClient will
                // - flush() - all data will be sent
                // - stop() - automatically too
                // logger->printf("server is busy with %d active connections\n", MAX_SRV_CLIENTS);
            }
        }

        // ---------------------

        // // === telnet as Keyb Mode ===

        // //check TCP clients for data
        // for (int i = 0; i < MAX_SRV_CLIENTS; i++) {
        //     bool hadSome = false;
        //     while (serverClients[i].available() /*&& Serial.availableForWrite() > 0*/ ) {
        //         // working char by char is not very efficient
        //         char ch = serverClients[i].read();
                    
        //         serverClients[i].println("YAEL Bye");
        //         serverClients[i].stop();


        //         // if ( ch == 27 ) { // Esc.
        //         //     ch = serverClients[i].read();
        //         //     if ( ch == 'q' ) {
        //         //         serverClients[i].stop();
        //         //         break;
        //         //     } else if ( ch == 'm' ) {
        //         //         telnet_client_menu(i);
        //         //         break;
        //         //     } else if ( ch == 'r' ) {
        //         //         telnet_client_uploadToYatl(i);
        //         //         break;
        //         //     }
        //         //     if (telnetMode == TELNET_MODE_KEYB) {
        //         //         keyboard0.injectChar(27);
        //         //         if ( ch != 0xFF ) {
        //         //             keyboard0.injectChar(ch);
        //         //         }
        //         //     } else {
        //         //         _send(ch);
        //         //     }
        //         // }
        //         // if (telnetMode == TELNET_MODE_KEYB) {
        //         //     keyboard0.injectChar(ch);
        //         // } else {
        //         //     _send(ch);
        //         // }

        //         hadSome = true;
        //     }
        // }

    }




#endif
// endif USE_WIFI 

#include <HardwareSerial.h>
//====================================================================================
//                                    AUX Serial Port
//====================================================================================
HardwareSerial Serial3(1); // use uart3
#define auxSerial Serial3

// 16,17 is RX2,TX2
#define AUX_IN  32
#define AUX_OUT 33

void setupAuxPort() {
    Serial3.begin(9600, SERIAL_8N1, AUX_IN, AUX_OUT); // pins 26 rxY, 14 txY, 9600 bps, 8 bits no parity 1 stop bit

    // Serial3.println("Hello Serial3 !");
    // Serial.println("Hello Serial3 !");

    // Serial.println("Reading Serial3 !");
    // char line[32];
    // while( Serial3.available() == 0 ) { delay(100); }
    // int nb = Serial3.readBytesUntil('\n', line, 32);
    // Serial3.println(line);
    // Serial.println(line);
}

//====================================================================================
//                                    MCU Bridge
//====================================================================================

// HardwareSerial Serial2(2); // use uart2
// can be TX only ...
// HardwareSerial Serial1(1);
// Serial1.begin(9600, SERIAL_7E1, 12, -1, true); ....

#define bridgeSerial Serial2

void cleanBridge() {
    while( Serial2.available() ) {
        Serial2.read();
    }
}

// forward symbol
void cleanKeyb();
void led(bool state, bool fastMode=false);

void setupBridge() {
    // Serial2.begin(9600, SERIAL_8N1, 16, 17); // pins 16 rx2, 17 tx2, 9600 bps, 8 bits no parity 1 stop bit
    Serial2.begin(9600);
    cleanBridge();
    led(true);
    cleanKeyb();
    led(false);
}

//====================================================================================
//                                    Led
//====================================================================================

void led(bool state, bool fastMode) {
    if ( LED_BUILTIN_PIN > 0 ) {
        digitalWrite(LED_BUILTIN_PIN, state ? HIGH : LOW);
    }

    if (fastMode) { return; } 

    if ( state ) { bridgeSerial.write('L'); }
    else  { bridgeSerial.write('l'); }
    delay(1);
}

void drive_led(bool state) {
    led(state, true);
}

//====================================================================================
//                                    Keyboard
//====================================================================================

#define KB_BUFF_LEN 20
char keyBuff[KB_BUFF_LEN + 1];
bool firstKeyUse = true;

long lastKeyTime = millis();

void cleanKeyb() {
    bridgeSerial.write((uint8_t)'K');
    delay(2);
}

char pollKeyb() {

    if ( firstKeyUse ) {
        firstKeyUse = false;
        memset(keyBuff, 0x00, KB_BUFF_LEN+1);
    }

    int tlen = strlen( keyBuff );
    if ( tlen == 0 && ( millis() - lastKeyTime > 60 ) ) {
      bridgeSerial.write( (uint8_t)'k');  
      delay(4);
      lastKeyTime = millis();
    } 
    
    if ( tlen < KB_BUFF_LEN ) {
        while( bridgeSerial.available() ) {
            keyBuff[ tlen++ ] = (char)bridgeSerial.read();
            if ( tlen >= KB_BUFF_LEN ) {
                break;
            }
        }
        keyBuff[ tlen ] = 0x00;
    }

    char customKey = 0x00;

    if ( tlen > 0 ) {
        customKey = keyBuff[0];

        // memmove ....
        for(int i=1; i < tlen; i++) {
            keyBuff[i-1] = keyBuff[i];
        }
        keyBuff[ tlen-1 ] = 0x00;
        keyBuff[ tlen ] = 0x00;
    }

return customKey;
}

//====================================================================================
//                                    LCD 20x4
//====================================================================================

void lcd_clear() {
    bridgeSerial.write( (uint8_t)'C' );
    delay(2);
}

void lcd_setCursor(int col, int row) {
    bridgeSerial.write( (uint8_t)'c' );
    bridgeSerial.write( (uint8_t)col );
    bridgeSerial.write( (uint8_t)row );
    delay(4);
}

void lcd_home() {
    lcd_setCursor(0,0);
    delay(2);
}

void lcd_print(char* str) {
    bridgeSerial.write( (uint8_t)'P' );
    bridgeSerial.print( str );
    bridgeSerial.write( (uint8_t)'\n' );
    delay(4);
}

void lcd_println(char* str, int lineNum) {
    lcd_setCursor( 0, lineNum );

    bridgeSerial.write( (uint8_t)'P' );
    bridgeSerial.print( str );
    // bridgeSerial.write( (uint8_t)'\r' );
    bridgeSerial.write( (uint8_t)'\n' );
    delay(4);
}

//====================================================================================
//                                    MP3 Player
//====================================================================================
void setupMp3() {
    // sndCard.init();
    // delay(500);
    // sndCard.volume(25);

    pinMode(MP3_BUSY_PIN, INPUT);
}



//====================================================================================
//                                  Libraries
//====================================================================================

#include "SPI.h"

// Call up the TFT library
#include <TFT_eSPI.h> // Hardware-specific library for ESP8266

// Invoke TFT library this will set the TFT chip select high
TFT_eSPI tft = TFT_eSPI();



// ==== Wiring =====
#define OWN_SPI_CS   5
#define OWN_SPI_DC   15
#define OWN_SPI_MOSI 23
#define OWN_SPI_CLK  18
#define OWN_SPI_MISO 19

#define TFT_CS OWN_SPI_CS
#define TS_CS 26
#define SD_CS 13 // SD chip select

//                ___________
//            3.3|           |VIN
//            GND|           |GND
//  TFT_DC    D15|           |D13 SD_CS
//  SD_MISO   D02|           |D12 BUZZER - certified
//  SD_MOSI   D04|           |D14 SD_CLK
//  Sub_MCU   RX2|           |D27 LED
//  Sub_MCU   TX2|           |D26 TS_CS
//  TFT_CS    D05|           |D25 ~~
//  TFT_CLK   D18|           |D33 TX1 (aux ExtPort)
//  TFT_MISO  D19|           |D32 RX1 (aux ExtPort)
//            D21|           |D35 SubMCU /READY \__ Cf spec: INPUT ONLY
//            RX0|           |D34 MP3 /BUSY     /
//            TX0|           |VN
//            D22|           |VP
//  TFT_MOSI  D23|___________|EN /RESET

// #include "FS.h"
// #include "SD.h"

// in SdFatConfig.h (of SdFatLib)
//  * If the symbol ENABLE_EXTENDED_TRANSFER_CLASS is nonzero, the class SdFatEX
//  * will be defined. If the symbol ENABLE_SOFTWARE_SPI_CLASS is also nonzero,
//  * the class SdFatSoftSpiEX will be defined.
//  * These classes used extended multi-block SD I/O for better performance.
//  * the SPI bus may not be shared with other devices in this mode.

/*
rst:0x10 (RTCWDT_RTC_RESET),boot:0x33 (SPI_FAST_FLASH_BOOT)
flash read err, 1000
ets_main.c 371 
 */

#include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
// SdFatSoftSpiEX<12, 14, 27> SD; // MISO, MOSI, SCK Some boards use 2,15,14,13, other 12,14,27,26
// SdFatSoftSpiEX<2, 15, 14> SD; // MISO, MOSI, SCK Some boards use 2,15,14,13, other 12,14,27,26

SdFatSoftSpiEX<2, 4, 14> SD; // MISO, MOSI, SCK Some boards use 2,15,14,13, other 12,14,27,26

// ====== Fs Routines =================

const int _fullyQualifiedFileNameSize = 1+5 + (8+1+3) + 1;
char _assetEntry[ _fullyQualifiedFileNameSize ];

// not ThreadSafe !
char* yael_fs_getAssetsFileEntry(char* assetName) {
    if ( assetName == NULL || strlen(assetName) <= 0 ) { yael_dbug("NULL filename"); return NULL; }
    memset(_assetEntry, 0x00, _fullyQualifiedFileNameSize);

    if ( assetName[1] == ':' ) {
        // ex. "Y:IMG.PAK"
        sprintf( _assetEntry, "/%c/0/%s", assetName[0], &assetName[2] );
    } else {
        sprintf( _assetEntry, "/Z/0/%s", &assetName[0] );
    }

    return _assetEntry;
}




// === Now that TFT & SD Loaded ... ===
#include "xts_yael_soft_drawBMP.h"
#include "xts_yael_soft_drawPAK.h"


void _setupCSlines() {
  // disable Screen & TouchScreen SPI C-select
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  pinMode(TS_CS, OUTPUT);
  digitalWrite(TS_CS, HIGH);
  // enable SD card C-select
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, LOW);
}

// ========================================


// ========================================
// forward symbols
void _setConsoleMode(int mode);

bool Y_setup()
{
  _setupCSlines();

  if ( SUBMCU_READY_PIN > 0 ) {
    pinMode(SUBMCU_READY_PIN, INPUT);
  }

  if ( LED_BUILTIN_PIN > 0 ) {
   pinMode(LED_BUILTIN_PIN, OUTPUT);
   digitalWrite(LED_BUILTIN_PIN, LOW);
  }

  Serial.begin(115200); // Used for messages and the C array generator
//   Serial.begin(9600); // Used for messages and the C array generator

  setupBridge();
  setupAuxPort();

  yael_buzzer_init();

  // Now initialise the TFT
  lcd_print("Init TFT\n");
  tft.begin();
  tft.setRotation(DEFAULT_TFT_ROTATION);  // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_BLACK);

  _setConsoleMode(1); // compute 80 cols mode

  // Initialise the SD library before the TFT so the chip select gets set
  // have some issues w/ RESETing Screen ...
  lcd_print("Init SD\n");
  int retry = 1;
  bool ok = false;
  while( retry <= 3 ) {
    if (!SD.begin(SD_CS)) {
        Serial.println("SD Initialisation failed!");
        lcd_print("! SD failed ! ");
        yael_tft_print("! SD failed ! ");

        yael_buzzer_beep();
        // return false;
    } else {
        ok = true;
        break;
    }
    delay(300);
    _setupCSlines();
    yield();
    retry++;
  }
  if ( !ok ) {
      return false;
  }
  Serial.println("\r\nSD Initialisation done.");

  lcd_print("Init MP3\n");
  setupMp3();

//   // Now initialise the TFT
//   lcd_print("Init TFT\n");
//   tft.begin();
//   tft.setRotation(DEFAULT_TFT_ROTATION);  // 0 & 2 Portrait. 1 & 3 landscape
//   tft.fillScreen(TFT_BLACK);

  // aux screen
  lcd_setCursor(0,0);
  lcd_print("== Xtase @Aug2019 ==");
  lcd_setCursor(0,1);
  //         12345678901234567890
  lcd_print("OrdiSavant new YATL");
  lcd_setCursor(0,2);
  lcd_print("Layout...");
  lcd_setCursor(0,3);
  lcd_print("Have fun !");

return true;
}

// ===================================
    bool yael_fs_downloadFromSerial() {
        while( Serial.available() ) { Serial.read(); delay(2); }
        yael_warn("Download in progress");
        Serial.println("+OK");
        while( !Serial.available() ) { delay(2); }
        // for now : file has to be like "/C/0/XTSDEMO.PAS"
        int tlen = 0;
        char txt[128+1]; 
        char name[64+1]; memset(name, 0x00, 64); tlen = Serial.readBytesUntil(0x0A, name, 64);
        if ( tlen <= 0 ) {
            sprintf(txt, "Downloading %s (error)", name);
            yael_warn((const char*)txt);
            Serial.println("Download not ready");
            Serial.println(name);
            Serial.println("-OK");
            return false;
        }

        // Cf CPM may padd the original file
        File f = SD.open(name, O_CREAT | O_WRITE);
        if ( !f ) {
          Serial.println("-OK");
          return false;    
        }
        f.remove();
        f.close();
        // Cf CPM may padd the original file

        f = SD.open(name, O_CREAT | O_WRITE);
        if ( !f ) {
          Serial.println("-OK");
          return false;    
        }

        Serial.println("+OK");
        while( !Serial.available() ) { delay(2); }
        char sizeStr[12+1]; memset(sizeStr, 0x00, 12); tlen = Serial.readBytesUntil(0x0A, sizeStr, 12);
        long size = atol(sizeStr);
        sprintf(txt, "Downloading %s (%ld bytes)", name, size);
        yael_warn((const char*)txt);
        char packet[128+1];
        Serial.println("+OK");
        for(int readed=0; readed < size;) {
            while( !Serial.available() ) { delay(2); }
            int packetLen = Serial.readBytes( packet, 128 );
            f.write(packet, packetLen);
            f.flush();
            readed += packetLen;
        }
        f.close();
        yael_warn("-EOF-");
        yael_buzzer_beep();
        return true;
    }




// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$ YAEL API

bool yael_setup() { return Y_setup(); }

void yael_tft_cls() { tft.fillScreen(TFT_BLACK); tft.setCursor(0,0); }
void yael_tft_setCursor(int col, int row) { tft.setCursor(col,row); }

// see. xts_yael_dev_console.cpp ::
// void yael_tft_print(char ch) { 
//     if ( ch == 26 ) { yael_tft_cls(); return; }
//     tft.print(ch); 
// }
// void yael_tft_print(char* str) { tft.print(str); }
// void yael_tft_println(char* str) { tft.println(str); }

void yael_tft_drawBMP(char* filename, int x, int y) { 
    tft.setRotation(DEFAULT_TFT_ROTATION == 1 ? 2 : 0);
    drawBmp(filename, x, y);
    tft.setRotation(DEFAULT_TFT_ROTATION);
}
void yael_tft_drawPAK(char* filename, int x, int y, int imgNum) { drawImgFromPAK(filename, x, y, imgNum); }


void yael_tft_drawRect(int x, int y, int w, int h, uint16_t color) { tft.drawRect( x, y, w, h, color ); }
void yael_tft_fillRect(int x, int y, int w, int h, uint16_t color) { tft.fillRect( x, y, w, h, color ); }
void yael_tft_drawCircle(int x, int y, int radius, uint16_t color) { tft.drawCircle( x, y, radius, color ); }
void yael_tft_fillCircle(int x, int y, int radius, uint16_t color) { tft.fillCircle( x, y, radius, color ); }
void yael_tft_drawLine(int x, int y, int x2, int y2, uint16_t color) { tft.drawLine( x, y, x2, y2, color ); }


void yael_lcd_cls() { lcd_clear(); }
void yael_lcd_setCursor(int col, int row) { lcd_setCursor(col,row); }
void yael_lcd_print(char* str) { lcd_print(str); }
void yael_lcd_println(char* str, int row) { lcd_println(str, row); }

bool _mp3Playing = false;

void yael_mp3Play(int trackNum) { 
    bridgeSerial.print("MP"); bridgeSerial.write( (char)(trackNum/256) ); bridgeSerial.write( (char)(trackNum%256) ); delay(2);
    _mp3Playing = true; 
}
void yael_mp3Loop(int trackNum) { yael_lcd_print( (char*)"(!!) MP3 LOOP NYI" ); }
void yael_mp3Vol(int volume) { bridgeSerial.print("MV"); bridgeSerial.write( (char)(volume%(30+1)) ); delay(2); }

void yael_mp3Pause() { bridgeSerial.print("Mp"); delay(2); _mp3Playing = !_mp3Playing;}
void yael_mp3Stop() { bridgeSerial.print("Ms"); delay(2); _mp3Playing = false; }
void yael_mp3Next() { bridgeSerial.print("Mn"); delay(2); _mp3Playing = true; }
void yael_mp3Prev() { bridgeSerial.print("Mv"); delay(2); _mp3Playing = true; }

// bool yael_mp3IsPlaying() { return _mp3Playing; }
bool yael_mp3IsPlaying() { 
    return digitalRead(MP3_BUSY_PIN) == LOW; 
}

bool yael_subMcuIsReady() {
    if ( SUBMCU_READY_PIN < 0 ) { return true; }
    return digitalRead(SUBMCU_READY_PIN) == LOW; 
}

void yael_led(bool state, bool fastMode) { led(state, fastMode); }

char yael_keyb_poll() { return pollKeyb(); }

// ==========================
char* yael_wifi_getIP() { return getLocalIP(); }
char* yael_wifi_getSSID() { return getSSID(); }

bool yael_wifi_close() { stopWiFi(); return true; }
bool yael_wifi_beginAP() { bool ok = startWiFi(false); return ok; }
bool yael_wifi_startTelnetd() { stopTelnetd(); bool ok = startTelnetd(); return ok; }

bool yael_wifi_loop() { loopTelnetd(); }

void yael_wifi_telnetd_broadcast(char ch) { broadcastToTelnetd(ch); }
int  yael_wifi_telnetd_available() { return telnetdInputAvailable(); }
int  yael_wifi_telnetd_read() { return telnetdInputRead(); }

// run-time handler function
  void xts_hdl() {
      yael_wifi_loop();
  }

// ==========================

void yael_dbug(char* str) { 
    Serial.print("(ii) "); Serial.println(str); 
    yael_lcd_print(" (ii) ");
    yael_lcd_print(str);
    yael_lcd_print("\n");
}

void yael_dbug(const char* str) { yael_dbug( (char*)str ); }

void yael_warn(char* str) { 
    // no serial write : because used while Serial copy
    // Serial.print("(!!) "); Serial.println(str); 

    yael_lcd_cls();

    yael_lcd_print(" (!!) ");
    yael_lcd_print(str);
    yael_lcd_print("\n");
}

void yael_warn(const char* str) { yael_warn( (char*)str ); }

// end of if ESP32 ....
#endif