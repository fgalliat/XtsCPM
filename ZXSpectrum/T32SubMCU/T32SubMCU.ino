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


int bridgeAvailable() { return Serial.available(); }
int bridgeRead() { return Serial.read(); }
int bridgeWait() { 
    while( bridgeAvailable() <= 0 ) { delay(5); }
    return bridgeRead(); 
}
void bridgeWrite(uint8_t bt) { Serial.write(bt); }


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

  for(int i=0; i < NB_LEDS; i++) {
      pinMode( ledPins[i], OUTPUT );
      led( i, false );
  }

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

          Serial.print("LED:");Serial.print( (int)ledCh);
          Serial.print(":");Serial.print( (int)stateCh);
          Serial.println("");

      } else if ( ch == 'L' ) { // LED mask {0..255} controls 8 LEDs
	  uint8_t mask = (uint8_t)bridgeWait();
	  ledMask(mask);
      } 
      // ====== BUZZER ======
      else if ( ch == 'b' ) { // simple beep
	  uint8_t mask = (uint8_t)bridgeWait();
	  beep(440, 200);
      } else if ( ch == 'B' ) { // beep noteOrFreq,duration
	  uint8_t f0 = (uint8_t)bridgeWait();
	  uint8_t f1 = (uint8_t)bridgeWait();
	  uint8_t d0 = (uint8_t)bridgeWait();
	  uint8_t d1 = (uint8_t)bridgeWait();

	  uint16_t freqOrNote = (f0 << 8) + f1;
	  uint16_t duration   = (d0 << 8) + d1;
	  beep(freqOrNote, duration);
      }

      // ====== MP3 =================
      else if ( ch == 'M' ) { // isPlaying
          bridgeWrite( 0 ); // not plaing for now
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