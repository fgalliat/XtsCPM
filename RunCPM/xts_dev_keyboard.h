#ifndef _XTS_DEV_KBD_H_
#define _XTS_DEV_KBD_H_ 1
/**
 * Xtase - fgalliat @May 2019
 * 
 * abstract Keyboard Header file
 * 
 * 
 * 
 */

 bool setupKeyb();
 void pollKeyb();
 int  availableKeyb();
 int  readKeyb();

 #ifdef LAYOUT_FOLDABLE
   // from xts_submcu.h -- autoPoll mode
   bool setupKeyb() { initKeyb(); return true; }
   void pollKeyb() { kbPoll(); }
   int  availableKeyb() { return kbAvailable(); }
   int  readKeyb() { return kbRead(); }
 #elif defined LAYOUT_MOBIGO
    #include "xts_dev_MobigoKeyboard.h"

    #define KB_AUTO_POLL false

    MobigoKeyboard kbd(&io, KB_AUTO_POLL);

   // from xts_lay_mobigo.h -- forced to autoPoll mode
   bool setupKeyb() { 
      kbd.setup(LED_PIN, LED_PIN, LED_PIN);
      return true; 
   }
   void pollKeyb() { kbd.poll(); }
   int  availableKeyb() { return kbd.available(); }

   uint8_t kbMap(uint8_t code) {
      if ( code == '\n' ) { code = '\r'; }
      return code;
   }

   int  readKeyb() { 
     int ch = kbd.read();
     if ( ch > -1 ) {
       ch = kbMap(ch);
     }
     return ch; 
   }
 #endif

#endif