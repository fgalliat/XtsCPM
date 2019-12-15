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

char* _wifi_getLocalHomeServer() {
    return NULL;
}

char* _wifi_getRemoteHomeServer() {
    return NULL;
}

void _wifi_refreshHomeConf() {
    memset( wifi_home_conf, 0x00, WIFI_HOME_CONF_LEN );
 
    char tmp[ WIFI_HOME_CONF_LEN ];
    int read = yat4l_fs_readTextFile( yat4l_fs_getAssetsFileEntry( (char*)"wifi.psk"), tmp, WIFI_HOME_CONF_LEN );
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

bool _wifi_savePSKs() {
    // TODO
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