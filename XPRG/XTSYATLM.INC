{.PA}
{ Xtase YATL @May 2019 }
{* Xtase specific BdosCall : [225..229] *}

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

procedure volMp3(volum : integer);
begin
 volum := (4 * 256) + volum;
 Bdos(227, volum );
end;

{ -eof- }