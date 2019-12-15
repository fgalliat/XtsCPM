bool _kbhit() {
    return Serial.available() > 0;
}

int _getch() {
  if (_kbhit()) { return Serial.read(); }
  return -1;
}

int _getche() {
  if (_kbhit()) { int ch = Serial.read(); Serial.write(ch); return ch; }
  return -1;
}

char _kline[255+1];
char* _kbReadLine(bool echo=true) {
  memset(_kline, 0x00, 255+1);
  int cpt=0, ch;

  while( true ) {
    while(!_kbhit()) {
        // xts_hdl()
        delay(10);
    }

    while(_kbhit()) {
        ch = echo ? _getche() : _getch();
        if ( ch == '\r' ) {
            delay(10);
            // reads trailing "\n" if any
            if ( _kbhit() ) { ch = echo ? _getche() : _getch(); }
            break;
        } else if ( ch == '\n' ) {
            break;
        } else if ( ch == 0x03 ) {
            // Ctrl-C
            return NULL;
        }
        _kline[cpt++] = ch;
    }
  }

  return _kline;
}

void _puts(char* str) { Serial.print(str); }
void _puts(const char* str) { Serial.print(str); }

char* _kbInput(const char* prompt, bool echo=true) {
    _puts(prompt); _puts("\r\n");
    return _kbReadLine(echo);
}

