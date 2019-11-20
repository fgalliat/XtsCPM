#ifndef ABSTRACT_H
#define ABSTRACT_H

/**
 * Abstraction for YAT4L platforms
 * 
 * NYI :
 * 
 */


#ifdef HAS_BUILTIN_LCD
  bool currentlyUseScreen() {
	  #ifdef USE_BUILTIN_LCD
	    bool curStdOutIsScreen = true;
	    return curStdOutIsScreen;
	  #else
	    return false;
	  #endif
  }
#else
  bool currentlyUseScreen() {
	  return false;
  }
#endif

// turn on/off drive LED
void driveLED(bool state) {
#if YAT4L_PLATFORM
    yat4l_led(state, true);
#else
	if ( LED <= 0 ) { return; }
	if (state) digitalWrite(LED, HIGH^LEDinv);
	else digitalWrite(LED, LOW^LEDinv);
#endif
}


#ifdef PROFILE
#define printf(a, b) Serial.println(b)
#endif

#if defined ARDUINO_SAM_DUE || defined ADAFRUIT_GRAND_CENTRAL_M4
#define HostOS 0x01
#endif
#ifdef CORE_TEENSY
#define HostOS 0x04
#endif
#ifdef ESP32
#define HostOS 0x05
#endif
#ifdef _STM32_DEF_
#define HostOS 0x06
#endif


char _entryName[96+1];

char* getFileEntryPath( char *filename ) {
	if ( filename == NULL || strlen(filename) <= 0 ) { Serial.println("[GRAVE] null filename"); return NULL; }
	if ( filename[0] == '/' ) { return filename; }
	// add leading "/"
	sprintf( _entryName, "/%s", filename );
	return _entryName;
}

char* getFileEntryName( char* filepath ) {
	if ( filepath == NULL || strlen(filepath) <= 0 ) { Serial.println("[GRAVE] null filepath"); return NULL; }
	int tlen = strlen(filepath);
	int lastSlash = -1;
	for (int i = 0; i < tlen; i++) {
		if ( filepath[i] == '/' ) {
			lastSlash = i;
		}
	}
	// -1 protected
	return &filepath[lastSlash+1];
}
char _msg[64]; 

/* Memory abstraction functions */
/*===============================================================================*/
bool _RamLoad(char *filename, uint16 address) {
	// long t0, t1;
	// t0 = millis();
	File f;
	bool result = false;

	if (f = SD.open( getFileEntryPath(filename), FILE_READ)) {
		while (f.available())
			_RamWrite(address++, f.read());
		f.close();
		result = true;
	}
	// t1 = millis();
	// sprintf( _msg, "RamL %lu ms", (t1-t0) ); 
	// Serial.println(_msg);
	return(result);
}

/* Filesystem (disk) abstraction fuctions */
/*===============================================================================*/
File root;
#define FOLDERCHAR '/'

typedef struct {
	uint8 dr;
	uint8 fn[8];
	uint8 tp[3];
	uint8 ex, s1, s2, rc;
	uint8 al[16];
	uint8 cr, r0, r1, r2;
} CPM_FCB;

File _sys_fopen_w(uint8 *filename) {
	return(SD.open((char *)filename, O_CREAT | O_WRITE));
	// return(SD.open( getFileEntryPath((char *)filename), O_WRITE));
}

int _sys_fputc(int ch, File f) {
	return(f.write(ch));
}

void _sys_fflush(File f) {
	f.flush();
}

void _sys_fclose(File f) {
  f.close();
}

int _sys_select(uint8 *disk) {
	uint8 result = FALSE;
	File f;

	driveLED(true);
	if (f = SD.open( getFileEntryPath((char *)disk), O_READ)) {
		if (f.isDirectory())
			result = TRUE;
		f.close();
	}
	driveLED(false);
	return(result);
}

long _sys_filesize(uint8 *filename) {
	long l = -1;
	File f;

	driveLED(true);
	if (f = SD.open( getFileEntryPath((char *)filename), O_RDONLY)) {
		l = f.size();
		f.close();
	}
	driveLED(false);
	return(l);
}

long _t0,_t1;
void startCounter() {
  _t0 = millis();
}

void stopCounter(const char* signature) {
  _t1 = millis();
  sprintf( _msg, "%s %lu ms", signature, (_t1-_t0) ); 
  Serial.println(_msg);
}


int _sys_openfile(uint8 *filename) {
	// startCounter();

	File f;
	int result = 0;

	driveLED(true);
	f = SD.open( getFileEntryPath(((char *)filename) ), O_READ);
	if (f) {
		f.close();
		result = 1;
	}
	driveLED(false);

	// stopCounter("opnF");
	return(result);
}

