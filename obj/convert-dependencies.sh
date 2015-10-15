#!/bin/sh
# AUTO-GENERATED FILE, DO NOT EDIT!
if [ -f $1.org ]; then
  sed -e 's!^F:/cygwin64/lib!/usr/lib!ig;s! F:/cygwin64/lib! /usr/lib!ig;s!^F:/cygwin64/bin!/usr/bin!ig;s! F:/cygwin64/bin! /usr/bin!ig;s!^F:/cygwin64/!/!ig;s! F:/cygwin64/! /!ig;s!^I:!/cygdrive/i!ig;s! I:! /cygdrive/i!ig;s!^G:!/cygdrive/g!ig;s! G:! /cygdrive/g!ig;s!^F:!/cygdrive/f!ig;s! F:! /cygdrive/f!ig;s!^C:!/cygdrive/c!ig;s! C:! /cygdrive/c!ig;' $1.org > $1 && rm -f $1.org
fi
