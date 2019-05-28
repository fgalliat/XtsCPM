#ifndef _XTS_YATL_SETTINGS_H_
#define _XTS_YATL_SETTINGS_H_ 1

/**
 * Yatl SubMCU Settings
 * 
 * Xtase - fgalliat @May 2019
 */

 #define LAYOUT_MOBIGO 1

 #ifdef LAYOUT_MOBIGO
   #define KEYB_MOBIGO 1
 #else
   #define KEYB_CHATPAD 1
 #endif


#endif