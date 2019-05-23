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
    MobigoKeyboard kbd(&io, true);

   // from xts_lay_mobigo.h -- forced to autoPoll mode
   bool setupKeyb() { 
      kbd.setup(LED_PIN, LED_PIN, LED_PIN);
      return true; 
   }
   void pollKeyb() { kbd.poll(); }
   int  availableKeyb() { return kbd.available(); }
   int  readKeyb() { return kbd.read(); }
 #endif

#endif