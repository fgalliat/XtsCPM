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

      if ( ch == 'l' ) { // l : LED {0..7} {0..1} => switch ON/OFF a LED 
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

      }
  }
  delay(5);
}