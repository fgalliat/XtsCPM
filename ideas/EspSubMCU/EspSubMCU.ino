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

// =============] YATL [===================
#include "xts_yatl_settings.h"


// =============] Devices [================

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

#define LED2 16
void led2(bool state) {
    digitalWrite(LED2, state ? HIGH : LOW);
}

#define HAS_KEYB 1
#define HAS_MP3  1


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
        // #define KB_AUTO_POLL false
        #define KB_AUTO_POLL true
        MobigoKeyboard kbd(&io, KB_AUTO_POLL);

        #define keyboard0 kbd

        bool setupKeyboard() {
            if (!io.begin(SX1509_ADDRESS))
            {
                Serial.println("Failed to communicate.");
                // while (1) ; // If we fail to communicate, loop forever.
                return false;
            }
            delay(300);
            
            kbd.setup(LED2, LED2, LED2);
            return true;
        }
    #endif
#endif

// =============] UART [================

#include <SoftwareSerial.h>
SoftwareSerial SerialX(14,12);
#define mp3Serial SerialX

#define BAUD_SERIAL 115200
#define BAUD_LOGGER 115200
#define RXBUFFERSIZE 1024

// #define logger (&Serial1)
#define logger (&Serial)
// #define logger (&SerialX)

// =============] MP3 [=================
#ifdef HAS_MP3
 #include <DFRobotDFPlayerMini.h>
 DFRobotDFPlayerMini myDFPlayer;
 #define SerialMP3 SerialX
 bool mp3InPause = false;
 // TODO myDFPlayer.begin()
#endif

// =============] WiFi [================

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


void setup() {
    pinMode(LED, OUTPUT);  led(false);
    pinMode(LED2, OUTPUT); led2(false);

    Serial.begin(BAUD_SERIAL);
    Serial.setRxBufferSize(RXBUFFERSIZE);

    while(!Serial) { delay(1); }

    Serial.println("ESP is ON");

    bool kbOk = false;
    bool mp3Ok = false;

    led2(!false);
    #ifdef HAS_KEYB
      // Wire.begin(4,5);
      // Wire.begin(); is done by SX1509 begin() -> init();
      kbOk = setupKeyboard();
    #endif

    #if 0
    logger->begin(BAUD_LOGGER);
    // logger->listen(); // till logger is a SoftwareSerial
    logger->println("\n\nUsing Serial1 for logging");
    #endif

    logger->println(ESP.getFullVersion());
    logger->printf("Serial baud: %d (8n1: %d KB/s)\n", BAUD_SERIAL, BAUD_SERIAL * 8 / 10 / 1024);
    logger->printf("Serial receive buffer size: %d bytes\n", RXBUFFERSIZE);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    logger->print("\nConnecting to ");
    logger->println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        logger->print('.');
        delay(500);
    }
    logger->println();
    logger->print("connected, address=");
    logger->println(WiFi.localIP());

    //start server
    server.begin();
    server.setNoDelay(true);

    logger->print("Ready! Use 'telnet ");
    logger->print(WiFi.localIP());
    logger->printf(" %d' to connect\n", port);
    led2(false);
}

void loop() {
    toogleLed();
    // Serial.print(".");
    // delay(500);

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

    //check TCP clients for data
    #if 1
    // Incredibly, this code is faster than the bufferred one below - #4620 is needed
    // loopback/3000000baud average 348KB/s
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
        while (serverClients[i].available() && Serial.availableForWrite() > 0) {
            // working char by char is not very efficient
            char ch = serverClients[i].read();
            if ( ch == ':' ) {
                ch = serverClients[i].read();
                if ( ch == 'q' ) {
                    serverClients[i].stop();
                    break;
                }
                Serial.write( ch );
            }
            Serial.write( ch );
        }
    #else
    // loopback/3000000baud average: 312KB/s
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
        while (serverClients[i].available() && Serial.availableForWrite() > 0) {
            size_t maxToSerial = std::min(serverClients[i].available(), Serial.availableForWrite());
            maxToSerial = std::min(maxToSerial, (size_t)STACK_PROTECTOR);
            uint8_t buf[maxToSerial];
            size_t tcp_got = serverClients[i].read(buf, maxToSerial);
            size_t serial_sent = Serial.write(buf, tcp_got);
            if (serial_sent != maxToSerial) {
                logger->printf("len mismatch: available:%zd tcp-read:%zd serial-write:%zd\n", maxToSerial, tcp_got, serial_sent);
            }
        }
    #endif
    
    yield(); // ESP8266 specific -- else keeps on rebooting
    while ( keyboard0.available() > 0 ) {
        int ch = keyboard0.read(); // unconditional read else will make an inf. loop
        Serial.write( (char)ch );
        // only for 1st connected client
        if (serverClients[0]) {
            serverClients[0].write( (char)ch );
        }
        yield(); // ESP8266 specific
    }
    yield(); // ESP8266 specific


    // determine maximum output size "fair TCP use"
    // client.availableForWrite() returns 0 when !client.connected()
    size_t maxToTcp = 0;
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
        if (serverClients[i]) {
            size_t afw = serverClients[i].availableForWrite();
            if (afw) {
                if (!maxToTcp) {
                    maxToTcp = afw;
                } else {
                    maxToTcp = std::min(maxToTcp, afw);
                }
            } else {
                // warn but ignore congested clients
                logger->println("one client is congested");
            }
        }

    //check UART for data
    size_t len = std::min((size_t)Serial.available(), maxToTcp);
    len = std::min(len, (size_t)STACK_PROTECTOR);
    if (len) {
        uint8_t sbuf[len];
        size_t serial_got = Serial.readBytes(sbuf, len);
        // push UART data to all connected telnet clients
        for (int i = 0; i < MAX_SRV_CLIENTS; i++)
        // if client.availableForWrite() was 0 (congested)
        // and increased since then,
        // ensure write space is sufficient:
        if (serverClients[i].availableForWrite() >= serial_got) {
            size_t tcp_sent = serverClients[i].write(sbuf, serial_got);
            if (tcp_sent != len) {
              logger->printf("len mismatch: available:%zd serial-read:%zd tcp-write:%zd\n", len, serial_got, tcp_sent);
            }
        }
    }
}