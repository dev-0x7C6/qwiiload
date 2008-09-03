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

unit WiiHomeBrew;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, WiiUnit, Console, Sockets;

 procedure DoWork(const WiiHost :AnsiString; var FileStream :TFileStream);

implementation

const
 WiiLoadVersion :TWiiLoadVersion = (0, 3);
 Port :Word = 4299;

procedure DoWork(const WiiHost :AnsiString; var FileStream :TFileStream);
var
 WiiConnect :TWiiConnect;
 WiiDatagram :TWiiDatagram;
 FileSize :Longword;
begin
 WiiConnect.Port := Port;
 if WiiConnectFunc(WiiHost, WiiConnect) = True then
 begin
  SetSocketOptions(WiiConnect.Sock, SOL_SOCKET, SO_SNDBUF, 16777216, 4);
  SetSocketOptions(WiiConnect.Sock, SOL_SOCKET, SO_RCVBUF, 16777216, 4);
  WiiDatagram[0] := Ord('H');
  WiiDatagram[1] := Ord('A');
  WiiDatagram[2] := Ord('X');
  WiiDatagram[3] := Ord('X');
  WiiSendData(WiiConnect, WiiDatagram);
  WiiDatagram[0] := WiiLoadVersion[0];
  WiiDatagram[1] := WiiLoadVersion[1];
  WiiDatagram[2] := (0 shr 8) and $FF;
  WiiDatagram[3] := 0 and $FF;
  WiiSendData(WiiConnect, WiiDatagram);
  FileSize := FileStream.Size;
  WiiDatagram[0] := (FileSize shr 24) and $FF;
  WiiDatagram[1] := (FileSize shr 16) and $FF;
  WiiDatagram[2] := (FileSize shr 8) and $FF;
  WiiDatagram[3] := FileSize and $FF;
  WiiSendData(WiiConnect, WiiDatagram);

  WiiSendFile(WiiHost, WiiConnect, FileStream);
  Writeln(Prefix, 'Done');
 end else
  Writeln(Prefix, 'Can''t connect');
 PressEnterToContinue;
end;

end.

