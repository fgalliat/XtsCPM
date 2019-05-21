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
   // from xts_lay_mobigo.h -- forced to autoPoll mode
   bool setupKeyb() { xxxx(); return true; }
   void pollKeyb() { xxx(); }
   int  availableKeyb() { pollKeyb(); return xxx(); }
   int  readKeyb() { pollKeyb(); return xxx(); }
 #endif

#endif