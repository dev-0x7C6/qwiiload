#!/bin/sh

fpc -O2 wiitcpload.pas -v0i
strip --strip-all wiitcpload