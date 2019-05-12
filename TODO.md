# TODO (current work)

- [x] compile MainMCU code
- [x] upload & test MainMCU code (failure on playT5K files **FIXED** the bug was in _SD_readBinFile() )
- [x] upload XTASE.PAS demo (tested **W/O** SubMCU module)
- [x] test MainMCU + SubMCU layouts connection (had to use pins 20&21 on SubMCU for Bridge RX/TX)
- [x] manage MP3 module (add missing commands in XTASE.PAS demo)
- [x] make an include for XTSDEMO.PAS demo (split BdosCalls & VT-Ext routines from code)
- [x] manage Keyboard
- [ ] manage Keyboard Ctrl Key (cf TE.COM & TURBO.COM inner editor)
- [ ] manage Joystick
- [ ] manage LEDs
- [ ] manage MP3/Busy line
- [ ] XTSDEMO.PAS as mp3 JUKEBOX demo too (play/pause/next) **(in Progress)**
- [ ] add **native JUKE.BAD reading** to make displayable the SongName
- [ ] Make Docs about pinout
- [ ] Make Docs about VT-Ext + BdosCall extension mapping **(in Progress)**