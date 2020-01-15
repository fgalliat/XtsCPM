// just for devl purposes
// #define ARDUINO 1


#if defined ARDUINO && defined CORE_TEENSY && not defined __IMXRT1062__
 /**
 * Xtase - fgalliat @Jun2019
 * 
 * implementaion of Yatl hardware API (Arduino Version)
 * 
 * 
 */

    #warning "-= Yatl API Arduino Mode =-"
    #include "Arduino.h"

    #include "xts_yatl_settings.h"
    #include "xts_yatl_api.h"

    #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
    extern SdFatSdio SD;

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
        Serial.begin(115200);
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
        Serial.println(str); 
        if ( screenDBUG && screenReady ) {
            this->getScreen()->println( str );
        }
    }
    void Yatl::dbug(const char* str, int val) { 
        char msg[128+1]; sprintf(msg, "%s %d", str, val);
        Serial.println(msg); 
        if ( screenDBUG && screenReady ) {
            this->getScreen()->println( msg );
        }
    }
    void Yatl::dbug(const char* str, float val) { 
        char msg[128+1]; sprintf(msg, "%s %g", str, val);
        Serial.println(msg); 
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
            Serial.println("*****************************");
            Serial.println("*  ALERT                    *");
            Serial.print("* ");
            Serial.println(msg);
            Serial.println("*****************************");
        }
        this->buzzer->beep(440, 200);
    }
    void Yatl::warn(const char* msg) {
        if ( screenReady ) {
            this->getScreen()->drawTextBox( "WARNING", msg, 7 ); // cyan
        } else {
            this->blink(5);
            Serial.println("*****************************");
            Serial.println("*  WARNING                  *");
            Serial.print("* ");
            Serial.println(msg);
            Serial.println("*****************************");
        }
    }

    // ===============] FileSystem [===========
    // ex. MONKEY.T5K -> /Z/0/MONKEY.T5K
    // spe DISK for assets : "Z:"
    // 'cause CP/M supports ONLY up to P:

    const int _fullyQualifiedFileNameSize = 5 + (8+1+3) + 1;
    char _assetEntry[ _fullyQualifiedFileNameSize ];

    SdFatSdio SD;

    bool YatlFS::setup() {
        return SD.begin();
    }

    // not ThreadSafe !
    char* YatlFS::getAssetsFileEntry(char* assetName) {
        memset(_assetEntry, 0x00, _fullyQualifiedFileNameSize);
        strcpy( _assetEntry, "/Z/0/" );
        strcat( _assetEntry, assetName );
        return _assetEntry;
    }

    bool YatlFS::downloadFromSerial() {
        while( Serial.available() ) { Serial.read(); delay(2); }
        this->yatl->warn("Download in progress");
        Serial.println("+OK");
        while( !Serial.available() ) { delay(2); }
        // for now : file has to be like "/C/0/XTSDEMO.PAS"
        int tlen = 0;
        char txt[128+1]; 
        char name[64+1]; memset(name, 0x00, 64); tlen = Serial.readBytesUntil(0x0A, name, 64);
        if ( tlen <= 0 ) {
            sprintf(txt, "Downloading %s (error)", name);
            this->yatl->warn((const char*)txt);
            Serial.println("Download not ready");
            Serial.println(name);
            Serial.println("-OK");
            return false;
        }

        // Cf CPM may padd the original file
        File f = SD.open(name, O_CREAT | O_WRITE);
        if ( !f ) {
          Serial.println("-OK");
          return false;    
        }
        f.remove();
        f.close();
        // Cf CPM may padd the original file

        f = SD.open(name, O_CREAT | O_WRITE);
        if ( !f ) {
          Serial.println("-OK");
          return false;    
        }

        Serial.println("+OK");
        while( !Serial.available() ) { delay(2); }
        char sizeStr[12+1]; memset(sizeStr, 0x00, 12); tlen = Serial.readBytesUntil(0x0A, sizeStr, 12);
        long size = atol(sizeStr);
        sprintf(txt, "Downloading %s (%ld bytes)", name, size);
        this->yatl->warn((const char*)txt);
        char packet[128+1];
        Serial.println("+OK");
        for(int readed=0; readed < size;) {
            while( !Serial.available() ) { delay(2); }
            int packetLen = Serial.readBytes( packet, 128 );
            f.write(packet, packetLen);
            f.flush();
            readed += packetLen;
        }
        f.close();
        this->yatl->warn("-EOF-");
        this->yatl->beep();
        return true;
    }

    bool YatlFS::downloadFromSubMcu() {
        this->yatl->getSubMCU()->lock();
        this->yatl->getSubMCU()->cleanBuffer();


        while( this->yatl->getSubMCU()->available() ) { this->yatl->getSubMCU()->read(); delay(2); }
        this->yatl->warn("Download in progress");
        this->yatl->getSubMCU()->println("+OK");
Serial.println("+OK");
        // while( !this->yatl->getSubMCU()->available() ) { delay(2); }
        // while( this->yatl->getSubMCU()->available() ) { this->yatl->getSubMCU()->read(); delay(2); }

        // for now : file has to be like "/C/0/XTSDEMO.PAS"
        int tlen = 0;
        char txt[128+1]; 
        // while( !this->yatl->getSubMCU()->available() ) { delay(2); }
        char name[64+1]; memset(name, 0x00, 64); tlen = this->yatl->getSubMCU()->readBytesUntil(0x0A, name, 64);
Serial.print("Name : ");
Serial.println(name);
        if ( tlen <= 0 ) {
            sprintf(txt, "Downloading %s (error)", name);
            this->yatl->warn((const char*)txt);
            // Serial.println("Download not ready");
            // Serial.println(name);
            // Serial.println("-OK");
            this->yatl->getSubMCU()->unlock();
            return false;
        }

        sprintf(txt, "Downloading %s (0x01)", name);
        this->yatl->warn((const char*)txt);

        // Cf CPM may padd the original file
        File f = SD.open(name, O_CREAT | O_WRITE);
        if ( !f ) {
          this->yatl->getSubMCU()->println("-OK");
          this->yatl->getSubMCU()->unlock();
          return false;    
        }
        f.remove();
        f.close();
        // Cf CPM may padd the original file

        f = SD.open(name, O_CREAT | O_WRITE);
        if ( !f ) {
            sprintf(txt, "Failed to open %s (0x02)", name);
            this->yatl->warn((const char*)txt);
            this->yatl->getSubMCU()->println("-OK");
            this->yatl->getSubMCU()->unlock();
          return false;    
        }
Serial.println("+OK");
        this->yatl->getSubMCU()->println("+OK");
        sprintf(txt, "wait for fileLen %s (0x03)", name);
        this->yatl->warn((const char*)txt);
        while( !this->yatl->getSubMCU()->available() ) { delay(2); }
        char sizeStr[12+1]; memset(sizeStr, 0x00, 12); tlen = this->yatl->getSubMCU()->readBytesUntil(0x0A, sizeStr, 12);
Serial.print("Size : ");
Serial.println(sizeStr);
        long size = atol(sizeStr);
        sprintf(txt, "Downloading %s (%ld bytes)", name, size);
        this->yatl->warn((const char*)txt);
        char packet[128+1];
Serial.println("+OK");
        this->yatl->getSubMCU()->println("+OK");
        for(int readed=0; readed < size;) {
            while( !this->yatl->getSubMCU()->available() ) { delay(2); }
            int packetLen = this->yatl->getSubMCU()->readBytes( packet, 128 );
            f.write(packet, packetLen);
            f.flush();
            readed += packetLen;
        }
        f.close();
Serial.println("-EOF-");
        this->yatl->getSubMCU()->unlock();
        this->yatl->warn("-EOF-");
        this->yatl->beep();
        return true;
    }

    // ===============] SubMCU [===============
    bool YatlSubMCU::setup() {
        BRIDGE_MCU_SERIAL.begin( BRIDGE_MCU_BAUDS );

        // send 'i' wait for response ....
        // will see later Cf synchro.

        return true;
    }

    void YatlSubMCU::reboot(bool waitFor) {
        this->send('r');
        if (waitFor) { delay(1000); }
    }

    void YatlSubMCU::flush() { BRIDGE_MCU_SERIAL.flush(); }
    void YatlSubMCU::send(char ch) { BRIDGE_MCU_SERIAL.write( ch ); }
    void YatlSubMCU::send(const char* str) { BRIDGE_MCU_SERIAL.print( str ); }
    void YatlSubMCU::send(char* str) { BRIDGE_MCU_SERIAL.print( str ); }

    void YatlSubMCU::println(const char* str) { BRIDGE_MCU_SERIAL.println( str ); }

    extern bool keybLocked;

    void YatlSubMCU::lock() {
        keybLocked = true;
    }

    void YatlSubMCU::unlock() {
        keybLocked = false;
    }

    int YatlSubMCU::available() { return BRIDGE_MCU_SERIAL.available(); }
    int YatlSubMCU::read() { 
        // keybLocked = true;
        int ret = BRIDGE_MCU_SERIAL.read(); 
        // keybLocked = false;
        return ret;
    }

    int YatlSubMCU::readUntil(uint8_t until, char* dest, int maxLen) {
      int readed = BRIDGE_MCU_SERIAL.readBytesUntil((char)until, dest, maxLen);
      return readed;
    }

    int YatlSubMCU::readBytes(char* dest, int maxLen) {
      int readed = BRIDGE_MCU_SERIAL.readBytes(dest, maxLen);
      return readed;
    }

    #define MAX_SUBMCU_LINE_LEN 255
    char lastSubMCULine[MAX_SUBMCU_LINE_LEN+1];
    char* YatlSubMCU::readLine() {
        // keybLocked = true;
        memset(lastSubMCULine, 0x00, MAX_SUBMCU_LINE_LEN+1);
        BRIDGE_MCU_SERIAL.readBytesUntil( '\n', lastSubMCULine, MAX_SUBMCU_LINE_LEN );
        // keybLocked = false;
        return lastSubMCULine;
    }

    char* YatlSubMCU::waitForNonEmptyLine(unsigned long timeout) {
        // keybLocked = true;
        if ( timeout == 0 ) { timeout = 10*1000; } // 10sec
        unsigned long began = millis();

memset(lastSubMCULine, 0x00, MAX_SUBMCU_LINE_LEN+1);
  uint8_t idx = 0;
  char c;
  do
  {
    while (BRIDGE_MCU_SERIAL.available() == 0) {
        delay(1);
        if (millis()-began >= timeout) { keybLocked = false; return lastSubMCULine; }
    } // wait for a char this causes the blocking
    c = BRIDGE_MCU_SERIAL.read();
    lastSubMCULine[idx++] = c;

    if (millis()-began >= timeout) { keybLocked = false; return lastSubMCULine; }
  }
  while (c != '\n' && c != '\r'); 
  lastSubMCULine[idx] = 0;


//         /*
//         const int curRespLen = 255;
//         char curResp[curRespLen+1]; memset(curResp, 0x00, curRespLen+1);
//         memset(lastSubMCULine, 0x00, MAX_SUBMCU_LINE_LEN+1);


//         int t;
//         while( (t = BRIDGE_MCU_SERIAL.available() ) <= 0 ) { 
//             delay(2); 
//             Serial.print( (millis()-began) ); Serial.println();
//             if (millis()-began >= timeout) { return lastSubMCULine; } 
//         }
//         Serial.print("Found ");
//         Serial.print(t);
//         Serial.println(" bytes to read");
//         do {
//             while ( t > 0 ) {
//                int tlen = strlen(lastSubMCULine);
//                int ii=0;
//                for(int i=0; i < t; i++) {
//                    char ch = BRIDGE_MCU_SERIAL.read();
//                    if ( ch == '\r' ) { continue; }
//                    if ( ch == '\n' ) { 
//                        // what if line is empty !!!!
//                        lastSubMCULine[tlen+ii] = 0x00;
//                        return lastSubMCULine;
//                    }
//                    lastSubMCULine[tlen+ii] = ch;
//                    if ( tlen+ii >= MAX_SUBMCU_LINE_LEN ) { break; }
//                    ii++;
//                    if (millis()-began >= timeout) { return lastSubMCULine; }
//                }
//                lastSubMCULine[tlen+ii] = 0x00;

//                if (millis()-began >= timeout) { return lastSubMCULine; }

//                t = BRIDGE_MCU_SERIAL.available();
//             }
//             // Serial.print( lastSubMCULine );

            

//             t = BRIDGE_MCU_SERIAL.available();
//         } while( true );

//         return lastSubMCULine;
//         */

//        BRIDGE_MCU_SERIAL.flush();

//        BRIDGE_MCU_SERIAL.setTimeout(3000);
//        memset(lastSubMCULine, 0x00, MAX_SUBMCU_LINE_LEN+1);
//        char subResp[64+1]; 
//        do {
//            bool found = false;
//         //    int t = BRIDGE_MCU_SERIAL.available();
//         //    if ( t == 0 ) { 
//         //        if (millis()-began >= timeout) { keybLocked = false; return lastSubMCULine; }
//         //        continue; 
//         //    }
//            memset(subResp, 0x00, 64+1);
//         //    Serial.print("Found Nb-a ");
//         //    Serial.println(t);
//         //    t = BRIDGE_MCU_SERIAL.readBytes(subResp, t);

// int t=0;
// while( (t=BRIDGE_MCU_SERIAL.available()) == 0 ) { ; }
// if (millis()-began >= timeout) { keybLocked = false; return lastSubMCULine; }

//            int realT = 0;
//            for(int i=0; i < t; i++) {
//                int ch = BRIDGE_MCU_SERIAL.read();
//                if ( ch <= 0 ) { subResp[i] = 0x00; break; }
//                subResp[i] = (char)ch;
//                realT = i;
//            }
//            t = realT;

//            Serial.print("Found Nb ");
//            Serial.println(t);
//            Serial.println(subResp);
//            for(int i=0; i < t; i++) {
//                if ( subResp[i] == '\n' ) {
//                    Serial.print("Found @ ");
//                    Serial.println(i);
//                    int tt = strlen(lastSubMCULine);
//                    for(int j=0; j < i; j++) {
//                        lastSubMCULine[tt+j] = subResp[j];
//                    }
//                    lastSubMCULine[tt+i] = 0x00;
//                    found = true;
//                    break;
//                }
//            }
//            if ( found ) { Serial.print("Found : ");Serial.println(lastSubMCULine); break; }
//            if (millis()-began >= timeout) { keybLocked = false; return lastSubMCULine; }
//        } while(true);

//        BRIDGE_MCU_SERIAL.setTimeout(1000);

    //    keybLocked = false;
       return lastSubMCULine;
    }

    void YatlSubMCU::cleanBuffer() {
        while( this->available() ) { this->read(); }
    }

    // ===============] PWR [=================

    float YatlPWRManager::getVoltage() {
        this->yatl->getSubMCU()->send('v'); this->yatl->getSubMCU()->flush();
        float volt = -1.0f;
        volt = atof( this->yatl->getSubMCU()->waitForNonEmptyLine() );
        return volt;
    }

    #define RESTART_ADDR 0xE000ED0C
    #define READ_RESTART() (*(volatile uint32_t *)RESTART_ADDR)
    #define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))
    void softReset() {
        WRITE_RESTART(0x5FA0004);
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

    void YatlLEDs::any(int num, bool state) {
        if ( num == 1 ) { red(state); }
        else if ( num == 2 ) { green(state); }
        else if ( num == 3 ) { blue(state); }
        else builtin(state); // 0 & others
    }

    void YatlLEDs::mask(uint8_t mask) {
        // TODO : better
        if ( (mask & 128) == 128 ) { any(7, true); }
        else { any(7, false); }
        if ( (mask & 64) == 64 ) { any(6, true); }
        else { any(6, false); }
        if ( (mask & 32) == 32 ) { any(5, true); }
        else { any(5, false); }
        if ( (mask & 16) == 16 ) { any(4, true); }
        else { any(4, false); }
        if ( (mask & 8) == 8 ) { any(3, true); }
        else { any(3, false); }
        if ( (mask & 4) == 4 ) { any(2, true); }
        else { any(2, false); }
        if ( (mask & 2) == 2 ) { any(1, true); }
        else { any(1, false); }
        if ( (mask & 1) == 1 ) { any(0, true); }
        else { any(0, false); }
    }

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