int _sys_makefile(uint8 *filename) {
	File f;
	int result = 0;

	driveLED(true);
	f = SD.open((char *)filename, O_CREAT | O_WRITE);
	// f = SD.open( getFileEntryPath((char *)filename), O_WRITE);
	if (f) {
		f.close();
		result = 1;
	}
	driveLED(false);
	return(result);
}

int _sys_deletefile(uint8 *filename) {
	driveLED(true);
	return(SD.remove( getFileEntryPath((char *)filename)) );
	driveLED(false);
}

int _sys_renamefile(uint8 *filename, uint8 *newname) {
  File f;
  int result = 0;

  driveLED(true);

  // TODO : on ESP : check SD.exists(....)
//   Serial.println("(FS) RENAME NYI");

  f = SD.open((char *)filename, O_WRITE | O_APPEND);
//   f = SD.open( getFileEntryPath((char *)filename), O_APPEND_WR );
  if (f) {
    if (f.rename(SD.vwd(), (char*)newname)) {
      f.close();      
      result = 1;
    }
  }
  driveLED(false);
  return(result);
}

#ifdef DEBUGLOG
void _sys_logbuffer(uint8 *buffer) {
#ifdef CONSOLELOG
	puts((char *)buffer);
#else
	File f;
	uint8 s = 0;
	while (*(buffer+s))	// Computes buffer size
		++s;
	if(f = SD.open( getFileEntryPath(LogName), O_CREAT | O_APPEND | O_WRITE)) {
		f.write(buffer, s);
		f.flush();
		f.close();
	}
#endif
}
#endif


// ** SLOW POINT **
bool _sys_extendfile(char *fn, unsigned long fpos)
{
	uint8 result = true;
	File f;
	unsigned long i;

	driveLED(true);
	if (f = SD.open(fn, O_WRITE | O_APPEND)) {
	// if (f = SD.open( getFileEntryPath(fn) , O_APPEND_WR)) {
		if (fpos > f.size()) {

			// // Xtase version -- avoid TURBO from compilling big files !!!
            // int blen = f.size() - fpos;
			// uint8 buff[ blen ];
			// memset(buff, 0x00, blen);
			// result = f.write( buff, blen ); 

			for (i = 0; i < f.size() - fpos; ++i) {
				if (f.write((uint8_t)0) < 0) {
					result = false;
					break;
				}
			}
		}
		f.close();
	} else {
		result = false;
	}
	driveLED(false);
	return(result);
}

// =========== Read Cache ==================
// == by Xtase
char cachedFileName[64];
File cachedFile;
long cachedFpos = 0L; 

// Xtase - I did modify this routine to speed system up
// ** SLOW POINT **
uint8 _sys_readseq(uint8 *filename, long fpos) {
	// startCounter();

	uint8 result = 0xff;
	File f;
	uint8 bytesread;
	uint8 dmabuf[128];
	uint8 i;

	driveLED(true);

	// 82ms
	// if (_sys_extendfile((char*)filename, fpos))
	// 	f = SD.open( getFileEntryPath((char*)filename), O_READ);
    // 42ms
	f = SD.open( getFileEntryPath((char*)filename), O_READ);

	// if ( strcmp((const char*)filename, (const char*)cachedFileName) == 0 &&
	//      fpos >= cachedFpos // don't know rewind ....
	//    ) {
	// 	f = cachedFile;
	// } else {
	// 	f = SD.open( getFileEntryPath((char*)filename), O_READ);
	// 	cachedFile = f;
	// 	sprintf(cachedFileName, "%s", (char*)filename);
	// }

	// cachedFpos = fpos;

	if ( f ) {
		if ( fpos > f.size() ) {
			// Serial.println( "I did need to extendFile !" );
			// Serial.println(fpos);
			// Serial.println((char*)filename);
			

			uint8 _result = _sys_extendfile((char*)filename, fpos);
			// re-open file
			f = SD.open( getFileEntryPath((char*)filename), O_READ);
			sprintf(cachedFileName, "%s", "XXX.XX"); // erase cache
		}
	} else {
		Serial.println( "There was a mistake ..." );
		Serial.println(fpos);
		Serial.println((char*)filename);

		// re-open file
		f = SD.open( getFileEntryPath((char*)filename), O_READ);
		sprintf(cachedFileName, "%s", "XXX.XX"); // erase cache
	}

	if (f) {
		if (f.seek(fpos)) {
			// for (i = 0; i < 128; ++i)
			// 	dmabuf[i] = 0x1a;
			memset(dmabuf, 0x1a, 128);
			bytesread = f.read(&dmabuf[0], 128);
			if (bytesread) {
				// for (i = 0; i < 128; ++i)
				// 	_RamWrite(dmaAddr + i, dmabuf[i]);
				memcpy( &RAM[dmaAddr], &dmabuf[0], 128 );
			}
			result = bytesread ? 0x00 : 0x01;
		} else {
			Serial.println("could not seek");
			Serial.println(fpos);
			Serial.println((char*)filename);

			result = 0x01;
		}
		f.close();
	} else {
		Serial.println("file was false");
		result = 0x10;
	}
	driveLED(false);

	// stopCounter("redS");
	return(result);
}

