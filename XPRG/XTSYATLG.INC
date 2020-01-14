{.PA}
{ Xtase YATL @May 2019 }
{* Xtase specific BdosCall : [225..229] *}

type aStr = string[255];

{* procedure Bdos(reg: integer; val: word);
begin
end; *}

procedure drawBmp(text : aStr);
begin
 Bdos(225, addr(text) );
end;

{ numFrame is 1based }
procedure drawPakImage(text : aStr; numFrame: integer; x,y:integer);
var
  xx: string[4];
  yy: string[4];
begin
 str(x, xx);
 str(y, yy);
 drawBmp(chr(numFrame)+xx+','+yy+','+text+'.pak'); 
end;

{ color: word doesn't work on TP3 CP/M }
procedure defineSprite(num, x,y,w,h : integer);
 var 
   buffer : string[16];
   color : integer;
   stroke : integer;
   shape : integer;
begin
 color := 0;
 stroke := 0;
 shape := 1;
 buffer := '';

 buffer[0] := Chr(16); { cause not ass. as a reg. str. }
 buffer[1] := #127;
 buffer[2] := #128; { readed as Str[1] }
 buffer[3] := Chr( shape );
 buffer[4] := Chr( stroke ); { 0-draw/1-fill }
 buffer[5] := Chr( color div 256 );
 buffer[6] := Chr( color mod 256 );
 buffer[7] := Chr( x div 256 );
 buffer[8] := Chr( x mod 256 );
 buffer[9] := Chr( y div 256 );
 buffer[10] := Chr( y mod 256 );
 buffer[11] := Chr( w div 256 );
 buffer[12] := Chr( w mod 256 );
 buffer[13] := Chr( h div 256 );
 buffer[14] := Chr( h mod 256 );

 buffer[15] := Chr( num );

 Bdos(225, addr(buffer) );
end;

procedure drawSprite(num, x,y : integer);
 var 
   buffer : string[16];
   color : integer;
   stroke : integer;
   shape : integer;
begin
 color := 0;
 stroke := 0;
 shape := 2;
 buffer := '';
 buffer[0] := Chr(16); { cause not ass. as a reg. str. }
 buffer[1] := #127;
 buffer[2] := #128; { readed as Str[1] }
 buffer[3] := Chr( shape );
 buffer[4] := Chr( stroke ); { 0-draw/1-fill }
 buffer[5] := Chr( color div 256 );
 buffer[6] := Chr( color mod 256 );
 buffer[7] := Chr( x div 256 );
 buffer[8] := Chr( x mod 256 );
 buffer[9] := Chr( y div 256 );
 buffer[10] := Chr( y mod 256 );

 buffer[11] := Chr( num );

 Bdos(225, addr(buffer) );
end;

var 
  shBuff : string[16];

procedure _drawShape(shape, x,y,w,h,stroke: integer; color: integer);
{ var 
   shBuff : string[16]; }
begin
 shBuff := '';
 shBuff[0] := Chr(16); { cause not ass. as a reg. str. }
 shBuff[1] := #127;
 shBuff[2] := #127; { readed as Str[1] }
 shBuff[3] := Chr( shape );
 shBuff[4] := Chr( stroke ); { 0-draw/1-fill }
 shBuff[5] := Chr( color div 256 );
 shBuff[6] := Chr( color mod 256 );
 shBuff[7] := Chr( x div 256 );
 shBuff[8] := Chr( x mod 256 );
 shBuff[9] := Chr( y div 256 );
 shBuff[10] := Chr( y mod 256 );
 shBuff[11] := Chr( w div 256 );
 shBuff[12] := Chr( w mod 256 );
 shBuff[13] := Chr( h div 256 );
 shBuff[14] := Chr( h mod 256 );

 Bdos(225, addr(shBuff) );
end;

procedure drawRect(x,y,w,h,stroke: integer; color: integer);
begin
 _drawShape(1, x,y,w,h,stroke,color);
end;

procedure drawCircle(x,y,r,stroke: integer; color: integer);
begin
 _drawShape(2, x,y,r,0,stroke,color);
end;

procedure drawLine(x,y,x2,y2: integer; color: integer);
begin
 _drawShape(3, x,y,x2,y2,0,color);
end;


{* -EOF- *}