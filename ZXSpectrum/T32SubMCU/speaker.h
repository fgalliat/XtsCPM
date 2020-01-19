#ifndef __SPK_DEV___
#define __SPK_DEV___ 1

char charUpCase(char ch) {
	if ( ch >= 'a' && ch <= 'z' ) { ch -= 'a'; ch += 'A'; }
	return ch;
}


    // ex. "AC#B"
    void playTuneString(const char* strTune) {
        // if (this->_mute) { return; } 
        noTone(BUZZER_PIN);

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

            tone(BUZZER_PIN, pitch, defDuration);
            delay(defDuration);
        }
        noTone(BUZZER_PIN);
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

    void led2(bool state) { led(2, state); }
    void led3(bool state) { led(3, state); }

    bool anyBtn() { return false; }
    bool checkbreak() { return Serial.available(); }

    typedef struct Note {
        unsigned char note;
        unsigned short duration;
    } Note;
    
    // where tuneStream is the audio buffer content
    // T5K audio format
    bool __playTune(unsigned char* tuneStream, bool btnStop = false) {
        noTone(BUZZER_PIN);
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

        float tempo = (float)tempoPercent / 100.0;
        // cf a bit too slow (Cf decoding)
        tempo *= 0.97;
        for (int thisNote = 0; thisNote < nbNotes; thisNote++) {
            int note = *tuneStream++;
            short duration = (*tuneStream++ << 8) | (*tuneStream++);
            // note 0 -> silence
            if ( note > 0 ) {
                tone(BUZZER_PIN, notes[ note-1 ], duration);
                led2( note > 30 );
                led3( note > 36 );
            }
            // to distinguish the notes, set a minimum time between them.
            // the note's duration + 10% seems to work well => 1.10:
            int pauseBetweenNotes = duration * tempo;
            delay(pauseBetweenNotes);
            // stop the tone playing:
            noTone(BUZZER_PIN);

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
        noTone(BUZZER_PIN);
    
        short nbNotes = (*tuneStream++ << 8) | (*tuneStream++);
        char songname[16];
        memset(songname, 0x00, 16); // BUGFIX -> YES
        for(int i=0; i < 16; i++) {
            songname[i] = *tuneStream++;
        }
        short tempoPercent = (*tuneStream++ << 8) | (*tuneStream++);

        // if ( true ) {
        //     _puts("   -= Playing =-");
        //     _puts( songname );
        //     _puts("\n");
        // }
    
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
                tone(BUZZER_PIN, note, duration);
            }

            delay(wait*tempo);
            // stop the tone playing:
            noTone(BUZZER_PIN);

            if (btnStop && ( anyBtn() || checkbreak() ) ) {
                return true;
            }
        }
        return true;
    } // end of play T53 function



#endif