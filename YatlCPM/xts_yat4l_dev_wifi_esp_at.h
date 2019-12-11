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

    bool yat4l_wifi_setup() { 
        WIFI_SERIAL.begin(WIFI_SERIAL_BAUDS); 

        unsigned long t0 = millis();
        unsigned long tmo = 1500;

        while( !WIFI_SERIAL ) {
            ; // TODO : timeout
            if ( millis() - t0 > tmo ) { break; }
        }

        while(WIFI_SERIAL.available() == 0) {
            delay(50);
            if ( millis() - t0 > tmo ) { break; }
        }

        while(WIFI_SERIAL.available() > 0) {
            int ch = WIFI_SERIAL.read();
            Serial.write(ch);
        }
        Serial.println("module powered");


        return true; 
    }

    void _wifiSendCMD(const char* cmd) {
// flushRX
while( WIFI_SERIAL.available() > 0) {
  int c = WIFI_SERIAL.read();
}

        // add CRLF
        Serial.print("WIFI >");Serial.println(cmd);
        int tlen = strlen( cmd ) + 2;
        if ( Serial.availableForWrite() < tlen ) {
            Serial.println("NotEnoughtAvailableForWrite !!!!");
        }

        WIFI_SERIAL.print( cmd );
        WIFI_SERIAL.print( "\r\n" );
        WIFI_SERIAL.flush();

        yield();
        Serial.println("Sent packet");
    }

    const int LINE_LEN = 1024;
    char LINE[LINE_LEN+1];
    bool LINE_INIT = false;
    unsigned long lasttm = -1;

