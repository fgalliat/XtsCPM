/**
 * Yat4l Wifi by ESP8266 (esp12) AT cmds Driver impl.
 * 
 * 
 * Xtase - fgalliat @Dec2019
 * 
 */

    #define WIFI_SERIAL Serial2
    #define WIFI_CMD_TIMEOUT 6000

    bool yat4l_wifi_setup() { WIFI_SERIAL.begin(115200); return true; }

    void _wifiSendCMD(const char* cmd) {
        // add CRLF
        Serial.print("WIFI >");Serial.println(cmd);
        char buff[128]; sprintf(buff, "%s\r\n", cmd);
        Serial.print("WIFI >>");Serial.println(buff);
        WIFI_SERIAL.write( buff, strlen( buff ) );
        // WIFI_SERIAL.flush();

        // delay(10);
        // yield();
        Serial.println("Sent packet");
    }

    char _line[512+1];
    char* _wifiReadline(int timeout=WIFI_CMD_TIMEOUT) {
        memset(_line, 0x00, 512+1);
        // removes CRLF
        Serial.print("WIFI READ >");Serial.println(timeout);
        // WIFI_SERIAL.setTimeout( timeout );

        long t0 = millis();
        while( WIFI_SERIAL.available() <= 0 ) {
            delay(10);
            // yield();
            if ( millis() - t0 >= timeout ) {
                Serial.println("EJECTED 2"); return NULL;
            }
        }


        Serial.println(".before");
        // int readed = WIFI_SERIAL.readBytesUntil('\n', _line, 512);
int cpt = 0;
while( WIFI_SERIAL.available() > 0 ) {
    if ( cpt >= 512 ) { break; }
    int ch0 = WIFI_SERIAL.read();
    if ( ch0 == -1 ) { break; }
    if ( (char)ch0 == '\n' ) { break; }

    _line[cpt] = (char)ch0;
    Serial.write( (char)ch0 );
    cpt++;
}
int readed = cpt;

        Serial.println(".after");
        // if (readed <= 0 ) { Serial.println("EJECTED"); return NULL; }
        if (readed < 0 ) { Serial.println("EJECTED"); return NULL; }
        int t = strlen(_line);
        if ( t > 0 && _line[t-1] == '\r' ) {
            _line[t-1] = 0x00;
        }
        Serial.print("WIFI READ <");Serial.println(_line);
        return _line;
    }

    #define _RET_TIMEOUT 0
    #define _RET_OK 1
    #define _RET_ERROR 2

    extern bool equals(char* s, char* t);

    int _wifi_waitForOk() {
        char* resp;
        while (true) {
            resp = _wifiReadline();
            if ( resp == NULL ) { return _RET_TIMEOUT; }
            if ( equals(resp, (char*)"OK") ) { return _RET_OK; }
            if ( equals(resp, (char*)"ERROR") ) { return _RET_ERROR; }
        }
        return -1;
    }

    // TODO : call it
    bool yat4l_wifi_init() { 
        _wifiReadline(500); // reads garbage
        return yat4l_wifi_testModule(); 
    }

    bool yat4l_wifi_testModule() { 
        _wifiSendCMD("AT"); 
        return _wifi_waitForOk() == _RET_OK;
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


    bool yat4l_wifi_resetModule() { return false; }

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


