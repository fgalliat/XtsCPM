/*************************
 * 
 * WiFi Software toolkit
 * 
 * 
 * Xtase - fgalliat @Dec 2019
 * 
 */

#define WIFI_HOME_CONF_LEN ( 64 + 32 + 32 )
char wifi_home_conf[WIFI_HOME_CONF_LEN];
bool wifi_home_conf_init = false;

#define WIFI_FULL_CONF_LEN 2048
char fullWifiConf[ WIFI_FULL_CONF_LEN ];

#define WIFI_CONF_FILE yat4l_fs_getAssetsFileEntry( (char*)"wifi.psk")

// char* _wifi_getLocalHomeServer() {
//     return NULL;
// }

// char* _wifi_getRemoteHomeServer() {
//     return NULL;
// }

void _wifi_refreshHomeConf() {
    memset( wifi_home_conf, 0x00, WIFI_HOME_CONF_LEN );
 
    char tmp[ WIFI_HOME_CONF_LEN ];
    int read = yat4l_fs_readTextFile( WIFI_CONF_FILE, tmp, WIFI_HOME_CONF_LEN );
    if ( read < 0 ) {
        Serial.println("Error could not read WiFi config !!!!");
        return;
    }

    for(int i=0; i < read; i++) {
        int ch = tmp[i];
        if ( ch == '\n' ) { break; }
        wifi_home_conf[i] = ch;
    }

    wifi_home_conf_init = true;
}

// ==========================================
// DUMMY MOCK

// char* yat4l_wifi_getSSID() {
//     return (char*)"xxxxxx";
//     // return (char*)"xxxxxy";
// }

// ==========================================

char homeSSID[32];
bool homeSSID_init = false;
char* yat4l_wifi_getHomeSSID(bool refresh=false) {
    if ( refresh || !homeSSID_init || !wifi_home_conf_init ) { 
        _wifi_refreshHomeConf(); 
        char* tokn = str_split(wifi_home_conf, ':', 0);
        sprintf( homeSSID, "%s", tokn );
        free(tokn);
        homeSSID_init = true;
    }
    return homeSSID;
}

bool yat4l_wifi_isAtHome(bool refresh/*=false*/) {
    char* curSSID = yat4l_wifi_getSSID();
    return curSSID != NULL && equals( curSSID, yat4l_wifi_getHomeSSID() );
}


char homeSRV[32];
char* yat4l_wifi_getHomeServer(bool refresh/*=false*/) {
    if ( refresh || !wifi_home_conf_init ) { _wifi_refreshHomeConf(); }

    int confNum = yat4l_wifi_isAtHome() ? 1 : 2;
    char* tokn = str_split(wifi_home_conf, ':', confNum);
    sprintf( homeSRV, "%s", tokn );
    free(tokn);
    return homeSRV;
}

bool _wifi_readConf() {
    int read = yat4l_fs_readTextFile( WIFI_CONF_FILE, fullWifiConf, WIFI_FULL_CONF_LEN );
    if (read > 0 && !wifi_home_conf_init) _wifi_refreshHomeConf(); 
    return read > -1;
}

bool _wifi_savePSKs() {
    _wifi_readConf();

    int tlen = strlen( fullWifiConf );

    if ( tlen == 0 ) {
        // nothing saved
        // Serial.println(" nothing saved");
        if ( !wifi_home_conf_init ) {
            // Serial.println(" nothing to save");
            // nothing to save
            // but no error
            return true;
        } else {
            // Serial.println(" save home conf : ");
            // Serial.println(wifi_home_conf);
            sprintf( fullWifiConf, "%s\n", wifi_home_conf );
            bool ok = yat4l_fs_writeTextFile( WIFI_CONF_FILE, fullWifiConf, strlen( fullWifiConf ) );
            return ok;
        }
    }

    if ( wifi_home_conf_init && strlen(wifi_home_conf) > 0 ) {
        // Serial.println(" save home conf");
        // Serial.println(wifi_home_conf);

        int i = indexOf(fullWifiConf, '\n');

        // Serial.println(" save other conf");
        // Serial.println( &fullWifiConf[i+1] );
        // Serial.println("----------");

        char tmp[WIFI_FULL_CONF_LEN]; memset(tmp, 0x00, WIFI_FULL_CONF_LEN);
        sprintf( tmp, "%s\n%s", wifi_home_conf, &fullWifiConf[i+1] );
        // Serial.println(" saved other conf");
        memset( fullWifiConf, 0x00, WIFI_FULL_CONF_LEN);
        sprintf( fullWifiConf, "%s", tmp );

        // Serial.println("---------------");
        // Serial.println(fullWifiConf);
        // Serial.println("---------------");

        bool ok = yat4l_fs_writeTextFile( WIFI_CONF_FILE, fullWifiConf, strlen( fullWifiConf ) ) > -1;
        return ok;
    } else {
        // Serial.println(" saved dummy conf");
        bool ok = yat4l_fs_writeTextFile( WIFI_CONF_FILE, fullWifiConf, strlen( fullWifiConf ) ) > -1;
        return ok;
    }

    return false;
}