int copyLineTo(char* dest, char* source) {
  int tmp = strlen(source);
  if ( tmp > 0 ) {
    int i=0;
    for(i=0; i < tmp; i++) {
        char ch = source[i];
        if ( i >= 512 ) { break; }
        if ( ch == '\r' ) { continue; }
        if ( ch == '\n' ) { break; }
        dest[i] = ch;
    }
    // memmove(&source[0], &source[i], (tmp-i) );
    // memmove(&source[0], &source[i], (tmp-i) );

    if ( source[i] == '\n' ) {
        i++;
    }
 
    for(int j=i; j < tmp; j++) {
        source[j-i] = source[j];
        source[j] = 0x00;
    }

// for(i=0;i<n-1;i++)
//     {
//         a[i]=a[i+1];
//     }

    // memcpy(&source[0], &source[i], (tmp-i) );
    // memset(&source[i], 0x00, (tmp-i));

    return strlen(dest);  
  }
  return tmp;
}


    // removes CRLF
    // assumes that _line is 512+1 bytes allocated 
    int _wifiReadline(char* _line, unsigned long timeout=WIFI_CMD_TIMEOUT) {
        // Serial.println("::_wifiReadline()");
if ( !LINE_INIT ) {
        memset(LINE, 0x00, LINE_LEN+1);
    LINE_INIT = true;
}

if ( lasttm != timeout ) {
    lasttm = timeout;
    WIFI_SERIAL.setTimeout(timeout);
}


        memset(_line, 0x00, 512+1);
        Serial.print("WIFI READ >");Serial.println(timeout);

int tmp = strlen(LINE);
if ( tmp > 0 ) {
    Serial.print("BUFFER not empty >> "); Serial.println(tmp);
    int i = copyLineTo(_line, LINE);
    Serial.print("Will return >> "); Serial.println(_line);
    return i;
} else {
    Serial.println("BUFFER empty");
    while( true ) {
        tmp = strlen(LINE);
        if ( tmp >= LINE_LEN ) { break; }
        int howMany = 32;
        if ( tmp + howMany >= LINE_LEN ) {
            howMany = ( LINE_LEN - tmp );
        }
        if ( howMany <= 0 ) { break; }
        int r = Serial2.readBytes(&LINE[tmp], howMany);
        if ( r == 0 ) {
            if ( tmp == 0 ) {
                Serial.println(")) FULLY FAILED");
                return -1;
            } else {
                Serial.println(")) PARTIAL FAILED");
                break;
            }
        }
        int tmp2 = strlen(LINE);
        bool foundLF = false;
        for(int i=tmp; i < tmp2; i++) {
            if ( LINE[i] == '\n' ) {
                foundLF = true;
                break;
            }
        } 
        if (foundLF) { break; }
    }

    tmp = strlen(LINE);
    int i = copyLineTo(_line, LINE);
    Serial.print("2) Will return >> "); Serial.println(_line);
    return i;
}


// // int r = Serial2.readBytesUntil( 0X0A, _line, 32 );
// // int r = Serial2.readBytes(_line, 32);
// if ( r == 0 ) { return -1; }
// else { 
//     int tt = strlen(_line);
//     if ( tt > 0 && _line[tt-1] == '\r' ) { _line[tt-1] = 0x00; tt--; }
//     return tt; 
// }

//         yield();

//         unsigned long t0=millis();
//         bool timReached = false;
// Serial.println("aa");
//         while( !WIFI_SERIAL ) {
//             if ( millis() - t0 >= timeout ) { timReached = true; break; }
//         }
// Serial.println("bb");
//         if ( timReached ) { Serial.println("EJECT"); return -1; }
//         // while (WIFI_SERIAL.available() <= 0) {
//         //     if ( millis() - t0 >= timeout ) { timReached = true; break; }
//         //     delay(10);
//         // }

// unsigned long t1;
// int ch_0;
// while( (ch_0 = WIFI_SERIAL.read()) == -1 ) {
// Serial.print("-");

// t1 = millis();
// Serial.print("/");

//     if ( t1 - t0 >= timeout ) { timReached = true; break; }
// }


// Serial.println("cc1");
//         yield();
// Serial.println("cc2");
//         if ( timReached ) { Serial.println("EJECT 4"); return -1; }

//         int cpt = 0;
//         int ch;
//         while (WIFI_SERIAL.available() > 0) {
// Serial.print("+");
//             if ( millis() - t0 >= timeout ) { Serial.println("EJECT 2"); timReached = true; break; }

//             ch = WIFI_SERIAL.read();
//             if ( ch == -1 ) { break; }
//             if ( ch == '\r' ) { 
//                 if (WIFI_SERIAL.available() > 0) {
//                     if ( WIFI_SERIAL.peek() == '\n' ) {
//                         continue; 
//                     }
//                 }
//                 break;
//             }
//             if ( ch == '\n' ) { break; }
//             _line[ cpt++ ] = (char)ch;
//         }
//         yield();
// Serial.println("dd");

//         if ( _line[0] == 0x00 && timReached ) {
//             Serial.println("EJECT 3");
//             return -1;
//         }
//         yield();

//         if ( _line[0] == 0x00 ) { return 0; }

//         int t = strlen(_line);
//         if ( t < 0 ) { _line[0] = 0x00; return -1; }

int t = -1;

        return t;
    }

    #define _RET_TIMEOUT 0
    #define _RET_OK 1
    #define _RET_ERROR 2

    extern bool equals(char* s, char* t);

    int _wifi_waitForOk() {
        char resp[512+1];
        while (true) {
            int readed = _wifiReadline(resp);

            yield();

            if ( readed == -1 ) { Serial.println("TIMEOUT--"); return _RET_TIMEOUT; }
            if ( strlen( resp ) > 0 ) {
                Serial.print("-->");
                Serial.println(resp);

                if ( equals(&resp[0], (char*)"OK") ) { Serial.println("OK--"); return _RET_OK; }
                if ( equals(&resp[0], (char*)"ERROR") ) { Serial.println("ERROR--"); return _RET_ERROR; }

            } else {
                Serial.println("--:EMPTY");
            }
        }
        yield();
        return -1;
    }

    // TODO : call it
    bool yat4l_wifi_init() {
        WIFI_SERIAL.begin(WIFI_SERIAL_BAUDS);
        delay(300);

        unsigned long t0 = millis();
        Serial.println("Waiting for Serial2");
        while( !WIFI_SERIAL ) {
            delay(10);
            // delayMicroseconds(10);
            if ( millis() - t0 >= 1500 ) { return false; }
        }

        Serial.println("Check for garbage");
        while(WIFI_SERIAL.available() > 0) {
            WIFI_SERIAL.read();
        }
        Serial.println("Found some garbage");

        delay(300);

        bool ok = false;
        // Serial.println("Reset Module");
        // yat4l_wifi_resetModule(); 
        
        Serial.println("Test for Module");
        ok = yat4l_wifi_testModule();
        Serial.print("Tested Module : "); 
        Serial.println(ok ? "OK" : "NOK"); 


        Serial.println("Have finished !!!");

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
        // WIFI_SERIAL.begin(WIFI_SERIAL_BAUDS);
        delay(300);

        unsigned long t0 = millis();
        Serial.println("Waiting for Serial2");
        while( !WIFI_SERIAL ) {
            delay(10);
            if ( millis() - t0 >= 1500 ) { return false; }
        }

        Serial.println("Check for garbage");

        t0 = millis();

        unsigned long timOut = 3500;

        yield();

        while( true ) {
            if ( millis() - t0 > timOut ) {break;}

            while(WIFI_SERIAL.available() == 0) {
            yield();
                delay(50);
                if ( millis() - t0 > timOut ) {break;}
            }

            yield();

            while(WIFI_SERIAL.available() > 0) {
                int ch = WIFI_SERIAL.read();
                Serial.write(ch);
            }

            yield();

        }
        yield();

        Serial.println("Found some garbage");

        return true;
    }

    char* yat4l_wifi_getIP() { return (char*)"0.0.0.0"; }
    char* yat4l_wifi_getSSID() { return (char*)"NotConnected"; }

    bool yat4l_wifi_close() { return true; }
    bool yat4l_wifi_beginAP() { return false; }
    bool yat4l_wifi_startTelnetd() { return false; }

    bool yat4l_wifi_loop() { return false; }

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


