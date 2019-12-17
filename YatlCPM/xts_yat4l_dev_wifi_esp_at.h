/**
 * Yat4l Wifi by ESP8266 (esp12) AT cmds Driver impl.
 * 
 * 
 * Xtase - fgalliat @Dec2019
 * 
 * 
 * LOOK AT : https://forum.pjrc.com/threads/27850-A-Guide-To-Using-ESP8266-With-TEENSY-3
 * NO MORE USES : https://github.com/bportaluri/WiFiEsp
 * 
 * 
 * STILL TODO
 *  connectToAp(SSID, ?PSK?) -> PSK : from file / from interactive KB read
 *  listAPs()
 *  getSSID()
 *  isAtHome()
 *  getHomeServerName()
 *  wget(server, port, query)
 * 
 * Cf z:/wifi.psk
 *  storeAP(ssid, psk)
 *  getAPpsk(ssid)
 *  setHome(ssid, local_home, remote_home)
 *  isAtHome()
 * 
 * Cf textFile(s)
 *  write(char* content)
 *  read() => char* => split(content, '\n', x)
 *  appendLine(char*) => appends line+"\n"
 * 
 * Cf input
 *  _kbreadLine() => char* (w/o CR/LF)
 *  may be based on _kbhit() + _getch() / _getche()
 * 
 * Make an interactive system
 *  prompt SSID, PSK => add it
 *  prompt SSID, PSK => change it
 */

