program cube;

{$I XTSYATLG.INC gfx control routines}

const
  NAPPMAARA=6; { nb of key controls }
  MI_W = 239;
  MI_H = 159;
  C_BLK = 0;
  C_WHT = 1;
  CUB_SIZ = 50;
  { SPEED = 0.03; }
  SPEED = 2.0; { in deg now }

  PI = 3.141593;

var PISTE:ARRAY[1..8,1..3] OF INTEGER;
    OLDX,OLDY,OLDZ,UUSIX,UUSIY,UUSIZ:ARRAY[1..8] OF INTEGER;
    VIIVE,I:INTEGER;
    XANGLE,YANGLE,ZANGLE:REAL;
    NAPP:ARRAY[1..NAPPMAARA]OF BOOLEAN;
    choiceCh: char;
    var _vCos,_vSin:ARRAY[1..91] OF REAL;

{ default cos/sin are in radians }
PROCEDURE initTrg;
 var ang : INTEGER;
BEGIN
 FOR I:=1 TO 91 DO BEGIN
   ang := I-1;
   _vCos[I] := cos(ang / 180.0 * PI);
   _vSin[I] := sin(ang / 180.0 * PI);
 END;
END;

FUNCTION normAngle(ang:real): INTEGER;
 var ang2: INTEGER;
begin
  { write(' angR '); writeln(ang:0:2); }
  { bad floor() replacement see frac() too }
  { !! mod avoids sign }
  ang2 := round(ang) mod 360;
  { ang2 is now positive }
  if ( ang < 0 ) then
   ang2 := 360 - ang2;
  { write(' angI '); writeln(ang2); }
  normAngle := ang2;
end;


FUNCTION _cosX(ang2:INTEGER): real;
 var ang3: INTEGER;
 var sig: REAL;
begin
  if ( ang2 >= 0) AND (ang2 <= 90 ) then
   begin
    ang3 := ang2;
    sig := 1.0;
   end
  else if ( ang2 > 90) AND (ang2 <= 180 ) then
   begin
    ang3 := 90-(ang2-90);
    sig := -1.0;
   end
  else if ( ang2 > 180) AND (ang2 <= 270 ) then
   begin
    ang3 := (ang2-180);
    sig := -1.0;
   end 
  else if ( ang2 > 270) AND (ang2 < 360 ) then
   begin
    ang3 := 90-(ang2-270);
    sig := 1.0;
   end; 

  _cosX := sig * _vCos[(ang3)+1]
end;


FUNCTION _sinX(ang2: INTEGER): real;
 var ang3: INTEGER;
 var sig: REAL;
begin
  if ( ang2 >= 0) AND (ang2 <= 90 ) then
   begin
    ang3 := ang2;
    sig := 1.0;
   end
  else if ( ang2 > 90) AND (ang2 <= 180 ) then
   begin
    ang3 := 90-(ang2-90);
    sig := 1.0;
   end
  else if ( ang2 > 180) AND (ang2 <= 270 ) then
   begin
    ang3 := (ang2-180);
    sig := -1.0;
   end 
  else if ( ang2 > 270) AND (ang2 < 360 ) then
   begin
    ang3 := 90-(ang2-270);
    sig := -1.0;
   end; 

  _sinX := sig * _vSin[(ang3)+1]
end;

PROCEDURE PYORITA(X,Y,Z:INTEGER; XAN,YAN,ZAN:REAL; VAR NEWX,NEWY,NEWZ:INTEGER);
var
 cXAN,sXAN,cYAN,sYAN,cZAN,sZAN : REAL;
 iXAN,iYAN,iZAN: INTEGER;
BEGIN
  iXAN := normAngle(XAN);
  iYAN := normAngle(YAN);
  iZAN := normAngle(ZAN);

  cXAN := _COSX(iXAN);
  sXAN := _SINX(iXAN);
  cYAN := _COSX(iYAN);
  sYAN := _SINX(iYAN);
  cZAN := _COSX(iZAN);
  sZAN := _SINX(iZAN);


  NEWX:=ROUND(X*(sYAN*sZAN*sXAN+cYAN*cXAN)+Y*(cZAN*sXAN)+
  Z*(cYAN*sZAN*sXAN-cXAN*sYAN));
  NEWY:=ROUND(X*(cXAN*sYAN*sZAN-cYAN*sXAN)+Y*(cZAN*cXAN)+
  Z*(cYAN*cXAN*sZAN+sYAN*sXAN));
  { NEWZ:=ROUND(X*(cYAN*sZAN*sXAN-cXAN*sYAN)+Y*(-sZAN)+
  Z*(cYAN*cZAN)); }
