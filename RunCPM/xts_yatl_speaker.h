#ifndef YATL_SPEAKER_H_
#define YATL_SPEAKER_H_ 1

 #include "Arduino.h"

  class YatlSpeaker {
    public:
      YatlSpeaker() {
      }
      ~YatlSpeaker() {
      }
      void setup() {
          pinMode(SPEAKER_PIN, OUTPUT);
          digitalWrite(SPEAKER_PIN, LOW);
      }

      void tone(int freq, int duration) {
          ::tone(SPEAKER_PIN, freq, duration);
      }

      void noTone() {
          ::noTone(SPEAKER_PIN);
      }

      void beep() {
        this->tone(440, 50);
        delay(50);
        this->noTone();    
      }
  };

  YatlSpeaker buzzer;

  // declared in xts_yatl.h
  void setupArduinoSpeaker() {
      buzzer.setup();

      buzzer.beep();
  }
#endif