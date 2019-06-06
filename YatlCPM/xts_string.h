#ifndef _XTS_STRING_H_
#define _XTS_STRING_H_ 1

#include "Arduino.h"

/**
 * Xtase - fgalliat @Jan2019
 * str functions IMPL
 */

// always free-able
char* makeString(int len) {
    char* p = (char*)malloc(len+1);
    p[len] = (char)0x00;
  return p;
}

// makes a safe copy
char* copyOf(char* str, int len=-1) {
  if (len < 0) {
      len = strlen(str);
  }
  char* p = makeString(len);
  memcpy(p, str, len);
  return p;
}

// makes a safe copy
char* substring(char* str, int start, int stop=-1) {
    int len = strlen(str);
    if ( start >= len ) {
        return makeString(0);
    }
    if ( stop < 0 ) {
        stop = len;
    }
    if ( start < 0 ) {
        start = 0;
    }
    int rlen = stop-start;
    char* res = makeString( rlen );

    for(int i=0; i < rlen; i++) {
        res[i] = str[i+start];
    }

    return res;
}

int indexOf(char* str, char ch, int start=0) {
  if ( start < 0 ) {
      start = 0;
  }

  int len = strlen(str);
  if ( start >= len ) {
    //   start = -1;
    return -1;
  }

  for(int i=start; i < len; i++) {
      if ( str[i] == ch ) {
          return i;
      }
  }

return -1;
}


char* str_split(char* stringToSplit, char delim, int tokenNum) {
    if ( tokenNum < 0 ) {
        return makeString(0);
    }

    int curToken = 0;
    int idx = 0, lastIdx = 0;

    while( true ) {
        lastIdx = idx;
        idx = indexOf(stringToSplit, delim, idx);
        if ( idx < 0 ) {
            return NULL;
        }
        if ( curToken == tokenNum ) {
            return substring(stringToSplit, lastIdx, idx);
        }
        idx++; // += delim.length
        curToken++;
    }

    return NULL;
}

int str_count(char* stringToSplit, char delim) {
    int curToken = 0;
    int idx = 0, lastIdx = 0;

    while( true ) {
        lastIdx = idx;
        idx = indexOf(stringToSplit, delim, idx);
        if ( idx < 0 ) {
            break;
        }
        idx++; // += delim.length
        curToken++;
    }

    return curToken;
}

char* upper(char* str) {
    int slen = strlen( str );
    for(int i=0; i < slen; i++) {
        if ( str[i] >= 'a' && str[i] <= 'z' ) {
            str[i] = str[i] - 'a' + 'A';
        }
    }
    return str;
}

char* lower(char* str) {
    int slen = strlen( str );
    for(int i=0; i < slen; i++) {
        if ( str[i] >= 'A' && str[i] <= 'Z' ) {
            str[i] = str[i] - 'A' + 'a';
        }
    }
    return str;
}

bool endsWith(char* str, char* toFind) {
    int slen = strlen( str );
    int tlen = strlen( toFind );

    if ( tlen > slen ) { return false; }

    int tc = tlen - 1;

    for(int i=slen-1; i >= 0; i--) {
        if ( str[i] == toFind[tc--] ) {
            if (tc < 0) { break; }
        } else { return false; }
    }
    return true;
}

#endif