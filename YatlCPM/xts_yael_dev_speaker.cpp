#if defined(ARDUINO) && defined(ESP32)
/**
 * Xtase - fgalliat @Sept 2019
 * 
 * Buzzer support for YAEL platform
 * 
 */

#include "Arduino.h"

// #include "xts_string.h"
extern char charUpCase(char ch);

#include "xts_yael.h"
extern SdFatSoftSpiEX<2, 4, 14> SD;

    #include "xts_yatl_dev_speaker_notes.h"

#define BUZ_channel 0
#define BUZ_resolution 8
    
// cetified
#define BUZZER_PIN 12

bool BUZZER_MUTE = false;

void yael_buzzer_init() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    int freq = 2000;
    ledcSetup(BUZ_channel, freq, BUZ_resolution);
    ledcAttachPin(BUZZER_PIN, BUZ_channel);

    // sets the volume 
    ledcWrite(BUZ_channel, 0);
}

void yael_buzzer_tone(int freq, int duration) {
    if ( !BUZZER_MUTE ) {
        // tone(BUZZER_PIN, freq, duration);

        ledcWrite(BUZ_channel, 125); // volume
        ledcWriteTone(BUZ_channel, freq); // freq
    }
}

void yael_buzzer_noTone() {
    // noTone(BUZZER_PIN);
    ledcWrite(BUZ_channel, 0); // volume  
}

    // ===================================================


    void yael_buzzer_beep(int freq, int duration) { 
        // if (this->_mute) { return; } 
        yael_buzzer_tone( freq, duration ); 
        delay( duration );
        yael_buzzer_noTone();
    }

    /**
     * plays a note or pseudo freq.
     * duration is a 1/50th
     */
    void yael_buzzer_playNote(int noteOrFreq, int duration) {
        // if (this->_mute) { return; } 

        if ( noteOrFreq >= 1 && noteOrFreq <= 48 ) {
            // 0..48 octave2 to 5
            noteOrFreq = notes[ noteOrFreq-1 ];
        } else if ( noteOrFreq >= 49 && noteOrFreq <= 4096 ) {
            // 49..4096 -> 19200/note in Hz
            noteOrFreq *= 20;
        } else {
            noteOrFreq = 0;
        }

        yael_buzzer_noTone();
        yael_buzzer_tone( noteOrFreq, duration*50 );
        delay(duration*50);
        yael_buzzer_noTone(); // MANDATORY for ESP32
    }



    // plays a "c#ab" sequence
    void yael_buzzer_playTuneString(char* strTune) {
        // if (this->_mute) { return; } 
        yael_buzzer_noTone();

        int defDuration = 200;
        int slen = strlen( strTune );

        for (int i=0; i < slen; i++) {
            char ch = strTune[i];
            ch = charUpCase(ch);
            bool sharp = false;
            if ( i+1 < slen && strTune[i+1] == '#' ) { 
                sharp = true; 
                i++; 
            }  

            int pitch = 0;
            switch (ch) {
                case 'C' :
                    if ( sharp ) { pitch = notes[ NOTE_CS4 ]; }
                    else pitch = notes[ NOTE_C4 ];
                    break;
                case 'D' :
                    if ( sharp ) { pitch = notes[ NOTE_DS4 ]; }
                    else pitch = notes[ NOTE_D4 ];
                    break;
                case 'E' :
                    pitch = notes[ NOTE_E4 ];
                    break;
                case 'F' :
                    if ( sharp ) { pitch = notes[ NOTE_FS4 ]; }
                    else pitch = notes[ NOTE_F4 ];
                    break;
                case 'G' :
                    if ( sharp ) { pitch = notes[ NOTE_GS4 ]; }
                    else pitch = notes[ NOTE_G4 ];
                    break;
                case 'A' :
                    if ( sharp ) { pitch = notes[ NOTE_AS4 ]; }
                    else pitch = notes[ NOTE_A4 ];
                    break;
                case 'B' :
                    pitch = notes[ NOTE_B4 ];
                    break;
            }

            yael_buzzer_tone(pitch, defDuration);
            ::delay(defDuration);
        }
        yael_buzzer_noTone(); // MANDATORY for ESP32
    }




#endif