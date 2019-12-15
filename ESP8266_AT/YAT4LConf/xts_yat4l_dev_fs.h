/**
 * 
 * YAT4L FileSystem routines impl.
 * 
 * Xtase - fgalliat @Dec 2019
 */

int yat4l_fs_readTextFile(char* fileName, char* dest, int maxLen) {
    if ( fileName == NULL || dest == NULL || maxLen < 0 ) {
        return -1;
    }

    memset(dest, 0x00, maxLen);

    if ( maxLen == 0 ) { return 0; }

    File f = SD.open( fileName, O_READ );
    if ( !f ) {
        return -1;
    }

    int cpt=0, ch;
    while(true) {
        ch = f.read();
        if ( ch == -1 ) { break; }
        dest[cpt] = ch;
        cpt++;
        if ( cpt >= maxLen ) { break; }
    }

    f.close();

return cpt;
}

// stops on 0x00 (because of TEXTFile)
int yat4l_fs_writeTextFile(char* fileName, char* source, int maxLen) {
    if ( fileName == NULL || source == NULL || maxLen < 0 ) {
        return -1;
    }

    if ( maxLen == 0 ) { return 0; }

    File f = SD.open( fileName, O_CREAT | O_WRITE );
    if ( !f ) {
        return -1;
    }
    // erase file content
    f.truncate(0);

    int ch,i;
    for(i=0; i < maxLen; i++) {
        ch = source[i];
        if ( ch <= 0 ) { break; }
        f.write(ch);
        if ( i % 32 == 0 ) { f.flush(); }
    }
    f.flush();

    f.close();

return i;
}


