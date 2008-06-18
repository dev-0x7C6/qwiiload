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
 Windows, Winsock,
{$endif} Classes, SysUtils, Sockets;

type
 TWiiDatagram = Array[0..3] of Byte;
 TWiiLoadVersion = Array[0..1] of Byte;
 
type
 TWiiConnect = packed record
  Sock :Longint;
  Port :Word;
  Addr :TInetSockAddr;
 end;

const
 WiiLoadVersion :TWiiLoadVersion = (0, 3);
 AddrSize :Longint = SizeOf(TINetSockAddr);
 INVALID_SOCKET = -1;
 DatagramSize = 16 * 1024;
 HomebrewChannelPort = 4299;
 TCPLoaderChannelPort = 8080;
 
const
 prefix = '>>> ';
 

function WiiConnectFunc(WiiHost :String; var WiiConnect :TWiiConnect) :Boolean;
begin
 WiiConnect.Sock := Socket(AF_INET, SOCK_STREAM, 0);
 Write(Prefix, 'Creating new socket ... ');
 if WiiConnect.Sock <> INVALID_SOCKET then
 begin
  Writeln('[done]');
  Write(Prefix, 'Connecting to ', WiiHost, ':', WiiConnect.Port, ' ... ');
  WiiConnect.Addr.Family := AF_INET;
  WiiConnect.Addr.Port := HTons(WiiConnect.Port);
  WiiConnect.Addr.Sin_addr := HostToNet(StrToHostAddr(WiiHost));
  Result := FPConnect(WiiConnect.Sock, @WiiConnect.Addr, AddrSize) = 0;
  if Result = True then
   Writeln('[done]') else
   Writeln('[fail]');
  Exit;
 end else
  Writeln('[fail]');
 Result := False;
end;

function WiiSendData(var WiiConnect :TWiiConnect; WiiDatagram :TWiiDatagram) :Boolean;
begin
 SendTo(WiiConnect.Sock, WiiDatagram, SizeOf(TWiiDatagram), 0, WiiConnect.Addr, AddrSize);
end;

function WiiSendFile(WiiHost :String; var WiiConnect :TWiiConnect; var FileStream :TFileStream) :Boolean;
var
 Opt, X :Longint;
 Buffer :Array[0..DatagramSize - 1] of Byte;
begin
 FileStream.Seek(0, 0);
 Opt := FileStream.Size div DatagramSize;
 
 if opt > 0 then
 begin
  for X := 1 to Opt do
  begin
   FileStream.ReadBuffer(Buffer, DatagramSize);
   SendTo(WiiConnect.Sock, Buffer, DatagramSize, 0, WiiConnect.Addr, AddrSize);
   Writeln('Packet [', X, '] ', DatagramSize, 'Bytes >> ', WiiHost, ':', WiiConnect.Port);
  end;
 end;
 
 Opt := FileStream.Size mod DatagramSize;
 
 if Opt <> 0 then
 begin
  FileStream.ReadBuffer(Buffer, Opt);
  SendTo(WiiConnect.Sock, Buffer, Opt, 0, WiiConnect.Addr, AddrSize);
  Writeln('Packet [', X+1, '] ', Opt, 'Bytes >> ', WiiHost, ':', WiiConnect.Port);
 end;
 Writeln('Done !');
end;

