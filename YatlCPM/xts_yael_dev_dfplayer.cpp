#if defined ESP32

/**
 * 
 * DFPlayer mini Serial MP3 Playback module
 * 
 * Xtase - fgalliat @ Aug 2018
 * 
 * 
 * https://community.particle.io/t/quickest-easiest-way-of-controlling-the-dfplayer-mini/40246/8
 * https://community.particle.io/t/a-great-very-cheap-mp3-sound-module-without-need-for-a-library/20111/62
 * 
 * 
 * 
0x01 1 Next * [DH]=0, [DL]=0 Next file in current folder.Loops when last file played
0x02 2 Previous * [DH]=0, [DL]=0 Previous file in current folder.Loops when last file played
0x03 3 Specify track(NUM) * [DH]=highByte(NUM), [DL]=lowByte(NUM) 1~2999 Playing order is order in which the numbers are stored. Filename and foldername are arbitrary, but when named starting with an increasing number and in one folder, files are played in that order and with correct track number. e.g. 0001-Joe Jackson.mp3...0348-Lets dance.mp3)
0x06 6 Specify volume * [DH]=0, [DL]=Volume (0-0x30) Default=0x30
0x07 7 Specify Equalizer * [DH]=0, [DL]= EQ(0/1/2/3/4/5) [Normal/Pop/Rock/Jazz/Classic/Base]
0x08 8 Specify repeat(NUM) * [DH]=highByte(NUM), [DL]=lowByte(NUM).Repeat the specified track number
0x0A 10 Enter into standby – low power loss * [DH]=0, [DL]=0 Works, but no command found yet to end standby (insert TF-card again will end standby mode)
0x0C 12 Reset module * [DH]=0, [DL]=0 Resets all (Track = 0x01, Volume = 0x30) Will return 0x3F initialization parameter (0x02 for TF-card) Clap sound after excecuting command (no solution found)
0x0D 13 Play * [DH]=0, [DL]=0 Play current selected track
0x0E 14 Pause * [DH]=0, [DL]=0 Pause track
0x0F 15 Specify folder and file to playback * [DH]=Folder, [DL]=File - Important: Folders must be named 01~99, files must be named 001~255
0x11 17 Loop play - Start at track 1 * [DH]=0, [DL]=1:play, 0:stop play
0x12 18 Play mp3 file [NUM] in mp3 folder * [DH]=highByte(NUM), [DL]=lowByte(NUM) Play mp3 file in folder named mp3 in your TF-card. File format exact - 4-digit number (0001~2999) e.g. 0235.mp3
0x16 22 Stop * [DH]=0, [DL]=0, Stop playing current track
0x17 23 Loop Folder 01 * [DH]=0, [DL]=1~99, Loops all tracks in folder named "01"
0x18 24 Random play * [DH]=0, [DL]=0 Random all tracks, always starts at track 1
0x19 25 Single loop * [DH]=0, [DL]=0 Loops the track that is playing
0x1A 26 Pause * [DH]=0, [DL]=(0x01:pause, 0x00:stop pause)
0x3F 63 Initialization parameters * [DH]=0, [DL]= 0 ~ 0x0F. Returned code when Reset (0x12) is used.(each bit represent one device of the low-four bits) See Datasheet. 0x02 is TF-card. Error 0x01 when no medium is inserted.
0x4F 79 The total number of folders * [DH]=0, [DL]=(NUM), Total number of folders, including root directory
 * 
 * default : 9600 bps
 * 
 */
 
#include "Arduino.h"
 
//  #include "SoundCard.h"
 #include "xts_yael_dev_dfplayer.h"
 
//   void _error(char* str) { printf("(EE) "); printf(str); printf("\n"); }
  void _error(char* str) { Serial.print("(EE) "); Serial.print(str); Serial.print("\n"); }
  void _error(const char* str) { _error( (char*)str ); }
  
