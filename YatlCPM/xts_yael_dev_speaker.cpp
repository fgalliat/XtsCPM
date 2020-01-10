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

    // ============= PLAY Tune Files ======================
    
    // forward symbols
    // extern int _kbhit(void);
    // extern uint8_t _getch(void);

    bool checkbreak() { return false; }
    // bool anyBtn() { if (_kbhit() > 0) { _getch(); return true; } else { return false; } }
    bool anyBtn() { return false; }

    #define _puts Serial.print


    // returns nb of bytes readed
    int _SD_readBinFile(char* filename, uint8_t* outBuff, long fileLength) {
      File f;
      
      if (f = SD.open(filename, FILE_READ) ) {
        // ....
        f.seek(0);
      } else { return -1; }

      int readed = 0;

      while (f.available()) {
			outBuff[readed++] = (uint8_t)f.read();
            if ( fileLength > -1 && readed >= fileLength ) {
                break;
            }
      }

      f.close();

      return readed;
    }

    // ============]]]]]]]]]]]]]]

    // T5K Format
    bool __playTune( unsigned char* tuneStream, bool btnStop);
    // T53
    bool __playTuneT53( unsigned char* tuneStream, bool btnStop);

    // 5KB audio buffer
    #define AUDIO_BUFF_SIZE (5 * 1024)
    uint8_t audiobuff[AUDIO_BUFF_SIZE];
    void cleanAudioBuff() { memset(audiobuff, 0x00, AUDIO_BUFF_SIZE); }

    // void led3(bool state) { yatl.getLEDs()->green(state); }
    // void led2(bool state) { yatl.getLEDs()->blue(state); }
    void led3(bool state) { yael_led(state, true); }
    void led2(bool state) { yael_led(state, true); }

    typedef struct Note {
        unsigned char note;
        unsigned short duration;
    } Note;
    
    // ex. "MONKEY.T5K"
    bool yael_buzzer_playTuneFile(const char* tuneStreamName) { 
        // if (this->_mute) { return true; } 
        yael_buzzer_noTone();

        cleanAudioBuff();
        int tlen = strlen(tuneStreamName);
        if ( tlen < 1 ) {
            char msg[64+1];
            sprintf(msg, "BUZ NO File provided");
            yael_warn(msg);
            return false;
        }

        char* ftuneStreamName = yael_fs_getAssetsFileEntry( (char*)tuneStreamName );
        int format = 0;
        bool btnStop = true;

        char lastCh = tuneStreamName[ tlen-1 ];
        lastCh = charUpCase(lastCh);
        if ( (( lastCh == 'K' || lastCh == '3') && (tlen >= 2 && tuneStreamName[ tlen-2 ] == '5') ) ) {
            if ( lastCh == 'K' ) {
                format = AUDIO_FORMAT_T5K;
            } else {
                format = AUDIO_FORMAT_T53;
            }
        }

        /*static*/ unsigned char preBuff[2];
        memset(preBuff, 0x00, 2);
        int n = _SD_readBinFile(ftuneStreamName, preBuff, 2);
        if ( n <= 0 ) {
            char msg[64+1];
            sprintf(msg, "BUZ File not ready %s", ftuneStreamName);
            yael_warn(msg);
            return false;
        }
        int nbNotes = (preBuff[0]<<8)|preBuff[1];

        int fileLen = (nbNotes*sizeof(Note))+2+16+2;
        if ( format == AUDIO_FORMAT_T53 ) {
            fileLen = (nbNotes*(3+3+3))+2+16+2;
        }
        n = _SD_readBinFile(ftuneStreamName, audiobuff, fileLen);

        bool ok = false;
        if ( format == AUDIO_FORMAT_T5K ) {
            ok = __playTune( &audiobuff[0], btnStop );  
        } else {
            ok = __playTuneT53( &audiobuff[0], btnStop );  
        }
        yael_buzzer_noTone();
        return ok;
    }

    // where tuneStream is the audio buffer content
    // T5K audio format
    bool __playTune(unsigned char* tuneStream, bool btnStop = false) {
        yael_buzzer_noTone();
        /*static*/ short nbNotes = (*tuneStream++ << 8) | (*tuneStream++);
        /*static*/ char songname[16+1];
        memset(songname, 0x00, 16+1); // BUGFIX -> YES
        for(int i=0; i < 16; i++) {
            songname[i] = *tuneStream++;
        }
        short tempoPercent = (*tuneStream++ << 8) | (*tuneStream++);

        // _puts("   -= Playing =-\n");
        // _puts( (const char*)songname );
        // _puts("\n");
        if ( true ) {
            yael_lcd_cls();
            //                12345678901234567890
            yael_lcd_println("   -= Playing =-", 1);
            yael_lcd_println( songname, 2 );
        }

        float tempo = (float)tempoPercent / 100.0;
        // cf a bit too slow (Cf decoding)
        tempo *= 0.97;
        for (int thisNote = 0; thisNote < nbNotes; thisNote++) {
            int note = *tuneStream++;
            short duration = (*tuneStream++ << 8) | (*tuneStream++);
            // note 0 -> silence
            if ( note > 0 ) {
                yael_buzzer_tone(notes[ note-1 ], duration);
                //   led2( note > 30 );
                //   led3( note > 36 );
            }
            // to distinguish the notes, set a minimum time between them.
            // the note's duration + 10% seems to work well => 1.10:
            int pauseBetweenNotes = duration * tempo;
            delay(pauseBetweenNotes);
            // stop the tone playing:
            yael_buzzer_noTone();

            if (btnStop && ( anyBtn() || checkbreak() ) ) {
                return true;
            }
        } // end of note loop

        led2(false);
        led3(false);
        return true;
    } // end of play T5K function

    // T53 Format
    // where tuneStream is the audio buffer content
    bool __playTuneT53( unsigned char* tuneStream, bool btnStop = false) {
        yael_buzzer_noTone();
    
        short nbNotes = (*tuneStream++ << 8) | (*tuneStream++);
        char songname[16];
        memset(songname, 0x00, 16); // BUGFIX -> YES
        for(int i=0; i < 16; i++) {
            songname[i] = *tuneStream++;
        }
        short tempoPercent = (*tuneStream++ << 8) | (*tuneStream++);

        if ( true ) {
            // _puts("   -= Playing =-");
            // _puts( songname );
            // _puts("\n");
            yael_lcd_cls();
            //                12345678901234567890
            yael_lcd_println("   -= Playing =-", 1);
            yael_lcd_println( songname, 2 );
        }
    
        float tempo = (float)tempoPercent / 100.0;
        // cf a bit too slow (Cf decoding)
        tempo *= 0.97;

        short note, duration, wait;
        for (int thisNote = 0; thisNote < nbNotes; thisNote++) {
            note = (*tuneStream++ << 8) | (*tuneStream++);
            duration = (*tuneStream++ << 8) | (*tuneStream++);
            wait = (*tuneStream++ << 8) | (*tuneStream++);
            
            // note 0 -> silence
            if ( note > 0 ) {
                yael_buzzer_tone(note, duration);
            }

            delay(wait*tempo);
            // stop the tone playing:
            yael_buzzer_noTone();

            if (btnStop && ( anyBtn() || checkbreak() ) ) {
                return true;
            }
        }
        return true;
    } // end of play T53 function



#endif