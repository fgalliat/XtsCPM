program reboot;
{* Reboot YATL *}

var
  ret: integer;
begin
  writeln( 'Rebooting ...' );
  ret := Bdos( 228, 1 shl 8 );
end.