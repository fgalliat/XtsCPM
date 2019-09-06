#ifndef ABSTRACT_H
#define ABSTRACT_H

/**
 * Abstraction for ESP boards
 * 
 * NYI :
 *  - rename
 *  - truncate
 * 
 */

#define FILE_READ       "r"
#define FILE_WRITE      "w"
#define FILE_APPEND     "a"

#define O_READ       "r"
#define O_RDONLY     "r"
#define O_WRITE      "w"
#define O_RDWR       "rw"
#define O_APPEND_WR  "wa"


#ifdef YATL_PLATFORM
  #include "xts_yatl_api.h"
	extern Yatl yatl;
#endif

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
	if ( LED <= 0 ) { return; }
	if (state) digitalWrite(LED, HIGH^LEDinv);
	else digitalWrite(LED, LOW^LEDinv);
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
	// Serial.print( "FS: " );	Serial.println( _entryName );
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

/* Memory abstraction functions */
/*===============================================================================*/
bool _RamLoad(char *filename, uint16 address) {
	File f;
	bool result = false;

	if (f = SD.open( getFileEntryPath(filename), FILE_READ)) {
		while (f.available())
			_RamWrite(address++, f.read());
		f.close();
		result = true;
	}
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
	// return(SD.open((char *)filename, O_CREAT | O_WRITE));
	return(SD.open( getFileEntryPath((char *)filename), O_WRITE));
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

int _sys_openfile(uint8 *filename) {
	File f;
	int result = 0;

	driveLED(true);
	f = SD.open( getFileEntryPath(((char *)filename) ), O_READ);
	if (f) {
		f.close();
		result = 1;
	}
	driveLED(false);
	return(result);
}

int _sys_makefile(uint8 *filename) {
	File f;
	int result = 0;

	driveLED(true);
	// f = SD.open((char *)filename, O_CREAT | O_WRITE);
	f = SD.open( getFileEntryPath((char *)filename), O_WRITE);
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
  Serial.println("(FS) RENAME NYI");

// //   f = SD.open((char *)filename, O_WRITE | O_APPEND);
//   f = SD.open( getFileEntryPath((char *)filename), O_APPEND_WR );
//   if (f) {
//     if (f.rename(SD.vwd(), (char*)newname)) {
//       f.close();      
//       result = 1;
//     }
//   }
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

bool _sys_extendfile(char *fn, unsigned long fpos)
{
	uint8 result = true;
	File f;
	unsigned long i;

	driveLED(true);
	// if (f = SD.open(fn, O_WRITE | O_APPEND)) {
	if (f = SD.open( getFileEntryPath(fn) , O_APPEND_WR)) {
		if (fpos > f.size()) {
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

uint8 _sys_readseq(uint8 *filename, long fpos) {
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
			result = 0x01;
		}
		f.close();
	} else {
		result = 0x10;
	}
	driveLED(false);
	return(result);
}

uint8 _sys_writeseq(uint8 *filename, long fpos) {
	uint8 result = 0xff;
	File f;

	driveLED(true);
	if (_sys_extendfile((char*)filename, fpos))
		f = SD.open( getFileEntryPath((char*)filename), O_RDWR);
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
	return(result);
}

uint8 _sys_writerand(uint8 *filename, long fpos) {
	uint8 result = 0xff;
	File f;

	driveLED(true);
	if (_sys_extendfile((char*)filename, fpos)) {
		f = SD.open( getFileEntryPath((char*)filename), O_RDWR);
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
        // f.getName((char*)&dirname[0], 13);
		// getFileEntryName(...) returns only entry name (no path)
		sprintf( (char*)&dirname[0], "%s", getFileEntryName( (char*) f.name() ) ); // auto add 0-terminated

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
  File f;
  int result = 0;

  driveLED(true);
//   f = SD.open((char *)filename, O_WRITE | O_APPEND);
  f = SD.open( getFileEntryPath((char *)filename), O_APPEND_WR);
  if (f) {

	// see :: SD/src/utility/SdFat.h:281:  uint8_t truncate(uint32_t size);
	// in SD.h :: class File contains a private : SdFile *_file;

    // if (f.truncate(rc*128)) {
    //   f.close();
    //   result = 1;
    // }

	Serial.println("(FS) TRUNCATE NYI");

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

int _kbhit(void) {
	#ifdef HAS_KEYBOARD
	  int kavail = keybLocked ? 0 : yatl.getKeyboard()->available();
	  if ( kavail > 0 ) {
			return kavail;
	  }
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
			if ( !keybLocked && yatl.getKeyboard()->available() > 0 ) {
				return yatl.getKeyboard()->read();
			}
		}
	#else
		while (!Serial.available());
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
	  #elif YAEL_PLATFORM
	    yael_tft_print(ch); 
	  #endif
  }
#endif
	return(ch);
}

void _putch(uint8 ch) {
#ifdef HAS_BUILTIN_LCD
  if ( currentlyUseScreen() ) { 
	  #if YATL_PLATFORM
	    yatl.getScreen()->write(ch); 
	  #elif YAEL_PLATFORM
	    yael_tft_print(ch); 
	  #endif
  }
	else
#endif
	Serial.write(ch);
}

void _clrscr(void) {
#ifdef HAS_BUILTIN_LCD
  if ( currentlyUseScreen() ) {
	  #if YATL_PLATFORM
	    yatl.getScreen()->cls(); 
	  #elif YAEL_PLATFORM
	    yael_tft_cls(); 
	  #endif
  }
	else
#endif
	Serial.println("\e[H\e[J");
}

#endif
