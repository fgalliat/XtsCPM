{.PA}
{ Xtase YATL AdventureGame @Jul 2019 }
{ Place include @Jul 2019 }


const 
  nbPlace = 4;


var
 place : array[1..nbPlace] of string[32];
 placeVisit : array[1..nbPlace] of integer; 
 i : integer;

procedure initPlaces;
begin
  place[1] := 'The forest';
  place[2] := 'The mountain';
  place[3] := 'The river';
  place[4] := 'The castle';

  for i := 1 to nbPlace do 
    placeVisit[i] := 0;
end;


