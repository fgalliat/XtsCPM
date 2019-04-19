## Xts cpm



( just an **unoffical** fork of the excelent [RunCPM](https://github.com/MockbaTheBorg/RunCPM) that was did by MockbaTheBorg )

**You should rather take a look @ [official implementation](https://github.com/MockbaTheBorg/RunCPM).** 



Xtase - fgalliat version @ Apr2109

Usefull links :

- https://github.com/MockbaTheBorg/RunCPM
- http://www.z80.eu/pas-compiler.html (that provides versions of Pascal language for CPM)



**Aim** : run interpreters on Teensy 3.6 (& even a small OS)




**Current/Initial compilation : **
  On a Teensy 3.6
  Sketch uses 73184 bytes (6%) on 1048576 bytes (+1MB).
  Global vars uses 72436 bytes (27%) of RAM, still 189708 free for local vars. max is 262144 bytes (~262KB).

  - enough RAM & Flash remaining to add some hardware supports 

  - **debug @ 9600bps / default** 



**Layer impl.**

- video
  - start by console (6x8 -> 53x30chars) // (3+1x5+1 -> 80x40chars)
  - the TinyFont mode should be enough (80x25chars required)
  - VT100 emulation ( for colors + clrscr cmd + locate cmd )
  - can have gfx (seems that OS itself is text based only)

- audio
  - MP3 tracks playback via DFPlayer
  - PCSpeaker like 

- find a way to call ext. hardware
  - may be by poke on specific mem address
  - Current CCP uses 60KB of RAM -> 4KB remains to make the stuff !?
    - 'cause if uses 60KB that means that mem pointers are 16bit long
    - so could address 64KB of mem ( /!\\ BEWARE of ROM location if any )
  - look @ 2x PseudoFiles that can deals w/ Printers&Devices (PUN: & LST:) ....



**Software**

- Pascal IDE (text)
  - tpas30.zip
  - http://www.z80.eu/pas-compiler.html
- mBASIC
  - 40KB RAM available when launched
- Google Search
  - https://www.google.com/search?q=program+for+cp%2Fm&oq=program+for+cp%2Fm



**Runs on**

  - Teensy (Yeah)
  - 'nux (ARM64 - Android's chrooted kali - native cpp) / (x64 ubuntu Linux)
  - Android (Dalvik nor NDK : Not yet Ported)

**How to run**
 - Teensy
   - copy CCP/* @ sdcard:/
   - unzip DISK/A.zip to sdcard:/A/
   - then boot (each DISK letter is a folder of ONE letter)
  - 'Nux
  - make a ./distro/ directory
   - copy CCP/* @ ./distro/
   - unzip DISK/A.zip to ./distro/A/
   - copy RunCPM executable ELF to ./distro/
   - then boot : cd ./distro && ./RunCPM (each DISK letter is a folder of ONE letter)



@ Xtase - fgalliat :: Apr2019