//   void _warn(char* str) { printf("(!!) "); printf(str); printf("\n"); }
  void _warn(char* str) { Serial.print("(!!) "); Serial.print(str); Serial.print("\n"); }
  void _warn(const char* str) { _warn( (char*)str ); }

//   // =============================
  
//   char** _readLines(char* filename, int maxLines, int maxLineLen) {
//   	char** lines = (char**)malloc( maxLines * (sizeof( char* )) * 1 );

//   	FILE* file = fopen(filename, "r");
//   	if ( !file ) { _warn("(READLINES) file missing"); return NULL; }
//   	fseek(file, 0, SEEK_SET);

//   	char* line = NULL, *lineS = NULL;
//   	int lineNum = 0;
//   	int readed = 1; // SURE
//   	// line = (char*)malloc( maxLineLen +1 );
//   	lineS = (char*)malloc( maxLineLen +1 );
//   	while( !feof( file ) ) {
//   		line = (char*)malloc( maxLineLen +1 );
//   		memset(line, 0x00, maxLineLen+1 );
//   		lineS = fgets(line, maxLineLen, file);
//   		if ( lineS == NULL ) {
//   			break;
//   		}
//   		int len = strlen(line);
//   		lines[ lineNum ] = line;
//   		lineNum++;
//   	}
//   	fclose( file );
//   	return lines;
//   }

//   char** _readBADCol(char* filename, int column, int& count) {
//   	int maxLines = 99;
//   	int maxLineLen = 256;
//   	char** lines = _readLines(filename, maxLines, maxLineLen);

// 	count = 0;

//   	if ( lines == NULL || sizeof(lines) <= 0 /*|| lines[0] == NULL || lines[0][0] == 0x00*/ ) {
//   		count = -1;
//   		return NULL;
//   	}

// 	if ( lines[0] == NULL || strlen( lines[0] ) <= 0 ) {
// 		count = -1;
// 		return NULL;
// 	}
	
// 	int nbEntries = atoi( lines[0] );
// 	if ( nbEntries <= 0 ) {
// 		count = -1;
// 		return NULL;
// 	}
	
//   	char** entries = (char**)malloc( nbEntries * ( sizeof(char*) ) );
  	
// 	for(int i=0+1; i < maxLines+1; i++) {
// 		if ( count >= nbEntries ) { break; }
		
// 		if ( lines[i] == NULL || strlen( lines[i] ) <= 0 ) {
// 			break;
// 		}
		
// 		if ( lines[i][0] == '#' ) {
// 			continue;
// 		}
		

// 		int len = strlen(lines[i]);
// 		char* tmp = (char*)malloc( (len+1) * 1 );

// 		// columns filter TODO BETTER		
// 		char ch; int s=0; bool yet=false;
// 		for(int e=0; e < len; e++) {
// 			ch = lines[i][e];
// 			if ( !yet ) {
// 				if ( ch == ';' ) { 
					
// 					if ( lines[i][e+1] >= '0' && lines[i][e+1] <= '9' && lines[i][e+4] == '_' ) {
// 						// still 002_xxxx in name
// 						e+=4;
// 					}
					
// 					yet = true; 
// 					continue; 
// 				}
// 				continue;
// 			}
			
// 			if ( ch == '\r' || ch == '\n' ) { break; }
			
// 			tmp[s++] = ch;
// 		}
// 		tmp[s] = 0x00;
		
// 		entries[count] = tmp;
// 		count++;
// 	}
//   	return entries;
//   }
 
//   // =============================
  
