#ifndef ARDUINO

#include "Desktop.h"

void delay(long millis) {
    printf("delay() to impl.");    
}

void pinMode(int pin, int mode) {}
void digitalWrite(int pin, int value) {}


#endif