bool yat4l_wifi_setHomeConfig(char* homeSSID, char* homeLocal, char* homeRemote) {
    // TODO : BEWARE 64+32+32
    memset( wifi_home_conf, 0x00, WIFI_HOME_CONF_LEN);
    sprintf(wifi_home_conf, "%s:%s:%s", homeSSID, homeLocal, homeRemote);

    // Serial.print(">>");
    // Serial.print(wifi_home_conf);
    // Serial.print("<<");

    wifi_home_conf_init = true;
   return _wifi_savePSKs();
}

bool yat4l_wifi_setNoHomeConfig() {
    return yat4l_wifi_setHomeConfig( (char*)"#####", (char*)"##.##.##.##", (char*)"##.####.###");
}

bool yat4l_wifi_addWifiPSK(char* ssid, char* psk) {
    _wifi_readConf();
    if ( ssid == NULL || psk == NULL || strlen(ssid) == 0 ) { return false; }
    if ( strlen( fullWifiConf ) + 64 + 64 +1 >= WIFI_FULL_CONF_LEN ) { return false; }
    sprintf( fullWifiConf, "%s%s:%s\n", fullWifiConf, ssid, psk );

        // Serial.println("---------------");
        // Serial.println(fullWifiConf);
        // Serial.println("---------------");

    // return _wifi_savePSKs();
    bool ok = yat4l_fs_writeTextFile( WIFI_CONF_FILE, fullWifiConf, strlen( fullWifiConf ) ) > -1;
    return ok;
}

// not to expose public
void __DBUG_WIFI_CONF() {
    _wifi_readConf();
    Serial.print( fullWifiConf );
}

void __ERASE_WIFI_CONF() {
    Serial.println("Erasing Wifi Conf");
    SD.remove( WIFI_CONF_FILE );
    memset(fullWifiConf, 0x00, WIFI_FULL_CONF_LEN);
}

// later => directly set on connectToAp(ssid, PSK=NULL)
char pskRes[32+1];
char* __WIFI_GET_PSK(char* ssid) {
    if ( ! _wifi_readConf() ) { return NULL; }

    memset(pskRes, 0x00, 32+1);
    int pskResCurs = 0;

    int tlen = strlen( fullWifiConf );
    if ( tlen < 1 ) { return NULL; }
    bool lineHome = true;
    int cpt = 0; 
    bool found = false; bool sfound = false; bool readPSK = false;
    for(int i=0; i < tlen; i++) {
        char ch = fullWifiConf[i];
        if ( lineHome && ch != '\n' ) { continue; }
        else if ( lineHome && ch == '\n' ) { lineHome = false; continue; }
        else {
            if ( ch == '\n' ) {
                if (found) return pskRes;
                sfound = false;
                readPSK = false;
                cpt = 0;
            }
            else {
                if ( readPSK ) {
                  pskRes[ pskResCurs++ ] = ch;
                } else
                if (ch == ssid[cpt++]) {
                    sfound = true;
                } else if (ch != ssid[cpt++] && sfound) {
                    if ( ch == ':' ) {
                        readPSK = true;
                        found = true;
                    } else {
                        sfound = false;
                    }
                }
            }
        }
    }

    return NULL;
}


#define known_ssidsLEN (20*32)+1 
char known_ssids[known_ssidsLEN];
char* __WIFI_GET_KNWON_SSIDS() {
    if ( ! _wifi_readConf() ) { return NULL; }

    memset(known_ssids, 0x00, known_ssidsLEN);
    int ssidResCurs = 0;

    int tlen = strlen( fullWifiConf );
    if ( tlen < 1 ) { return NULL; }
    bool lineHome = true;
    int sfound = -1; bool readPSK = false;
    for(int i=0; i < tlen; i++) {
        char ch = fullWifiConf[i];
        if ( lineHome && ch != '\n' ) { continue; }
        else if ( lineHome && ch == '\n' ) { lineHome = false; continue; }
        else {
            if ( ch == '\n' ) {
                known_ssids[sfound++] = '\n';
                // sfound = -1;
                readPSK = false;
            }
            else {
                if ( !readPSK && sfound < 0 ) { sfound = 0; }
                if ( readPSK ) {
                    // nothing
                } else
                if (ch != ':') {
                    known_ssids[sfound++] = ch;
                } else if (ch == ':' && sfound > -1) {
                    if ( ch == ':' ) {
                        readPSK = true;
                    }
                }
            }
        }
    }

    return known_ssids;
}

bool yat4l_wifi_connectToAP(int conf) {
    if ( conf < -1 || conf > 99 ) { return false; }
    char* ssids = __WIFI_GET_KNWON_SSIDS();
    if ( ssids == NULL ) { return false; }
    char* ssid = str_split( ssids, '\n', conf );
    if ( ssid == NULL ) { return false; }
    return yat4l_wifi_connectToAP( ssid );
}