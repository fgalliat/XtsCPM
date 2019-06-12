// just for devl purposes
#define ARDUINO 1


#ifdef ARDUINO
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
            this->getScreen()->drawTextBox( "ALERT", msg );
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
            this->getScreen()->drawTextBox( "WARNING", msg );
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

        File f = SD.open(name, O_CREAT | O_WRITE);
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

    void YatlSubMCU::send(char ch) { BRIDGE_MCU_SERIAL.write( ch ); }
    void YatlSubMCU::send(const char* str) { BRIDGE_MCU_SERIAL.print( str ); }
    void YatlSubMCU::send(char* str) { BRIDGE_MCU_SERIAL.print( str ); }

    int YatlSubMCU::available() { return BRIDGE_MCU_SERIAL.available(); }
    int YatlSubMCU::read() { return BRIDGE_MCU_SERIAL.read(); }

    int YatlSubMCU::readUntil(uint8_t until, char* dest, int maxLen) {
      int readed = BRIDGE_MCU_SERIAL.readBytesUntil((char)until, dest, maxLen);
    return readed;
  }

    #define MAX_SUBMCU_LINE_LEN 255
    char lastSubMCULine[MAX_SUBMCU_LINE_LEN+1];
    char* YatlSubMCU::readLine() {
        memset(lastSubMCULine, 0x00, MAX_SUBMCU_LINE_LEN+1);
        BRIDGE_MCU_SERIAL.readBytesUntil( '\n', lastSubMCULine, MAX_SUBMCU_LINE_LEN );
        return lastSubMCULine;
    }

    void YatlSubMCU::cleanBuffer() {
        while( this->available() ) { this->read(); }
    }

    // ===============] PWR [=================

    float YatlPWRManager::getVoltage() {
        this->yatl->getSubMCU()->send('v');
        float volt = -1.0f;
        volt = atof( this->yatl->getSubMCU()->readLine() );
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

    // ==============] WiFi [==================

    bool YatlWiFi::beginSTA() { 
        this->yatl->getSubMCU()->send("wcs");
        int cpt = 0;
        while( this->yatl->getSubMCU()->available() == 0 ) {
            delay(100);
            if ( cpt >= 999 ) { break; }
            cpt++;
        }
        char* resp = this->yatl->getSubMCU()->readLine();
        ::delay(200);
        // Serial.println("----------->");
        // Serial.println(resp);
        // Serial.println("----------->");
        return strlen(resp) > 0 && resp[0] == '+';
    }
    bool YatlWiFi::beginAP() { 
        this->yatl->getSubMCU()->send("wca");
        int cpt = 0;
        while( this->yatl->getSubMCU()->available() == 0 ) {
            delay(100);
            if ( cpt >= 999 ) { break; }
            cpt++;
        }
        ::delay(200);
        char* resp = this->yatl->getSubMCU()->readLine();
        return strlen(resp) > 0 && resp[0] == '+';
    }

    void YatlWiFi::close() { 
        this->yatl->getSubMCU()->send("ws"); 
        ::delay(200);
        this->yatl->getSubMCU()->readLine(); 
    }

    char* YatlWiFi::getIP() { this->yatl->getSubMCU()->send("wi"); return this->yatl->getSubMCU()->readLine(); }
    char* YatlWiFi::getSSID() { this->yatl->getSubMCU()->send("we"); return this->yatl->getSubMCU()->readLine(); }

    // bool YatlWiFi::startTelnetd();
    // bool YatlWiFi::stopTelnetd();
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