procedure CClrScr;
{$ifdef windows}
var
 Data :CONSOLE_SCREEN_BUFFER_INFO;
 dw :DWord;
 co :_COORD;
 Handle :THandle;
{$endif}
begin
{$ifdef unix}
 Write(#27'[H'#27'[2J');
{$endif}
{$ifdef windows}
 ZeroMemory(@co, sizeof(co));
 Handle:=GetStdHandle(STD_OUTPUT_HANDLE);
 GetConsoleScreenBufferInfo(Handle, Data);
 FillConsoleOutputCharacter(Handle, ' ', Data.dwSize.X*Data.dwSize.Y, co, dw);
 SetConsoleCursorPosition(Handle, co);
{$endif}
end;

procedure PressEnterToContinue;
begin
 Writeln;
 Write('Press Enter to continue');
 Readln;
end;

var
 WiiHost, ElfFile :String;
 WiiConnect :TWiiConnect;
 WiiDatagram :TWiiDatagram;
 FileSize :Longint;
 FileStream :TFileStream;
 StrChoice :String;
 Protocol :Longint;

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
 WiiHost := '';
 ElfFile := '';

 repeat
  CClrScr;
  Writeln('Welcome in WiiTCPLoader 0.00.1, Developing by Bartlomiej Burdukiewicz');
  if WiiHost <> '' then
   Writeln('Wii ', WiiHost) else
   Writeln;
  Writeln;

  Writeln('   [1]. Connect with Homebrew Channel');
  Writeln('   [2]. Connect with TCP Loader Channel');
  Writeln('   [3]. Set Wii console IP address');
  Writeln('   [4]. Set file to send');
  Writeln('   [5]. Quit');
  Writeln;
  Write('Your choice: ');
  Readln(StrChoice);
  Writeln;
  Protocol := StrToIntDef(StrChoice, 0);
  
  case Protocol of
   1:begin
      if Check then
      begin
       WiiConnect.Port := HomebrewChannelPort;
       if WiiConnectFunc(WiiHost, WiiConnect) then
       begin
        Writeln(Prefix, 'Sending HAXX char array');
        WiiDatagram[0] := Ord('H');
        WiiDatagram[1] := Ord('A');
        WiiDatagram[2] := Ord('X');
        WiiDatagram[3] := Ord('X');

        WiiSendData(WiiConnect, WiiDatagram);

        Writeln(Prefix, 'Sending client version info');
        WiiDatagram[0] := WiiLoadVersion[0];
        WiiDatagram[1] := WiiLoadVersion[1];
        WiiDatagram[2] := (0 shr 8) and $FF;
        WiiDatagram[3] := 0 and $FF;

        WiiSendData(WiiConnect, WiiDatagram);
        FileSize := FileStream.Size;

        Writeln(Prefix, 'Sending file size');
        WiiDatagram[0] := (FileSize shr 24) and $FF;
        WiiDatagram[1] := (FileSize shr 16) and $FF;
        WiiDatagram[2] := (FileSize shr 8) and $FF;
        WiiDatagram[3] := FileSize and $FF;

        WiiSendData(WiiConnect, WiiDatagram);
        Writeln(Prefix, 'Sending file');
        WiiSendFile(WiiHost, WiiConnect, FileStream);
       end;
      end;
      PressEnterToContinue;
     end;
   2:begin
      if Check then
      begin
       WiiConnect.Port := TCPLoaderChannelPort;
       if WiiConnectFunc(WiiHost, WiiConnect) then
       begin
        Writeln(Prefix, 'WARNING: You must accept connection on your Wii');
        Writeln(Prefix, 'Sending file size');
        FileSize := FileStream.Size;

        WiiDatagram[0] := (FileSize shr 24) and $FF;
        WiiDatagram[1] := (FileSize shr 16) and $FF;
        WiiDatagram[2] := (FileSize shr 8) and $FF;
        WiiDatagram[3] := FileSize and $FF;

        WiiSendData(WiiConnect, WiiDatagram);
        Writeln(Prefix, 'Sending file');
        WiiSendFile(WiiHost, WiiConnect, FileStream);
       end;
      end;
      PressEnterToContinue;
     end;
   3:begin
      Write('Your wii ip: ');
      Readln(WiiHost);
     end;
   4:begin
      if ElfFile <> '' then
       FileStream.Free;
      Write('File to open: ');
      Readln(ElfFile);
      if not FileExists(ElfFile) then
      begin
       ElfFile := '';
       Writeln('File not found');
       Readln;
      end else
       FileStream := TFileStream.Create(ElfFile, fmOpenRead);
     end;
  end;
  
 until Protocol = 5;

 if ElfFile <> '' then
  FileStream.Free;

end.

