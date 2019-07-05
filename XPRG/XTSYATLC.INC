{.PA}
{ Xtase YATL @May 2019 }
{* Xtase specific BdosCall : [225..229] *}


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