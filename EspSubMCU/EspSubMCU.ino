/**
 * Esp8266 - ESP12 as SubMCU replacement
 * 
 * Xtase - fgalliat @May 2019
 * 
 * 
 * Le croquis utilise 277344 octets (26%) de l'espace de stockage de programmes. Le maximum est de 1044464 octets.
 * Les variables globales utilisent 27416 octets (33%) de mémoire dynamique, ce qui laisse 54504 octets pour les variables locales. Le maximum est de 81920 octets.
 * 
 * wca -> WiFi Connect Apmode
 * wto -> WiFi Telnetd Open
 * wtc -> WiFi Telnetd Close
 * 
 * wghttp://jigsaw.w3.org/HTTP/connection.html\n
 * **                                         **
 *  -> will fetch that URL
 * 
 * ws  -> WiFi Stop
 */

#include <ESP8266WiFi.h>

#include <algorithm> // std::min

void _yield() {
    // prevent from deadlock (specific to ESP8266)
    yield();
}

// =============] YATL [===================
#include "xts_yatl_settings.h"


// =============] Devices [================

#define HAS_KEYB    1
#define HAS_MP3     1
#define HAS_JOYPAD  0

bool mp3ok = false;
bool kbdok = false;

// NodeMCU led GPIO16
// ESP led GPIO2
#define LED 2
bool ledState = false;

void led(bool state) {
    digitalWrite(LED, state ? HIGH : LOW);
    ledState = state;
}

void toggleLed() {
    led( !ledState );
}

// D4,D0,D7 -- RGB LED
#define LED_R 2
#define LED_G 16
#define LED_B 13

void red(bool state) { digitalWrite(LED_R, state ? HIGH : LOW); }
void green(bool state) { digitalWrite(LED_G, state ? HIGH : LOW); }
void blue(bool state) { digitalWrite(LED_B, state ? HIGH : LOW); }

void blink(int times, bool fast=true) {
    long time = fast ? 100 : 400;
    for(int i=0; i < times; i++) {
        red(true);
        delay(time);
        red(false);
        delay(time);
    }
}

// A0 -- default linked to Esp.getVcc() function

// D8 -- DFPlayer is playing ?
// BEWARE : can be used while flashing ...
// #define MP3_PLAYING 15
#define MP3_PLAYING -1

void setupAddPins() {
    pinMode(LED_R, OUTPUT); digitalWrite(LED_R, LOW);
    pinMode(LED_G, OUTPUT); digitalWrite(LED_G, LOW);
    pinMode(LED_B, OUTPUT); digitalWrite(LED_B, LOW);

    if (MP3_PLAYING > -1) {
        pinMode(MP3_PLAYING, INPUT);
    }
}

// =============] I2C [=================

// infos
// D1 & D2 (A4 & A5) because SX1509 lib does Wire.begin() w/ specifing SDA,SCL pins ...

#include <Wire.h>

#ifdef HAS_KEYB
    #include <SparkFunSX1509.h> // Include SX1509 library
    const byte SX1509_ADDRESS = 0x3E;  // SX1509 I2C address
    SX1509 io;
    
    #ifdef KEYB_MOBIGO
        #warning "-= Has Keyboard =-"

        #include "xts_dev_MobigoKeyboard.h"
        #define KB_AUTO_POLL false
        // #define KB_AUTO_POLL true
        MobigoKeyboard kbd(&io, KB_AUTO_POLL);

        #define keyboard0 kbd

        bool setupKeyboard() {
            if (!io.begin(SX1509_ADDRESS))
            {
                // Serial.println("SX1509 Failed.");
                blink(4);
                keyboard0.gpio_ok = false;
                return false;
            }
            keyboard0.gpio_ok = true;
            delay(300);
            
            kbd.setup(LED_R, LED_G, LED_B);
            return true;
        }
    #endif
#endif

// =============] UART [================
// D5 (GPIO14) & D6 (GPIO12)
#include <SoftwareSerial.h>
SoftwareSerial SerialX(14,12);
#define mp3Serial SerialX

