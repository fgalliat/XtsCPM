{ INFOCOM INTERPRETER BY NOCASH 1993   }
{ SOURCE:CP/M VERSION C UPDATED 2.5.85 }
{ LANGUAGE:TURBO PASCAL 4.0            }

{ ABOUT THE ROOM-SUBROUTINES/PARAMETERS: as I decoded the CPM interpreter }
{ I thought these would be used for locations/rooms only, in fact they    }
{ seem to process various varibles... }

{ Also available: the nocash infocom interpreter for nintendo gameboy (!) }
{ contact:                                               }
{  -> Martin Korth, Ringheide 44, 21149 Hambrug, Germany }
{  -> nocash@work.de                                     }
{ noca$h updates & other projects: }
{  -> www.work.de/nocash           }

program infocom_interp;
uses crt,printer;
{$I-}
const
 defname  = '';         {----- or for example : ='\games\infocom\zork1'; }
 scrflg   = false;      {----- CHANGE HERE TO ENABLE SCRIPT -----}
type
 array6   = array[1..6] of byte;
 sectype  = array[0..$1ff] of byte;
 fileptr  = record reqsec:byte; reqinbl:word; end;
const
 ramsize  = $a000;
 chrlst   : string = '0123456789.,!?_#''"/\-:()';
 comnam0  : array[0..$f] of string =
            ('EXIACT1','EXIACT0','WRSTRPRG','EXIACT1WRSTR',
             '-','SAVE_GAME','RESTORE_GAME','RESTART',
             'EXIACTHEAP','SHRINKHEAP','QUIT','WR0D',
             'STATLINE','VERIFY','XXX','XXX');
 comnam1  : array[0..$f] of string =
            ('TST0PAR1','LOCBYT5','LOCBYT6','LOCBYT4',
             'STORDIRDATASIZ','INCWORD','DECWORD','WRSTR64',
             '-','STRANGESTUFF','DSPROOMNAME','EXITACTION',
             'RELJUMP','WRSTR128','MOVWORD','CPLP1STORE');
 comnam2  : array[0..$1f] of string =
            ('-','P1CPOTHERS','CPPARA21','CPPARA12',
             'DECCPP2','INCCPP2','CPP2LOCINF4','SAMEBITS',
             'ORPPSTOR','ANDPPSTOR','TSTROOMBIT','SETROOMBIT',
             'RESROOMBIT','STOREP2VP1','MORESTRANGE','STOREFRMPPW',
             'STOREFRMPPB','STORDIRDATERR','STORDIRPTR','STORENXTDIR',
             'ADDPARA','SUBPARA','MULPARA','DIVPARA',
             'MODPARA','XXX','XXX','XXX',
             'XXX','XXX','XXX','XXX');
 comnamn  : array[0..$f] of string =
            ('GOSUB','MOVP123W','MOVP123B','CHANGEDIRDATA',
             'PROMPT','WRPARA1CHR','WRPARA1DEZ','STORRNDP1',
             'PUSHPARAM1','STORHEAPVP1','XXX','XXX',
             'XXX','XXX','XXX','XXX');
var
 scriptf                : file of byte;
 datefdat,datefsav      : file of sectype;
 datefcon               : text;
 morecnt                : byte;
 fname                  : string;
 basebls,nextbl         : byte;
 seclist,bllist         : array[0..$ff] of byte;
 progptr,dataptr        : fileptr;
 ram                    : array[0..ramsize] of byte;
 heap                   : array[1..$e0] of word;
 heapcnt,oldheapcnt     : byte;
 xxmemf                 : array[0..$e] of word;
 prfxstrs,srsmem        : word;
 directoutput           : boolean;
 wrbufmem               : string;
 param1,param2,param3   : word;
 param4                 : word;
 numofparams            : byte;
 scrxsiz,scrysiz        : byte;
 restartflg             : boolean;
 skiplf,usedline        : boolean;
 trace,tracekey         : boolean;
{----------------------------}
{ ram[ 0..basebls*$200-1 ]   basemem }
{ ram[ basebls*$200..+$1ff]  iosec   }
{----------------------------}
function hex(w:longint; b:byte):string;
var strn:string;
begin
 strn:='';
 while b>0 do begin
  if (w and $f)<$a then strn:=chr((w and $f)+$30)+strn
  else strn:=chr((w and $f)+$37)+strn;
  dec(b);
  w:=w div $10;
 end;
 hex:=strn;
end;
{----------------------------}
function lowcase(c:char):char;
begin
 if c in ['A'..'Z'] then lowcase:=chr(ord(c)+$20) else lowcase:=c;
end;
{----------------------------}
procedure panic(strn:string);
begin
 writeln('PANIC: ',strn);
 halt;
end;
{----------------------------WRITE HL ON HEAP }
procedure heappushhl(w:word);
begin
 heap[heapcnt]:=w;
 inc(heapcnt);
end;
{----------------------------READ HL FROM HEAP }
function heappophl:word;
begin
 dec(heapcnt);
 heappophl:=heap[heapcnt];
end;
{---------------------------- read sec s to block destbl (0..nn,0..nn)}
procedure readsec(s,destbl:byte);
var buf : sectype;
    i   : word;
