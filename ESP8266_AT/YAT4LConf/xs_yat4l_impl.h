

void yat4l_dbug(const char* str) { Serial.println(str); }
void yat4l_dbug(char* str) { Serial.println(str); }




    // ===================================================================================
    //                                   FileSystem
    // ===================================================================================

    const int _fullyQualifiedFileNameSize = 1+5 + (8+1+3) + 1;
    char _assetEntry[ _fullyQualifiedFileNameSize ];

    // not ThreadSafe !
    char* yat4l_fs_getAssetsFileEntry(char* assetName) {
        if ( assetName == NULL || strlen(assetName) <= 0 ) { yat4l_dbug("NULL filename"); return NULL; }
        memset(_assetEntry, 0x00, _fullyQualifiedFileNameSize);

        if ( assetName[1] == ':' ) {
            // ex. "Y:IMG.PAK"
            // sprintf( _assetEntry, "/%c/0/%s", assetName[0], &assetName[2] );
            sprintf( _assetEntry, "%c/0/%s", assetName[0], &assetName[2] );
        } else {
            // sprintf( _assetEntry, "/Z/0/%s", &assetName[0] );
            sprintf( _assetEntry, "Z/0/%s", &assetName[0] );
        }

        return _assetEntry;
    }