uint8 _sys_writeseq(uint8 *filename, long fpos) {
	uint8 result = 0xff;
	File f;

	driveLED(true);
	if (_sys_extendfile((char*)filename, fpos))
		f = SD.open( getFileEntryPath((char*)filename), O_RDWR);
		// f = SD.open( getFileEntryPath((char*)filename), O_WRITE);
	if (f) {
		if (f.seek(fpos)) {
			if (f.write(_RamSysAddr(dmaAddr), 128))
				result = 0x00;
		} else {
			result = 0x01;
		}
		f.close();
	} else {
		result = 0x10;
	}
	driveLED(false);
	return(result);
}

uint8 _sys_readrand(uint8 *filename, long fpos) {
	// startCounter();

	uint8 result = 0xff;
	File f;
	uint8 bytesread;
	uint8 dmabuf[128];
	uint8 i;

	driveLED(true);
	if (_sys_extendfile((char*)filename, fpos))
		f = SD.open( getFileEntryPath((char*)filename), O_READ);
	if (f) {
		if (f.seek(fpos)) {
			for (i = 0; i < 128; ++i)
				dmabuf[i] = 0x1a;
			bytesread = f.read(&dmabuf[0], 128);
			if (bytesread) {
				for (i = 0; i < 128; ++i)
					_RamWrite(dmaAddr + i, dmabuf[i]);
			}
			result = bytesread ? 0x00 : 0x01;
		} else {
			result = 0x06;
		}
		f.close();
	} else {
		result = 0x10;
	}
	driveLED(false);

	// stopCounter("redR");
	return(result);
}

uint8 _sys_writerand(uint8 *filename, long fpos) {
	uint8 result = 0xff;
	File f;

	driveLED(true);
	if (_sys_extendfile((char*)filename, fpos)) {
		f = SD.open( getFileEntryPath((char*)filename), O_RDWR);
		// f = SD.open( getFileEntryPath((char*)filename), O_WRITE);
	}
	if (f) {
		if (f.seek(fpos)) {
			if (f.write(_RamSysAddr(dmaAddr), 128))
				result = 0x00;
		} else {
			result = 0x06;
		}
		f.close();
	} else {
		result = 0x10;
	}
	driveLED(false);
	return(result);
}

uint8 _findnext(uint8 isdir) {
	File f;
	uint8 result = 0xff;
	#define FNAME_SECU 6
	uint8 dirname[13 + FNAME_SECU];
	bool isfile;

	driveLED(true);
	while (f = root.openNextFile()) {
        f.getName((char*)&dirname[0], 13);
		// getFileEntryName(...) returns only entry name (no path)
		// sprintf( (char*)&dirname[0], "%s", getFileEntryName( (char*) f.name() ) ); // auto add 0-terminated

		isfile = !f.isDirectory();
		f.close();
		if (!isfile)
			continue;
		_HostnameToFCBname(dirname, fcbname);
		if (match(fcbname, pattern)) {
			if (isdir) {
				_HostnameToFCB(dmaAddr, dirname);
				_RamWrite(dmaAddr, 0x00);
			}
			_RamWrite(tmpFCB, filename[0] - '@');
			_HostnameToFCB(tmpFCB, dirname);
			result = 0x00;
			break;
		}
	}
	driveLED(false);
	return(result);
}

uint8 _findfirst(uint8 isdir) {
	uint8 path[4] = { '?', FOLDERCHAR, '?', 0 };
	path[0] = filename[0];
	path[2] = filename[2];
	if (root)
		root.close();
	root = SD.open( getFileEntryPath((char *)path)); // Set directory search to start from the first position
	_HostnameToFCBname(filename, pattern);
	return(_findnext(isdir));
}