begin
 seek(datefdat,s);
 read(datefdat,buf);
 if ioresult<>0 then if s<ram[$1a] then panic('DSKDAT');
 move(buf,ram[destbl*$200],$200);
end;
{----------------------------set fileptr, 64K range }
procedure setfileptr(var ptr:fileptr; adr:word);
begin
 ptr.reqsec:=adr div $200;
 ptr.reqinbl:=(adr and $1ff);
end;
{----------------------------set fileptr, 128K range }
procedure setfileptr2(var ptr:fileptr; adr:word);
begin
 ptr.reqsec:=hi(adr);
 ptr.reqinbl:=lo(adr)*2;
end;
{----------------------------READ ONE BYTE FROM DAT_FILE}
function getbyte(var ptr:fileptr):byte;
var i:byte;
begin
 with ptr do begin
  if reqsec<basebls then getbyte:=ram[reqsec*$200+reqinbl]
  else begin
   if seclist[reqsec]=0 then begin
    seclist[reqsec]:=nextbl;
    seclist[bllist[nextbl]]:=0;
    bllist[nextbl]:=reqsec;
    readsec(reqsec,nextbl);
    inc(nextbl);
    if nextbl>=ramsize div $200 then nextbl:=basebls;
   end;
   getbyte:=ram[seclist[reqsec]*$200+reqinbl];
  end;
  inc(reqinbl);
  if reqinbl=$200 then begin inc(reqsec); reqinbl:=0; end;
 end;
end;
{----------------------------}
function get2bytes(var ptr:fileptr):word;
var w:word;
begin
 w:=getbyte(ptr)*$100;  { erst hi }
 inc(w,getbyte(ptr));   { dann lo }
 get2bytes:=w;
end;
{----------------------------get a word via value }
function restorehlva(a:byte):word;
var w:word;
begin
 case a of
  $00     : begin w:=heappophl; restorehlva:=w; heappushhl(w); end;
  $01..$0f: restorehlva:=xxmemf[a-1];
  $10..$ff: restorehlva:=ram[a*2-$20+srsmem]*$100+ram[a*2-$20+srsmem+1];
 end;
end;
{----------------------------get a word via getbyte }
function restorehl:word;
var a:byte;
begin
 a:=getbyte(progptr);
 if a=0 then restorehl:=heappophl
 else restorehl:=restorehlva(a);
end;
{----------------------------store a word via value }
procedure storehlva(a:byte; w:word);
var z:word;
begin
 case a of
  $00     : begin z:=heappophl; heappushhl(w); end;
  $01..$0f: xxmemf[a-1]:=w;
  $10..$ff: begin ram[a*2-$20+srsmem]:=hi(w); ram[a*2-$1f+srsmem]:=w; end;
 end;
end;
{----------------------------store a word via getbyte }
procedure storehl(w:word);
var a:byte;
begin
 a:=getbyte(progptr);
 if a=0 then heappushhl(w) else storehlva(a,w);
end;
{----------------------------}
procedure script(c:byte);
begin
 if ((ram[$11] and 1)>0) and (directoutput=false) then write(lst,chr(c));
