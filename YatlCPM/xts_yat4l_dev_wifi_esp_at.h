/**
 * Yat4l Wifi by ESP8266 (esp12) AT cmds Driver impl.
 * 
 * 
 * Xtase - fgalliat @Dec2019
 * 
 */

    #define WIFI_SERIAL Serial2
    #define WIFI_CMD_TIMEOUT 6000
    #define WIFI_SERIAL_BAUDS 115200 
    // #define WIFI_SERIAL_BAUDS 9600 

    bool yat4l_wifi_setup() { 
        WIFI_SERIAL.begin(WIFI_SERIAL_BAUDS); 

long t0 = millis();

while( !WIFI_SERIAL ) {
    ; // TODO : timeout
    if ( millis() - t0 > 1500 ) {break;}
}

while(WIFI_SERIAL.available() == 0) {
    delay(50);
    if ( millis() - t0 > 1500 ) {break;}
}

        while(WIFI_SERIAL.available() > 0) {
            int ch = WIFI_SERIAL.read();
            Serial.write(ch);
        }
        Serial.println("module powered");


        return true; 
    }

    void _wifiSendCMD(const char* cmd) {
        // Serial.println("flush 1.a");
        WIFI_SERIAL.flush();
        // Serial.println("flush 1.b");

        // add CRLF
        Serial.print("WIFI >");Serial.println(cmd);
        // char buff[128]; memset(buff, 0x00, 128); sprintf(buff, "%s\r\n", cmd);


        // int tlen = strlen( buff );
        int tlen = strlen( cmd ) + 2;
        if ( Serial.availableForWrite() < tlen ) {
            Serial.println("NotEnoughtAvailableForWrite !!!!");
        }

        // Serial.print("WIFI >>");Serial.println(buff);
        // Serial.print("WIFI >>");Serial.println(cmd);
        // WIFI_SERIAL.write( buff, tlen );
        WIFI_SERIAL.print( cmd );
        WIFI_SERIAL.print( "\r\n" );
        // Serial.println("flush 2.a");
        WIFI_SERIAL.flush();
        // Serial.println("flush 2.b");

        // delay(100);
        yield();
        // delayMicroseconds(10);
        Serial.println("Sent packet");
    }

    // char _line[512+1];
    elapsedMillis sinceStartRead; // Teensy Specific

    // assumes that _line is 512+1 bytes allocated 
    int _wifiReadline(char* _line, int timeout=WIFI_CMD_TIMEOUT) {
        Serial.println("::_wifiReadline()");

        // WIFI_SERIAL.flush();

        memset(_line, 0x00, 512+1);
        // removes CRLF
        Serial.print("WIFI READ >");Serial.println(timeout);
        yield();

        WIFI_SERIAL.setTimeout( timeout );
// int readed = WIFI_SERIAL.readBytesUntil('\n', _line, 512);
int readed = WIFI_SERIAL.readBytesUntil('\r', _line, 512);
if ( readed <= 0 ) { _line[0] = 0x00; return -1; }

if ( WIFI_SERIAL.available() >= 1 ) {
    if (  WIFI_SERIAL.peek() == '\n' ) {
        WIFI_SERIAL.read();
    }
}


if ( _line[0] == 0x00 ) { return 0; }

int t = strlen(_line);
        // if ( t > 0 && _line[t-1] == '\r' ) {
        //     _line[t-1] = 0x00;
        //     t--;
        // }

if ( t < 0 ) { _line[0] = 0x00; return -1; }

return t;

//         long t0 = millis();
//         // sinceStartRead = 0;
//         // while( WIFI_SERIAL.available() <= 0 ) {
//         while( true ) {
//             // yield();
//             delay(10);
//             if ( WIFI_SERIAL.available() > 0 ) { break; }
//             // yield();
//             if ( millis() - t0 >= timeout ) {
//             // Serial.print(".");
//             // if ( sinceStartRead >= timeout ) {
//                 Serial.println("EJECTED 2"); 
//                 return -1;
//                 // return _line;
//             }
//             // delayMicroseconds(100);
//             // Serial.print("+");
//         }


//         Serial.println(".before");
//         // int readed = WIFI_SERIAL.readBytesUntil('\n', _line, 512);
// int cpt = 0;
// while( WIFI_SERIAL.available() > 0 ) {
//     if ( cpt >= 512 ) { break; }
//     int ch0 = WIFI_SERIAL.read();
//     if ( ch0 == -1 && cpt == 0 ) { Serial.println("-1"); cpt = -1; break; }
//     // Serial.write( (char)ch0 );
//     if ( (char)ch0 == '\n' ) { break; }

//     _line[cpt] = (char)ch0;
//     // Serial.write( (char)ch0 );
//     cpt++;
// }
// int readed = cpt;

//         Serial.println(".after");
//         // if (readed <= 0 ) { Serial.println("EJECTED"); return NULL; }
//         if (readed < 0 ) { Serial.println("EJECTED"); return -1; }
//         int t = strlen(_line);
//         if ( t > 0 && _line[t-1] == '\r' ) {
//             _line[t-1] = 0x00;
//             t--;
//         }
//         Serial.print("WIFI READ <");Serial.println(_line);
//         return t;
    }

    #define _RET_TIMEOUT 0
    #define _RET_OK 1
    #define _RET_ERROR 2

    extern bool equals(char* s, char* t);

    int _wifi_waitForOk() {
        char resp[512+1];
        while (true) {
            // Serial.println("--:beforeReadline");
            // char* resp = _wifiReadline();
            int readed = _wifiReadline(resp);
            // if ( resp == NULL ) { Serial.println("TIMEOUT--"); return _RET_TIMEOUT; }
            if ( readed == -1 ) { Serial.println("TIMEOUT--"); return _RET_TIMEOUT; }
            if ( strlen( resp ) > 0 ) {
                Serial.print("-->");
                Serial.println(resp);

                // Serial.print("-->a");
                if ( equals(&resp[0], (char*)"OK") ) { Serial.println("OK--"); return _RET_OK; }
                // Serial.print("-->b");
                if ( equals(&resp[0], (char*)"ERROR") ) { Serial.println("ERROR--"); return _RET_ERROR; }

            } else {
                Serial.println("--:EMPTY");
            }
            // Serial.println("--:beforeDelay");
            // delay(10);
            // delayMicroseconds(10);
            // Serial.println("--:afterDelay");
        }
        // Serial.println("--:ejected");
        return -1;
    }

    // TODO : call it
    bool yat4l_wifi_init() {
        WIFI_SERIAL.begin(WIFI_SERIAL_BAUDS);
        delay(300);

        long t0 = millis();
        Serial.println("Waiting for Serial2");
        while( !WIFI_SERIAL ) {
            delay(10);
            // delayMicroseconds(10);
            if ( millis() - t0 >= 1500 ) { return false; }
        }

        // char line[512+1];
        // while( _wifiReadline(line, 500) > -1 ) {
        //     Serial.print("=>");
        //     Serial.println(line);
        // }
        Serial.println("Check for garbage");
        while(WIFI_SERIAL.available() > 0) {
            WIFI_SERIAL.read();
        }
        Serial.println("Found some garbage");

        delay(300);

        Serial.println("Reset Module");
        yat4l_wifi_resetModule(); 
        
        // Serial.println("Test for Module");
        // bool ok = yat4l_wifi_testModule();
        // Serial.print("Tested Module : "); 
        // Serial.println(ok ? "OK" : "NOK"); 


Serial.println("Have finished !!!");

        bool ok = true;
        return ok;
    }

    bool yat4l_wifi_testModule() { 
        _wifiSendCMD("AT"); 
        return _wifi_waitForOk() == _RET_OK;
    }

    bool yat4l_wifi_resetModule() { 
        _wifiSendCMD("AT+RST"); 

Serial.println("======= 2nd pass =======");

delay(300);
        WIFI_SERIAL.begin(WIFI_SERIAL_BAUDS);
        delay(300);

        long t0 = millis();
        Serial.println("Waiting for Serial2");
        while( !WIFI_SERIAL ) {
            delay(10);
            if ( millis() - t0 >= 1500 ) { return false; }
        }

        Serial.println("Check for garbage");

while(WIFI_SERIAL.available() == 0) {
    delay(50);
}

        while(WIFI_SERIAL.available() > 0) {
            int ch = WIFI_SERIAL.read();
            Serial.write(ch);
        }
        Serial.println("Found some garbage");

        delay(300);


// delay(700);
// // WIFI_SERIAL.begin(WIFI_SERIAL_BAUDS);
// // while( !WIFI_SERIAL ) {
// //     ; // beware w/ timeout
// // }

// // delay(700);

// while(WIFI_SERIAL.available() > 0) {
//            int ch = WIFI_SERIAL.read();

//            Serial.write( ch );
//         }
//         Serial.println("module reseted");

// //         // return _wifi_waitForOk() == _RET_OK;    
// //         char line[512+1];
// //         int cpt = 0;
// //         while(true) {
// //             // char* line = _wifiReadline(500); // reads garbage
// //             int readed = _wifiReadline(line, 500); // reads garbage
// //             // equals(line, "ready") -- ESP12 protocol
// //             // if ( line == NULL || equals(line, "ready") ) { break; }

// // if (readed < 0) {
// //     break;
// // } 

// //             if ( 
// //                  // readed < 0 || (readed == 0 && (cpt++) > 50) || 
// //                  ( strlen(line) == 0 && (cpt++) > 50) ||
// //                  equals(line, "ready") ) { break; }
// //         }
        return true;
    }

    char* yat4l_wifi_getIP() { return "0.0.0.0"; }
    char* yat4l_wifi_getSSID() { return "NotConnetcted"; }

    bool yat4l_wifi_close() { return true; }
    bool yat4l_wifi_beginAP() { return false; }
    bool yat4l_wifi_startTelnetd() { return false; }

    bool yat4l_wifi_loop() { ; }

    void yat4l_wifi_telnetd_broadcast(char ch)  { ; }
    int  yat4l_wifi_telnetd_available()  { return 0; }
    int  yat4l_wifi_telnetd_read() { return -1; }



    bool yat4l_wifi_setWifiMode(int mode) { return false; }
    int yat4l_wifi_getWifiMode() { return -1; }

    // Soft AP
    bool yat4l_wifi_openAnAP(char* ssid, char* psk) { return false; }

    // STA (client of an AP)
    bool yat4l_wifi_connectToAP(char* ssid, char* psk) { return false; }
    bool yat4l_wifi_disconnectFromAP() { return false; }
    // returns a 'ssid \n ssid \n ....'
    char* yat4l_wifi_scanAPs() { return (char*)""; }

    // return type is not yet certified, may use a packetHandler ....
    // ex. yat4l_wifi_wget("www.google.com", 80, "/search?q=esp8266" 
    // ex. yat4l_wifi_wget("$home", 8089, "/login?username=toto&pass=titi" 
    char* yat4l_wifi_wget(char* host, int port, char* query) {
        return NULL;
    }

    bool yat4l_wifi_isAtHome() { return false; }
    char* yat4l_wifi_getHomeServer() { return NULL; }


