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

    extern Yatl yatl;
    YatlSubMCU _subMcu(&yatl);
    YatlBuzzer _buzzer(&yatl);
    YatlPWRManager _pwrManager(&yatl);
    YatlLEDs _leds(&yatl);
    YatlWiFi _wifi(&yatl);
    YatlMusicPlayer _mp3(&yatl);

    Yatl::Yatl() {
        this->subMcu = &_subMcu;
        this->buzzer = &_buzzer;
        this->pwrManager = &_pwrManager;
        this->leds = &_leds;
        this->wifi = &_wifi;
        this->mp3 = &_mp3;
    }

    Yatl::~Yatl() {
    }

    bool Yatl::setup() {
        Serial.begin(115200);
        this->dbug("Yatl setuping ...");

        pinMode( BUILTIN_LED, OUTPUT );
        this->led(false);

        bool ok = this->subMcu->setup();
        if (!ok) { this->warn("SubMCU Setup Failed"); }

        this->delay(200);

        return true;
    }

    void Yatl::dbug(const char* str) { Serial.println(str); }
    void Yatl::dbug(const char* str, int val) { Serial.print(str); Serial.print(' '); Serial.println(val); }
    void Yatl::dbug(const char* str, float val) { Serial.print(str); Serial.print(' '); Serial.println(val); }

    void Yatl::delay(long time) {
        ::delay(time);
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
        this->blink(5);
        Serial.println("*****************************");
        Serial.println("*  ALERT                    *");
        Serial.print("* ");
        Serial.println(msg);
        Serial.println("*****************************");
        this->buzzer->beep(440, 200);
        // TODO : screen alert box
    }
    void Yatl::warn(const char* msg) {
        this->blink(5);
        Serial.println("*****************************");
        Serial.println("*  WARNING                  *");
        Serial.print("* ");
        Serial.println(msg);
        Serial.println("*****************************");
        // TODO : screen warning box
    }

    // ===============] BUZZER [===============

    void YatlBuzzer::noTone() { ::noTone( BUILTIN_BUZZER ); }
    void YatlBuzzer::tone(int freq, int duration) { ::tone( BUILTIN_BUZZER, freq, duration ); }

    void YatlBuzzer::beep(int freq, int duration) { 
        ::tone( BUILTIN_BUZZER, freq, duration ); 
        this->yatl->delay( duration );
        ::noTone( BUILTIN_BUZZER );
    }

    // ===============] SubMCU [===============
    bool YatlSubMCU::setup() {
        BRIDGE_MCU_SERIAL.begin( BRIDGE_MCU_BAUDS );

        // send 'i' wait for response ....
        // will see later Cf synchro.

        return true;
    }

    void YatlSubMCU::send(char ch) { BRIDGE_MCU_SERIAL.write( ch ); }
    void YatlSubMCU::send(const char* str) { BRIDGE_MCU_SERIAL.print( str ); }
    void YatlSubMCU::send(char* str) { BRIDGE_MCU_SERIAL.print( str ); }

    int YatlSubMCU::available() { return BRIDGE_MCU_SERIAL.available(); }
    int YatlSubMCU::read() { return BRIDGE_MCU_SERIAL.read(); }

    #define MAX_SUBMCU_LINE_LEN 255
    char lastSubMCULine[MAX_SUBMCU_LINE_LEN+1];
    char* YatlSubMCU::readLine() {
        memset(lastSubMCULine, 0x00, MAX_SUBMCU_LINE_LEN+1);
        BRIDGE_MCU_SERIAL.readBytesUntil( '\n', lastSubMCULine, MAX_SUBMCU_LINE_LEN );
        return lastSubMCULine;
    }

    // ===============] PWR [=================

    float YatlPWRManager::getVoltage() {
        this->yatl->getSubMCU()->send('v');
        float volt = -1.0f;
        volt = atof( this->yatl->getSubMCU()->readLine() );
        return volt;
    }
    // wholeSystem -> BOTH Main & SubMCU
    void YatlPWRManager::reset(bool wholeSystem) { /*TODO*/ this->yatl->warn("RESET NYI"); }
    void YatlPWRManager::deepSleep(bool wholeSystem) { /*TODO*/ this->yatl->warn("DEEPSLEEP NYI"); }

    // ==============] LEDs [==================

    void YatlLEDs::red(bool state) { this->yatl->getSubMCU()->send( state ? "l1xx" : "l0xx" ); }
    void YatlLEDs::green(bool state) { this->yatl->getSubMCU()->send( state ? "lx1x" : "lx0x" ); }
    void YatlLEDs::blue(bool state) { this->yatl->getSubMCU()->send( state ? "lxx1" : "lxx0" ); }
    void YatlLEDs::builtin(bool state) { this->yatl->led(state); }

    // ==============] WiFi [==================

    bool YatlWiFi::beginSTA() { this->yatl->getSubMCU()->send("wcs"); /*TODO*/ return true; }
    bool YatlWiFi::beginAP() { this->yatl->getSubMCU()->send("wca"); /*TODO*/ return true; }
    void YatlWiFi::close() { this->yatl->getSubMCU()->send("ws"); }

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