end;
{----------------------------}
procedure linefeed;
begin
 if usedline or (morecnt<scrysiz-1) then dec(morecnt);
 if skiplf then skiplf:=false
 else begin
  writeln;
  script($d);
  if morecnt=0 then begin
   morecnt:=scrysiz-1;
   write('[MORE]');
   if readkey=#0 then;
   write(#$d,'      ',#$d);
  end;
 end;
end;
{----------------------------WRITE WRCHR-BUF }
procedure wrbuf;
var xend,i : byte;
begin
 if wrbufmem<>'' then begin
  xend:=length(wrbufmem);
  if xend>=scrxsiz then
   for i:=1 to length(wrbufmem) do if wrbufmem[i]=' ' then xend:=i-1;
  if ord(wrbufmem[length(wrbufmem)])=$d then xend:=length(wrbufmem)-1;
  write(copy(wrbufmem,1,xend));
  for i:=1 to xend do script(ord(wrbufmem[i]));
  if wrbufmem[xend+1]=' ' then inc(xend);
  wrbufmem:=copy(wrbufmem,xend+1,200);
 end;
end;
{----------------------------WRITE CHAR TO WRBUFMEM, EMPTY BUF IF FULL}
procedure wrchr(c:byte);
begin
 if c<>$a then begin
  if directoutput then write(chr(c))
  else begin
   wrbufmem:=wrbufmem+chr(c);
   if (length(wrbufmem)>=scrxsiz) or (c=$d) then begin wrbuf; linefeed; end;
   if c<>$d then usedline:=true;
   if scrflg then begin
    if c =$20 then c:=$d;
    if c>$40 then c:=c or $20;
    if c in [$d,$27,$2d,$61..$7a] then write(scriptf,c);
    if c=$d then begin c:=$a; write(scriptf,c); end;
   end;
  end;
 end;
end;
{----------------------------}
function inpcommand:string;
var strn : string;
    i    : byte;
begin
 wrbuf;
 readln(datefcon,strn);
 skiplf:=true;
 for i:=1 to length(strn) do strn[i]:=lowcase(strn[i]);
 if (ram[$11] and 1)>0 then for i:=1 to length(strn) do
  script(ord(upcase(strn[i])));
 linefeed;
 morecnt:=scrysiz-1;
 usedline:=false;
 inpcommand:=strn;
end;
{----------------------------WRITE STR WHICH IS IN INTERNAL FORMAT }
procedure wrinternalstr(var ptr:fileptr);
var tbitpos      : byte;
    tbitlst      : word;
    defprefix    : byte;
    singleprefix : byte;
const preflist : array[0..3] of byte = (1,0,0,2);
    function trnschrback:word;
    begin
     case tbitpos of
      $00: begin
            tbitlst:=get2bytes(ptr);
            trnschrback:=(tbitlst div $400) and $1f;
            inc(tbitpos);
           end;
      $01: begin trnschrback:=(tbitlst div $20) and $1f; inc(tbitpos); end;
      $02: begin
            trnschrback:=tbitlst and $1f;
            if tbitlst>=$8000 then tbitpos:=$ff else tbitpos:=0;
           end;
      $ff: trnschrback:=$ffff;
     end;
    end;
var p,cc,b3 : byte;
    c,w,w2  : word;
begin
 defprefix:=0;
 tbitpos:=0;
 singleprefix:=$ff;
 repeat
  c:=trnschrback;       {separate one char, ffff=failed=stringend}
  case c of
   $00      : wrchr(ord(' '));
   $01..$03 : begin                             {write sub_string}
               cc:=trnschrback;
               if cc<>$ffff then begin
                w2:=dataptr.reqinbl;
                b3:=dataptr.reqsec;
                w:=prfxstrs+(cc+(c-1)*$20)*2;
                setfileptr2(dataptr,ram[w]*$100+ram[w+1]);
                wrinternalstr(dataptr);                        {rekursive}
                dataptr.reqinbl:=w2;
                dataptr.reqsec:=b3;
                singleprefix:=$ff;
               end else c:=$ffff;
              end;
   $04..$05 : begin                             {set prefix}
               if singleprefix<>$ff then begin  {0=lowchr, 1=upchr, 2=other}
                p:=singleprefix;
                singleprefix:=$ff;
               end else p:=defprefix;
               if p=0 then singleprefix:=c-3
               else defprefix:=preflist[2*p+c-6];
              end;
   $06..$ff : begin
               if singleprefix<>$ff then begin
                p:=singleprefix;
                singleprefix:=$ff;
               end else p:=defprefix;
               if p=0 then wrchr(c-6+ord('a'));
               if p=1 then wrchr(c-6+ord('A'));
               if p>1 then begin
                if c=6 then begin
                 p:=trnschrback*$20; inc(p,trnschrback);
                 if p=9 then wrchr(ord(' ')) else wrchr(p);
                end;
                if c=7 then begin wrchr($d); wrchr($a); end;
                if c>7 then wrchr(ord(chrlst[c-7]));
               end;
              end;
  end;
 until c=$ffff;
end;
{----------------------------get adr of 9_byte_info of room }
function getlocinfo(room:byte):word;
begin
 getlocinfo:=room*9+$35+ram[$a]*$100+ram[$b];
end;
{----------------------------get adr of ext_room_info, after room_name }
function extroomskpnam(room:byte):word;
var w:word;
begin
 w:=getlocinfo(room);
 w:=ram[w+7]*$100+ram[w+8];
 extroomskpnam:=w+ram[w]*2+1;
end;
{----------------------------}
procedure dsproomname(room:byte);
begin
 setfileptr(dataptr,ram[getlocinfo(room)+7]*$100+ram[getlocinfo(room)+8]+1);
 wrinternalstr(dataptr);
end;
{----------------------------RESET HEAP TO OLDVAL'S, OPPOSITE OF GOSUBACTION }
procedure exitaction;      { AND STORE PARAM1 VIA GETBYTE                    }
var w:word; i:byte;
begin
 heapcnt:=oldheapcnt;
 w:=heappophl;
 progptr.reqsec:=w;
 for i:=1 to hi(w) do xxmemf[hi(w)-i]:=heappophl;
 progptr.reqinbl:=heappophl;
 oldheapcnt:=heappophl;
 storehl(param1);
end;
{----------------------------IN:PARAM1=128KPTR  OPPOSITE OF JLEXITACTION }
procedure gosubaction;
var w : word;
    i : byte;
begin
 if param1=0 then storehl(0)
 else begin
  heappushhl(oldheapcnt);
  heappushhl(progptr.reqinbl);
  w:=progptr.reqsec;
  setfileptr2(progptr,param1);
  inc(w,getbyte(progptr)*$100);
  for i:=1 to hi(w) do begin
   heappushhl(xxmemf[i-1]);
   xxmemf[i-1]:=get2bytes(progptr);
  end;
  if numofparams>=2 then xxmemf[0]:=param2;
  if numofparams>=3 then xxmemf[1]:=param3;
  if numofparams>=4 then xxmemf[2]:=param4;
  heappushhl(w);
  oldheapcnt:=heapcnt;
 end;
end;
{----------------------------}
procedure xgoto(a:byte);
var adr:longint; w:word;
begin
 if trace then write('.JMP');
 if (a and $40)=0 then w:=getbyte(progptr)+(a and $3f)*$100 else w:=a and $3f;
 if (w and $2000)>0 then inc(w,$c000);
 if (w=0) or (w=1) then begin param1:=w; exitaction; end
 else with progptr do begin                                    {reljump}
  adr:=longint(reqsec)*$200+reqinbl+integer(w-2);
  reqsec:=adr shr 9;
  reqinbl:=adr and $1ff;
 end;
end;
{----------------------------}
procedure xfalse;
var a:byte;
begin
 if trace then write(' F');
 a:=getbyte(progptr);
 if a<$80 then xgoto(a) else if (a and $40)=0 then a:=getbyte(progptr);
end;
{----------------------------}
procedure xtrue;
var a:byte;
begin
 if trace then write(' T');
 a:=getbyte(progptr);
 if a>=$80 then xgoto(a) else if (a and $40)=0 then a:=getbyte(progptr);
end;
{----------------------------}
procedure strangestuff(room:byte);
var room2,roomc : byte;
    w           : word;
begin
 room2:=ram[getlocinfo(room)+4];
 if room2<>0 then begin
  roomc:=ram[getlocinfo(room2)+6];
  if roomc=room then ram[getlocinfo(room2)+6]:=ram[getlocinfo(room)+5]
  else begin
   repeat
    w:=getlocinfo(roomc)+5;
    roomc:=ram[w];
   until roomc=room;
   ram[w]:=ram[getlocinfo(room)+5];
  end;
  ram[getlocinfo(room)+4]:=0;
  ram[getlocinfo(room)+5]:=0;
 end;
end;
{----------------------------}
procedure morestrange(room1,room2:byte);
var b:byte;
begin
 strangestuff(room1);
 ram[getlocinfo(room1)+4]:=room2;
 b:=ram[getlocinfo(room2)+6];
 ram[getlocinfo(room2)+6]:=room1;
 if b<>0 then ram[getlocinfo(room1)+5]:=b;
end;
{----------------------------}
procedure stordirdaterr(room,dir:byte);
var w:word;
begin
 w:=extroomskpnam(room);
 while dir<(ram[w] and $1f) do inc(w,ram[w] div $20+2);
 if dir<>(ram[w] and $1f) then begin
  w:=ram[$a]*$100+ram[$b]+2*dir-2;
  storehl(ram[w]*$100+ram[w+1]);
 end else
  if (ram[w] div $20)=0 then storehl(ram[w+1])
  else storehl(ram[w+1]*$100+ram[w+2]);
end;
{----------------------------}
procedure stordirptr(room,dir:byte);
var w:word;
begin
 w:=extroomskpnam(room);
 while dir<(ram[w] and $1f) do inc(w,ram[w] div $20+2);
 if dir<>(ram[w] and $1f) then storehl(0)
 else storehl(w+1);
end;
{----------------------------}
procedure stornxtdir(room,dir:byte);
var w:word;
begin
 w:=extroomskpnam(room);
 if dir=0 then storehl(ram[w] and $1f) else
 while dir<(ram[w] and $1f) do inc(w,ram[w] div $20+2);
 if dir<>(ram[w] and $1f) then storehl(0)
 else storehl(ram[w+ram[w] div $20+2] and $1f);
end;
{----------------------------}
procedure changedirdata(room,dir:byte; newdata:word);
var w:word;
begin
 w:=extroomskpnam(room);
 while dir<(ram[w] and $1f) do inc(w,ram[w] div $20+2);
 if dir<>(ram[w] and $1f) then panic('CDD')
 else if ram[w] div $20=0 then ram[w+1]:=newdata
  else begin ram[w+1]:=hi(newdata); ram[w+2]:=newdata; end;
end;
{----------------------------DISPLAY STATUSLINE }
procedure statline;
var bx : byte;
    wx : word;
begin
 directoutput:=true;
 wx:=dataptr.reqinbl;
 bx:=dataptr.reqsec;
 window(1,1,scrxsiz,1);
 textcolor(black);
 textbackground(lightgray);
 write(' ');
 dsproomname(restorehlva($10));
 while wherex<scrxsiz-20 do write(' ');
 if (ram[1] and 2)>0 then begin     {time}
  write('Time: ',((restorehlva($11)+11) mod 12)+1,':');
  if restorehlva($12)<10 then write('0');
  write(restorehlva($12),' ');
  if (restorehlva($11) mod 24)<12 then write('am') else write('pm');
 end else begin
  write('Score: ',restorehlva($11),'/',restorehlva($12));
 end;
 clreol;
 window(1,2,scrxsiz,scrysiz+1);
 gotoxy(1,scrysiz);
 textcolor(lightgray);
 textbackground(black);
 dataptr.reqinbl:=wx;
 dataptr.reqsec:=bx;
 directoutput:=false;
end;
{----------------------------enter filename for load/save }
procedure getfname;
var strn:string; i:byte;
begin
 repeat
  strn:='Filename ('+fname+') : ';
  for i:=1 to length(strn) do wrchr(ord(strn[i]));
  strn:=inpcommand;
  if strn<>'' then fname:=strn;
  if pos('.',copy(fname,pos('\',fname)+1,200))>0 then begin
   strn:='Please do not specify an extension! (AUTO EXT: .SAV)'+#$d;
   for i:=1 to length(strn) do wrchr(ord(strn[i]));
  end else i:=0;
 until i=0;
end;
{----------------------------SAVE }
procedure savegame;
var i:byte; buf:sectype; j,err:word;
begin
 getfname;
 assign(datefsav,fname+'.sav');
 rewrite(datefsav);
 if ioresult<>0 then xfalse else begin
  buf[0]:=heapcnt;
  buf[1]:=oldheapcnt;
  buf[2]:=progptr.reqsec;
  buf[3]:=hi(progptr.reqinbl);
  buf[4]:=progptr.reqinbl;
  for i:=1 to $e0 do begin
   buf[5+i*2]:=hi(heap[i]);
   buf[6+i*2]:=heap[i];
  end;
  for i:=0 to $e do begin
   buf[$1fe-i*2]:=hi(xxmemf[i]);
   buf[$1ff-i*2]:=xxmemf[i];
  end;
  write(datefsav,buf);
  if ioresult<>0 then xfalse else begin
   for i:=0 to ram[$e] div 2 do begin
    for j:=0 to $1ff do buf[j]:=ram[j+i*$200];
    write(datefsav,buf);
    if ioresult<>0 then begin i:=ram[$e]; j:=$1000; end;
   end;
   close(datefsav);
   if (ioresult<>0) or (j=$1000) then xfalse else xtrue;
  end;
 end;
end;
{----------------------------RESTORE }
procedure restoregame;
var i:byte; buf:sectype; j:word; scriptflg:byte;
begin
 scriptflg:=ram[$11] and 1;
 getfname;
 assign(datefsav,fname+'.sav');
 reset(datefsav);
 if ioresult<>0 then xfalse else begin
  read(datefsav,buf);
  if ioresult<>0 then xfalse else begin
   heapcnt:=buf[0];
   oldheapcnt:=buf[1];
   progptr.reqsec:=buf[2];
   progptr.reqinbl:=buf[3]*$100+buf[4];
   for i:=1 to $e0 do
    heap[i]:=buf[5+i*2]*$100+buf[6+i*2];
   for i:=0 to $e do
    xxmemf[i]:=buf[$1fe-i*2]*$100+buf[$1ff-i*2];
   for i:=0 to ram[$e] div 2 do begin
    read(datefsav,buf);
    if ioresult<>0 then panic('DSK');
    for j:=0 to $1ff do ram[j+i*$200]:=buf[j];
   end;
   close(datefsav);
   if ioresult<>0 then panic('DSK');
   xtrue;
  end;
 end;
 ram[$11]:=(ram[$11] and $fe)+scriptflg;
end;
{----------------------------$VERIFY }
procedure verify;
var chk,len,u : word;
    j,origin  : word;
    buf       : sectype;
begin
 writeln('Interpreter: Nocash 1993 Ferco Pascal');
 seek(datefdat,0);
 chk:=0; origin:=$40; u:=$100;
 len:=ram[$1a]*$100+ram[$1b];
 repeat
  read(datefdat,buf);
  if len<$100 then u:=len;
  dec(len,u);
  for j:=origin to u*2-1 do inc(chk,buf[j]);
  origin:=0;
 until len=0;
 if chk=ram[$1c]*$100+ram[$1d] then xtrue else xfalse;
end;
{----------------------------}
procedure help;
begin
 writeln('INFOCOM INTERPRETER 1993 NOCASH');
 writeln('Syntax: inf [filename]');
 writeln('        inf /?');
 writeln;
 writeln('$trace will start tracing, ESC will stop it.');
 halt;
end;
{----------------------------}
procedure initgamename;
begin
 fname:=defname;
 if paramstr(1)<>'' then fname:=paramstr(1);
 if fname='' then begin
  writeln('Infocom Nocash Interpreter');
  write('Filename: ');
  readln(fname);
 end else if fname[1]='/' then help;
 if pos('.',copy(fname,pos('\',fname)+1,200))>0 then begin
  writeln('Please do not specify an extension! (AUTO EXT: .DAT)');
  halt;
 end;
 assign(datefdat,fname+'.dat');
 reset(datefdat);
 if ioresult<>0 then begin writeln('Failed.'); halt; end;
end;
{----------------------------}
procedure initmachine;
begin
 checkbreak:=false;
 textcolor(lightgray);
 textbackground(black);
 assign(datefcon,'con');
 reset(datefcon);
 clrscr;
 scrxsiz:=windmax+1;
 scrysiz:=hi(windmax);
 skiplf:=false;
 gotoxy(1,scrysiz);
 randomize;
end;
{----------------------------}
procedure initram;
var i:byte;
begin
 readsec(0,0);
 basebls:=(ram[4]*$100+ram[5]-1)div $200+1;
 if basebls*$200+$200>ramsize then panic('RAMSIZE');
 for i:=0 to $ff do begin seclist[i]:=0; bllist[i]:=0; end;
 nextbl:=basebls;
end;
{----------------------------}
procedure init;
var i:byte;
begin
 directoutput:=false;
 assign(scriptf,'SCRIPT.ZSP');
 if scrflg then rewrite(scriptf);
 heapcnt:=1;
 morecnt:=scrysiz-1;
 wrbufmem:='';
 readsec(0,0);
 for i:=1 to basebls-1 do readsec(i,i);
 setfileptr(progptr,ram[6]*$100+ram[7]);
 srsmem:=ram[$c]*$100+ram[$d];
 prfxstrs:=ram[$18]*$100+ram[$19];
 trace:=false; tracekey:=true;
end;
{----------------------------TRANSLATE 6_CHAR_ASC_WORD TO 4_BYTE_INTERNAL}
function trnspack(inpstr:array6):longint;
var buf             : array6;
    a,i,b1,b2,b3,b4 : byte;
    spacefre,inpadr : byte;
begin
 for i:=1 to 6 do buf[i]:=5;
 spacefre:=6; inpadr:=0;
 repeat
  b1:=$ff; b2:=$ff; b3:=$ff; b4:=$ff;
  inc(inpadr);
  a:=inpstr[inpadr];
  case a of
   $00      : b1:=5;
   $20      : b1:=0;
   $61..$7a : b1:=a-$61+6;
   $41..$5a : begin b1:=4; b2:=a-$41+6; end;
  end;
  if b1=$ff then begin
   b1:=5;
   if (a=$d) and (inpstr[inpadr+1]=$a) then begin b2:=7; inc(inpadr); end
   else begin
    for i:=1 to length(chrlst) do if a=ord(chrlst[i]) then b2:=i+7;
    if b2=$ff then begin b2:=6; b3:=a div $20; b4:=a and $1f; end;
   end;
  end;
  buf[7-spacefre]:=b1;
  dec(spacefre);
  if (spacefre>0) and (b2<>$ff) then begin
   buf[7-spacefre]:=b2;
   dec(spacefre);
  end;
  if (spacefre>0) and (b3<>$ff) then begin
   buf[7-spacefre]:=b3;
   dec(spacefre);
  end;
  if (spacefre>0) and (b4<>$ff) then begin
   buf[7-spacefre]:=b4;
   dec(spacefre);
  end;
 until spacefre=0;
 trnspack:=buf[1]*$400+buf[2]*$20+buf[3]
          +(buf[4]*$400+buf[5]*$20+buf[6]+$8000)*longint($10000);
end;
{----------------------------GET ADR OF WORD IN DAT_FILE, 0000=unknown }
function identifyword(val:longint):word;
var w       : word;
    words   : integer;
    wordlen : word;
    fnd,skp : boolean;
    buf     : array[1..4] of byte;
begin
 buf[1]:=val;
 buf[2]:=val shr 8;
 buf[3]:=val shr 16;
 buf[4]:=val shr 24;
 w:=ram[8]*$100+ram[9];    { list of special_separators in dat_file }
 inc(w,ram[w]+4);          { skip list, wordlen, words }
 wordlen:=ram[w-3]*$10;                 { length of data per 10h_word's }
 words:=ram[w-2]*$100+ram[w-1];         { number of recognized words }
 repeat                                 { fastsearch_loop }
  dec(words,$10);
  inc(w,wordlen);
 until (buf[2]<=ram[w]) or (words<=0);
 inc(words,$10);
 dec(w,wordlen);
 wordlen:=wordlen div $10;
 fnd:=false; skp:=false;
 repeat
  if buf[2]=ram[w] then
   if buf[1]=ram[w+1] then
    if buf[4]=ram[w+2] then
     if buf[3]=ram[w+3] then fnd:=true
     else begin if buf[3]<ram[w+3] then skp:=true; end
    else begin if buf[4]<ram[w+2] then skp:=true; end
   else begin if buf[1]<ram[w+1] then skp:=true; end
  else if buf[2]<ram[w] then skp:=true;
  inc(w,wordlen);
  dec(words);
 until fnd or skp or (words=0);
 if fnd then identifyword:=w-wordlen else identifyword:=0;
end;
{---------------------------- CHECK IF CHAR IS IN EXTENDED SEPARATOR LIST }
function tstxsepa(c:byte):boolean;
var w:word; i:byte; r:boolean;
begin
 r:=false;
 w:=ram[8]*$100+ram[9];
 for i:=1 to ram[w] do if ram[w+i]=c then r:=true;
 tstxsepa:=r;
end;
{---------------------------- CHECK IF CHAR IS ANY SEPARATOR LIST }
function tstsepa(c:byte):boolean;
begin
 tstsepa:=tstxsepa(c) or (c in [$20,$2e,$2c,$3f,$0d,$0a,$09,$0c,$22]);
end;
{---------------------------- GET COMMAND FROM PLAYER }
{IN: (PARAM2) = PTR TO LIST_FOR_FOUND_WORDS                             }
{       (BYTE 0   = MAXIMUM NUMBER OF WORDS               )             }
{       (BYTE 1   = NUMBER OF WORDS FOUND (UNDEF ON CALL) )             }
{       (BYTE 2,3 = ADR_OF_FIRST_WORD (0000 IF UNKNOWN)   )             }
{       (BYTE 4   = LENGTH OF FIRST_ASCII_WORD            )             }
{       (BYTE 5   = POSITION OF FIRST_WORD IN ASCII_STR   )             }
{       (BYTE 6,7 = ADR_OF_SECOND_WORD (0000 IF UNKNOWN)  )             }
{       ( ...                                             )             }
{    (PARAM1) = BUFFER_START FOR ASCII_COMMAND_LINE (USED BY INPCOMMAND)}
{       (BYTE 0   = LOW_BYTE OF ADR OF THIS BYTE          )             }
{       (           (FOR GETTING BYTE 5,9.. OF PARAM1)    )             }
{       (BYTE 1.. = ASCII_BUFFER (SIZE = 4DH)             )             }
procedure prompt(bufadr,infoadr:word);
var strn      : string;
    done,left : byte;
    w,de      : word;
    cn,i      : byte;
    buf       : array6;
begin
 statline;
 repeat
  strn:=inpcommand;
  if strn='$trace' then begin trace:=true; wrchr(ord('>')); end;
 until strn<>'$trace';
 left:=length(strn); { chars left in strn          }
 done:=0;            { chars per word done (max 6) }
 ram[infoadr+1]:=0;  { number of words found }
 while (ram[infoadr]<>ram[infoadr+1]) and { maximum number of words }
       (done+left<>0) do begin            { whole command line interpreted }
  cn:=ord(strn[length(strn)-left+1]);
  if done=6 then     {if maxlen is reached, skip rest of word}
   while (left>0) and (tstsepa(cn)=false) do begin
    dec(left);
    inc(done);
    cn:=ord(strn[length(strn)-left+1]);
   end;
  if done>0 then begin
   if (left=0) or tstsepa(cn) or tstxsepa(buf[1]) then begin
    de:=identifyword(trnspack(buf));   { get adr in command_list }
    w:=infoadr+ram[infoadr+1]*4+2;
    ram[w]:=hi(de);
    ram[w+1]:=de;
    ram[w+2]:=done;                    { store length of ascii_word }
    inc(ram[infoadr+1]);               { inc number of words found }
    done:=0;
   end else begin
    inc(done);
    buf[done]:=cn;
    dec(left);
   end;
  end else begin                { init buffer for next word }
   for i:=1 to 6 do buf[i]:=0;
   ram[infoadr+ram[infoadr+1]*4+2+3]:=1+length(strn)-left;
   dec(left);
   if tstsepa(cn)=false or tstxsepa(cn) then begin
    inc(done);
    buf[done]:=cn;
   end;
  end;
 end;
 for i:=1 to $4c do ram[bufadr+i]:=ord(strn[i]);
end;
{----------------------------}
procedure execom0type(a:byte);
begin
 if trace then write(comnam0[a]);
 case a of
  $00: begin param1:=1; exitaction; end;
  $01: begin param1:=0; exitaction; end;
  $02: wrinternalstr(progptr);
  $03: begin wrinternalstr(progptr); wrchr($d); param1:=1; exitaction; end;
  $04: ;
  $05: savegame;
  $06: restoregame;
  $07: restartflg:=true;
  $08: begin param1:=heappophl; exitaction; end;
  $09: if heappophl=0 then;
  $0a: begin if scrflg then close(scriptf); halt; end;
  $0b: wrchr($d);
  $0c: statline;
  $0d: verify;
 end;
end;
{----------------------------}
procedure execom1type(a:byte);
var zsp : byte;
    adr : longint;
begin
 if trace then write(comnam1[a]);
 case a of
  $00: if param1=0 then xtrue else xfalse;
  $01: begin
        zsp:=ram[getlocinfo(param1)+5];
        storehl(zsp);
        if zsp=0 then xfalse else xtrue;
       end;
  $02: begin
        zsp:=ram[getlocinfo(param1)+6];
        storehl(zsp);
        if zsp=0 then xfalse else xtrue;
       end;
  $03: storehl(ram[getlocinfo(param1)+4]);
  $04: storehl(ram[param1-1] div $20+1);       {store size of data_per_dir}
  $05: storehlva(param1,restorehlva(param1)+1);                 {increment}
  $06: storehlva(param1,restorehlva(param1)-1);                 {decrement}
  $07: begin setfileptr(dataptr,param1); wrinternalstr(dataptr); end;
  $08: ;
  $09: strangestuff(param1);
  $0a: dsproomname(param1);
  $0b: exitaction;
  $0c: with progptr do begin                                    {reljump}
        adr:=longint(reqsec)*$200+reqinbl+integer(param1-2);
        reqsec:=adr shr 9;
        reqinbl:=adr and $1ff;
       end;
  $0d: begin setfileptr2(dataptr,param1); wrinternalstr(dataptr); end;
  $0e: storehl(restorehlva(param1));
  $0f: storehl(param1 xor $ffff);
 end;
end;
{----------------------------}
procedure execom2type(a:byte);
var w1:word;
begin
 if trace then write(comnam2[a]);
 case a of
  $00: ;
  $01: if param1=param2 then xtrue
        else if (param1=param3) and (numofparams>=3) then xtrue
         else if (param1=param4) and (numofparams>=4) then xtrue
          else xfalse;
  $02: if integer(param2)>integer(param1) then xtrue else xfalse;
  $03: if integer(param1)>integer(param2) then xtrue else xfalse;
  $04: begin
        w1:=restorehlva(param1)-1;
        storehlva(param1,w1);
        if integer(param2)>integer(w1) then xtrue else xfalse;
       end;
  $05: begin
        w1:=restorehlva(param1)+1;
        storehlva(param1,w1);
        if integer(w1)>integer(param2) then xtrue else xfalse;
       end;
  $06: if param2=ram[getlocinfo(param1)+4] then xtrue else xfalse;
  $07: if param2=(param1 and param2) then xtrue else xfalse;
  $08: storehl(param1 or param2);
  $09: storehl(param1 and param2);
  $0a: if (ram[getlocinfo(param1)+param2 div 8])
          and ($80 shr (param2 and 7))>0 then xtrue else xfalse;
  $0b: ram[getlocinfo(param1)+param2 div 8]:=
       ram[getlocinfo(param1)+param2 div 8] or ($80 shr (param2 and 7));
  $0c: ram[getlocinfo(param1)+param2 div 8]:=
       ram[getlocinfo(param1)+param2 div 8] and ($ff7f shr (param2 and 7));
  $0d: storehlva(param1,param2);
  $0e: morestrange(param1,param2);
  $0f: begin
        setfileptr(dataptr,param1+2*param2);
        storehl(get2bytes(dataptr));
       end;
  $10: begin
        setfileptr(dataptr,param1+param2);
        storehl(getbyte(dataptr));
       end;
  $11: stordirdaterr(param1,param2);
  $12: stordirptr(param1,param2);
  $13: stornxtdir(param1,param2);
  $14: storehl(param1+param2);
  $15: storehl(param1-param2);
  $16: storehl(integer(param1)*integer(param2));
  $17: storehl(integer(param1) div integer(param2));
  $18: storehl(integer(param1) mod integer(param2));
 end;
end;
{----------------------------}
procedure execomntype(a:byte);
var w    : word;
    i    : byte;
    strn : string;
begin
 if trace then write(comnamn[a]);
 case a of
  $00: gosubaction;
  $01: begin
        ram[param1+2*param2]:=hi(param3);
        ram[param1+2*param2+1]:=param3;
       end;
  $02: ram[param1+param2]:=param3;
  $03: changedirdata(param1,param2,param3);
  $04: prompt(param1,param2);
  $05: wrchr(param1);
  $06: begin
        str(param1,strn);
        for i:=1 to length(strn) do wrchr(ord(strn[i]));
       end;
  $07: storehl(random(param1)+1);
  $08: heappushhl(param1);
  $09: storehlva(param1,heappophl);
 end;
end;
{----------------------------}
procedure get1params(a:byte);
begin
 if (a and $30)=$00 then param1:=get2bytes(progptr);
 if (a and $30)=$10 then param1:=getbyte(progptr);
 if (a and $30)=$20 then param1:=restorehl;
 numofparams:=1;
end;
{----------------------------}
procedure get2params(a:byte);
begin
 if (a and $40)>0 then param1:=restorehl else param1:=getbyte(progptr);
 if (a and $20)>0 then param2:=restorehl else param2:=getbyte(progptr);
 numofparams:=2;
end;
{----------------------------}
procedure getnparams;
var a:byte; w:word;
begin
 a:=getbyte(progptr);
 while (a and $c0)<>$c0 do begin
  if (a and $c0)=$00 then w:=get2bytes(progptr);
  if (a and $c0)=$40 then w:=getbyte(progptr);
  if (a and $c0)=$80 then w:=restorehl;
  a:=a*4+3;
  inc(numofparams);
  case numofparams of
   1: param1:=w;
   2: param2:=w;
   3: param3:=w;
   4: param4:=w;
  end;
 end;
end;
{----------------------------}
procedure mainloop;
var a : byte;
begin
 numofparams:=0;
 a:=getbyte(progptr);
 if trace then write(' COMMAND: ',hex(a,2));
 case a of
  $00..$7f: get2params(a);
  $80..$af: get1params(a);
  $b0..$bf: ;
  $c0..$df: getnparams;
  $e0..$ff: getnparams;
 end;
 if trace then begin
  write(' PARAMS:');
  if numofparams>=1 then write(' ',hex(param1,4));
  if numofparams>=2 then write(' ',hex(param2,4));
  if numofparams>=3 then write(' ',hex(param3,4));
  if numofparams>=4 then write(' ',hex(param4,4));
  while wherex<50 do write(' ');
 end;
 case a of
  $00..$7f: execom2type(a and $1f);
  $80..$af: execom1type(a and $f);
  $b0..$bf: execom0type(a and $f);
  $c0..$df: execom2type(a-$c0);
  $e0..$ff: execomntype(a-$e0);
 end;
end;
{----------------------------}
begin
 initmachine;
 initgamename;
 initram;
 repeat
  init;
  restartflg:=false;
  repeat
   if trace then write(hex(progptr.reqsec*$200+progptr.reqinbl,5));
   mainloop;
   if trace then begin
    writeln;
    if keypressed or tracekey then if readkey=#$1b then trace:=false;
   end;
  until restartflg;
  clrscr;
  gotoxy(1,scrysiz);
 until 1=2;
end.
