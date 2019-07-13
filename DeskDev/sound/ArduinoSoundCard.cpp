/**
 * 
 * DFPlayer mini Serial MP3 Playback module over Arduino
 * 
 * Xtase - fgalliat @ Dec 2018
 * 
 * default : 9600 bps
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../serial/Serial.h"
#include "SoundCard.h"

extern bool __ext_mp3Playing();

void _error(char *str)
{
	printf("(EE) ");
	printf(str);
	printf("\n");
}
void _error(const char *str) { _error((char *)str); }

void _warn(char *str)
{
	printf("(!!) ");
	printf(str);
	printf("\n");
}
void _warn(const char *str) { _warn((char *)str); }

// =============================

char **_readLines(char *filename, int maxLines, int maxLineLen)
{
	char **lines = (char **)malloc(maxLines * (sizeof(char *)) * 1);

	FILE *file = fopen(filename, "r");
	if (!file)
	{
		_warn("(READLINES) file missing");
		return NULL;
	}
	fseek(file, 0, SEEK_SET);

	char *line = NULL, *lineS = NULL;
	int lineNum = 0;
	int readed = 1; // SURE
	// line = (char*)malloc( maxLineLen +1 );
	lineS = (char *)malloc(maxLineLen + 1);
	while (!feof(file))
	{
		line = (char *)malloc(maxLineLen + 1);
		memset(line, 0x00, maxLineLen + 1);
		lineS = fgets(line, maxLineLen, file);
		if (lineS == NULL)
		{
			break;
		}
		int len = strlen(line);
		lines[lineNum] = line;
		lineNum++;
	}
	fclose(file);
	return lines;
}

char **_readBADCol(char *filename, int column, int &count)
{
	int maxLines = 99;
	int maxLineLen = 256;
	char **lines = _readLines(filename, maxLines, maxLineLen);

	count = 0;

	if (lines == NULL || sizeof(lines) <= 0 /*|| lines[0] == NULL || lines[0][0] == 0x00*/)
	{
		count = -1;
		return NULL;
	}

	if (lines[0] == NULL || strlen(lines[0]) <= 0)
	{
		count = -1;
		return NULL;
	}

	int nbEntries = atoi(lines[0]);
	if (nbEntries <= 0)
	{
		count = -1;
		return NULL;
	}

	char **entries = (char **)malloc(nbEntries * (sizeof(char *)));

	for (int i = 0 + 1; i < maxLines + 1; i++)
	{
		if (count >= nbEntries)
		{
			break;
		}

		if (lines[i] == NULL || strlen(lines[i]) <= 0)
		{
			break;
		}

		if (lines[i][0] == '#')
		{
			continue;
		}

		int len = strlen(lines[i]);
		char *tmp = (char *)malloc((len + 1) * 1);

		// columns filter TODO BETTER
		char ch;
		int s = 0;
		bool yet = false;
		for (int e = 0; e < len; e++)
		{
			ch = lines[i][e];
			if (!yet)
			{
				if (ch == ';')
				{

					if (lines[i][e + 1] >= '0' && lines[i][e + 1] <= '9' && lines[i][e + 4] == '_')
					{
						// still 002_xxxx in name
						e += 4;
					}

					yet = true;
					continue;
				}
				continue;
			}

			if (ch == '\r' || ch == '\n')
			{
				break;
			}

			tmp[s++] = ch;
		}
		tmp[s] = 0x00;

		entries[count] = tmp;
		count++;
	}
	return entries;
}

// =============================

void delay(int time) { usleep(time * 1000); }

// =============================

int _snd_trackNum = 1;
int _snd_vol = 50; // percent
bool _snd_isPlaying = false;

int _snd_trackNb = -1;
char **_snd_trackNames = NULL;

SoundCard::SoundCard(Serial *serial) { this->serial = serial; }
SoundCard::~SoundCard() {}

bool SoundCard::init()
{
	// TODO : read '/vm_mnt/data/JUKE.BAD' & get trackNb & trackNames
	int count = 0;
#ifdef MY_PC
	char **JUKE = _readBADCol((char *)"/vm_mnt/dat2/JUKE.BAD", 1, count);
#else
	char **JUKE = _readBADCol((char *)"/vm_mnt/data/JUKE.BAD", 1, count);
#endif
	// printf("There is %d lines in JUKE file !\n", count);
	if (count <= 0)
	{
		_warn("Error reading JUKE list file");
	}

	_snd_trackNb = count;
	_snd_trackNames = JUKE;

	// execute_CMD(this->serial, 0x0C, 0, 0); // reset
	// delay(500);

	// execute_CMD(this->serial, 0x3F, 0, 0); // init
	// delay(500);

	this->stop();
	this->volume(_snd_vol);
	_snd_isPlaying = false;
}
void SoundCard::close()
{
	this->stop();
}

void SoundCard::play(int track)
{
	if (track <= 0)
	{
		_warn("Track Number starts from 1 !");
	}
	if (_snd_trackNb > 0 && track >= _snd_trackNb)
	{
		_warn("Track Number ends @ endOfList !");
	}

	_snd_trackNum = track;
	// execute_CMD(this->serial, 0x03, (uint8_t)(track>>8) , (uint8_t)(track));  // play track 1 from SDCARD
	char msg[16];
	sprintf(msg, "MPL%d", track);
	this->serial->write(msg, strlen(msg));
	_snd_isPlaying = true;
	delay(100);
}

void SoundCard::volume(int volPercent)
{
	_snd_vol = volPercent;
	// execute_CMD(this->serial, 0x06, 0, _snd_vol); // Set the volume (0x00~0x30)
	char msg[16];
	sprintf(msg, "MVL%d", volPercent);
	this->serial->write(msg, strlen(msg));
	delay(100);
}
int SoundCard::getVolume()
{
	return _snd_vol;
}

int SoundCard::getTrackNum()
{
	return _snd_trackNum;
}

char toto[32];
char *SoundCard::getTrackName()
{
	if (_snd_trackNb <= 0 || _snd_trackNames == NULL)
	{
		sprintf(toto, "TRACK : %d", _snd_trackNum);
		return toto;
	}
	else
	{
		return _snd_trackNames[_snd_trackNum - 1]; // trackNum is 1-based
	}
}

int SoundCard::getTrackNb()
{
	return _snd_trackNb;
}
char **SoundCard::getTrackNames()
{
	return _snd_trackNames;
}

void SoundCard::next()
{
	_snd_trackNum++;
	// execute_CMD(this->serial, 0x01,0,1);
	this->serial->writestr("MNX");
	_snd_isPlaying = true;
	delay(100);
}
void SoundCard::prev()
{
	_snd_trackNum--; // TODO : optional mod to _snd_trackNb
					 // execute_CMD(this->serial, 0x02,0,1);
	this->serial->writestr("MPV");
	_snd_isPlaying = true;
	delay(100);
}

bool SoundCard::isPlaying()
{
	_snd_isPlaying = __ext_mp3Playing();
	return _snd_isPlaying;
}

void SoundCard::pause()
{
	if (_snd_isPlaying)
	{
		// execute_CMD(this->serial, 0x0E,0,0);
		this->serial->writestr("MPA");
		_snd_isPlaying = false;
		delay(100);
	}
	else
	{
		// execute_CMD(this->serial, 0x0D,0,1);
		this->serial->writestr("MPA");
		_snd_isPlaying = true;
		delay(100);
	}
}
void SoundCard::stop()
{
	// execute_CMD(this->serial, 0x0E,0,0);
	this->serial->writestr("MSP");
	_snd_isPlaying = false;
	delay(100);
}