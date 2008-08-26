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

unit Proc;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils; 

 procedure GetWiiHost(var WiiHost :String);
 procedure GetElfFile(var ElfFile :String; var FileStream :TFileStream);

implementation

procedure GetWiiHost(var WiiHost :String);
begin
 Write('Get wii ip: ');
 Readln(WiiHost);
end;

procedure GetElfFile(var ElfFile :String; var FileStream :TFileStream);
begin
 if FileStream <> nil then
 begin
  FileStream.Free;
  FileStream := nil;
 end;
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


end.

