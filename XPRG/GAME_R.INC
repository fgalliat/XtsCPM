{.PA}
{ Xtase YATL AdventureGame @Jul 2019 }
{ Routines include @Jul 2019 }

var 
  lcap : array[0..10] of string[32]; {label}
  acap : array[0..10] of integer; {addr}
  icap : integer; {nb}
  
procedure parseCapas(capas : aStr);
var
  tmpStr: string[32];
  zz,err: integer;
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
    tmpStr := '';

    for i:= 1 to tlen do
    begin
      if ( capas[i] = ',' ) then 
       begin 
        val(tmpStr, acap[icap],err);
        {writeln( ' $ '+tmpStr);}
        icap := icap + 1; zz:=0; 
        tmpStr := '';
       end
      else if ( capas[i] = ':' ) then 
       begin
        zz:=1;
        lcap[icap] := tmpStr;
        {writeln(' > '+lcap[icap]);}
        tmpStr := '';
       end
      else
       begin
         tmpStr := tmpStr + capas[i];
       end;
    end;
    val(tmpStr, acap[icap],err);
    {writeln( ' $ '+tmpStr);}
end;

{ 'a' -> 1 }
function getChoiceNum(ch : char) : integer;
begin
  if ( ch = '' ) then getChoiceNum := 0;
  getChoiceNum := ord(ch) - ord('a') + 1;
end;
