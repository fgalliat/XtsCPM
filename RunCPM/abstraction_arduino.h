#ifndef ABSTRACT_H
#define ABSTRACT_H

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

/* Memory abstraction functions */
/*===============================================================================*/
bool _RamLoad(char *filename, uint16 address) {
	File f;
	bool result = false;

	if (f = SD.open(filename, FILE_READ)) {
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
	return(SD.open((char *)filename, O_CREAT | O_WRITE));
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

	digitalWrite(LED, HIGH^LEDinv);
	if (f = SD.open((char *)disk, O_READ)) {
		if (f.isDirectory())
			result = TRUE;
		f.close();
	}
	digitalWrite(LED, LOW^LEDinv);
	return(result);
}

long _sys_filesize(uint8 *filename) {
	long l = -1;
	File f;

	digitalWrite(LED, HIGH^LEDinv);
	if (f = SD.open((char *)filename, O_RDONLY)) {
		l = f.size();
		f.close();
	}
	digitalWrite(LED, LOW^LEDinv);
	return(l);
}

int _sys_openfile(uint8 *filename) {
	File f;
	int result = 0;

	digitalWrite(LED, HIGH^LEDinv);
	f = SD.open((char *)filename, O_READ);
	if (f) {
		f.close();
		result = 1;
	}
	digitalWrite(LED, LOW^LEDinv);
	return(result);
}

int _sys_makefile(uint8 *filename) {
	File f;
	int result = 0;

	digitalWrite(LED, HIGH^LEDinv);
	f = SD.open((char *)filename, O_CREAT | O_WRITE);
	if (f) {
		f.close();
		result = 1;
	}
	digitalWrite(LED, LOW^LEDinv);
	return(result);
}

int _sys_deletefile(uint8 *filename) {
	digitalWrite(LED, HIGH^LEDinv);
	return(SD.remove((char *)filename));
	digitalWrite(LED, LOW^LEDinv);
}

int _sys_renamefile(uint8 *filename, uint8 *newname) {
  File f;
  int result = 0;

  digitalWrite(LED, HIGH^LEDinv);
  f = SD.open((char *)filename, O_WRITE | O_APPEND);
  if (f) {
    if (f.rename(SD.vwd(), (char*)newname)) {
      f.close();      
      result = 1;
    }
  }
  digitalWrite(LED, LOW^LEDinv);
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
	if(f = SD.open(LogName, O_CREAT | O_APPEND | O_WRITE)) {
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

	digitalWrite(LED, HIGH^LEDinv);
	if (f = SD.open(fn, O_WRITE | O_APPEND)) {
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
	digitalWrite(LED, LOW^LEDinv);
	return(result);
}

uint8 _sys_readseq(uint8 *filename, long fpos) {
	uint8 result = 0xff;
	File f;
	uint8 bytesread;
	uint8 dmabuf[128];
	uint8 i;

	digitalWrite(LED, HIGH^LEDinv);
	if (_sys_extendfile((char*)filename, fpos))
		f = SD.open((char*)filename, O_READ);
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
	digitalWrite(LED, LOW^LEDinv);
	return(result);
}

uint8 _sys_writeseq(uint8 *filename, long fpos) {
	uint8 result = 0xff;
	File f;

	digitalWrite(LED, HIGH^LEDinv);
	if (_sys_extendfile((char*)filename, fpos))
		f = SD.open((char*)filename, O_RDWR);
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
	digitalWrite(LED, LOW^LEDinv);
	return(result);
}

uint8 _sys_readrand(uint8 *filename, long fpos) {
	uint8 result = 0xff;
	File f;
	uint8 bytesread;
	uint8 dmabuf[128];
	uint8 i;

	digitalWrite(LED, HIGH^LEDinv);
	if (_sys_extendfile((char*)filename, fpos))
		f = SD.open((char*)filename, O_READ);
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
	digitalWrite(LED, LOW^LEDinv);
	return(result);
}

uint8 _sys_writerand(uint8 *filename, long fpos) {
	uint8 result = 0xff;
	File f;

	digitalWrite(LED, HIGH^LEDinv);
	if (_sys_extendfile((char*)filename, fpos)) {
		f = SD.open((char*)filename, O_RDWR);
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
	digitalWrite(LED, LOW^LEDinv);
	return(result);
}

uint8 _findnext(uint8 isdir) {
	File f;
	uint8 result = 0xff;
	uint8 dirname[13];
	bool isfile;

	digitalWrite(LED, HIGH^LEDinv);
	while (f = root.openNextFile()) {
    f.getName((char*)&dirname[0], 13);
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
	digitalWrite(LED, LOW^LEDinv);
	return(result);
}

uint8 _findfirst(uint8 isdir) {
	uint8 path[4] = { '?', FOLDERCHAR, '?', 0 };
	path[0] = filename[0];
	path[2] = filename[2];
	if (root)
		root.close();
	root = SD.open((char *)path); // Set directory search to start from the first position
	_HostnameToFCBname(filename, pattern);
	return(_findnext(isdir));
}

uint8 _Truncate(char *filename, uint8 rc) {
  File f;
  int result = 0;

  digitalWrite(LED, HIGH^LEDinv);
  f = SD.open((char *)filename, O_WRITE | O_APPEND);
  if (f) {
    if (f.truncate(rc*128)) {
      f.close();
      result = 1;
    }
  }
  digitalWrite(LED, LOW^LEDinv);
  return(result);
}

void _MakeUserDir() {
	uint8 dFolder = cDrive + 'A';
	uint8 uFolder = toupper(tohex(userCode));

	uint8 path[4] = { dFolder, FOLDERCHAR, uFolder, 0 };

	digitalWrite(LED, HIGH^LEDinv);
	SD.mkdir((char*)path);
	digitalWrite(LED, LOW^LEDinv);
}

uint8 _sys_makedisk(uint8 drive) {
	uint8 result = 0;
	if (drive < 1 || drive>16) {
		result = 0xff;
	} else {
		uint8 dFolder = drive + '@';
		uint8 disk[2] = { dFolder, 0 };
		digitalWrite(LED, HIGH^LEDinv);
		if (!SD.mkdir((char*)disk)) {
			result = 0xfe;
		} else {
			uint8 path[4] = { dFolder, FOLDERCHAR, '0', 0 };
			SD.mkdir((char*)path);
		}
		digitalWrite(LED, LOW^LEDinv);
	}

	return(result);
}

/* Console abstraction functions */
/*===============================================================================*/

int _kbhit(void) {
	#ifdef HAS_KEYBOARD
	  int kavail = availableKeyb();
	  if ( kavail > 0 ) {
			return kavail;
		}
	#endif
	return(Serial.available());
}

uint8 _getch(void) {
	#ifdef HAS_KEYBOARD
		while (!Serial.available()) {
			if ( availableKeyb() > 0 ) {
				return readKeyb();
			}
		}
	#else
		while (!Serial.available());
	#endif
	return(Serial.read());
}

uint8 _getche(void) {
	uint8 ch = _getch();
	Serial.write(ch); // local echo
#ifdef HAS_BUILTIN_LCD
  if ( currentlyUseScreen() ) { screen.write(ch); }
#endif
	return(ch);
}

void _putch(uint8 ch) {
#ifdef HAS_BUILTIN_LCD
  if ( currentlyUseScreen() ) { screen.write(ch); }
	else
#endif
	Serial.write(ch);
}

void _clrscr(void) {
#ifdef HAS_BUILTIN_LCD
  if ( currentlyUseScreen() ) { screen.cls(); }
	else
#endif
	Serial.println("\e[H\e[J");
}

#endif
