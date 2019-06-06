#ifndef _YATL_API_H_
#define _YATL_API_H_ 1

/**
 * Xtase - fgalliat @Jun2019
 * 
 * definition of Yatl hardware API
 * 
 * 
 */

class Yatl;

class YatlSubMCU {
    private:
      Yatl* yatl;
    public:
      YatlSubMCU(Yatl* yatl) { this->yatl = yatl; }

      bool setup();
      void reboot(bool waitFor=true);

      void send(char ch);
      void send(const char* ch);
      void send(char* ch);

      int available();
      int read();
      char* readLine();
      int readUntil(uint8_t until, char* dest, int maxLen);

      void cleanBuffer();
};

class YatlPWRManager {
    private:
      Yatl* yatl;
    public:
      YatlPWRManager(Yatl* yatl) { this->yatl = yatl; }
      float getVoltage();
      void reset(bool wholeSystem=true);
      void deepSleep(bool wholeSystem=true);
};

class YatlBuzzer {
    private:
      Yatl* yatl;
      bool _mute = false;
    public:
      YatlBuzzer(Yatl* yatl) { this->yatl = yatl; }
      void setup();

      void mute(bool _mute=true) { this->_mute = _mute; }

      void tone(int freq, int duration);
      void beep(int freq, int duration);
      void noTone();

      /**
       * plays a note or pseudo freq.
       * duration is a 1/50th
       */
      void playNote(int noteOrFreq, int duration);

      // ex. "AC#B"
      void playTuneString(const char* notes);
      // ex. "MONKEY.T5K"
      bool playTuneFile(const char* tuneFileName);
};

class YatlMusicPlayer {
    private:
      Yatl* yatl;
    public:
      YatlMusicPlayer(Yatl* yatl) { this->yatl = yatl; }
      void play(int trackNum);
      void loop(int trackNum);
      void stop();
      void pause();
      void next();
      void prev();
      void volume(uint8_t vol);
};

class YatlClock {
    private:
    public:
      YatlClock();
      long getTime();
      void setTime(long time);
};

class YatlFS {
    private:
      Yatl* yatl;
    public:
      YatlFS(Yatl* yatl) { this->yatl = yatl; }
      bool setup();
      char* getAssetsFileEntry(char* entryName);
};

class YatlWiFi {
    private:
      Yatl* yatl;
    public:
      YatlWiFi(Yatl* yatl) { this->yatl = yatl; }

      bool beginSTA();
      bool beginAP();
      void close();

      char* getIP();
      char* getSSID();

      bool startTelnetd();
      bool stopTelnetd();
      char* wget(char* url);
};

class YatlScreen {
    private:
      Yatl* yatl;
    public:
      YatlScreen(Yatl* yatl) { this->yatl = yatl; }
      bool setup();

      void write(char ch);
      void println(const char* str);
      void println(char* str);

      void cls();
      void drawWallpaper(char* assetName);
      void drawTextBox(const char* title, const char* msg);
};

class YatlJoypad {
    private:
    public:
      YatlJoypad();
};

class YatlKeyboard {
    private:
      Yatl* yatl;
    public:
      YatlKeyboard(Yatl* yatl) { this->yatl = yatl; }
      bool setup();
      void poll();
      int available();
      int read();
};

class YatlLEDs {
    private:
      Yatl* yatl;
    public:
      YatlLEDs(Yatl* yatl) { this->yatl = yatl; }
      void red(bool state=true);
      void green(bool state=true);
      void blue(bool state=true);
      void builtin(bool state=true);
};


class Yatl {
    private:
      YatlSubMCU* subMcu;
      YatlBuzzer* buzzer;
      YatlPWRManager* pwrManager;
      YatlLEDs* leds;
      YatlWiFi* wifi;
      YatlMusicPlayer* mp3;
      YatlFS* fs;
      YatlKeyboard* keyb;
      YatlScreen* screen;
    public:
      Yatl();
      ~Yatl();

      // rather Serial DBUG
      void dbug(const char* str);
      void dbug(const char* str, int val);
      void dbug(const char* str, float val);

      void beep();

      // rather Screen (+ Serial)
      // for Immediate Native User Interaction
      void alert(const char* msg);
      void warn(const char* msg);
      bool confirm(const char* msg);

      bool setup();

      void delay(long time);
      void led(bool state);
      void blink(int times);

      // TODO : look at protected
      YatlSubMCU* getSubMCU() { return this->subMcu; }

      YatlPWRManager* getPWRManager() { return this->pwrManager; }

      YatlBuzzer* getBuzzer() { return this->buzzer; }
      YatlMusicPlayer* getMusicPlayer() { return this->mp3; }

      YatlClock* getClock();

      YatlLEDs* getLEDs() { return this->leds; }

      YatlScreen* getScreen() { return this->screen; }
      YatlKeyboard* getKeyboard() { return this->keyb; }
      YatlJoypad* getJoypad();

      YatlFS* getFS() { return this->fs; }

      YatlWiFi* getWiFi() { return this->wifi; }

};

#endif