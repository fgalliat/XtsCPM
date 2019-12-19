Program testBdos;
{* https://www.onlinegdb.com/online_pascal_compiler *}

function callBdos(bd,a1,a0 : integer) : integer;
begin
  writeln('BDOS ', bd);
  writeln('A1 ', a1);
  writeln('A0 ', a0);

  { callBdos :=  0; }
  callBdos :=  Bdos(bd, (a1*8)+a0 );
end;


Var
  bdos : integer;
  a1 : integer;
  a0 : integer;
  res : integer;

begin
   bdos := 229;
   a1 := 0; 
   a0 := 0; 

   write('Bdos num '); readln(bdos);
   write('Bdos HI  '); readln(a1);
   write('Bdos LOW '); readln(a0);

    res := callBdos(bdos, a1, a0);
    writeln( res );
end.