@echo off

fpc wiitcpload.pas -O2 -owiitcpload.exe -v0i
strip --strip-all wiitcpload.exe