## Reuse of Vtech Mobigo Gaming Console

### Sliding console with Keyboard

**Keyboard**
  - 10 cols x 5 rows matrix Keyboard w/ 35 keys
  - @ this time : 9x5 really useable for 34 keys

  - driven by an SX1509
    - 5rows as OUTPUT
    - 10cols as INPUT (no pullUp)

    - Behavior to read 1 key :
      - send HIGH for only 1 ROW
      - read each COL input
      - send LOW for all ROWS
      - gives a ROW,COL address

  - TODO :
    - make some key maps (regular, shift, ...)
    - change layout from ABCDEF (french edition) to AZERTY or QWERTY
    - make buffer for keys **(done)**
    - handle key combos (nums, symbols, Ctrl)
    - find missing key : [ ? ]
      - could act as Ctrl, else have to find another combo
    - allow multiple key read on same row
    - distinct pollKey Vs readKey **(done)**
    - re-use fold/open detector push  contact -> wire it on SX1509
    - not able to scan combos yet -> make as remanant ????
    