// forwards
extern int _kbhit();
extern uint8_t _getch();
extern uint8_t _getche();

    #define WIFI_SERIAL Serial2
    #define WIFI_CMD_TIMEOUT 6000
    #define WIFI_SERIAL_BAUDS 115200 

    #define DBUG_WIFI 0

    // STA : wifi AP client
    // const int WIFI_MODE_STA = 1;
    // AP : wifi soft AP server
    // const int WIFI_MODE_AP = 2;

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
            // Serial.write(ch);
        }
        Serial.println("module powered");

        return true; 
    }

    void _wifiSendCMD(const char* cmd) {
        // add CRLF
        if (DBUG_WIFI) { Serial.print("WIFI >");Serial.println(cmd); }
        int tlen = strlen( cmd ) + 2;
        if ( Serial.availableForWrite() < tlen ) {
            Serial.println("NotEnoughtAvailableForWrite !!!!");
        }

        WIFI_SERIAL.print( cmd );
        WIFI_SERIAL.print( "\r\n" );
        WIFI_SERIAL.flush();

        yield();
        // Serial.println("Sent packet");
    }

    // removes CRLF
    // assumes that _line is 512+1 bytes allocated 
    int _wifiReadline(char* _line, unsigned int timeout=WIFI_CMD_TIMEOUT) {
        // Serial.println("::_wifiReadline()");

        memset(_line, 0x00, 512+1);
        // Serial.print("WIFI READ >");Serial.println(timeout);
        yield();

        unsigned long t0=millis();
        bool timReached = false;
        while (WIFI_SERIAL.available() <= 0) {
            if ( millis() - t0 >= timeout ) { timReached = true; break; }
            delay(10);
        }
        yield();

        if ( timReached ) { return -1; }

        int cpt = 0;
        int ch;
        // t0=millis();
        while (WIFI_SERIAL.available() > 0) {
            if ( millis() - t0 >= timeout ) { timReached = true; break; }

            ch = WIFI_SERIAL.read();
            if ( ch == -1 ) { timReached = true; break; }
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

    int _wifi_waitForOk(char* dest=NULL) {
        char resp[512+1];
        while (true) {
            int readed = _wifiReadline(resp);

            yield();

            if ( readed == -1 ) { Serial.println("TIMEOUT--"); return _RET_TIMEOUT; }
            if ( strlen( resp ) > 0 ) {
                if (DBUG_WIFI) { Serial.print("-->"); Serial.println(resp); }
                
                if ( equals(&resp[0], (char*)"OK") ) { 
                    // Serial.println("OK--"); 
                    return _RET_OK; 
                }
                if ( equals(&resp[0], (char*)"ERROR") ) { 
                    // Serial.println("ERROR--"); 
                    return _RET_ERROR; 
                }

                if ( dest != NULL ) {
                    // copy the last non-empty line
                    sprintf(dest, "%s", resp);
                }

            } else {
                // Serial.println("--:EMPTY");
            }
        }
        yield();
        return -1;
    }




    // TODO : call it
    bool yat4l_wifi_init() {
        unsigned long t0 = millis();
        if (DBUG_WIFI) { Serial.println("Waiting for Serial2"); }
        while(WIFI_SERIAL.available() > 0) {
            WIFI_SERIAL.read();
        }
        if (DBUG_WIFI) { Serial.println("Found some garbage"); }


        bool ok = false;
        // Serial.println("Reset Module");
        // yat4l_wifi_resetModule(); 
        
        if (DBUG_WIFI) { Serial.println("Test for Module"); }
        ok = yat4l_wifi_testModule();
        if (DBUG_WIFI) { Serial.print("Tested Module : "); 
        Serial.println(ok ? "OK" : "NOK"); }

        if (DBUG_WIFI) { Serial.println("set mode for Module"); }
        ok = yat4l_wifi_setWifiMode( WIFI_MODE_STA );
        if (DBUG_WIFI) { Serial.print("Module mode set : "); 
        Serial.println(ok ? "OK" : "NOK"); }

        int mode = yat4l_wifi_getWifiMode();
        if (DBUG_WIFI) { Serial.print("Module mode : "); 
        Serial.println(mode); }

        char* ssids = __WIFI_GET_KNWON_SSIDS();
        Serial.println("Configured APs ...");
        Serial.println(ssids);

        if ( ssids != NULL ) {
            Serial.println("Select your AP (1 to 9)");
            int ch = -1;
            while( _kbhit() <= 0 ) {
                delay(5);
            }
            ch = _getch(); // no echo

            while( _kbhit() > 0  ) {
                _getch(); // read trailling chars
            }

            if ( ch != -1 ) {
                ch = ch - '1';
                if ( ch >= 0 ) {
                    char ssid[32+1]; memset(ssid, 0x00, 32+1);
                    char* vol = str_split(ssids, '\n', ch);
                    sprintf(ssid, "%s", vol);
                    Serial.println("Connecting to AP ...");
                    Serial.println(ssid);
                    ok = yat4l_wifi_connectToAP(ssid);
                    // if (DBUG_WIFI) 
                    { Serial.print("Connected to AP : ");
                    Serial.println(ok ? "OK" : "NOK"); }
                }
            }
        }

        Serial.println("Try to GET / @Home Server...");
        char* ignored = yat4l_wifi_wget((char*)"$home", 8089, "/");


        if (DBUG_WIFI) { Serial.println("Have finished !!!"); }

        ok = true;
        return ok;
    }

    bool yat4l_wifi_testModule() { 
        _wifiSendCMD("AT"); 
        return _wifi_waitForOk() == _RET_OK;
    }

    bool yat4l_wifi_resetModule() { 
        _wifiSendCMD("AT+RST"); 

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
                if ( millis() - t0 > timOut ) {break;}
            }

            yield();

        }
        yield();

        Serial.println("Found some garbage");

        return true;
    }

    bool yat4l_wifi_setWifiMode(int mode) {
        char cmd[32];
        sprintf(cmd, "AT+CWMODE=%d", mode);
        _wifiSendCMD(cmd);
        return _wifi_waitForOk() == _RET_OK;
    }

    int yat4l_wifi_getWifiMode() { 
        _wifiSendCMD("AT+CWMODE?");
        char resp[128];
        bool ok = _wifi_waitForOk( resp ) == _RET_OK;
        int mode = -1;
        if ( ok && startsWith(resp, (char*)"+CWMODE:") ) {
            // Serial.println("Found a mode :");
            // Serial.println(resp);
            mode = atoi( &resp[8] );
        }
        return mode; 
    }

    bool isStaMode() {
      // TODO : better cf can be BOTH
      int wmode = yat4l_wifi_getWifiMode();
      return wmode == WIFI_MODE_STA ||
      false;
      // wmode == WIFI_MODE_STA + WIFI_MODE_STA;
    }

    char* NOIP = (char*)"x.x.x.x";
    char CURIP[16+1];

    char* XX_getIP(bool STA) {
      if ( STA ) { _wifiSendCMD("AT+CIPSTA?"); }
      else { _wifiSendCMD("AT+CIPAP?"); }

      memset(CURIP, 0x00, 16+1);

      // char resp[256+1]; memset(resp, 0x00, 256+1);
      //   _wifi_waitForOk( resp )    --->> 256 is enought
      char resp[512+1]; // _wifiReadline(resp); requires 512 bytes long

        bool found = false;
        while (!found) {
            int readed = _wifiReadline(resp);
            
            if (readed < 0) {
                break;
            }

            // when not connected seems to finish with "+" (no netmask)
            if ( equals( resp, "+" ) ) {
                // Serial.println( "EJECT II" );
                break;
            }

            if ( startsWith(resp, (char*)"+CIP") ) {
                // ip: / gateway: / netmask:
                if ( contains(resp, (char*)"ip:") ) {
                    // +CIPSTA:ip:"0.0.0.0"
                    char* subResult = str_split(resp, '"', 1);
                    if ( subResult == NULL ) {
                        sprintf(CURIP, "%s", NOIP);
                    } else {
                        sprintf(CURIP, "%s", subResult);
                        free(subResult);
                    }
                    found = true;
                    break;
                }
            }
        }

        // must not return an function-local pointer
      return CURIP;
    }


    // --------------------

    char* NOSSID = (char*)"NotConnected";
    char CURSSID[32+1];

    char* XX_getSSID(bool STA) {
      if ( STA ) { _wifiSendCMD("AT+CWJAP?"); }
      else { _wifiSendCMD("AT+CWSAP?"); }

      memset(CURSSID, 0x00, 32+1);

      char resp[512+1]; // _wifiReadline(resp); requires 512 bytes long

        bool found = false;
        while (!found) {
            int readed = _wifiReadline(resp);
            
            if (readed < 0) {
                break;
            }

            // when not connected seems to finish with "+" (no netmask)
            if ( equals( resp, "+" ) ) {
                // Serial.println( "EJECT II" );
                break;
            }

            if ( equals( resp, "OK" ) ) {
                break;
            }

            if ( equals( resp, "ERROR" ) ) {
                return NOSSID;
            }

            if ( startsWith(resp, (char*)"+CW") ) {
                    // +CWJAP:__?__"MySSID"
                    char* subResult = str_split(resp, '"', 1);
                    if ( subResult == NULL ) {
                        sprintf(CURSSID, "%s", NOIP);
                    } else {
                        sprintf(CURSSID, "%s", subResult);
                        free(subResult);
                    }
                    found = true;
                    break;
            }
        }

        // must not return an function-local pointer
      return CURSSID;
    }

    // --------------------

    char* yat4l_wifi_getIP() { return XX_getIP( isStaMode() ); }
    char* yat4l_wifi_getSSID() { return XX_getSSID( isStaMode() ); }

    // --------------------

    bool yat4l_wifi_close() { return true; }
    bool yat4l_wifi_beginAP() { return false; }
    bool yat4l_wifi_startTelnetd() { return false; }

    bool yat4l_wifi_loop() { return false; }

    void yat4l_wifi_telnetd_broadcast(char ch)  { ; }
    int  yat4l_wifi_telnetd_available()  { return 0; }
    int  yat4l_wifi_telnetd_read() { return -1; }


    // Soft AP
    bool yat4l_wifi_openAnAP(char* ssid, char* psk) { return false; }

    // STA (client of an AP)
    bool yat4l_wifi_connectToAP(char* ssid, char* psk) { 
        if ( psk == NULL ) {
            psk = __WIFI_GET_PSK(ssid);
        }

        if ( psk == NULL ) {
            yat4l_dbug("No PSK provided for that SSID");
            return false;
        }

        char cmd[96+1]; memset(cmd, 0x00, 96+1);
        sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, psk);
        _wifiSendCMD(cmd);

        return _wifi_waitForOk() == _RET_OK;
    }

    bool yat4l_wifi_disconnectFromAP() { return false; }

    // returns a 'ssid \n ssid \n ....'
    char* yat4l_wifi_scanAPs() { return (char*)""; }


    void yat4l_wifi_closeSocket() {
        _wifiSendCMD("AT+CIPCLOSE()");
        if ( ! _wifi_waitForOk() ) { return false; }
        return true;
    }

    // return type is not yet certified, may use a packetHandler ....
    // ex. yat4l_wifi_wget("www.google.com", 80, "/search?q=esp8266" 
    // ex. yat4l_wifi_wget("$home", 8089, "/login?username=toto&pass=titi" 
    char* yat4l_wifi_wget(char* host, int port, char* query) {

      char* usedHOST = host;

      if ( equals(host, (char*)"$home") ) {
        char* homeSrv = yat4l_wifi_getHomeServer();
        if ( homeSrv != NULL ) {
            usedHOST = homeSrv;
        }          
      }

      char cmd[128];
      sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d", usedHOST, port);
      Serial.println(cmd);

      _wifiSendCMD(cmd);
      if (! _wifi_waitForOk() ) {
          yat4l_wifi_closeSocket();
          return NULL;
      }
      Serial.println("OK");

      char resp[512+1]; // _wifiReadline(resp); requires 512 bytes long
      char fullQ[128];
      sprintf( fullQ, "GET %s\r\n", query );

      sprintf(cmd, "AT+CIPSEND=%d", strlen( fullQ ));
      _wifiSendCMD( cmd );
      _wifiReadline(resp);
      Serial.println(cmd);

      _wifiSendCMD( fullQ );
      _wifiReadline(resp);
      Serial.println(fullQ);

      _wifiSendCMD("+++"); // EOT
      _wifiReadline(resp);
      Serial.println("+++");


        bool found = false;
        while (!found) {
            int readed = _wifiReadline(resp);
            
            if (readed < 0) {
                yat4l_wifi_closeSocket();
                return NULL;
            }

            if ( equals( resp, "ERROR" ) ) {
                yat4l_wifi_closeSocket();
                return NULL;
            } else if ( equals( resp, "SEND OK" ) ) {
                break;
            }

            Serial.print("SEND>");
            Serial.println(resp);
        }
        Serial.println("SEND OK");


        found = false;
        while (!found) {
            int readed = _wifiReadline(resp);
            
            if (readed < 0) {
                yat4l_wifi_closeSocket();
                return NULL;
            }

            if ( equals( resp, "CLOSED" ) ) {
                yat4l_wifi_closeSocket();
                break;
            } else if ( endsWith( resp, "CLOSED" ) ) {
                Serial.println( resp );
                break;
            }

            Serial.println( resp );
        }
        Serial.println("READ OK");


        yat4l_wifi_closeSocket();


        // must not return an function-local pointer
      return NULL;
    }


    // // return type is not yet certified, may use a packetHandler ....
    // // ex. yat4l_wifi_wget("www.google.com", 80, "/search?q=esp8266" 
    // // ex. yat4l_wifi_wget("$home", 8089, "/login?username=toto&pass=titi" 
    // char* yat4l_wifi_wget(char* host, int port, char* query) {
    //     return NULL;
    // }

    // bool yat4l_wifi_isAtHome() { return false; }
    // char* yat4l_wifi_getHomeServer() { return NULL; }


