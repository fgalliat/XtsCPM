Program WiFiMananger;

type aStr = string[255];

procedure DrawBox(X1,Y1,X2,Y2: Integer);
 var I: Integer; 
 begin 
   GotoXY(X1, Y1); 
   for I := X1 to X2 do write ( '-' ); 
   for I := Y1+1 to Y2 do 
     begin 
       GotoXY(X1,I); Write('!'); 
       GotoXY(X2,I); Write('!'); 
     end; 
   GotoXY(X1,Y2);
   for I := X1 to X2 do Write('-'); 
 end; 

 procedure DrawWin(X1, Y1, X2, Y2: integer; title :aStr);
 var I: integer;
 begin
   GotoXY(X1, Y1); 
   for I := X1 to X2 do write ( '-' ); 
   I := Y1+1;
   GotoXY(X1,I); Write('!'); 
   GotoXY(X2,I); Write('!'); 

   DrawBox(X1,Y1+2,X2,Y2);
   GotoXY(X1+2,I); Write(title);
 end;

 { =========================== }

 const 
   WIFI_BDOS = 228;

 function wifi_getIP : aStr;
 var ip : aStr;
     bdRes : integer;
 begin
   bdRes := Bdos(WIFI_BDOS, (65 shl 8)+0 );
   if ( bdRes = 0 ) then
   begin
     wifi_getIP := 'Error';
   end {* no ';' *}
  else
  begin
    readln(ip);
    wifi_getIP := ip;
  end;
 end;


var
   choice : integer;
   title : string[80];
   x,y,i : integer;
begin

  ClrScr;
  {        345678901234567890123456789012345678901234567890123456789012345678901234567890}
  title:= 'xxx.xxx.xxx.xxx              xxxxxxxxxxxxx                            xx:xx';


  DrawWin(1, 1, 80, 25, title);
  i := 1; x := 5; y:= 5;
  GotoXY(x, y+i-1); Write('1. Get IP / SSID'); i := i+1;
  GotoXY(x, y+i-1); Write('2. Connect to an AP'); i := i+1;
  GotoXY(x, y+i-1); Write('3. Start a Soft AP'); i := i+1;
  GotoXY(x, y+i-1); Write('4. Wget time'); i := i+1;
  GotoXY(x, y+i-1); Write('5. Wget Weather'); i := i+1;
  GotoXY(x, y+i-1); Write('6. Wget Add note'); i := i+1;
  GotoXY(x, y+i-1); Write('7. Wget List notes'); i := i+1;
  GotoXY(x, y+i-1); Write('8. '); i := i+1;
  GotoXY(x, y+i-1); Write('9. '); i := i+1;
  GotoXY(x, y+i-1); Write('0. Exit'); i := i+1;

  i := 1;

  GotoXY(x, 23); Write('> ');
  readln(choice);

  GotoXY(x + 10, 23);
  if ( choice = 1 ) then
    writeln( wifi_getIP );

end.