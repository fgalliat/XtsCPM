#ifndef YATL_SPEAKER_H_
#define YATL_SPEAKER_H_ 1

 #include "Arduino.h"

  #include "xts_yatl_speaker_notes.h"

  class YatlSpeaker {
    private:
      bool muted = false;
    public:
      YatlSpeaker() {
      }
      ~YatlSpeaker() {
      }

      void setup() {
          pinMode(SPEAKER_PIN, OUTPUT);
          digitalWrite(SPEAKER_PIN, LOW);
      }

      void mute(bool m=true) {
          this->muted = m;
      }

      void tone(int freq, int duration) {
          ::tone(SPEAKER_PIN, freq, duration);
      }

      void noTone() {
          ::noTone(SPEAKER_PIN);
      }

      void beep() {
        if ( this->muted ) { return; }

        this->tone(440, 50);
        delay(50);
        this->noTone();    
      }

      /**
       * plays a note or pseudo freq.
       * duration is a 1/50th
       */
      void playNote(int note_freq, int duration) {
        if ( this->muted ) { return; }

        if ( note_freq >= 1 && note_freq <= 48 ) {
            // 0..48 octave2 to 5
            note_freq = notes[ note_freq-1 ];
        } else if ( note_freq >= 49 && note_freq <= 4096 ) {
            // 49..4096 -> 19200/note in Hz
            note_freq *= 20;
        } else {
            note_freq = 0;
        }

        this->noTone();
        this->tone( note_freq, duration*50 );
        delay(duration*50);
        this->noTone(); // MANDATORY for ESP32
      }
  };

  YatlSpeaker buzzer;

  // forward symbol
  void playTuneString(char* strTune);

  // declared in xts_yatl.h
  void setupArduinoSpeaker() {
      buzzer.setup();

      buzzer.beep();

      // playTuneString( (char*)"aac#d");
  }

  // =========================================================
  // to move away...
  char charUpCase(char ch) {
      if ( ch >= 'a' && ch <= 'z' ) {
          return ch - 'a' + 'A';
      }
      return ch;
  }

    bool checkbreak() { return false; }
    bool anyBtn() { return false; }

  // returns nb of bytes readed
  // n = _SD_readBinFile(ftuneStreamName, audiobuff, fileLen);
  int _SD_readBinFile(char* filename, uint8_t* outBuff, long fileLength) {
      File f;
      
      if (f = SD.open(filename, FILE_READ) ) {
        // ....
      } else { return -1; }

      int readed = 0;

      while (f.available())
			outBuff[readed++] = (uint8_t)f.read();

      f.close();

      return readed;
  }
  // =========================================================

  // ex. PLAY "aac#d"
  void playTuneString(char* strTune) {
    buzzer.noTone();

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

      buzzer.tone(pitch, defDuration);
      delay(defDuration);
    }
    buzzer.noTone(); // MANDATORY for ESP32
  } // end of playTuneStreamSTring



  #define _MUSIC_TONES_SUPPORT 1
  #ifdef _MUSIC_TONES_SUPPORT
    // T5K Format
    bool __playTune(unsigned char* tuneStream, bool btnStop);
    // T53
    bool __playTuneT53(unsigned char* tuneStream, bool btnStop);

    // 5KB audio buffer
    #define AUDIO_BUFF_SIZE (5 * 1024)
    uint8_t audiobuff[AUDIO_BUFF_SIZE];
    void cleanAudioBuff() { memset(audiobuff, 0x00, AUDIO_BUFF_SIZE); }

    void led3(bool state) { /*mcu.led(0, state);*/ }
    void led2(bool state) { /*mcu.led(0, state);*/ }

 typedef struct Note {
  unsigned char note;
  unsigned short duration;
 } Note;


    bool playTuneFromStorage(char* tuneStreamName, int format = AUDIO_FORMAT_T5K, bool btnStop = true) {
        buzzer.noTone();

        cleanAudioBuff();
        // led3(true);

        int tlen = strlen(tuneStreamName);
        if ( tlen < 1 ) {
            _puts("ERR : NO file provided\n");
            // led3(false);
            return false;
        }

        // make a routine to clean filenames

        // spe DISK for assets : "Z:"
        // 'cause CP/M supports ONLY up to P:
        // so "/Z/0/MARIO.T53"
        static char ftuneStreamName[1+4+(8+1+3)+1];
        strcpy(ftuneStreamName, "/Z/0/");
        strcat(ftuneStreamName, tuneStreamName);

        // auto complete filename if ext is not provided
        // ~experimental
        char lastCh = tuneStreamName[ tlen-1 ];
        lastCh = charUpCase(lastCh);

        if ( !(( lastCh == 'K' || lastCh == '3') && (tlen >= 2 && tuneStreamName[ tlen-2 ] == '5') ) ) {
            if ( format == AUDIO_FORMAT_T5K ) {
                strcat(ftuneStreamName, ".T5K");
            } else {
                strcat(ftuneStreamName, ".T53");
            }
        }

        static unsigned char preBuff[2];
        int n = _SD_readBinFile(ftuneStreamName, preBuff, 2);
        if ( n <= 0 ) {
            _puts( ftuneStreamName );
            _puts("\n");
            _puts("ZIK File not ready\n");
            return false;
        }

        int nbNotes = (preBuff[0]<<8)|preBuff[1];

        int fileLen = (nbNotes*sizeof(Note))+2+16+2;
        if ( format == AUDIO_FORMAT_T53 ) {
            fileLen = (nbNotes*(3+3+3))+2+16+2;
        }

        //file.read( audiobuff, fileLen );
        n = _SD_readBinFile(ftuneStreamName, audiobuff, fileLen);
        // led3(false);

        bool ok = false;
        if ( format == AUDIO_FORMAT_T5K ) {
            ok = __playTune( audiobuff, btnStop );  
        } else {
            ok = __playTuneT53( audiobuff, btnStop );  
        }

        buzzer.noTone();
        return ok;
    }

    // where tuneStream is the audio buffer content
    // T5K audio format
    bool __playTune(unsigned char* tuneStream, bool btnStop = false) {
        buzzer.noTone();

        static short nbNotes = (*tuneStream++ << 8) | (*tuneStream++);
        static char songname[16];
        for(int i=0; i < 16; i++) {
            songname[i] = *tuneStream++;
        }
        short tempoPercent = (*tuneStream++ << 8) | (*tuneStream++);

        //printfln("nbN:%d title:'%s' tmp:%d", nbNotes, (const char*)songname, tempoPercent);
        _puts("   -= Playing =-\n");
        _puts( songname );
        _puts("\n");

        float tempo = (float)tempoPercent / 100.0;
        // cf a bit too slow (Cf decoding)
        tempo *= 0.97;

        for (int thisNote = 0; thisNote < nbNotes; thisNote++) {
            int note = *tuneStream++;
            short duration = (*tuneStream++ << 8) | (*tuneStream++);
            // note 0 -> silence
            if ( note > 0 ) {
                buzzer.tone(notes[ note-1 ], duration);
                //   led2( note > 30 );
                //   led3( note > 36 );
            }

            // to distinguish the notes, set a minimum time between them.
            // the note's duration + 10% seems to work well => 1.10:
            int pauseBetweenNotes = duration * tempo;
            delay(pauseBetweenNotes);
            // stop the tone playing:
            buzzer.noTone();

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
    bool __playTuneT53(unsigned char* tuneStream, bool btnStop = false) {
        buzzer.noTone();
    
        short nbNotes = (*tuneStream++ << 8) | (*tuneStream++);
        char songname[16];
        for(int i=0; i < 16; i++) {
            songname[i] = *tuneStream++;
        }
        short tempoPercent = (*tuneStream++ << 8) | (*tuneStream++);

        //printfln("nbN:%d title:'%s' tmp:%d", nbNotes, (const char*)songname, tempoPercent);
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
                buzzer.tone(note, duration);
            }

            delay(wait*tempo);
            // stop the tone playing:
            buzzer.noTone();

            if (btnStop && ( anyBtn() || checkbreak() ) ) {
                return true;
            }
        }
        return true;
    } // end of play T53 function



  #endif



#endif