#ifndef __SOUNDCARD_H_
#define __SOUNDCARD_H_ 1

#include "../serial/Serial.h"

  class SoundCard {
      private:
        Serial* serial = NULL;
        
      public:
        SoundCard(Serial* serial);
        ~SoundCard();
        
        bool init();
        void close();
        
        void play(int track);
        void volume(int vol);
        
        int getTrackNum();
        char* getTrackName();
        
        int getTrackNb();
        char** getTrackNames();
        
        void next();
        void prev();
        
        bool isPlaying();
        int  getVolume();
        
        void pause();
        void stop();
  };

#endif
