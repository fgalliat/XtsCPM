program HelloWorld;

type aStr = string[255];

var 
  lcap : array[0..10] of string[32]; {label}
  acap : array[0..10] of integer; {addr}
  icap : integer; {nb}
  i: integer;
  
procedure parseCapas(capas : aStr);
var
  tmpStr: string;
  zz: integer;
  tlen,i  : integer;
begin
    {writeln(capas);}
    
    tlen := length( capas );

    if ( tlen = 0 ) then
    begin
     icap := 0;
     exit;
    end;

    icap := 1;
    zz:=0;

    for i:= 1 to tlen do
    begin
      if ( capas[i] = ',' ) then 
       begin 
        val(tmpStr, acap[icap]);
        writeln( ' $ '+tmpStr);
        icap := icap + 1; zz:=0; 
        tmpStr := '';
       end
      else if ( capas[i] = ':' ) then 
       begin
        zz:=1;
        lcap[icap] := tmpStr;
        writeln(' > '+lcap[icap]);
        tmpStr := '';
       end
      else
       begin
         tmpStr := tmpStr + capas[i];
       end;
    end;
    val(tmpStr, acap[icap]);
    writeln( ' $ '+tmpStr);
    
end;


begin
  parseCapas('Yes:12,No:10,Maybe:1');

  write('nb of choices : ');
  write(icap);
  writeln;
  
  for i:=1 to icap do
  begin
    writeln( '@ '+lcap[i] );
  end;
  
end.



