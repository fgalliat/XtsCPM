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

procedure drawRect(x,y,w,h:integer);
 var buffer : string[16];
begin
 { remember starts @[2] ? [0] nominal len }
 buffer[2] := #127;
 buffer[3] := #1;
 buffer[4] := #0;
 buffer[5] := #255; { color }
 buffer[6] := #255; { color }
 buffer[7] := #0;   { x }
 buffer[8] := #10;  { x }
 { y,w,h }

 Bdos(229, addr(buffer) );
end;

{* Mp3 device section *}
procedure playMp3(track : integer);
begin
 if ( track < 1 ) then
   exit;
 { track := track + (128*256); loop }
 track := track + (64*256);
 Bdos(227, track );
end;

procedure stopMp3;
begin
 Bdos(227, 0 shl 8 );
end;

procedure pauseMp3;
begin
 Bdos(227, 1 shl 8 );
end;

procedure nextMp3;
begin
 Bdos(227, 2 shl 8 );
end;

procedure prevMp3;
begin
 Bdos(227, 3 shl 8 );
end;

{* Console Section *}
procedure setConsoleColors(mode : integer);
begin
 Bdos(226, (0 shl 8) + mode );
end;

procedure setConsoleCols(mode : integer);
begin
 Bdos(226, (1 shl 8) + mode );
end;

{* proc w/o args don't have parent. *}
procedure clScr;
begin
  write( #26 );
end;

procedure bell;
begin
  write( #7 );
end;

procedure toggleConsoleSize;
begin
  write( #127 );
end;

procedure txtColor;
begin
  write( #27 );
  write( 'B1' );
end;

procedure txtNormal;
begin
  write( #27 );
  write( 'C1' );
end;

{* ex. play('AC#D'), play('MONKEY.T5K'), play('MARIO.T53') *}
procedure play(text : aStr);
begin
  write( #27 );
  write( '$' );
  write( text );
  write( '!' );
end;

procedure cursor(x : integer; y : integer);
begin
  write( #27 );
  write( '[' );
  write( (y+1) );
  write( ';' );
  write( (x+1) );
  write( 'H' );
end;

{* -EOF- *}