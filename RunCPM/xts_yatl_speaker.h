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

  char charUpCase(char ch) {
      if ( ch >= 'a' && ch <= 'z' ) {
          return ch - 'a' + 'A';
      }
      return ch;
  }


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




#endif