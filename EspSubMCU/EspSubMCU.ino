/**
 * Esp8266 - ESP12 as SubMCU replacement
 * 
 * Xtase - fgalliat @May 2019
 * 
 * 
 * Le croquis utilise 277344 octets (26%) de l'espace de stockage de programmes. Le maximum est de 1044464 octets.
 * Les variables globales utilisent 27416 octets (33%) de mémoire dynamique, ce qui laisse 54504 octets pour les variables locales. Le maximum est de 81920 octets.
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

#define HAS_KEYB 1
#define HAS_MP3  1

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
void _send(const char* str) { serialBridge.print(str); }
void _send(char* str) { serialBridge.print(str); }
void _send(char ch) { serialBridge.write(ch); }
void _send(int ch) { serialBridge.write( (char) ch); } // !!!! BEWARE : to look




// =============] WiFi [================

#define ACTIVE_WIFI 1

#if ACTIVE_WIFI
    #warning "WiFi Active code"

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

    bool startWiFi(bool staMode=true) {
        wifiStarted = false;
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        // logger->print("\nConnecting to ");
        // logger->println(ssid);
        red(true); green(false);
        int retry = 0;
        while (WiFi.status() != WL_CONNECTED) {
            // logger->print('.');
            delay(500);
            if (retry > 10) { return false; }
            retry++;
        }
        red(false); green(true);
        // logger->println();
        // logger->print("connected, address=");
        // logger->println(WiFi.localIP());
        wifiStarted = true;
        return true;
    }

    void stopWiFi() {
        WiFi.disconnect();
        wifiStarted = false;
    }

    char* getLocalIP() {
        return (char*) WiFi.localIP().toString().c_str();
    }

    bool startTelnetd() {
        if ( !wifiStarted ) { return false; }
        telnetdStarted = false;
        server.begin();
        server.setNoDelay(true);
        telnetdStarted = true;
        return true;
    }

    void stopTelnetd() {
        if ( !telnetdStarted ) { return; }
        server.close();
        telnetdStarted = false;
    }
#else
    bool startWiFi() { return false; }
    void stopWiFi() {}
    char* getLocalIP() { return (char*)"0.0.0.0"; }
    bool startTelnetd() { return false; }
    void stopTelnetd() {}
#endif

// =============] Core Code [=============

void setup() {
    pinMode(LED, OUTPUT);  led(false);
    setupAddPins();

    // Serial.begin(BAUD_SERIAL);
    // Serial.setRxBufferSize(RXBUFFERSIZE);

    setupJoypad();

    Serial.begin(115200);
    // serialBridge.begin(115200);


   // + system info
   // > logging info

   // ===== Ms ChatPad for XBOX 360 =====
   #ifdef HAS_KEYB
    kbdok = setupKeyboard();
   #else
    kbdok = false; // keyboard is NOK
   #endif

   // ===== DFPlayer mini MP3 =====
   #ifdef HAS_MP3
    SerialMP3.begin(9600);
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
   _send("> Ready to work");
}

void testWiFi() {
    _send("Connecting to WiFi ...");
    bool ok = startWiFi();
    if ( ok ) {
        _send("Connected to WiFi");

        _send("Starting telnetd ...");
        bool ok2 = startTelnetd();
        if ( ok2 ) {
            _send("Started telnetd");
        } else {
            _send("Could not start telnetd");
            _send("Closing WiFi...");
            stopWiFi();
        }
    } else {
        _send("Could not Connect to WiFi");
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
    pollJoypad();

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
            debugJoypad();

            _send( joypadX );
            _send( joypadY );
            _send( joypadB1 );
            _send( joypadB2 );
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