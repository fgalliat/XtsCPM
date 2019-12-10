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

    // removes CRLF
    // assumes that _line is 512+1 bytes allocated 
    int _wifiReadline(char* _line, unsigned int timeout=WIFI_CMD_TIMEOUT) {
        // Serial.println("::_wifiReadline()");

        memset(_line, 0x00, 512+1);
        Serial.print("WIFI READ >");Serial.println(timeout);
        yield();

        unsigned long t0=millis();
        bool timReached = false;
Serial.println("aa");
        while( !WIFI_SERIAL ) {
            if ( millis() - t0 >= timeout ) { timReached = true; break; }
        }
Serial.println("bb");
        if ( timReached ) { return -1; }
        while (WIFI_SERIAL.available() <= 0) {
            if ( millis() - t0 >= timeout ) { timReached = true; break; }
            delay(10);
        }
        yield();
Serial.println("cc");
        if ( timReached ) { return -1; }

        int cpt = 0;
        int ch;
        while (WIFI_SERIAL.available() > 0) {
Serial.print("+");
            if ( millis() - t0 >= timeout ) { timReached = true; break; }

            ch = WIFI_SERIAL.read();
            if ( ch == -1 ) { break; }
            if ( ch == '\r' ) { 
                if (WIFI_SERIAL.available() > 0) {
                    if ( WIFI_SERIAL.peek() == '\n' ) {
                        continue; 
                    }
                }
                break;
            }
            if ( ch == '\n' ) { break; }
            _line[ cpt++ ] = (char)ch;
        }
        yield();
Serial.println("dd");

        if ( _line[0] == 0x00 && timReached ) {
            return -1;
        }
        yield();

        if ( _line[0] == 0x00 ) { return 0; }

        int t = strlen(_line);
        if ( t < 0 ) { _line[0] = 0x00; return -1; }

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
        Serial.println("Reset Module");
        yat4l_wifi_resetModule(); 
        
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