// =============] MP3 [=================
#ifdef HAS_MP3
 #include <DFRobotDFPlayerMini.h>
 DFRobotDFPlayerMini myDFPlayer;
 #define SerialMP3 SerialX
 bool mp3InPause = false;
#endif

// ===========] Joypad [===============
#if HAS_JOYPAD
    uint8_t joypadX = 127;
    uint8_t joypadY = 127;
    uint8_t joypadB1 = 0;
    uint8_t joypadB2 = 0;

    void setupJoypad() {
    }

    void pollJoypad() {
    }

    void debugJoypad() {
        Serial.println("DBUGing Joypad");
    }
#endif
// ===========] Bridge [===============

#define BRIDGE_ON_SERIAL 1

// Set the Brigde Serial port
#if BRIDGE_ON_SERIAL
    #define serialBridge Serial
#else
    // #define serialBridge ats_serialBridge
#endif

int _avail() { return serialBridge.available(); }
int _read() { return serialBridge.read(); }

int _waitCh() {
   // _yield() ????
   while( _avail() <= 0 ) { delay(2); }
   return _read();
}

#define MAX_BRIDGE_LINE_LEN 255
char lastBridgeLine[MAX_BRIDGE_LINE_LEN+1];
char* _readLine() {
    memset(lastBridgeLine, 0x00, MAX_BRIDGE_LINE_LEN+1);
    serialBridge.readBytesUntil( '\n', lastBridgeLine, MAX_BRIDGE_LINE_LEN );
    return lastBridgeLine;
}

void _send(const char* str) { serialBridge.print(str); }
void _send(char* str) { serialBridge.print(str); }
void _send(char ch) { serialBridge.write(ch); }
void _send(int ch) { serialBridge.write( (char) ch); } // !!!! BEWARE : to look
void _send(float val) { serialBridge.print(val); }

void _waitBeforeResp() { delay(50); }

// requires that keyboard is wired !!!!
void sendLineToCPM(const char* line) {
   keyboard0.injectStr(line);
   keyboard0.injectChar('\r');
}

// =============] WiFi [================

#define ACTIVE_WIFI 1

