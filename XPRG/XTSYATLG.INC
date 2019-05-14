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
procedure drawRect(x,y,w,h,stroke: integer; color: integer);
 var 
   buffer : string[16];
   c0,c1: integer;
begin
 buffer[1] := Chr(16); { cause not ass. as a reg. str. }
 buffer[2] := #127; { readed as Str[1] }
 buffer[3] := #1; { rect }
 buffer[4] := Chr( stroke ); { 0-draw/1-fill }
 c0 := color div 256;
 c1 := color mod 256;
 buffer[5] := Chr( c0 );
 buffer[6] := Chr( c1 );
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

{* -EOF- *}