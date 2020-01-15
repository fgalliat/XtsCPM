program Test;
{* Test Leds Bdos *}

procedure led(value:integer);
begin
  if ( value > 255 ) then
    value := 255;
  Bdos( 228, (3 shl 8) + value );
end;


var
  ret: integer;
  i  : integer;
begin
  writeln( 'Testing ...' );
  { ret := Bdos( 229, 0 shl 8 ); }
  for i := 0 to 15 do
  begin
    led( i );
    delay(1800);
  end;
end.