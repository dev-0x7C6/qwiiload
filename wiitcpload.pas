{ Copyright (C) 2007-2008 Bartlomiej Burdukiewicz

  This source is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option)
  any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  A copy of the GNU General Public License is available on the World Wide Web
  at <http://www.gnu.org/copyleft/gpl.html>. You can also obtain it by writing
  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
  MA 02111-1307, USA.
}

program WiiTcpLoad;

{$mode objfpc}{$H+}

uses
 Classes, SysUtils, Sockets, WiiUnit, Console, WiiHomebrew, WiiTcpLoader, Proc;


var
 WiiHost :String = '';
 ElfFile :String = '';
 FileStream :TFileStream = nil;

 strOption :String;
 intOption :Longint;

function Check :Boolean;
begin
 if WiiHost = '' then
 begin
  Writeln('You must set wii console ip address');
  Exit(False);
 end;
 if ElfFile = '' then
 begin
  Writeln('You must open file to send');
  Exit(False);
 end;
 Result := True;
end;


begin
 repeat
  ClrScr;
  Writeln('Welcome in WiiTCPLoader 0.00.2, Developing by Bartlomiej Burdukiewicz');
  Writeln;

  Writeln('   [1]. Connect with Homebrew Channel');
  Writeln('   [2]. Connect with TCP Loader Channel');
  Writeln('   [3]. Set Wii console IP address');
  Writeln('   [4]. Set file to send');
  Writeln('   [5]. Quit');
  Writeln;
  Write('Your choice: ');
  Readln(strOption);
  Writeln;
  intOption := StrToIntDef(strOption, 0);
  
  case intOption of
   1: if Check then WiiHomeBrew.DoWork(WiiHost, FileStream);
   2: if Check then WiiTcpLoader.DoWork(WiiHost, FileStream);
   3: GetWiiHost(WiiHost);
   4: GetElfFile(ElfFile, FileStream);
  end;
  
 until intOption = 5;

 if ElfFile <> '' then
  FileStream.Free;
end.

