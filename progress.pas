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

unit Progress;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils;

const
 ProgressBar :Array[0..1] of Char  = ('[', ']'); 
 EmptyField :Char = '.';
 FillField :Char = '*';

type TTxtProgress = class
 private
  fField :Longint;
  fProgress :Longint;
  fPosition :Longint;
 protected
 public
  procedure Draw(Field :Longint);
  procedure Inc(X :Longint);

  procedure SetEmpty;
  procedure SetFill;

  constructor Create;
  destructor Destroy; override;
end;

implementation

constructor TTxtProgress.Create;
begin
 inherited Create;
 fField := 0;
 fProgress := 0;
 fPosition := 0;
end;

destructor TTxtProgress.Destroy;
begin
 inherited Destroy;
end;

procedure TTxtProgress.Draw(Field :Longint);
var
 x :Longint;
 str :AnsiString;
begin
 fProgress := 0;
 fPosition := 1;
 write(ProgressBar[0]);
 fField := Field;
 str := '';
 for x := 1 to fField do
  str := str + EmptyField;
 write(str, ProgressBar[1]);
end;

procedure TTxtProgress.Inc(X :Longint);
begin
end;

procedure TTxtProgress.SetEmpty;
begin
end;

procedure TTxtProgress.SetFill;
var
 x :Longint;
 str :AnsiString;
begin
 fProgress := 0;
 fPosition := fField; 
 write(ProgressBar[0]);
 str := '';
 for x := 1 to fField do
  str := str + FillField;
 write(str, ProgressBar[1]); 
end;

end.

