## Xts cpm



( just an **unoffical** fork of the excelent [RunCPM](https://github.com/MockbaTheBorg/RunCPM) that was did by MockbaTheBorg )

**You should rather take a look @ [official implementation](https://github.com/MockbaTheBorg/RunCPM).** 



Xtase - fgalliat version @ Apr2109

Usefull links :

- https://github.com/MockbaTheBorg/RunCPM
- http://www.z80.eu/pas-compiler.html (that provides versions of Pascal language for CPM)



**Aim** : run interpreters on Teensy 3.6 (& even a small OS)




**Current/Initial compilation : **
  Le croquis utilise 73184 octets (6%) de l'espace de stockage de programmes. Le maximum est de 1048576 octets.
  Les variables globales utilisent 72436 octets (27%) de mémoire dynamique, ce qui laisse 189708 octets pour les variables locales. Le maximum est de 262144 octets.

  - il reste suffisament de flash et de RAM pour ajouter des fonctions
    materieles 
  - **j'ai une carte SD de 8GB avec les fichiers de la distro dessus**
  - **remonter le Teensy 3.6 avec ce carte SD**
  - **debugage a 9600bps / default** 



**Layer impl.**

- video
  - start by console (6x8 -> 53x30chars) // (3+1x5+1 -> 80x40chars)
  - the TinyFont mode should be enough (80x25chars required)
  - VT100 emulation ( for colors + clrscr cmd )
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



**Run on**

- Teensy (@ this time : compiles but not yet tested)
- 'nux (ARM64 - Android's chrooted kali - native cpp) / (x64 ubuntu Linux)
- Android (Dalvik nor NDK : Not yet Ported)



@ Xtase - fgalliat :: Apr2019