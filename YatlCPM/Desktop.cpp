#ifndef ARDUINO

#include "Desktop.h"

// for sleep
#include <unistd.h>


void delay(long millis) {
    // printf("delay() to impl.");    
    // BEWARE w/ SDL & threads
    usleep(millis*10);
}

void pinMode(int pin, int mode) {}
void digitalWrite(int pin, int value) {}

// int min(int a, int b) { return a < b ? a : b; }

#endif