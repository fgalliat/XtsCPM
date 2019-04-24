{* Turn a LED on or off *}

program led;

const
  INPUT = 0;
  OUTPUT = 1;

var
  pin, value: Integer;

begin
  write('Begin');

  {*
  write('Enter pin number: ');
  readln(pin); *}
  {* write('Enter value (0 = LOW, 1 = HIGH): ');
  readln(value); *}

  Bdos(220, (pin shl 8) + OUTPUT);
  {* beware 500 is nearly immediate *}
  pin := 14;

  value := 1;
  Bdos(222, (pin shl 8) + value);
  delay(5000);

  value := 0;
  Bdos(222, (pin shl 8) + value);
  delay(5000);

  value := 1;
  Bdos(222, (pin shl 8) + value);
  delay(5000);

  value := 0;
  Bdos(222, (pin shl 8) + value);
  delay(5000);

  write('Finished');


end.
