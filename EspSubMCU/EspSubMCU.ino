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

void toogleLed() {
    led( !ledState );
}

// D4,D0,D7 -- RGB LED
#define LED_R 2
#define LED_G 16
#define LED_B 13

void red(bool state) { digitalWrite(LED_R, state ? HIGH : LOW); }
void green(bool state) { digitalWrite(LED_G, state ? HIGH : LOW); }
void blue(bool state) { digitalWrite(LED_B, state ? HIGH : LOW); }

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
                Serial.println("SX1509 Failed.");
                return false;
            }
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
    #ifndef STASSID
        #define STASSID "your-ssid"
        #define STAPSK  "your-password"
    #endif

    #define STACK_PROTECTOR  512 // bytes

    //how many clients should be able to telnet to this ESP8266
    #define MAX_SRV_CLIENTS 2
    const char* ssid = STASSID;
    const char* password = STAPSK;

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
            WiFi.begin(ssid, password);
            red(true); green(false);
            int retry = 0;
            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                if (retry > 10) { return false; }
                retry++;
            }
            red(false); green(true);
            wifiConnMode = WIFI_CONN_MODE_STA;
            wifiStarted = true;
        } else {
            WiFi.mode(WIFI_AP);
            delay(200);
            const char* _ssid = "YatlShift";
            // must be longer then 8 chars
            const char* _password = "yatlshift";
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
            strcpy(currentSsid, "YatlShift" );
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

    void loopTelnetd() {
        if (!telnetdStarted) { return; }

        //check if there are any new clients
        if (server.hasClient()) {
            //find free/disconnected spot
            int i;
            for (i = 0; i < MAX_SRV_CLIENTS; i++)
            if (!serverClients[i]) { // equivalent to !serverClients[i].connected()
                serverClients[i] = server.available();
                logger->print("New client: index ");
                logger->print(i);
                break;
            }

            //no free/disconnected spot so reject
            if (i == MAX_SRV_CLIENTS) {
                server.available().println("busy");
                // hints: server.available() is a WiFiClient with short-term scope
                // when out of scope, a WiFiClient will
                // - flush() - all data will be sent
                // - stop() - automatically too
                logger->printf("server is busy with %d active connections\n", MAX_SRV_CLIENTS);
            }
        }

        // ---------------------

        // === telnet as Keyb Mode ===

        //check TCP clients for data
        for (int i = 0; i < MAX_SRV_CLIENTS; i++) {
            while (serverClients[i].available() && Serial.availableForWrite() > 0) {
                // working char by char is not very efficient
                char ch = serverClients[i].read();
                if ( ch == ':' ) {
                    ch = serverClients[i].read();
                    if ( ch == 'q' ) {
                        serverClients[i].stop();
                        break;
                    }
                    // Serial.write( ch );
                    _send(ch);
                }
                // Serial.write( ch );
                _send(ch);
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
        Serial.println(F("> Unable to begin:"));
        Serial.println(F("> 1.Please recheck the connection!"));
        Serial.println(F("> 2.Please insert the SD card!"));
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


   led(true); delay(300);
   toogleLed(); delay(300);
   toogleLed(); delay(300);
   toogleLed(); delay(300);

   #if not BRIDGE_ON_SERIAL
     Serial.println("> Ready to work");
   #endif
   _send("> Ready to work\n");
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
                _send("NO Joypad");
            #endif
        } else if ( ch == 'v' ) {
            // Voltage Control
            float  voltage = (ESP.getVcc() / 1000.0); // + 0.2 ;// avec offset empirique
            _send( voltage );
            _send( '\n' );
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
                    if ( ok ) { _send("Wifi connected : ");_send((const char*) getLocalIP() );_send('\n'); }
                    else { _send("Wifi not connected\n"); }
                } else if ( subCmd == 's' ) {
                    stopWiFi();
                    _send("Wifi disconnected\n");
                } else if ( subCmd == 'i' ) {
                    // getIP
                    _send((const char*) getLocalIP() );_send('\n');
                } else if ( subCmd == 'e' ) {
                    // getESSID
                    _send((const char*) getSSID() );_send('\n');
                } else if ( subCmd == 't' ) {
                    // telnet control
                    char mode = (char)_waitCh();
                    if ( mode == 'o' ) {
                        // Open - "wto"
                        bool ok = isWiFiStarted() && startTelnetd();
                        if ( ok ) { _send("Telnetd opened : ");_send((const char*) getLocalIP() );_send(":23\n"); }
                        else { _send("Telnetd not opened\n"); }
                    } else if ( mode == 'c' ) {
                        // Close - "wtc"
                        stopTelnetd();
                    } 
                } else if ( subCmd == 'g' ) {
                    // wget '....\n' - beware w/ EOL & Arduino Serial Monitor ....
                    char* url = _readLine();
                    // Serial.println("Will fetch ");
                    // Serial.println(url);
                    _send( (const char*)wget(url) );
                    _send( '\n' );
                } else {
                    Serial.print("WiFi SubComand NYI");
                    Serial.print('\n');
                }
            }
        } else if ( ch == 't' ) {
            testRoutine();
        } else if ( ch == '\n' || ch == '\r' ) {
            // may be some dusty end of line due to terminal 
            // that was used
        } else {
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