// just for devl purposes
// #define ARDUINO 1


#ifdef ARDUINO
 /**
 * Xtase - fgalliat @Jun2019
 * 
 * implementaion of Yatl hardware API (Arduino Version)
 * SPEAKER / BUZZER impl.
 * 
 */

  #include "Arduino.h"

  // #include "xts_string.h"
  extern char charUpCase(char ch);

  #include "xts_yatl_settings.h"
  #include "xts_yatl_api.h"
  extern Yatl yatl;

    #include "xts_yatl_dev_speaker_notes.h"

    #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
    extern SdFatSdio SD;

    // ===============] BUZZER [===============

    void YatlBuzzer::setup() { pinMode( BUILTIN_BUZZER, OUTPUT ); digitalWrite( BUILTIN_BUZZER, LOW ); }

    void YatlBuzzer::noTone() { if (this->_mute) { return; } ::noTone( BUILTIN_BUZZER ); }
    void YatlBuzzer::tone(int freq, int duration) { if (this->_mute) { return; } ::tone( BUILTIN_BUZZER, freq, duration ); }

    void YatlBuzzer::beep(int freq, int duration) { 
        if (this->_mute) { return; } 
        ::tone( BUILTIN_BUZZER, freq, duration ); 
        this->yatl->delay( duration );
        ::noTone( BUILTIN_BUZZER );
    }

    /**
       * plays a note or pseudo freq.
       * duration is a 1/50th
       */
      void YatlBuzzer::playNote(int noteOrFreq, int duration) {
        if (this->_mute) { return; } 

        if ( noteOrFreq >= 1 && noteOrFreq <= 48 ) {
            // 0..48 octave2 to 5
            noteOrFreq = notes[ noteOrFreq-1 ];
        } else if ( noteOrFreq >= 49 && noteOrFreq <= 4096 ) {
            // 49..4096 -> 19200/note in Hz
            noteOrFreq *= 20;
        } else {
            noteOrFreq = 0;
        }

        this->noTone();
        this->tone( noteOrFreq, duration*50 );
        ::delay(duration*50);
        this->noTone(); // MANDATORY for ESP32
    }

    // ex. "AC#B"
    void YatlBuzzer::playTuneString(const char* strTune) {
        if (this->_mute) { return; } 
        this->noTone();

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

            this->tone(pitch, defDuration);
            ::delay(defDuration);
        }
        this->noTone(); // MANDATORY for ESP32
    }

    // =====]]]]]]]]]]]]]]]]]]]]]
    bool checkbreak() { return false; }

    // forward symbols
    extern int _kbhit(void);
    extern uint8_t _getch(void);

    // TMP impl.
    // int _kbhit(void) { return Serial.available(); }
    // uint8_t _getch(void) { return Serial.read(); }
    // int _kbhit(void) { return yatl.getKeyboard()->available(); }
    // uint8_t _getch(void) { return yatl.getKeyboard()->read(); }

    #define _puts Serial.print

    bool anyBtn() { if (_kbhit() > 0) { _getch(); return true; } else { return false; } }

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
    bool __playTune(YatlBuzzer* _this, unsigned char* tuneStream, bool btnStop);
    // T53
    bool __playTuneT53(YatlBuzzer* _this, unsigned char* tuneStream, bool btnStop);

    // 5KB audio buffer
    #define AUDIO_BUFF_SIZE (5 * 1024)
    uint8_t audiobuff[AUDIO_BUFF_SIZE];
    void cleanAudioBuff() { memset(audiobuff, 0x00, AUDIO_BUFF_SIZE); }

    void led3(bool state) { yatl.getLEDs()->green(state); }
    void led2(bool state) { yatl.getLEDs()->blue(state); }

    typedef struct Note {
        unsigned char note;
        unsigned short duration;
    } Note;

    
    // ex. "MONKEY.T5K"
    bool YatlBuzzer::playTuneFile(const char* tuneStreamName) { 
        if (this->_mute) { return true; } 
        this->noTone();

        cleanAudioBuff();
        int tlen = strlen(tuneStreamName);
        if ( tlen < 1 ) {
            char msg[64+1];
            sprintf(msg, "BUZ NO File provided");
            this->yatl->warn(msg);
            return false;
        }

        char* ftuneStreamName = this->yatl->getFS()->getAssetsFileEntry( (char*)tuneStreamName );
        int format = 0;
        bool btnStop = true;

        char lastCh = tuneStreamName[ tlen-1 ];
        lastCh = charUpCase(lastCh);
        if ( !(( lastCh == 'K' || lastCh == '3') && (tlen >= 2 && tuneStreamName[ tlen-2 ] == '5') ) ) {
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
            this->yatl->warn(msg);
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
            ok = __playTune(this,  &audiobuff[0], btnStop );  
        } else {
            ok = __playTuneT53(this,  &audiobuff[0], btnStop );  
        }
        this->noTone();
        return ok;
    }

    // where tuneStream is the audio buffer content
    // T5K audio format
    bool __playTune(YatlBuzzer* _this, unsigned char* tuneStream, bool btnStop = false) {
        _this->noTone();
        /*static*/ short nbNotes = (*tuneStream++ << 8) | (*tuneStream++);
        /*static*/ char songname[16+1];
        memset(songname, 0x00, 16+1); // BUGFIX -> YES
        for(int i=0; i < 16; i++) {
            songname[i] = *tuneStream++;
        }
        short tempoPercent = (*tuneStream++ << 8) | (*tuneStream++);
        _puts("   -= Playing =-\n");
        _puts( (const char*)songname );
        _puts("\n");
        float tempo = (float)tempoPercent / 100.0;
        // cf a bit too slow (Cf decoding)
        tempo *= 0.97;
        for (int thisNote = 0; thisNote < nbNotes; thisNote++) {
            int note = *tuneStream++;
            short duration = (*tuneStream++ << 8) | (*tuneStream++);
            // note 0 -> silence
            if ( note > 0 ) {
                _this->tone(notes[ note-1 ], duration);
                //   led2( note > 30 );
                //   led3( note > 36 );
            }
            // to distinguish the notes, set a minimum time between them.
            // the note's duration + 10% seems to work well => 1.10:
            int pauseBetweenNotes = duration * tempo;
            delay(pauseBetweenNotes);
            // stop the tone playing:
            _this->noTone();

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
    bool __playTuneT53(YatlBuzzer* _this, unsigned char* tuneStream, bool btnStop = false) {
        _this->noTone();
    
        short nbNotes = (*tuneStream++ << 8) | (*tuneStream++);
        char songname[16];
        memset(songname, 0x00, 16); // BUGFIX -> YES
        for(int i=0; i < 16; i++) {
            songname[i] = *tuneStream++;
        }
        short tempoPercent = (*tuneStream++ << 8) | (*tuneStream++);

        if ( true ) {
            _puts("   -= Playing =-");
            _puts( songname );
            _puts("\n");
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
                _this->tone(note, duration);
            }

            delay(wait*tempo);
            // stop the tone playing:
            _this->noTone();

            if (btnStop && ( anyBtn() || checkbreak() ) ) {
                return true;
            }
        }
        return true;
    } // end of play T53 function

#endif