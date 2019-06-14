program telnetd;
{* Telnetd launcher *}

var
  ret: integer;
begin
  ret := Bdos( 228, 64 shl 8 );
  if ( ret = 1 ) then
     writeln( 'OK Telnetd Launched' );
  if ( ret = 0 ) then
     writeln( 'NOK Telnetd not Launched' );
end.