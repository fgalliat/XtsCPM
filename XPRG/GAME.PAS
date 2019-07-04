program Game1;

{ Xtase YATL AdventureGame @Jul 2019 }


var
 place : array[1..4] of string;
 placeVisit : array[1..4] of integer;

 choice : string[32];
 i : integer;
 tmp: string[16];
 zz,err : integer;
begin

  place[1] := 'The forest';
  place[2] := 'The mountain';
  place[3] := 'The river';
  place[4] := 'The castle';

  for i := 1 to 4 do 
    placeVisit[i] := 0;

  zz := 0;  
  choice := '*';

  repeat

    for i := 1 to 4 do
    begin

      str(i, tmp);
      write( tmp +' '+ place[i] );
      str(placeVisit[i], tmp);
      write( ' You visited '+tmp+' times' );
      writeln;

    end;

    writeln( 'Your choice ?' );
    { readln(choice); }
    if ( zz = 0 ) then
      choice := '2';
    if ( zz > 0 ) then
      choice := 'q';
      

    if ( choice = 'q' ) then exit;
    if ( choice[1] in ['1'..'4'] ) then
    begin
      val( choice, i, err );
      placeVisit[i] := placeVisit[i] + 1;
    end;

    zz := zz+1;


  until ( choice = 'q' );




end.
