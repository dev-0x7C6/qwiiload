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
{$ifdef windows}
 Windows, Registry,
{$endif}
 Classes, SysUtils, Sockets, WiiUnit, Console, WiiHomebrew, WiiTcpLoader, Proc,
 Settings, Progress;


var
 WiiHost :String = '';
 ElfFile :String = '';
 FileStream :TFileStream = nil;

 strOption :String;
 intOption :Longint;
 x :Longint;

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

{$ifdef unix}
const
 CfgDirName = '.wiitcpload';
var
 CfgDirPath :AnsiString;
 TextFile :Text;
{$endif}

{$ifdef windows}
var
 Reg :TRegistry;
{$endif}

begin
{$ifdef unix}
  CfgDirPath := GetHomeDirectory + CfgDirName;
  if not DirectoryExists(CfgDirPath) then
   mkdir(CfgDirPath);
  CfgDirPath := CfgDirPath + '/';
  if FileExists(CfgDirPath + 'wii') then
  begin
   AssignFile(TextFile, CfgDirPath + 'wii');
   Reset(TextFile);
   Readln(TextFile, WiiHost);
   CloseFile(TextFile);
  end;
  if FileExists(CfgDirPath + 'lastfile') then
  begin
   AssignFile(TextFile, CfgDirPath + 'lastfile');
   Reset(TextFile);
   Readln(TextFile, ElfFile);
   CloseFile(TextFile);
  end;
{$endif}

{$ifdef windows}
 Reg := TRegistry.Create;
 Reg.RootKey := HKEY_CURRENT_USER;
 Reg.OpenKey('Software\Microsoft\WiiTCPLoad', true);
 if Reg.ValueExists('Wii') then
  WiiHost := Reg.ReadString('Wii');
 if Reg.ValueExists('LastFile') then
  ElfFile := Reg.ReadString('LastFile');
{$endif}

 if ParamCount > 1 then
 begin
  ElfFile := ParamStr(1);
  if ParamCount > 2 then
   WiiHost := ParamStr(2);
 end;

 if FileExists(ElfFile) then
  FileStream := TFileStream.Create(ElfFile, fmOpenRead) else
  ElfFile := '';

 repeat
  ClrScr;

  Writeln('Welcome in WiiTCPLoader 0.00.3, Developing by Bartlomiej Burdukiewicz');
  Writeln;
  Write('Wii--> '); if WiiHost = '' then Writeln('<empty>') else Writeln(WiiHost);
  Write('Elf--> '); if ElfFile = '' then Writeln('<empty>') else Writeln(ElfFile);
  Writeln;

  Writeln('-------------------------------------------');
  Writeln('   [1]. Connect with Homebrew Channel');
  Writeln('   [2]. Connect with TCP Loader Channel');
  Writeln('-------------------------------------------');
  Writeln('   [3]. Change wii hostname');
  Writeln('   [4]. Chose file to stream');
  Writeln('   [5]. Quit&Save');
  Writeln('-------------------------------------------');
  Writeln;
  Write('>');
  Readln(strOption);
  intOption := StrToIntDef(strOption, 0);
  
  case intOption of
   1: if Check then WiiHomeBrew.DoWork(WiiHost, FileStream);
   2: if Check then WiiTcpLoader.DoWork(WiiHost, FileStream);
   3: GetWiiHost(WiiHost);
   4: GetElfFile(ElfFile, FileStream);
  end;
  
 until intOption = 5;

{$ifdef unix}
 CfgDirPath := GetHomeDirectory + CfgDirName;
 if not DirectoryExists(CfgDirPath) then
  mkdir(CfgDirPath);
 CfgDirPath := CfgDirPath + '/';

 AssignFile(TextFile, CfgDirPath + 'wii');
 Rewrite(TextFile);
 Writeln(TextFile, WiiHost);
 CloseFile(TextFile);

 AssignFile(TextFile, CfgDirPath + 'lastfile');
 Rewrite(TextFile);
 Writeln(TextFile, ElfFile);
 CloseFile(TextFile);
{$endif}

{$ifdef windows}
 Reg.WriteString('Wii', WiiHost);
 Reg.WriteString('LastFile', ElfFile);
 Reg.Free;
{$endif}

 if ElfFile <> '' then
  FileStream.Free;
end.

