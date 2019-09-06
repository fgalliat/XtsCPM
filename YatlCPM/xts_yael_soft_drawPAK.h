/**
 
 drawPak routine

*/


// filename is "/Z/0/packXXX.pak"
// reads&display image #numInPak of packed image from filename
void drawImgFromPAK(char* filename, int x, int y, int numInPak) {
    if ( filename == NULL || strlen(filename) <= 0 || strlen(filename) >= 32 ) {
        Serial.println("(WW) Wrong PAK filename !");
        return;
    }

    File pakFile;

    drive_led(true);
    if (!(pakFile = SD.open(filename))) {
        Serial.println("PAK File not found");
        drive_led(false);
        return;
    }

    uint16_t w = ( pakFile.read() * 256 ) + pakFile.read();
    uint16_t h = ( pakFile.read() * 256 ) + pakFile.read();
    uint8_t nbImgs = pakFile.read();

    if ( x < 0 ) { x = (TFT_WIDTH-w)/2; }
    if ( y < 0 ) { y = (TFT_HEIGHT-h)/2; }

    if ( numInPak < 0 ) { numInPak=0; }
    if ( numInPak > nbImgs ) { numInPak=nbImgs-1; }

    pakFile.seek( numInPak * ( w*h*2 ) ); // beware : seems to be relative ? 
    // uint16_t scanLine[w];

    #define SCAN_ARRAY_HEIGHT 8
    uint16_t scanArray[w*SCAN_ARRAY_HEIGHT]; // 32KB bytes // 1KB for 2 rows


    for(int yy=0; yy < h; yy+=SCAN_ARRAY_HEIGHT) {

        int ct = pakFile.read( (uint8_t*)scanArray, SCAN_ARRAY_HEIGHT * w*2 ); // *2 cf U16
        if ( ct <= 0 ) { Serial.println("Oups EOF !"); break; }

#if not MODE_4INCH
        // Cf ESP32 isn't an ARM -> it's a RISC MCU
        for(int i=0; i < ct/2; i++) {
            int u80 = scanArray[ (i)+0 ] / 256;
            int u81 = scanArray[ (i)+0 ] % 256;
            scanArray[i] = (u81*256)+u80;            
        }
#endif

        int usedHeight = SCAN_ARRAY_HEIGHT;
        usedHeight = ct / 2 / w;
        tft.pushRect(x, yy+y, w, usedHeight, scanArray);

    } // for yy


    pakFile.close();
    drive_led(false);
}