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

unit Console;

{$mode objfpc}{$H+}

interface

uses
 {$ifdef windows} Windows, {$endif} Classes, SysUtils;

const
 prefix = '>>> ';

 procedure ClrScr;
 procedure PressEnterToContinue;

implementation

procedure ClrScr;
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


end.

