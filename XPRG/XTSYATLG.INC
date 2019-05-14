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

{ color: word doesn't work on TP3 CP/M }
procedure _drawShape(shape, x,y,w,h,stroke: integer; color: integer);
 var 
   buffer : string[16];
begin
 buffer[0] := Chr(16); { cause not ass. as a reg. str. }
 buffer[2] := #127; { readed as Str[1] }
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

 Bdos(229, addr(buffer) );
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