END;

PROCEDURE PLAY;
var
  tx, ty : INTEGER;
BEGIN
  IF NAPP[1]=TRUE THEN XANGLE:=XANGLE+SPEED;
  IF NAPP[2]=TRUE THEN YANGLE:=YANGLE+SPEED;
  IF NAPP[3]=TRUE THEN XANGLE:=XANGLE-SPEED;
  IF NAPP[4]=TRUE THEN YANGLE:=YANGLE-SPEED;
  IF NAPP[5]=TRUE THEN ZANGLE:=ZANGLE+SPEED;
  IF NAPP[6]=TRUE THEN ZANGLE:=ZANGLE-SPEED;

  FOR I:=1 TO 8 DO BEGIN
    OLDX[I]:=UUSIX[I];
    OLDY[I]:=UUSIY[I];
    OLDZ[I]:=UUSIZ[I];
    PYORITA(PISTE[I,1],PISTE[I,2],PISTE[I,3],XANGLE,YANGLE,ZANGLE,UUSIX[I],UUSIY[I],UUSIZ[I]);
  END;

  drawLine(OLDX[1]+MI_W,OLDY[1]+MI_H,OLDX[2]+MI_W,OLDY[2]+MI_H,C_BLK);
  drawLine(OLDX[1]+MI_W,OLDY[1]+MI_H,OLDX[4]+MI_W,OLDY[4]+MI_H,C_BLK);
  drawLine(OLDX[1]+MI_W,OLDY[1]+MI_H,OLDX[5]+MI_W,OLDY[5]+MI_H,C_BLK);
  drawLine(OLDX[2]+MI_W,OLDY[2]+MI_H,OLDX[3]+MI_W,OLDY[3]+MI_H,C_BLK);
  drawLine(OLDX[2]+MI_W,OLDY[2]+MI_H,OLDX[6]+MI_W,OLDY[6]+MI_H,C_BLK);
  drawLine(OLDX[3]+MI_W,OLDY[3]+MI_H,OLDX[7]+MI_W,OLDY[7]+MI_H,C_BLK);
  drawLine(OLDX[3]+MI_W,OLDY[3]+MI_H,OLDX[4]+MI_W,OLDY[4]+MI_H,C_BLK);
  drawLine(OLDX[4]+MI_W,OLDY[4]+MI_H,OLDX[8]+MI_W,OLDY[8]+MI_H,C_BLK);
  drawLine(OLDX[5]+MI_W,OLDY[5]+MI_H,OLDX[6]+MI_W,OLDY[6]+MI_H,C_BLK);
  drawLine(OLDX[5]+MI_W,OLDY[5]+MI_H,OLDX[8]+MI_W,OLDY[8]+MI_H,C_BLK);
  drawLine(OLDX[6]+MI_W,OLDY[6]+MI_H,OLDX[7]+MI_W,OLDY[7]+MI_H,C_BLK);
  drawLine(OLDX[7]+MI_W,OLDY[7]+MI_H,OLDX[8]+MI_W,OLDY[8]+MI_H,C_BLK);
  tx := OLDX[1]+MI_W;
  ty := OLDY[1]+MI_H;
  if ( (tx > 0) and (tx < 480-20) and (ty > 0) and (ty < 320-20) ) then
    drawCircle(tx, ty,5, 1, C_BLK);

  drawLine(UUSIX[1]+MI_W,UUSIY[1]+MI_H,UUSIX[2]+MI_W,UUSIY[2]+MI_H,C_WHT);
  drawLine(UUSIX[1]+MI_W,UUSIY[1]+MI_H,UUSIX[4]+MI_W,UUSIY[4]+MI_H,C_WHT);
  drawLine(UUSIX[1]+MI_W,UUSIY[1]+MI_H,UUSIX[5]+MI_W,UUSIY[5]+MI_H,C_WHT);
  drawLine(UUSIX[2]+MI_W,UUSIY[2]+MI_H,UUSIX[3]+MI_W,UUSIY[3]+MI_H,C_WHT);
  drawLine(UUSIX[2]+MI_W,UUSIY[2]+MI_H,UUSIX[6]+MI_W,UUSIY[6]+MI_H,C_WHT);
  drawLine(UUSIX[3]+MI_W,UUSIY[3]+MI_H,UUSIX[7]+MI_W,UUSIY[7]+MI_H,C_WHT);
  drawLine(UUSIX[3]+MI_W,UUSIY[3]+MI_H,UUSIX[4]+MI_W,UUSIY[4]+MI_H,C_WHT);
  drawLine(UUSIX[4]+MI_W,UUSIY[4]+MI_H,UUSIX[8]+MI_W,UUSIY[8]+MI_H,C_WHT);
  drawLine(UUSIX[5]+MI_W,UUSIY[5]+MI_H,UUSIX[6]+MI_W,UUSIY[6]+MI_H,C_WHT);
  drawLine(UUSIX[5]+MI_W,UUSIY[5]+MI_H,UUSIX[8]+MI_W,UUSIY[8]+MI_H,C_WHT);
  drawLine(UUSIX[6]+MI_W,UUSIY[6]+MI_H,UUSIX[7]+MI_W,UUSIY[7]+MI_H,C_WHT);
  drawLine(UUSIX[7]+MI_W,UUSIY[7]+MI_H,UUSIX[8]+MI_W,UUSIY[8]+MI_H,C_WHT);
  
  tx := UUSIX[1]+MI_W;
  ty := UUSIY[1]+MI_H;

  if ( (tx > 0) and (tx < 480-20) and (ty > 0) and (ty < 320-20) ) then
   begin
    drawSprite( 0, tx,ty ); 
    { drawCircle(tx, ty,5, 1, 3); }
   end;

