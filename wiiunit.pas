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

unit WiiUnit;

{$mode objfpc}{$H+}

interface

uses
{$ifdef windows} Windows, Winsock2, {$endif} Classes, SysUtils, Sockets;

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
 INVALID_SOCKET = -1;
 DatagramSize = 4096;
  AddrSize :Longint = SizeOf(TINetSockAddr);

var
 WiiUnitLastError :AnsiString = '';


 function WiiConnectFunc(WiiHost :String; var WiiConnect :TWiiConnect) :Boolean;
 function WiiSendData(var WiiConnect :TWiiConnect; var WiiDatagram :TWiiDatagram) :Boolean;
 function WiiSendFile(WiiHost :String; var WiiConnect :TWiiConnect; var FileStream :TFileStream) :Boolean;

implementation

function WiiConnectFunc(WiiHost :String; var WiiConnect :TWiiConnect) :Boolean;
begin
 WiiConnect.Sock := Socket(AF_INET, SOCK_STREAM, 0);
 if WiiConnect.Sock <> INVALID_SOCKET then
 begin
  WiiConnect.Addr.Family := AF_INET;
  WiiConnect.Addr.Port := HTons(WiiConnect.Port);
  WiiConnect.Addr.Sin_addr := HostToNet(StrToHostAddr(WiiHost));
  Result := FPConnect(WiiConnect.Sock, @WiiConnect.Addr, AddrSize) = 0;
  if (Result = False) then
   WiiUnitLastError := 'Can''t connect to ' + WiiHost + ':' + IntToStr(WiiConnect.Port) else
   begin
    SetSocketOptions(WiiConnect.Sock, SOL_SOCKET, SO_SNDBUF, 16777216, 4);
    SetSocketOptions(WiiConnect.Sock, SOL_SOCKET, SO_RCVBUF, 16777216, 4);
   end;
  Exit(Result);
 end else
  WiiUnitLastError := 'Can''t create socket';
 Result := False;
end;

function WiiSendData(var WiiConnect :TWiiConnect; var WiiDatagram :TWiiDatagram) :Boolean;
begin
 sendto(WiiConnect.Sock, WiiDatagram, SizeOf(TWiiDatagram), 0, WiiConnect.Addr, SizeOf(WiiConnect.Addr));
end;

function WiiSendFile(WiiHost :String; var WiiConnect :TWiiConnect; var FileStream :TFileStream) :Boolean;
var
 Opt, X, Long :Longint;
 Buffer :Array[0..DatagramSize - 1] of Byte;
begin
 Writeln('WiiSendFile: Start sending...');
 FileStream.Seek(0, 0);
 Opt := FileStream.Size div DatagramSize;
 if opt > 0 then
 begin
  for X := 1 to Opt do
  begin
   FileStream.ReadBuffer(Buffer, DatagramSize);
   Long := SendTo(WiiConnect.Sock, Buffer, DatagramSize, 0, WiiConnect.Addr, SizeOf(WiiConnect.Addr));
   if Long <> DatagramSize then
   begin
    Writeln('WiiSendFile: Socket error ', SocketError);
    exit(false);
   end;
  end;
 end;

 Opt := FileStream.Size mod DatagramSize;

 if Opt <> 0 then
 begin
  FileStream.ReadBuffer(Buffer, Opt);
  sendto(WiiConnect.Sock, Buffer, Opt, 0, WiiConnect.Addr, SizeOf(WiiConnect.Addr));
 end;
 Writeln('WiiSendFile: Done');
end;

{$ifdef windows}
var
 _WSADATA :WSAData;
{$endif}

initialization
begin
{$ifdef windows}
 if WSAStartup ($101, _WSADATA) <> 0 then
 begin
  Writeln('WSAStartup: Fail');
  Halt;
 end;
{$endif}
end;

finalization
begin
{$ifdef windows}
 WSACleanup();
{$endif}
end;

end.