#if ACTIVE_WIFI
    #warning "WiFi Active code"

    #define WIFI_CONN_MODE_NONE 0
    #define WIFI_CONN_MODE_STA  1
    #define WIFI_CONN_MODE_AP   2

    #define WIFI_USE_MODE_NONE    0
    #define WIFI_USE_MODE_TELNETD 1
    #define WIFI_USE_MODE_WGET    2

    uint8_t wifiConnMode = WIFI_CONN_MODE_NONE;
    uint8_t wifiUseMode  = WIFI_USE_MODE_NONE;


    #include "ssid_psk.h"
    #ifndef STANB
    // #ifndef STASSID
        // #define STASSID "your-ssid"
        // #define STAPSK  "your-password"

        #define STANB 1
        const char* ssids[STANB] = {
            "your-ssid"
        };
        const char* pwds[STANB] = {
            "your-password"
        };

    #endif

    // the telnet access password
    #ifndef TELNETPASSWD
      #define TELNETUSER   "root"
      #define TELNETPASSWD "yatl"
    #endif

    #define STACK_PROTECTOR  512 // bytes

    //how many clients should be able to telnet to this ESP8266
    #define MAX_SRV_CLIENTS 2
    /*const*/ char* ssid = "None Yet";
    /*const*/ char* password = "None Yet";

    #ifndef AP_SSID
      #define AP_SSID "YatlShift"
      // must be at least 8 chars long
      #define AP_PSK  "yatlshift"
    #endif


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

    #define logger (&Serial)

    #define TELNET_MODE_NONE 0
    #define TELNET_MODE_KEYB 1 

    int telnetMode = TELNET_MODE_NONE;

    void telnet_client_uploadToYatl(int num);

    int telnet_client_menu(int num) {
       WiFiClient clt = serverClients[num];

       clt.println("O-------------------------O");
       clt.println("|    Yatl System Menu     |");
       clt.println("+-------------------------+");
       clt.println("| 1. Keyboard control     |");
       clt.println("| 2. Full console         |");
       clt.println("| 3. Recv File            |");
       clt.println("| 4. Reset YATL           |");
       clt.println("|                         |");
       clt.println("| x. Resume               |");
       clt.println("| y. Disconnect           |");
       clt.println("| z. Stop WiFi            |");
       clt.println("O-------------------------O");
       clt.println("");
       clt.flush();
    //    while( clt.available() ) { clt.read(); }

       while( !clt.available() ) { delay(2); yield(); }
       char ch = clt.read();
    //    while( clt.available() ) { clt.read(); } // potential CRLF chars

       if ( ch == '1' ) {
           telnetMode = TELNET_MODE_KEYB;
           clt.println("> Have Keyboard for now...");
       } else if ( ch == '2' ) {
           // TODO : TMP
           telnetMode = TELNET_MODE_KEYB;

           clt.println("> Full Console Access NYI !");
           clt.println("> Have Keyboard for now...");
           clt.flush();
       } else if (ch == '3' ) {
           telnetMode = TELNET_MODE_NONE;
           // call Down(From)SubMcu
           // sendLineToCPM("C:DOWNSM");
           // TODO : put in downloading state
           telnet_client_uploadToYatl(num);
           return 0;
       } else if (ch == '4' ) {
           telnetMode = TELNET_MODE_NONE;
           // do not call "C:REBOOT.COM"
           sendLineToCPM("C:REBOOT");
       } else if (ch == 'x' ) {
           // Resume
           return 0;
       } else if (ch == 'y' ) {
           telnetMode = TELNET_MODE_NONE;
           // Disconnect client
           clt.flush();
           clt.stop();
       } else if (ch == 'z' ) {
           telnetMode = TELNET_MODE_NONE;
           // Stop Whole WiFi
           clt.flush();
           clt.stop();
           stopTelnetd();
           stopWiFi();
       }

       return 1;
    }

    void telnet_client_uploadToYatl(int num) {
       WiFiClient clt = serverClients[num];
       clt.println("***************************");
       clt.println("* Wait for upload to YATL *");
       clt.println("***************************");
       clt.println("");
       clt.flush();

    //    sendLineToCPM("C:DOWNSM");
       _send("\r"); // just to flush current line if any
       _send("C:DOWNSM\r"); // because of next _readLine() that waits before keyBeenRead
       delay(5);
       while( !serialBridge.available() ) { delay(2); yield(); }
       char* _tmp = _readLine(); // +OK
       if ( _tmp[0] != '+' ) {
           clt.println( _tmp );
       }

       int tmp;
       char filename[64+1]; memset(filename, 0x00, 64+1);
       char sizeStr[12+1]; memset(sizeStr, 0x00, 12+1);

       char* resp;

       clt.println("+OK Name for dest file ?"); clt.flush();
       while( !clt.available() ) { delay(2); yield(); }
       tmp = clt.readBytesUntil(0x0A, filename, 64);
       //serialBridge.println(filename);
       serialBridge.print(filename);
       serialBridge.write(0x0A); serialBridge.flush();
       delay(5);
       while( !serialBridge.available() ) { delay(2); yield(); }
       resp = _readLine(); if ( resp[0] == '-' ) { clt.println("-OK error"); clt.stop(); return; }

       clt.println("+OK Size of dest file ?"); clt.flush();
       while( !clt.available() ) { delay(2); yield(); }
       tmp = clt.readBytesUntil(0x0A, sizeStr, 32);

       long len = atol(sizeStr);
       if ( len <= 0 ) {
           serialBridge.println("0");
           clt.println("-OK WRONG Size of dest file !");
           return; // false
       }
    //    serialBridge.println(sizeStr);
       serialBridge.print(sizeStr);
       serialBridge.write(0x0A); serialBridge.flush();
       delay(5);
       while( !serialBridge.available() ) { delay(2); yield(); }
       resp = _readLine(); if ( resp[0] == '-' ) { clt.println("-OK error"); clt.stop(); return; }

       const int packetLen = 64;
       char packet[packetLen+1]; memset(packet, 0x00, packetLen+1);
       int readed = 0;
       for(int i=0; i < len;) {
           while( !clt.available() ) { delay(2); yield(); }
           readed = clt.readBytesUntil(0x0A, packet, packetLen);
           if ( readed == 0 ) {
             clt.println("-OK WRONG Packet !");
             return; // false
           }

           serialBridge.write(packet, readed);

           i += readed;
       }
    //    _readLine(); // no -EOF- line for now

       clt.println("+OK EOF");
    }

    void telnet_client_connected(int num) {
       WiFiClient clt = serverClients[num];

       clt.println("***************************");
       clt.println("* Welcome to Yatl System  *");
       clt.println("* Xtase-fgalliat @Jun2019 *");
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

       int rc;
       do {
         rc = telnet_client_menu(num);
         if ( rc == 0 && telnetMode == TELNET_MODE_NONE ) {
             continue;
         } else {
             break;
         }
       } while(true);

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
            if (i == MAX_SRV_CLIENTS) {
                server.available().println("busy");
                // hints: server.available() is a WiFiClient with short-term scope
                // when out of scope, a WiFiClient will
                // - flush() - all data will be sent
                // - stop() - automatically too
                // logger->printf("server is busy with %d active connections\n", MAX_SRV_CLIENTS);
            }
        }

        // ---------------------

        // === telnet as Keyb Mode ===

        //check TCP clients for data
        for (int i = 0; i < MAX_SRV_CLIENTS; i++) {
            bool hadSome = false;
            while (serverClients[i].available() /*&& Serial.availableForWrite() > 0*/ ) {
                // working char by char is not very efficient
                char ch = serverClients[i].read();
                // if ( ch == ':' ) {
                if ( ch == 27 ) { // Esc.
                    ch = serverClients[i].read();
                    if ( ch == 'q' ) {
                        serverClients[i].stop();
                        break;
                    } else if ( ch == 'm' ) {
                        telnet_client_menu(i);
                        break;
                    } else if ( ch == 'r' ) {
                        telnet_client_uploadToYatl(i);
                        break;
                    }
                    if (telnetMode == TELNET_MODE_KEYB) {
                        keyboard0.injectChar(27);
                        if ( ch != 0xFF ) {
                            keyboard0.injectChar(ch);
                        }
                    } else {
                        _send(ch);
                    }
                }
                if (telnetMode == TELNET_MODE_KEYB) {
                    keyboard0.injectChar(ch);
                } else {
                    _send(ch);
                }
                hadSome = true;
            }
        }

    }

    // ===] Client Mode[===
    /**
     * BEFORE :
     * Le croquis utilise 289808 octets (27%) de l'espace de stockage de programmes. Le maximum est de 1044464 octets.
     * Les variables globales utilisent 28644 octets (34%) de mémoire dynamique, ce qui laisse 53276 octets pour les variables locales. Le maximum est de 81920 octets.
     * 
     * AFTER :
     * Le croquis utilise 299800 octets (28%) de l'espace de stockage de programmes. Le maximum est de 1044464 octets.
     * Les variables globales utilisent 28948 octets (35%) de mémoire dynamique, ce qui laisse 52972 octets pour les variables locales. Le maximum est de 81920 octets.
     */
    #include <ESP8266HTTPClient.h>
    #include <WiFiClient.h>

    // ex. StarWars API https://swapi.co/api/people/1/ (BEWARE uses https)
    // ex. http://jigsaw.w3.org/HTTP/connection.html
    char* wget(char* url) {
        if ( !wifiStarted ) { return "! Wifi not Started"; }

        WiFiClient client;
        HTTPClient http;

        // Serial.print("[HTTP] begin...\n");
        if ( http.begin(client, url) ) {
            // Serial.print("[HTTP] GET...\n");
            // start connection and send HTTP header
            int httpCode = http.GET();

            // httpCode will be negative on error
            if (httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                // Serial.printf("[HTTP] GET... code: %d\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = http.getString();
                    // print directly on Brigde
                    Serial.println(payload);
                }
            } else {
                char ret[64+1];
                memset(ret, 0x00, 64+1); 
                sprintf( ret, "-NOK HTTP %s", http.errorToString(httpCode).c_str() );
                return ret;
            }

            http.end();
            return "+OK";
        } else {
            return (char*)"-NOK Could not connect";
        }

        return (char*)"-NOK Something went wrong";
    }