END;

BEGIN
  initTrg;
  {
  for i:=0 to 360 do begin
    drawLine( 30, 30, 30+round( 15.0 * _cosX(i) ), 30-round( 15.0 * _sinX(i) ), C_WHT );
  end;
  } 

  PISTE[1,1]:=CUB_SIZ;
  PISTE[1,2]:=CUB_SIZ;
  PISTE[1,3]:=CUB_SIZ;
  PISTE[2,1]:=CUB_SIZ;
  PISTE[2,2]:=-CUB_SIZ;
  PISTE[2,3]:=CUB_SIZ;
  PISTE[3,1]:=-CUB_SIZ;
  PISTE[3,2]:=-CUB_SIZ;
  PISTE[3,3]:=CUB_SIZ;
  PISTE[4,1]:=-CUB_SIZ;
  PISTE[4,2]:=CUB_SIZ;
  PISTE[4,3]:=CUB_SIZ;
  PISTE[5,1]:=CUB_SIZ;
  PISTE[5,2]:=CUB_SIZ;
  PISTE[5,3]:=-CUB_SIZ;
  PISTE[6,1]:=CUB_SIZ;
  PISTE[6,2]:=-CUB_SIZ;
  PISTE[6,3]:=-CUB_SIZ;
  PISTE[7,1]:=-CUB_SIZ;
  PISTE[7,2]:=-CUB_SIZ;
  PISTE[7,3]:=-CUB_SIZ;
  PISTE[8,1]:=-CUB_SIZ;
  PISTE[8,2]:=CUB_SIZ;
  PISTE[8,3]:=-CUB_SIZ;

  XANGLE := 0.0;
  YANGLE := 0.0;
  ZANGLE := 0.0;
  
 drawBmp('!sprite1.bmp'); {* loadSomeSprites *}
 defineSprite(0, 0, 1, 19, 19);  { upperL }

  clrScr;
  REPEAT

    XANGLE := XANGLE + SPEED;
    YANGLE := YANGLE + SPEED;
    ZANGLE := ZANGLE + SPEED;

    PLAY;

    choiceCh := '*';
    if ( KeyPressed ) then
        Read(Kbd,choiceCh);

    for i:=1 to NAPPMAARA do
    begin
      NAPP[i]:=FALSE;
    end;

    if ( choiceCh = 'a' ) then
      NAPP[1]:=TRUE;
    if ( choiceCh = 's' ) then
      NAPP[2]:=TRUE;
    if ( choiceCh = 'd' ) then
      NAPP[5]:=TRUE;

  UNTIL ( choiceCh = 'q' );
 
END.