// 	void delay(int time) { usleep(time*1000); }
	
	#define Start_Byte 0x7E
	#define Version_Byte 0xFF
	#define Command_Length 0x06
	#define End_Byte 0xEF
	#define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]

	
	
	void execute_CMD(Stream* serial, uint8_t CMD, uint8_t Par1, uint8_t Par2) {
	 // Calculate the checksum (2 bytes)
	 int16_t checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
	
	 // Build the command line
	 uint8_t Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge, Par1, Par2, (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF), End_Byte};
	
	 //Send the command line to the module
	 serial->write( Command_line, 10);
	}
 
  // =============================
 
    int  _snd_trackNum = 1;
    int  _snd_vol      = 15;
    bool _snd_isPlaying = false;
    
    int _snd_trackNb = -1;
    char** _snd_trackNames = NULL;
 
 	SoundCard::SoundCard(Stream* serial) { this->serial = serial; }
    SoundCard::~SoundCard() {}
        
    bool SoundCard::init() {
    	// TODO : read '/vm_mnt/data/JUKE.BAD' & get trackNb & trackNames
    	int count = 0;
		// #ifdef MY_PC
    	// char** JUKE = _readBADCol( (char*)"/vm_mnt/dat2/JUKE.BAD", 1, count);
		// #else
		// char** JUKE = _readBADCol( (char*)"/vm_mnt/data/JUKE.BAD", 1, count);
		// #endif
        char** JUKE = NULL;
    	// printf("There is %d lines in JUKE file !\n", count);
    	if ( count <= 0 ) {
    		_warn("Error reading JUKE list file");
    	}
    	
    	_snd_trackNb = count;

        // TMP : tip
        _snd_trackNb = 999; 

    	_snd_trackNames = JUKE;
    	

		execute_CMD(this->serial, 0x0C, 0, 0); // reset
		delay(500);
		
		execute_CMD(this->serial, 0x3F, 0, 0); // init
  		delay(500);
		
		this->volume( _snd_vol );
	return true;
    }

    void SoundCard::close() {
    	this->stop();
    }
    
    void SoundCard::play(int track) {
    	if ( track <= 0 ) { _warn("Track Number starts from 1 !"); }
    	if ( _snd_trackNb > 0 && track >= _snd_trackNb ) { _warn("Track Number ends @ endOfList !"); }
    	
    	
    	_snd_trackNum = track;
    	execute_CMD(this->serial, 0x03, (uint8_t)(track>>8) , (uint8_t)(track));  // play track 1 from SDCARD 
		delay(100);
    }
    
    void SoundCard::volume(int vol) {
    	_snd_vol = vol;
    	execute_CMD(this->serial, 0x06, 0, _snd_vol); // Set the volume (0x00~0x30)
    	delay(100);
    }
    int  SoundCard::getVolume() {
    	return _snd_vol;
    }
    
    int SoundCard::getTrackNum() {
    	return _snd_trackNum;
    }
    
    char toto[32];
    char* SoundCard::getTrackName() {
    	if ( _snd_trackNb <= 0 || _snd_trackNames == NULL ) {
    	  sprintf( toto, "TRACK : %d", _snd_trackNum );
    	  return toto;
    	} else {
    	  return _snd_trackNames[ _snd_trackNum-1 ]; // trackNum is 1-based
    	}
    }
    
    int SoundCard::getTrackNb() {
    	return _snd_trackNb;
    }
    char** SoundCard::getTrackNames() {
    	return _snd_trackNames;
    }
    
    void SoundCard::next() {
    	_snd_trackNum++;
    	execute_CMD(this->serial, 0x01,0,1);
  		delay(100);
    }
    void SoundCard::prev() {
    	_snd_trackNum--; // TODO : optional mod to _snd_trackNb
    	execute_CMD(this->serial, 0x02,0,1);
  		delay(100);
    }
    
    bool SoundCard::isPlaying() {
    	return _snd_isPlaying;
    }
    
    void SoundCard::pause() {
    	if ( _snd_isPlaying ) {
    		execute_CMD(this->serial, 0x0E,0,0);
    		_snd_isPlaying = false;
    		delay(100);
    	} else {
    		execute_CMD(this->serial, 0x0D,0,1);
    		_snd_isPlaying = true;
    		delay(100);
    	}
    	
    }
    void SoundCard::stop() {
    	execute_CMD(this->serial, 0x0E,0,0);
		_snd_isPlaying = false;
		delay(100);
    }

// end of if ESP32 ....
#endif