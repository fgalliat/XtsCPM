/**
 * ZXSpectrum - ZXTS SubMCU project
 * 
 * Xtase - fgalliat @Jan 2020
 * 
 * 
 * SubMCU : Teensy3.2
 * 
 */

#define NB_LEDS 8
int ledPins[NB_LEDS] = { 14, 15, 16, 17, 18, 19, 20, 21 };


#define mp3Serial Serial2
#define MP3_BUSY_PIN 23
#include "xts_zxts_dev_dfplayer.h"
SoundCard mp3Player(&mp3Serial);

void led(int num, bool state);

#define BUZZER_PIN 22
#include "notes.h"
#include "speaker.h"

int bridgeAvailable() { return Serial.available(); }
int bridgeRead() { return Serial.read(); }
int bridgeWait() { 
    while( bridgeAvailable() <= 0 ) { delay(5); }
    return bridgeRead(); 
}
void bridgeWrite(uint8_t bt) { Serial.write(bt); }

int bridgeWaitStrZT(char* str, int maxLen) {
  return Serial.readBytesUntil( 0x00, str, maxLen );
}

// 5KB => 80 packets
#define SERIAL_PACKET_LEN 64
uint8_t SerialBinPacket[SERIAL_PACKET_LEN];
int bridgeWaitBinStream(uint8_t* dest, int maxLen) {
  memset( dest, 0x00, maxLen );

  int l0 = bridgeWait();
  int l1 = bridgeWait();

  int len = (l0*256) + l1;
  int total = 0;

  while( total < len ) {
    int read = Serial.readBytes( (char*)SerialBinPacket, SERIAL_PACKET_LEN );
    if ( read <= 0 ) {
      break;
    }

    memcpy( &dest[total], &SerialBinPacket[0], read );
    Serial.write( 0x01 ); // ACK

    total += read;
  }

  if ( total < len ) {
    // there was an error
    return -1;
  }

  return total;
}

void led(int num, bool state) {
    digitalWrite( ledPins[num], state ? HIGH : LOW );
}

void ledMask(uint8_t mask) {
  if ( (mask & 1  ) == 1   ) { led(0,true); } else { led(0,false); }
  if ( (mask & 2  ) == 2   ) { led(1,true); } else { led(1,false); }
  if ( (mask & 4  ) == 4   ) { led(2,true); } else { led(2,false); }
  if ( (mask & 8  ) == 8   ) { led(3,true); } else { led(3,false); }
  if ( (mask & 16 ) == 16  ) { led(4,true); } else { led(4,false); }
  if ( (mask & 32 ) == 32  ) { led(5,true); } else { led(5,false); }
  if ( (mask & 64 ) == 64  ) { led(6,true); } else { led(6,false); }
  if ( (mask & 128) == 128 ) { led(7,true); } else { led(7,false); }
}


void setup() {
  Serial.begin(115200);

  mp3Serial.begin(9600);
  pinMode(MP3_BUSY_PIN, INPUT);
  mp3Player.init();

  for(int i=0; i < NB_LEDS; i++) {
      pinMode( ledPins[i], OUTPUT );
      led( i, false );
  }

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

}

