## TODOs


 - **TODO**

    - TEXT console colorSet definition **(done)**
    - TEXT console colorSet changing routine **(done)** / made a fake monochrome LCD colorSet 

       - make a BdosCall to change colorset ... **(done)**
    - TEXT conditional space char rendering **(done)**
    - TEXT smoother scrolling (line per line erasing)
   - lower layout (w/ addditional Teensy3.2 3.3v ?)
   - REFACTO Screen API
   - REFACTO BdosCalls & VT-EXT **(done)**
   - GFX Pascal Demo (benchmark : loop for drawing rectangles as ex.)




 - **In Progress or Done**
 - TTY emulation on TFT Screen **(done)**
   - tinyFont 3x5 => 4x6 => @320x240 => 80x40 chars

   - regular font 6x8 => 53x30 chars

   - ILI9341_t3n scrolling impl. is very slow for now
     - when no scrolling & screen overflow : seems to deadlock the teensy
     - have to make a console in RAM (80x30 gives 3200 bytes) **[in progress & working]**
     - use that cls/write abstraction to provide VT100 escape chars emulation ?
        - ^C // \b // \t // \r \n
        - cls
        - color
        - locate ? / bold ? (could be just diff color)
          - if text has attribs I'll have to store those
          - by spans / by chars :-( => on WSL & ubuntu that chars attrs seems to be ignored ....
          - as memseg is 1590 or 3200 -> I choosed to double it & make an attribMap
            - not the better idea but have enought RAM & faster solution

    - 1st, try a benchmark : 
      - allocate memseg (1590 bytes for 53x30) **[v]**
      - fill all w/ a character **[v]**
      - display whole mem-console **[v]**
      - try to scroll **[v]**
      - see rendering time **[nearly good]** 

    - then try bigger memseg that screen caps **(DONE)**
      - ex. 80x25 on 53x25
      - just for compatibility of softwares display

    - try to find the tinyFont for that lib **(DONE)**


- **Xtase handle** inside CP/M code
  - find main loop
  - insert a call to one mine functions xts_hdl()
  - prevent from need of an Interrupt / Timer...
  - beware if cycles emulation ....
  - for System MENU (rather make CP/M pausing while UI)
  - Ext. Hardware functions ...

  - done **once per operation** (not cycle emulation)
- **Pascal (VT100Ex)** extensions (see XTASE.PAS) 
  - cls **(done)**
  - locate row,col **(done)**
  - erase till end of line
  - BeginColor **(done)**
  - EndColor **(done)**
  - play('abcd') **(done)**

  - Define a change FG,BG text colors routine
  - add ifWasChar before rendering 'space' chars  

  - Pascal online compiller + console
    https://rextester.com/l/pascal_online_compiler
    https://www.jdoodle.com/execute-pascal-online

- Z: Assets filename completion routine **(done)**
  - char* getAssetsFileEntry(char* assetName) 
- **BMP Support :** 
  - before :
    Le croquis utilise 96188 octets (9%) de l'espace de stockage de programmes. Le maximum est de 1048576 octets.
    Les variables globales utilisent 85148 octets (32%) de mémoire dynamique, ce qui laisse 176996 octets pour les variables locales. Le maximum est de 262144 octets.
  - after : 
    Le croquis utilise 97364 octets (9%) de l'espace de stockage de programmes. Le maximum est de 1048576 octets.
    Les variables globales utilisent 85148 octets (32%) de mémoire dynamique, ce qui laisse 176996 octets pour les variables locales. Le maximum est de 262144 octets.

  - done **(minimal)**
  - have to support smaller BMP w/ location (& no screen rotation)
- **BPP & PCT support**
  - todo
- Only one showImage() routine w/ file ext. detection (.BMP, .PCT, .BPP)

  - started in Bdoscall for drawing wallpapers


- Console Mode Switcher **(DONE)**

    - 40 to 80 cols
    - (80 cols current font doesn't have lowerCase chars)
    - make a BDOS interrupt entry ... (maybe later)
    - or make a specific esc char sequence ( **0x7F** 127 (seems that console is 7bits per default) )
      - can use \$> ? chr\$(127) with mBASIC

  

- VTxx emulation

  - http://matthieu.benoit.free.fr/68hc11/vt100.htm
  - https://espterm.github.io/docs/vt100.codes.txt
  - not really as simple as that ...
  - <ESC> B1  =====> try : will begin color
  - <ESC> C1  =====> try : will end color****
  - <Esc> 26 is CLS
  - **WORKS :** 
    
    - **TURBO** (Pascal) works (even if could be better)
    - **OS** works
  - **DOESN'T WORKS:**
      
      - TURBO inside editor doesn't works very well (try **Ctrl k + d** to exit)
      - cause some different escapes
  - **in Progress**
    
    - **T**ext**E**ditor (nearly OK)
    
  - **small BASIC script** to handle VTxx
  
      - ? chr$(7)   BELL
      - ? chr$(26)  CLS
      - ? chr$(127) CONSOLE/MODE
      - ? chr$(27)+"B1"+"C"chr$(27)+"C1"+"oucou"
  
      


- **Keyboard support**

- - Microsoft XBOX 360 Chatpad

    - stdIO handling (Serial IN / Chatpad IN -- Serial OUT / TFT OUT)

    - Spe keys

      - will need to have a **CTRL like key** (ex. for **TE**)

      - greenSquare & redCircle are all used to make speChars

      - **no ESC key** // a bckSp key 

      - have a -peopleKey- that could emulate CTRL Key

      - may **-peopleKey- + [<-]** could do **ESC**

      - then **-peopleKey- + [->]** could do another spe key 

        - maybe YATL specific hdl : 

        - [MENU]

          \> TOGGLE CONSOLE MODE

          \> SELF TEST

          \> REBOOT

- cpm.h l. 736
  
- free Bdos hooks (225-229 inclusive are free)
  
- Z machine for CPM
  - http://www.z80.eu/adventures.html
  - have Zork1 on D:
  
- Pascal & realted resources
  - https://www.pascaland.org/pascall.htm
  - http://www.retroarchive.org/cpm/lang/lang.htm
  - https://www.pcengines.ch/tp3.htm

- **Buzzer support**
  
  - <Esc> 7 make bell tone
- to " play 'a#bbca' " could use a tty escSeq ?
  
  - T5K uses the following format <u8 noteIdx> <u16 duration> => 5KB / 3 => ~1700 notes (have headers)
- T53 uses => <u16 freq><u16 dur><u16 wait>
  
  - to play "MARIO.T53" could have something like :
    ? chr$(27)+ "$" + "MARIO.T53" +"!"
    ? chr$(27)+ "$" + "MONKEY.T5K" +"!"
  ? chr$(27)+ "$" + "A#BBCD" +"!" **(done)**
  
  - limited this system to 64 bytes !
  - by default : it will lock the system while playing
  - later : make a Threaded version (/!\\ with delay(..) & Timers )
  



- **LED support**

  


- **Usefull commands**
  
- > PIP B:ED.COM=A:ED.COM
Copy the disk file "ED.COM" from disk "A" to the file "ED.COM" on disk "B". The source file and the destination file will use the current user area code.
A shorter form of the command could be used because the destination file name is the same as the source file name.
  
- > PIP B:=A:ED.COM 
  A short version of the previous example.
