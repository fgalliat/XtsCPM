#ifndef ARDUINO

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/**
 * Arduino.h replacement for Desktop Computers
 * Xtase - fgalliat @Jul 2109
 */


#define OUTPUT 1

#define LOW 0
#define HIGH 1

void delay(long millis);
void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);

extern int min(int a, int b);


#endif