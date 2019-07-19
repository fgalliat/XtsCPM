// just for devl purposes
// #define DESKTOP 1


#ifndef ARDUINO
 /**
 * Xtase - fgalliat @Jul2019
 * 
 * implementation of Yatl hardware API (Desktop Version)
 * 
 * 
 */

    #warning "-= Yatl API Desktop Mode =-"
    #ifdef ARDUINO
    #include "Arduino.h"
    #else 
    #include "Desktop.h"
    #endif

    uint16_t rgb(uint8_t r,uint8_t g,uint8_t b) { return (uint16_t)( (( r *31/255 )<<11) | (( g *63/255 )<<5) | ( b *31/255 ) );}

    // @@@@@@@@@@@@@@@@@@
    #include "../arch/desktop/screen/WiredScreen.h"
    WiredScreen sdlScreen;
    // @@@@@@@@@@@@@@@@@@

    #include "xts_yatl_settings.h"
    #include "xts_yatl_api.h"

    // ::::::::::::::::::::::::::::::::::::::
    bool keybLocked = false;

    void YatlBuzzer::beep(int freq, int dur) {
        printf("beep(x,y)\n");
    }

    void YatlBuzzer::setup() {
    }

    bool YatlScreen::setup() {
        return sdlScreen.init();
    }

    bool YatlKeyboard::setup() {
        return false;
    }

    void YatlScreen::cls() {
        sdlScreen.cls();
    }

    void YatlScreen::write(char ch) {
        char chs[2] = {ch, 0x00};
        // TODO : better
        sdlScreen.print(chs);
    }

    void YatlScreen::println(const char* str) {
        printf("'%s' \n", str);
        sdlScreen.println((char*)str);
    }

    void YatlScreen::println(char* str) {
        printf("'%s' \n", str);
        sdlScreen.println(str);
    }

    void YatlScreen::drawTextBox(char const* title, char const* text, unsigned short color) {
        printf("TXT BOX '%s' \n", text);
    }
    // ::::::::::::::::::::::::::::::::::::::


    // #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
    // extern SdFatSdio SD;

    extern Yatl yatl;
    YatlSubMCU _subMcu(&yatl);
    YatlBuzzer _buzzer(&yatl);
    YatlPWRManager _pwrManager(&yatl);
    YatlLEDs _leds(&yatl);
    YatlWiFi _wifi(&yatl);
    YatlMusicPlayer _mp3(&yatl);
    YatlFS _fs(&yatl);
    YatlKeyboard _keyb(&yatl);
    YatlScreen _screen(&yatl);

    Yatl::Yatl() {
        this->subMcu = &_subMcu;
        this->buzzer = &_buzzer;
        this->pwrManager = &_pwrManager;
        this->leds = &_leds;
        this->wifi = &_wifi;
        this->mp3 = &_mp3;
        this->fs = &_fs;
        this->keyb = &_keyb;
        this->screen = &_screen;
    }

    Yatl::~Yatl() {
    }

    bool screenReady = false;
    bool screenDBUG = true;

    bool Yatl::setup() {
        // Serial.begin(115200);
        this->dbug("Yatl setuping ...");

        pinMode( BUILTIN_LED, OUTPUT );
        this->led(false);

        this->buzzer->setup();

        bool ok;
        ok = this->getScreen()->setup();
        screenReady = ok;
        if (!ok) { this->warn("Screen not ready !"); }

        ok = this->getFS()->setup();
        if (!ok) { this->warn("SD not ready !"); }

        ok = this->subMcu->setup();
        if (!ok) { this->warn("SubMCU Setup Failed"); }

        ok = this->keyb->setup();
        if (!ok) { this->warn("Keyb Setup Failed"); }

        this->delay(200);

        return true;
    }

    void Yatl::dbug(const char* str) { 
        // Serial.println(str); 
        printf( "%s\n", str );

        if ( screenDBUG && screenReady ) {
            this->getScreen()->println( str );
        }
    }
    void Yatl::dbug(const char* str, int val) { 
        char msg[128+1]; sprintf(msg, "%s %d", str, val);
        // Serial.println(msg); 
        printf( "%s\n", str );

        if ( screenDBUG && screenReady ) {
            this->getScreen()->println( msg );
        }
    }
    void Yatl::dbug(const char* str, float val) { 
        char msg[128+1]; sprintf(msg, "%s %g", str, val);
        //Serial.println(msg); 
        printf( "%s\n", str );

        if ( screenDBUG && screenReady ) {
            this->getScreen()->println( msg );
        }
    }

    void Yatl::delay(long time) {
        ::delay(time);
    }

    void Yatl::beep() {
        this->getBuzzer()->beep(440, 100);
    }

    void Yatl::led(bool state) {
        digitalWrite(BUILTIN_LED, state ? HIGH : LOW);
    }

    void Yatl::blink(int times) {
        this->led(false);
        for(int i=0; i < times; i++) {
            this->led(true);
            this->delay(200);
            this->led(false);
            this->delay(200);
        }
        this->led(false);
    }

    void Yatl::alert(const char* msg) {
        if ( screenReady ) {
            this->getScreen()->drawTextBox( "ALERT", msg, 2 ); // red
        } else {
            this->blink(5);
            printf("*****************************\n");
            printf("*  ALERT                    *\n");
            printf("* \n");
            printf("%s\n",msg);
            printf("*****************************\n");
        }
        this->buzzer->beep(440, 200);
    }
    void Yatl::warn(const char* msg) {
        if ( screenReady ) {
            this->getScreen()->drawTextBox( "WARNING", msg, 7 ); // cyan
        } else {
            this->blink(5);
            printf("*****************************\n");
            printf("*  WARNING                  *\n");
            printf("* ");
            printf("%s\n",msg);
            printf("*****************************\n");
        }
    }

    // ===============] FileSystem [===========
    // ex. MONKEY.T5K -> /Z/0/MONKEY.T5K
    // spe DISK for assets : "Z:"
    // 'cause CP/M supports ONLY up to P:

    const int _fullyQualifiedFileNameSize = 5 + (8+1+3) + 1;
    char _assetEntry[ _fullyQualifiedFileNameSize ];

    // SdFatSdio SD;

    bool YatlFS::setup() {
        // return SD.begin();
        return true;
    }

    // not ThreadSafe !
    char* YatlFS::getAssetsFileEntry(char* assetName) {
        memset(_assetEntry, 0x00, _fullyQualifiedFileNameSize);
        strcpy( _assetEntry, "/Z/0/" );
        strcat( _assetEntry, assetName );
        return _assetEntry;
    }

    bool YatlFS::downloadFromSerial() {
        // while( Serial.available() ) { Serial.read(); delay(2); }
        // this->yatl->warn("Download in progress");
        // Serial.println("+OK");
        // while( !Serial.available() ) { delay(2); }
        // // for now : file has to be like "/C/0/XTSDEMO.PAS"
        // int tlen = 0;
        // char txt[128+1]; 
        // char name[64+1]; memset(name, 0x00, 64); tlen = Serial.readBytesUntil(0x0A, name, 64);
        // if ( tlen <= 0 ) {
        //     sprintf(txt, "Downloading %s (error)", name);
        //     this->yatl->warn((const char*)txt);
        //     Serial.println("Download not ready");
        //     Serial.println(name);
        //     Serial.println("-OK");
        //     return false;
        // }

        // // Cf CPM may padd the original file
        // File f = SD.open(name, O_CREAT | O_WRITE);
        // if ( !f ) {
        //   Serial.println("-OK");
        //   return false;    
        // }
        // f.remove();
        // f.close();
        // // Cf CPM may padd the original file

        // f = SD.open(name, O_CREAT | O_WRITE);
        // if ( !f ) {
        //   Serial.println("-OK");
        //   return false;    
        // }

        // Serial.println("+OK");
        // while( !Serial.available() ) { delay(2); }
        // char sizeStr[12+1]; memset(sizeStr, 0x00, 12); tlen = Serial.readBytesUntil(0x0A, sizeStr, 12);
        // long size = atol(sizeStr);
        // sprintf(txt, "Downloading %s (%ld bytes)", name, size);
        // this->yatl->warn((const char*)txt);
        // char packet[128+1];
        // Serial.println("+OK");
        // for(int readed=0; readed < size;) {
        //     while( !Serial.available() ) { delay(2); }
        //     int packetLen = Serial.readBytes( packet, 128 );
        //     f.write(packet, packetLen);
        //     f.flush();
        //     readed += packetLen;
        // }
        // f.close();
        // this->yatl->warn("-EOF-");
        // this->yatl->beep();
        // return true;
        return false;
    }

    bool YatlFS::downloadFromSubMcu() {
//         this->yatl->getSubMCU()->lock();
//         this->yatl->getSubMCU()->cleanBuffer();


//         while( this->yatl->getSubMCU()->available() ) { this->yatl->getSubMCU()->read(); delay(2); }
//         this->yatl->warn("Download in progress");
//         this->yatl->getSubMCU()->println("+OK");
// Serial.println("+OK");
//         // while( !this->yatl->getSubMCU()->available() ) { delay(2); }
//         // while( this->yatl->getSubMCU()->available() ) { this->yatl->getSubMCU()->read(); delay(2); }

//         // for now : file has to be like "/C/0/XTSDEMO.PAS"
//         int tlen = 0;
//         char txt[128+1]; 
//         // while( !this->yatl->getSubMCU()->available() ) { delay(2); }
//         char name[64+1]; memset(name, 0x00, 64); tlen = this->yatl->getSubMCU()->readBytesUntil(0x0A, name, 64);
// Serial.print("Name : ");
// Serial.println(name);
//         if ( tlen <= 0 ) {
//             sprintf(txt, "Downloading %s (error)", name);
//             this->yatl->warn((const char*)txt);
//             // Serial.println("Download not ready");
//             // Serial.println(name);
//             // Serial.println("-OK");
//             this->yatl->getSubMCU()->unlock();
//             return false;
//         }

//         sprintf(txt, "Downloading %s (0x01)", name);
//         this->yatl->warn((const char*)txt);

//         // Cf CPM may padd the original file
//         File f = SD.open(name, O_CREAT | O_WRITE);
//         if ( !f ) {
//           this->yatl->getSubMCU()->println("-OK");
//           this->yatl->getSubMCU()->unlock();
//           return false;    
//         }
//         f.remove();
//         f.close();
//         // Cf CPM may padd the original file

//         f = SD.open(name, O_CREAT | O_WRITE);
//         if ( !f ) {
//             sprintf(txt, "Failed to open %s (0x02)", name);
//             this->yatl->warn((const char*)txt);
//             this->yatl->getSubMCU()->println("-OK");
//             this->yatl->getSubMCU()->unlock();
//           return false;    
//         }
// Serial.println("+OK");
//         this->yatl->getSubMCU()->println("+OK");
//         sprintf(txt, "wait for fileLen %s (0x03)", name);
//         this->yatl->warn((const char*)txt);
//         while( !this->yatl->getSubMCU()->available() ) { delay(2); }
//         char sizeStr[12+1]; memset(sizeStr, 0x00, 12); tlen = this->yatl->getSubMCU()->readBytesUntil(0x0A, sizeStr, 12);
// Serial.print("Size : ");
// Serial.println(sizeStr);
//         long size = atol(sizeStr);
//         sprintf(txt, "Downloading %s (%ld bytes)", name, size);
//         this->yatl->warn((const char*)txt);
//         char packet[128+1];
// Serial.println("+OK");
//         this->yatl->getSubMCU()->println("+OK");
//         for(int readed=0; readed < size;) {
//             while( !this->yatl->getSubMCU()->available() ) { delay(2); }
//             int packetLen = this->yatl->getSubMCU()->readBytes( packet, 128 );
//             f.write(packet, packetLen);
//             f.flush();
//             readed += packetLen;
//         }
//         f.close();
// Serial.println("-EOF-");
//         this->yatl->getSubMCU()->unlock();
//         this->yatl->warn("-EOF-");
//         this->yatl->beep();
//         return true;
        return false;
    }

    // ===============] SubMCU [===============
    bool YatlSubMCU::setup() {
        // BRIDGE_MCU_SERIAL.begin( BRIDGE_MCU_BAUDS );

        // send 'i' wait for response ....
        // will see later Cf synchro.

        return true;
    }

    void YatlSubMCU::reboot(bool waitFor) {
        // this->send('r');
        if (waitFor) { delay(1000); }
    }

    void YatlSubMCU::flush() { /*BRIDGE_MCU_SERIAL.flush(); */ }
    void YatlSubMCU::send(char ch) { /* BRIDGE_MCU_SERIAL.write( ch ); */ }
    void YatlSubMCU::send(const char* str) { /* BRIDGE_MCU_SERIAL.print( str ); */ }
    void YatlSubMCU::send(char* str) { /* BRIDGE_MCU_SERIAL.print( str ); */ }

    void YatlSubMCU::println(const char* str) { /* BRIDGE_MCU_SERIAL.println( str ); */ }

    extern bool keybLocked;

    void YatlSubMCU::lock() {
        keybLocked = true;
    }

    void YatlSubMCU::unlock() {
        keybLocked = false;
    }

    int YatlSubMCU::available() { return 0;/*BRIDGE_MCU_SERIAL.available(); */ }
    int YatlSubMCU::read() { 
        // int ret = BRIDGE_MCU_SERIAL.read(); 
        int ret = -1;
        return ret;
    }

    int YatlSubMCU::readUntil(uint8_t until, char* dest, int maxLen) {
    //   int readed = BRIDGE_MCU_SERIAL.readBytesUntil((char)until, dest, maxLen);
    int readed = 0;
      return readed;
    }

    int YatlSubMCU::readBytes(char* dest, int maxLen) {
    //   int readed = BRIDGE_MCU_SERIAL.readBytes(dest, maxLen);
    int readed = 0;
      return readed;
    }

    #define MAX_SUBMCU_LINE_LEN 255
    char lastSubMCULine[MAX_SUBMCU_LINE_LEN+1];
    char* YatlSubMCU::readLine() {
        memset(lastSubMCULine, 0x00, MAX_SUBMCU_LINE_LEN+1);
        // BRIDGE_MCU_SERIAL.readBytesUntil( '\n', lastSubMCULine, MAX_SUBMCU_LINE_LEN );
        return lastSubMCULine;
    }

    char* YatlSubMCU::waitForNonEmptyLine(unsigned long timeout) {
//         if ( timeout == 0 ) { timeout = 10*1000; } // 10sec
//         unsigned long began = millis();

memset(lastSubMCULine, 0x00, MAX_SUBMCU_LINE_LEN+1);
  uint8_t idx = 0;
//   char c;
//   do
//   {
//     while (BRIDGE_MCU_SERIAL.available() == 0) {
//         delay(1);
//         if (millis()-began >= timeout) { keybLocked = false; return lastSubMCULine; }
//     } // wait for a char this causes the blocking
//     c = BRIDGE_MCU_SERIAL.read();
//     lastSubMCULine[idx++] = c;

//     if (millis()-began >= timeout) { keybLocked = false; return lastSubMCULine; }
//   }
//   while (c != '\n' && c != '\r'); 
  lastSubMCULine[idx] = 0;

       return lastSubMCULine;
    }

    void YatlSubMCU::cleanBuffer() {
        while( this->available() ) { this->read(); }
    }

    // ===============] PWR [=================

    float YatlPWRManager::getVoltage() {
        // this->yatl->getSubMCU()->send('v'); this->yatl->getSubMCU()->flush();
        // float volt = -1.0f;
        // volt = atof( this->yatl->getSubMCU()->waitForNonEmptyLine() );
        float volt = 3.0f;
        return volt;
    }

    #define RESTART_ADDR 0xE000ED0C
    #define READ_RESTART() (*(volatile uint32_t *)RESTART_ADDR)
    #define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))
    void softReset() {
        // WRITE_RESTART(0x5FA0004);
        printf("Reset to Impl. \n");
    }


    // wholeSystem -> BOTH Main & SubMCU
    void YatlPWRManager::reset(bool wholeSystem) { 
        this->yatl->warn("Rebooting !");
        this->yatl->getSubMCU()->reboot(!false);
        softReset();
    }

    void YatlPWRManager::deepSleep(bool wholeSystem) { /*TODO*/ this->yatl->warn("DEEPSLEEP NYI"); this->yatl->getSubMCU()->send("h"); }

    // ==============] LEDs [==================

    void YatlLEDs::red(bool state) { this->yatl->getSubMCU()->send( state ? "l1xx" : "l0xx" ); }
    void YatlLEDs::green(bool state) { this->yatl->getSubMCU()->send( state ? "lx1x" : "lx0x" ); }
    void YatlLEDs::blue(bool state) { this->yatl->getSubMCU()->send( state ? "lxx1" : "lxx0" ); }
    void YatlLEDs::builtin(bool state) { this->yatl->led(state); }

    // ==============] WiFi [==================

    bool YatlWiFi::beginSTA() { 
        this->yatl->getSubMCU()->cleanBuffer();
        this->yatl->getSubMCU()->send("wcs");
        char* resp = this->yatl->getSubMCU()->waitForNonEmptyLine();
        ::delay(200);
        // Serial.println("----------->");
        // Serial.println(resp);
        // Serial.println("----------->");
        return strlen(resp) > 0 && resp[0] == '+';
    }
    bool YatlWiFi::beginAP() { 
        this->yatl->getSubMCU()->cleanBuffer();
        this->yatl->getSubMCU()->send("wca");
        char* resp = this->yatl->getSubMCU()->waitForNonEmptyLine();
        // Serial.print("-> WIFI : ");Serial.print((const char*)resp);Serial.print("\n");
        return strlen(resp) > 0 && resp[0] == '+';
    }

    void YatlWiFi::close() { 
        this->yatl->getSubMCU()->cleanBuffer();
        this->yatl->getSubMCU()->send("ws"); 
        this->yatl->getSubMCU()->waitForNonEmptyLine(); 
    }

    char* YatlWiFi::getIP() { this->yatl->getSubMCU()->send("wi"); return this->yatl->getSubMCU()->waitForNonEmptyLine(); }
    char* YatlWiFi::getSSID() { this->yatl->getSubMCU()->send("we"); return this->yatl->getSubMCU()->waitForNonEmptyLine(); }

    bool YatlWiFi::startTelnetd() {
        this->yatl->getSubMCU()->cleanBuffer();
        this->yatl->getSubMCU()->send("wto");
        char* resp = this->yatl->getSubMCU()->waitForNonEmptyLine();
        // Serial.print("-> TELNET : ");Serial.print((const char*)resp);Serial.print("\n");
        return strlen(resp) > 0 && resp[0] == '+';
    }
    bool YatlWiFi::stopTelnetd() {
        this->yatl->getSubMCU()->cleanBuffer();
        this->yatl->getSubMCU()->send("wto");
        char* resp = this->yatl->getSubMCU()->waitForNonEmptyLine();
        return true;
    }
    // char* YatlWiFi::wget(char* url);

    // ==============] MP3 [==================

    void YatlMusicPlayer::play(int trackNum) { 
        uint8_t d0 = trackNum / 256;
        uint8_t d1 = trackNum % 256;
        this->yatl->getSubMCU()->send("pp");
        this->yatl->getSubMCU()->send(d0);
        this->yatl->getSubMCU()->send(d1);
    }
    void YatlMusicPlayer::loop(int trackNum) {
        uint8_t d0 = trackNum / 256;
        uint8_t d1 = trackNum % 256;
        this->yatl->getSubMCU()->send("pl");
        this->yatl->getSubMCU()->send(d0);
        this->yatl->getSubMCU()->send(d1);
    }
    void YatlMusicPlayer::stop() { this->yatl->getSubMCU()->send("ps"); }
    void YatlMusicPlayer::pause() { this->yatl->getSubMCU()->send("pP"); }
    void YatlMusicPlayer::next() { this->yatl->getSubMCU()->send("pn"); }
    void YatlMusicPlayer::prev() { this->yatl->getSubMCU()->send("pv"); }
    void YatlMusicPlayer::volume(uint8_t vol) {
        this->yatl->getSubMCU()->send("pV");
        this->yatl->getSubMCU()->send(vol%256);
    }

#endif