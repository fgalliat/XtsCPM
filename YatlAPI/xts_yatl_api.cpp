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

    Yatl::Yatl() {
        this->subMcu = &_subMcu;
        this->buzzer = &_buzzer;
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

    // ===============] SubMCU [===============
    bool YatlSubMCU::setup() {
        BRIDGE_MCU_SERIAL.begin( BRIDGE_MCU_BAUDS );

        // send 'i' wait for response ....
        // will see later Cf synchro.

        return true;
    }

#endif