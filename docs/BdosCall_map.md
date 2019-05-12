# BdosCall map

| Reg. | Reg. Value                                                   | function                  |
| ---- | ------------------------------------------------------------ | ------------------------- |
| 225  | \<TP3 String address>                                        | draws a picture wallpaper |
| 226  | - 0 **HIGH_REG** mode **LOW_REG** set Console colorSet mode<br />- 1 **HIGH_REG** mode **LOW_REG** set Console columns mode (80/53) | deals w/ console & stdIO  |
| 227  | - 0 **HIGH_REG** stop playback<br /> - 1 **HIGH_REG** pause / unpause<br /> - 2 **HIGH_REG**  next<br /> - 3 **HIGH_REG** previous<br /> - 4 **HIGH_REG** set volume (**LOW_REG** 0..30 volume rate)<br /> - 5 **HIGH_REG** play a demo MP3<br /> - (128/64)<<8 + trackNum (0..9999) loop/play an MP3 | deals w/ mp3 module       |

