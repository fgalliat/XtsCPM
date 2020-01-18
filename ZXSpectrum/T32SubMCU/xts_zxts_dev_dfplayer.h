/**
 * Xtase - fgalliat Sept2019
 * 
 * DFPlayer - lite lib
 */

#ifndef __SOUNDCARD_H_
#define __SOUNDCARD_H_ 1

#include "SoftwareSerial.h"

  class SoundCard {
      private:
        Stream* serial = NULL;
        // SoftwareSerial* serial = NULL;
        // HardwareSerial* serial = NULL;
        
      public:
        SoundCard(Stream* serial);
        // SoundCard(SoftwareSerial* serial);
        // SoundCard(HardwareSerial* serial);
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