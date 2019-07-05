program download;
{* SubMCU Downloader *}

var
  ok: integer;
begin
  ok := Bdos( 228, 5 shl 8 );
  writeln( ok );
end.