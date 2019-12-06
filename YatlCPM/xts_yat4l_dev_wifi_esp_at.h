/**
 * Yat4l Wifi by ESP8266 (esp12) AT cmds Driver impl.
 * 
 * 
 * Xtase - fgalliat @Dec2019
 * 
 */
    bool yat4l_wifi_setup() { Serial2.begin(115200); return true; }

    char* yat4l_wifi_getIP() { return "0.0.0.0"; }
    char* yat4l_wifi_getSSID() { return "NotConnetcted"; }

    bool yat4l_wifi_close() { return true; }
    bool yat4l_wifi_beginAP() { return false; }
    bool yat4l_wifi_startTelnetd() { return false; }

    bool yat4l_wifi_loop() { ; }

    void yat4l_wifi_telnetd_broadcast(char ch)  { ; }
    int  yat4l_wifi_telnetd_available()  { return 0; }
    int  yat4l_wifi_telnetd_read() { return -1; }


    bool yat4l_wifi_testModule() { return false; }
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


