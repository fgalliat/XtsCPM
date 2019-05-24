/**
 * Esp8266 - ESP12 as SubMCU replacement
 * 
 * Xtase - fgalliat @May 2019
 * 
 * 
 * Le croquis utilise 224317 octets (21%) de l'espace de stockage de programmes. Le maximum est de 1044464 octets.
 * Les variables globales utilisent 31740 octets (38%) de mémoire dynamique, ce qui laisse 50180 octets pour les variables locales. Le maximum est de 81920 octets.
 * 
 */

#include <ESP8266WiFi.h>

#include <algorithm> // std::min



// NodeMCU led GPIO16
// ESP led GPIO2
#define LED 2
bool ledState = false;

void led(bool state) {
    digitalWrite(LED, state ? HIGH : LOW);
    ledState = state;
}

#include "ssid_psk.h"
#ifndef STASSID
 #define STASSID "your-ssid"
 #define STAPSK  "your-password"
#endif


#define BAUD_SERIAL 115200
#define BAUD_LOGGER 115200
#define RXBUFFERSIZE 1024

// #define logger (&Serial1)
#define logger (&Serial)

#define STACK_PROTECTOR  512 // bytes

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 2
const char* ssid = STASSID;
const char* password = STAPSK;

const int port = 23;

WiFiServer server(port);
WiFiClient serverClients[MAX_SRV_CLIENTS];


void setup() {
    pinMode(LED, OUTPUT);
    led(false);

    Serial.begin(BAUD_SERIAL);
    // Serial.setRxBufferSize(RXBUFFERSIZE);
    Serial.println("ESP is ON");

    logger->begin(BAUD_LOGGER);
    logger->println("\n\nUsing Serial1 for logging");
    // logger->println(ESP.getFullVersion());
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

}

void loop() {
    led( !ledState );
    Serial.print(".");
    delay(500);

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
        // while (serverClients[i].available() ) {
            // working char by char is not very efficient
            Serial.write(serverClients[i].read());
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