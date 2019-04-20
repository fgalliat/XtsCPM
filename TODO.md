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
      - allocate memseg (1590 bytes for 53x30)
      - fill all w/ a character
      - display whole mem-console
      - try to scroll
      - see rendering time 
    - then try bigger memseg that screen caps
      - ex. 80x25 on 53x25
      - just for compatibility of softwares display
    - try to find the tinyFont for that lib