uint8 _Truncate(char *filename, uint8 rc) {
	// Serial.println("(FS) TRUNCATE NYI");
  File f;
  int result = 0;

  driveLED(true);
  f = SD.open((char *)filename, O_WRITE | O_APPEND);
//   f = SD.open( getFileEntryPath((char *)filename), O_APPEND_WR);
  if (f) {
    if (f.truncate(rc*128)) {
      f.close();
      result = 1;
    }

	// Serial.println("(FS) TRUNCATE NYI");

  }
  driveLED(false);
  return(result);
}

void _MakeUserDir() {
	uint8 dFolder = cDrive + 'A';
	uint8 uFolder = toupper(tohex(userCode));

	uint8 path[4] = { dFolder, FOLDERCHAR, uFolder, 0 };

	driveLED(true);
	SD.mkdir( getFileEntryPath((char*)path) );
	driveLED(false);
}

uint8 _sys_makedisk(uint8 drive) {
	uint8 result = 0;
	if (drive < 1 || drive>16) {
		result = 0xff;
	} else {
		uint8 dFolder = drive + '@';
		uint8 disk[2] = { dFolder, 0 };
		driveLED(true);
		if (!SD.mkdir( getFileEntryPath( (char*)disk) ) ) {
			result = 0xfe;
		} else {
			uint8 path[4] = { dFolder, FOLDERCHAR, '0', 0 };
			SD.mkdir( getFileEntryPath( (char*)path) );
		}
		driveLED(false);
	}

	return(result);
}

/* Console abstraction functions */
/*===============================================================================*/

extern bool keybLocked;
extern bool Serial_useable;

// #define USE_TELNETD_AS_IO 1
#define USE_TELNETD_AS_IO 0

int _kbhit(void) {
	#ifdef HAS_KEYBOARD
	  int kavail = keybLocked ? 0 : yatl.getKeyboard()->available();
	  if ( kavail > 0 ) {
			return kavail;
	  }
	#endif

	#if USE_TELNETD_AS_IO
	  int k = yael_wifi_telnetd_available();
	  if ( k > 0 ) { return k; }
	#endif

	if ( !Serial_useable ) { return 0; }

	return(Serial.available());
}

uint8 _getch(void) {
	#ifdef HAS_KEYBOARD
		if ( !Serial_useable ) {
			// -= TODO BEWARE : keybLocked =-
			while ( yatl.getKeyboard()->available() == 0 ) {
				; // delay -or- not ??
			}
			return yatl.getKeyboard()->read();
		}

		while (!Serial.available()) {
#ifdef USE_XTS_HDL
        xts_hdl();
#endif

#if USE_TELNETD_AS_IO
	  int k = yael_wifi_telnetd_available();
	  if ( k > 0 ) { return yael_wifi_telnetd_read(); }
#endif

			if ( !keybLocked && yatl.getKeyboard()->available() > 0 ) {
				return yatl.getKeyboard()->read();
			}
		}
	#else
		while (!Serial.available()) {
#ifdef USE_XTS_HDL
        xts_hdl();
#endif

#if USE_TELNETD_AS_IO
	  int k = yael_wifi_telnetd_available();
	  if ( k > 0 ) { return yael_wifi_telnetd_read(); }
#endif

		}
	#endif
	return(Serial.read());
}

uint8 _getche(void) {
	uint8 ch = _getch();
	if ( Serial_useable ) { 
		Serial.write(ch); // local echo 
	}
	
#ifdef HAS_BUILTIN_LCD
  if ( currentlyUseScreen() ) { 
	  #if YATL_PLATFORM
	    yatl.getScreen()->write(ch); 
	  #elif YAT4L_PLATFORM
	    yat4l_tft_print(ch); 
	  #endif
  }
#endif

#if USE_TELNETD_AS_IO
	  yael_wifi_telnetd_broadcast(ch);
#endif

	return(ch);
}

void _putch(uint8 ch) {
#ifdef HAS_BUILTIN_LCD
  if ( currentlyUseScreen() ) { 
	  #if YATL_PLATFORM
	    yatl.getScreen()->write(ch); 
	  #elif YAT4L_PLATFORM
	    yat4l_tft_print(ch); 
	  #endif
  }
	else
#endif
	Serial.write(ch);

#if USE_TELNETD_AS_IO
	  yael_wifi_telnetd_broadcast(ch);
#endif

}

void _clrscr(void) {
#ifdef HAS_BUILTIN_LCD
  if ( currentlyUseScreen() ) {
	  #if YATL_PLATFORM
	    yatl.getScreen()->cls(); 
	  #elif YAT4L_PLATFORM
	    yat4l_tft_cls(); 
	  #endif
  }
	else
#endif
	Serial.println("\e[H\e[J");

#if USE_TELNETD_AS_IO
	  yael_wifi_telnetd_broadcast( (char)26 );
#endif

}

#endif