#else
    bool isWiFiStarted() { return false; }
    bool isTelnetdStarted() { return false; }
    bool startWiFi() { return false; }
    void stopWiFi() {}
    char* getLocalIP() { return (char*)"0.0.0.0"; }
    char* getSSID() { return (char*)"None"; }
    bool startTelnetd() { return false; }
    void stopTelnetd() {}
    char* wget(char* url) { return "404"; }
#endif

// =============] APM Code [==============

void reboot() {
    if ( telnetdStarted ) { stopTelnetd(); }
    if ( wifiStarted ) { stopWiFi(); }

    if ( kbdok ) { keyboard0.reboot(); }
    if ( mp3ok ) { myDFPlayer.stop(); myDFPlayer.reset(); }

    ESP.restart();
}

void shutdown() {
    if ( telnetdStarted ) { stopTelnetd(); }
    if ( wifiStarted ) { stopWiFi(); }

    if ( kbdok ) { keyboard0.reboot(); }
    if ( mp3ok ) { myDFPlayer.stop(); myDFPlayer.reset(); }

    ESP.deepSleep(0);
}

// =============] Core Code [=============

// to read MCU voltage internal way
// allows ESP.getVcc()
ADC_MODE(ADC_VCC);

void setup() {
    pinMode(LED, OUTPUT);  led(false);
    setupAddPins();

    // Serial.begin(BAUD_SERIAL);
    // Serial.setRxBufferSize(RXBUFFERSIZE);
    #if HAS_JOYPAD
        setupJoypad();
    #endif

    Serial.begin(115200);
    #if not BRIDGE_ON_SERIAL
        serialBridge.begin(115200);
    #endif


   // + system info
   // > logging info

   // ===== Ms ChatPad for XBOX 360 =====
   #if HAS_KEYB
    kbdok = setupKeyboard();
   #else
    kbdok = false; // keyboard is NOK
   #endif

   // ===== DFPlayer mini MP3 =====
   #if HAS_MP3
    SerialMP3.begin(9600);
    delay(100);
    if (!myDFPlayer.begin(SerialMP3)) {
        // Serial.println(F("> Unable to begin:"));
        // Serial.println(F("> 1.Please recheck the connection!"));
        // Serial.println(F("> 2.Please insert the SD card!"));
        blink(5);
        mp3ok = false;
    }
    else {
        mp3ok = true;
        myDFPlayer.volume(20);  //Set volume value. From 0 to 30
        // myDFPlayer.play(1);  //Play the first mp3
    }
   #else
    mp3ok = false;
   #endif


   blink(3, false);

   #if not BRIDGE_ON_SERIAL
     Serial.println("> Ready to work");
   #endif
//    _send("> Ready to work\n");
}

