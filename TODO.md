## TODOs


 - TTY emulation on TFT Screen
   - tinyFont 3x5 => 4x6 => @320x240 => 80x40 chars
   - regular font 6x8 => 53x30 chars
   - ILI9341_t3n scrolling impl. is very slow for now
     - when no scrolling & screen overflow : seems to deadlock the teensy
     - have to make a console in RAM (80x30 gives 3200 bytes)
     - use that cls/write abstraction to provide VT100 escape chars emulation ?
        - ^C // \b // \t // \r \n
        - cls
        - color
        - locate ? / bold ? (could be just diff color)
          - if text has attribs I'll have to store those
          - by spans / by chars :-( => on WSL & ubuntu that chars attrs seems to be ignored ....
    - 1st, try a benchmark : 
      - allocate memseg (1590 bytes for 53x30) **[v]**
      - fill all w/ a character **[v]**
      - display whole mem-console **[v]**
      - try to scroll **[v]**
      - see rendering time **[nearly good]** 
    - then try bigger memseg that screen caps
      - ex. 80x25 on 53x25
      - just for compatibility of softwares display
    - try to find the tinyFont for that lib



- VT52 emulation

  - http://matthieu.benoit.free.fr/68hc11/vt100.htm

  - not really that ...

  - <ESC> B1  =====> try : will begin color

  - <ESC> C1  =====> try : will end color

  - ##      VT52 Compatable Mode   

    ```
    ESC A           Cursor up
    ESC B           Cursor down
    ESC C           Cursor right
    ESC D           Cursor left
    ESC F           Special graphics character set
    ESC G           Select ASCII character set
    ESC H           Cursor to home
    ESC I           Reverse line feed
    ESC J           Erase to end of screen
    ESC K           Erase to end of line
    ESC Ylc         Direct cursor address (See note 1)
    ESC Z           Identify (See note 2)
    ESC =           Enter alternate keypad mode
    ESC >           Exit alternate keypad mode
    ESC 1           Graphics processor on (See note 3)
    ESC 2           Graphics processor off (See note 3)
    ESC <           Enter ANSI mode
    
    Note 1:  Line and column numbers for direct cursor addresses are single
             character codes whose values are the desired number plus 37 octal.
             Line and column numbers start at 1.
    
    Note 2:  Response to ESC Z is ESC/Z.
    
    Note 3:  Ignored if no graphics processor stored in the VT100
    ```