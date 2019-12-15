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

char* _wifi_getLocalHomeServer() {
    return NULL;
}

char* _wifi_getRemoteHomeServer() {
    return NULL;
}

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

char* yat4l_wifi_getSSID() {
    return (char*)"xxxxxx";
    // return (char*)"xxxxxy";
}

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

bool yat4l_wifi_isAtHome(bool refresh=false) {
    char* curSSID = yat4l_wifi_getSSID();
    return curSSID != NULL && equals( curSSID, yat4l_wifi_getHomeSSID() );
}


char homeSRV[32];
char* yat4l_wifi_getHomeServer(bool refresh=false) {
    if ( refresh || !wifi_home_conf_init ) { _wifi_refreshHomeConf(); }

    int confNum = yat4l_wifi_isAtHome() ? 1 : 2;
    char* tokn = str_split(wifi_home_conf, ':', confNum);
    sprintf( homeSRV, "%s", tokn );
    free(tokn);
    return homeSRV;
}

bool _wifi_readConf() {
    int read = yat4l_fs_readTextFile( WIFI_CONF_FILE, fullWifiConf, WIFI_FULL_CONF_LEN );
    return read > -1;
}

bool _wifi_savePSKs() {
    _wifi_readConf();

    int tlen = strlen( fullWifiConf );

    if ( tlen == 0 ) {
        // nothing saved
        if ( !wifi_home_conf_init ) {
            // nothing to save
            // but no error
            return true;
        } else {
            sprintf( fullWifiConf, "%s\n", wifi_home_conf );
            yat4l_fs_writeTextFile( WIFI_CONF_FILE, fullWifiConf, strlen( fullWifiConf ) );
            return true;
        }
    }

    if ( wifi_home_conf_init ) {
        int i = indexOf(fullWifiConf, '\n');
        char tmp[WIFI_FULL_CONF_LEN]; memset(tmp, 0x00, WIFI_FULL_CONF_LEN);
        sprintf( tmp, "%s\n", wifi_home_conf );
        sprintf( &tmp[ strlen(tmp) ], "%s", &fullWifiConf[i+1] );
        sprintf( fullWifiConf, "%s", tmp );

        bool ok = yat4l_fs_writeTextFile( WIFI_CONF_FILE, fullWifiConf, strlen( fullWifiConf ) ) > -1;
        return ok;
    } else {
        bool ok = yat4l_fs_writeTextFile( WIFI_CONF_FILE, fullWifiConf, strlen( fullWifiConf ) ) > -1;
        return ok;
    }

    return false;
}

bool yat4l_wifi_setHomeConfig(char* homeSSID, char* homeLocal, char* homeRemote) {
    // TODO : BEWARE 64+32+32
    sprintf(wifi_home_conf, "%s:%s:%s", homeSSID, homeLocal, homeRemote);
    wifi_home_conf_init = true;
   return _wifi_savePSKs();
}

bool yat4l_wifi_setNoHomeConfig() {
    return yat4l_wifi_setHomeConfig( (char*)"#####", (char*)"##.##.##.##", (char*)"##.####.###");
}

bool yat4l_wifi_addWifiPSK(char* ssid, char* psk) {
    if ( ssid == NULL || psk == NULL || strlen(ssid) == 0 ) { return false; }
    if ( strlen( fullWifiConf ) + 64 + 64 +1 >= WIFI_FULL_CONF_LEN ) { return false; }
    sprintf( &fullWifiConf[ strlen( fullWifiConf ) ], "%s:%s\n", ssid, psk );

    return _wifi_savePSKs();
}

// not to expose public
void __DBUG_WIFI_CONF() {
    Serial.print( fullWifiConf );
}

void __ERASE_WIFI_CONF() {
    Serial.println("Erasing Wifi Conf");
    SD.remove( WIFI_CONF_FILE );
    memset(fullWifiConf, 0x00, WIFI_FULL_CONF_LEN);
}