void testWiFi() {
    _send("Connecting to WiFi ...\n");
    bool ok = isWiFiStarted() || startWiFi();
    if ( ok ) {
        _send("Connected to WiFi\n");

        // _send("Starting telnetd ...\n");
        // bool ok2 = startTelnetd();
        // if ( ok2 ) {
        //     _send("Started telnetd\n");
        //     _send("IP: ");
        //     _send( getLocalIP() );
        //     _send("\n");
        // } else {
        //     _send("Could not start telnetd\n");
        //     _send("Closing WiFi...\n");
        //     stopWiFi();
        // }

        _send("Starting httpClient ...\n");
        char* result = wget("http://jigsaw.w3.org/HTTP/connection.html");
        _send(result);
        _send("\n");
        stopWiFi();

    } else {
        _send("Could not Connect to WiFi\n");
    }
}


void testRoutine() {
    // here the code to be tested ...
    Serial.println("Enter in tests");

    testWiFi();

    Serial.println("Exit from tests");
}

char tmpMsg[256+1];
int loopCounter = 0;
void loop() {
    #if HAS_JOYPAD
        pollJoypad();
    #endif

    #if ACTIVE_WIFI
        if (telnetdStarted) { loopTelnetd(); }
    #endif

    // TMP
    if (mp3ok) {
        if (MP3_PLAYING > -1 && digitalRead(MP3_PLAYING) == HIGH ) {
            Serial.println("MP3 Playing");
        }
    }

    #ifdef HAS_KEYB
     _yield();
     if (!KB_AUTO_POLL) { keyboard0.poll(); _yield(); }
    #endif

    if ( _avail() ) {
        char ch = _read();

        if ( ch == 'i' ) {
            _waitBeforeResp();
            if (!mp3ok) Serial.println("+m:NOK");
            else Serial.println("+m:OK");
            if (!kbdok) Serial.println("+k:NOK");
            else Serial.println("+k:OK");
            if (!wifiStarted) Serial.println("+w:OFF");
            else Serial.println("+w:ON");
        } else if ( ch == 'k' ) {
            #ifdef HAS_KEYB
                if ( !keyboard0.available() ) { 
                    // Serial.println("> <Empty Buffer>"); 
                    _send(0x00); 
                }
                else {
                    while( keyboard0.available() >= 1 ) { _send( keyboard0.read() ); _yield(); }
                    _send(0x00);
                }
            #else
                _send(0x00);
            #endif
        } else if ( ch == 'm' ) {
            // MP3 subCommands to dispatch
            #ifdef HAS_MP3
              myDFPlayer.play(65); // THEC64-MENU-Theme
            #endif
        } else if ( ch == 'p' ) {
            #ifdef HAS_MP3
              while( _avail() <= 0 ) { delay(2); }
              ch = _read();
              if ( ch == 'p' ) {
                  // play an MP3
                  while( _avail() <= 0 ) { delay(2); }
                  ch = _read();
                  uint8_t d0 = ch;
                  while( _avail() <= 0 ) { delay(2); }
                  ch = _read();
                  uint8_t d1 = ch;
                  int trackNumber = (d0<<8)+d1;
                  myDFPlayer.play(trackNumber);
              } else if ( ch == 'l' ) {
                  // loop an MP3
                  while( _avail() <= 0 ) { delay(2); }
                  ch = _read();
                  uint8_t d0 = ch;
                  while( _avail() <= 0 ) { delay(2); }
                  ch = _read();
                  uint8_t d1 = ch;
                  int trackNumber = (d0<<8)+d1;
                  myDFPlayer.loop(trackNumber);
              } else if ( ch == 'n' ) {
                  myDFPlayer.next();
              } else if ( ch == 'v' ) {
                  myDFPlayer.previous();
              } else if ( ch == 'P' ) {
                  if ( mp3InPause ) {
                      myDFPlayer.start();
                      mp3InPause = false;
                  } else {
                    myDFPlayer.pause();
                    mp3InPause = true;
                  }
              } else if ( ch == 's' ) {
                  myDFPlayer.stop();
              } else if ( ch == 'V' ) {
                  while( _avail() <= 0 ) { delay(2); }
                  ch = _read();
                  myDFPlayer.volume(ch);
              }
            #endif
        } else if ( ch == 'j' ) {
            #if HAS_JOYPAD
                debugJoypad();

                _send( joypadX );
                _send( joypadY );
                _send( joypadB1 );
                _send( joypadB2 );
            #else
                _send("-NO Joypad");
            #endif
        } else if ( ch == 'v' ) {
            // Voltage Control
            float  voltage = (ESP.getVcc() / 1000.0); // + 0.2 ;// avec offset empirique
            _waitBeforeResp();
            sprintf(tmpMsg, "%g\n", voltage);
            // _send( voltage );
            // _send( '\n' );
            _send( tmpMsg );

            // if (voltage < 3.0) {   // a 3V l'accu LiFePo4 est quasiment vide; le protéger d'une décharge excessive
            //     ESP.deepSleep(0);   // = arret définitif si 'LOW BATT'
            // }
        } else if ( ch == 'l' ) {
            // RGB led control [XXX] where X can be '0' or '1' else ignored
            while( _avail() < 3 ) { delay(2); _yield(); }
            char chR = _read();
            char chG = _read();
            char chB = _read();
            
            if ( chR == '0' ) red(false);
            else if ( chR == '1' ) red(true);
            // else ignore
            if ( chG == '0' ) green(false);
            else if ( chG == '1' ) green(true);
            if ( chB == '0' ) blue(false);
            else if ( chB == '1' ) blue(true);

        } else if ( ch == 'w' ) {
            // WiFi Control
            int bte = _waitCh();
            if ( bte >= -1 ) {
                char subCmd = (char)bte;
                if ( subCmd == 'c' ) {
                    char mode = (char)_waitCh();
                    // 's' -> STA mode
                    // 'a' -> AP mode
                    bool ok = isWiFiStarted() || startWiFi( mode != 'a' );
                    _waitBeforeResp();
                    if ( ok ) { 
                        sprintf(tmpMsg, "+OK Wifi connected : %s\n", (const char*) getLocalIP());
                        _send(tmpMsg); 
                    }
                    else { _send("-NOK Wifi not connected\n"); }
                } else if ( subCmd == 's' ) {
                    stopWiFi();
                    _waitBeforeResp();
                    _send("+OK Wifi disconnected\n");
                } else if ( subCmd == 'i' ) {
                    // getIP
                    _waitBeforeResp();
                    sprintf(tmpMsg, "%s\n", (const char*) getLocalIP());
                    _send(tmpMsg);
                } else if ( subCmd == 'e' ) {
                    // getESSID
                    _waitBeforeResp();
                    sprintf(tmpMsg, "%s\n", (const char*) getSSID());
                    _send(tmpMsg);
                } else if ( subCmd == 't' ) {
                    // telnet control
                    char mode = (char)_waitCh();
                    if ( mode == 'o' ) {
                        // Open - "wto"
                        bool ok = isWiFiStarted() && startTelnetd();
                        _waitBeforeResp();
                        if ( ok ) {
                            sprintf(tmpMsg, "+OK Telnetd opened : %s:23\n", (const char*) getLocalIP());
                            _send(tmpMsg); 
                        }
                        else { _send("-NOK Telnetd not opened\n"); }
                    } else if ( mode == 'c' ) {
                        // Close - "wtc"
                        stopTelnetd();
                        _waitBeforeResp();
                        _send("+OK Telnetd closed\n");
                    } 
                } else if ( subCmd == 'g' ) {
                    // wget '....\n' - beware w/ EOL & Arduino Serial Monitor ....
                    char* url = _readLine();
                    // Serial.println("Will fetch ");
                    // Serial.println(url);
                    _waitBeforeResp();
                    _send( (const char*)wget(url) );
                    _send( '\n' );
                } else {
                    _waitBeforeResp();
                    Serial.print("WiFi SubComand NYI");
                    Serial.print('\n');
                }
            }
        } else if ( ch == 't' ) {
            testRoutine();
        } else if ( ch == 'r' ) {
            reboot();
        } else if ( ch == 'h' ) {
            shutdown();
        } else if ( ch == '\n' || ch == '\r' ) {
            // may be some dusty end of line due to terminal 
            // that was used
        } else {
            _waitBeforeResp();
            Serial.print("> Command ");
            Serial.print(ch);
            Serial.println(" is NYI ");
        }

        _yield();
    }


    loopCounter++;
    loopCounter %= 200;
    if ( loopCounter == 0 ) { 
        // Serial.print('.'); 
        #ifdef HAS_KEYB
          // due to a frequent read bug ...
          // keyboard0.init(false);
          // now done only if error handled
        #endif
    }
    delay(2);

    _yield();
}