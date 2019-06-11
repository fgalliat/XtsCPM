program download;
{* Serial Downloader *}

var
  ok: integer;
begin
  ok := Bdos( 228, 4 shl 8 );
  writeln( ok );
end.