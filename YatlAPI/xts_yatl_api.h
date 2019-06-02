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

      void send(char ch);
      void send(const char* ch);
      void send(char* ch);

      int available();
      int read();
      char* readLine();
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
    public:
      YatlBuzzer(Yatl* yatl) { this->yatl = yatl; }
      void tone(int freq, int duration);
      void beep(int freq, int duration);
      void noTone();
};

class YatlMusicPlayer {
    private:
    public:
      YatlMusicPlayer();
      void play(int trackNum);
      void stop();
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
    public:
      YatlFS();
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
    public:
      YatlScreen();
};

class YatlJoypad {
    private:
    public:
      YatlJoypad();
};

class YatlKeyboard {
    private:
    public:
      YatlKeyboard();
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
    public:
      Yatl();
      ~Yatl();

      // rather Serial DBUG
      void dbug(const char* str);
      void dbug(const char* str, int val);
      void dbug(const char* str, float val);

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
      YatlMusicPlayer* getMusicPlayer();

      YatlClock* getClock();

      YatlLEDs* getLEDs() { return this->leds; }

      YatlScreen* getScreen();
      YatlKeyboard* getKeyboard();
      YatlJoypad* getJoypad();

      YatlFS* getFS();

      YatlWiFi* getWiFi() { return this->wifi; }

};

#endif