void loop() {
  if ( bridgeAvailable() ) {
      char ch = (char)bridgeRead();

      if ( ch == '@' ) { // PING the module to probe the port
      	bridgeWrite('@');
      }  


      // ========== LEDS =======
      else if ( ch == 'l' ) { // l : LED {0..7} {0..1} => switch ON/OFF a LED 
          char ledCh = (char)bridgeWait();
          char stateCh = (char)bridgeWait();

          if ( ledCh >= '0' && ledCh <= '7' ) {
              ledCh -= '0';
          }

          if ( stateCh >= '0' && stateCh <= '1' ) {
              stateCh -= '0';
          }

          if ( stateCh != 0x00 ) { stateCh = 1; }

          led( (int)ledCh, ((int)stateCh != 0) );

          // Serial.print("LED:");Serial.print( (int)ledCh);
          // Serial.print(":");Serial.print( (int)stateCh);
          // Serial.println("");

      } else if ( ch == 'L' ) { // LED mask {0..255} controls 8 LEDs
          uint8_t mask = (uint8_t)bridgeWait();
          ledMask(mask);
      } 
      // ====== BUZZER ======
      else if ( ch == 'b' ) { // simple beep
        beep(440/20, 200/50);
      } else if ( ch == 'B' ) { // beep noteOrFreq,duration
        uint8_t f0 = (uint8_t)bridgeWait();
        uint8_t f1 = (uint8_t)bridgeWait();
        uint8_t d0 = (uint8_t)bridgeWait();

        uint16_t freqOrNote = (f0 << 8) + f1;
        uint8_t duration   = (d0); // * 50
        beep(freqOrNote, duration);
      } else if ( ch == 'q' ) {
        char str[256]; memset(str, 0x00, 256);
        int tlen = bridgeWaitStrZT( str, 256-1 );
        if ( tlen > 0 ) {
          playTuneString( (const char*)str );
        }
      } else if ( ch == 't' ) {
        // .T5K tune stream
        int read = bridgeWaitBinStream(audiobuff, AUDIO_BUFF_SIZE);
        __playTune(audiobuff, true);
        bridgeWrite(0x01);
      } else if ( ch == 'T' ) {
        // .T53 tune stream
        int read = bridgeWaitBinStream(audiobuff, AUDIO_BUFF_SIZE);
        __playTuneT53(audiobuff, true);
        bridgeWrite(0x01);
      }

      // ====== MP3 =================
      else if ( ch == 'M' ) { // isPlaying
          bridgeWrite( isMp3Playing() ? 1 : 0 ); // not playing for now
      } else if ( ch == 'P' ) {
          uint8_t t0 = (uint8_t)bridgeWait();
          uint8_t t1 = (uint8_t)bridgeWait();
          uint16_t track = (t0 << 8) + t1;
          playMp3( track );  
      } else if ( ch == 'X' ) {
        uint8_t t0 = (uint8_t)bridgeWait();
        uint8_t t1 = (uint8_t)bridgeWait();
        uint16_t track = (t0 << 8) + t1;
        loopMp3( track );  
      } else if ( ch == 'p' ) { pauseMp3(); }
        else if ( ch == 'X' ) { stopMp3(); }
        else if ( ch == 'N' ) { nextMp3(); }
        else if ( ch == 'V' ) { prevMp3(); }
        else if ( ch == 'v' ) {
	        uint8_t vol = (uint8_t)bridgeWait();
          volumeMp3( vol );
	    }

      // ====== Aux. Serial  ========


  }
  delay(5);
}

// note is {1..48} / freq is {49.4096}
// beeps for 1/50 msec
void beep(uint16_t freqOrNote, uint8_t duration) {

  if ( freqOrNote >= 1 && freqOrNote <= 48 ) {
      // 0..48 octave2 to 5
      freqOrNote = notes[ freqOrNote-1 ];
  } else if ( freqOrNote >= 49 && freqOrNote <= 4096 ) {
      // 49..4096 -> 19200/note in Hz
      freqOrNote *= 20;
  } else {
      freqOrNote = 0;
  }

  int dd = duration * 50;

  tone(BUZZER_PIN, freqOrNote, dd);
  delay(dd);
  noTone(BUZZER_PIN);
}

bool isMp3Playing() { return digitalRead(MP3_BUSY_PIN) == LOW; }
void playMp3(uint16_t trackNum) { mp3Player.play(trackNum); }
void loopMp3(uint16_t trackNum) { ; }
void pauseMp3() { mp3Player.pause(); }
void stopMp3() { mp3Player.stop(); }
void nextMp3() { mp3Player.next(); }
void prevMp3() { mp3Player.prev(); }
void volumeMp3(uint8_t vol) { mp3Player.volume(vol); }

