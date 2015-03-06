#!/bin/bash

set -e

original=$(readlink -f $1)
target=$(readlink -f $2)

if [ -z $3 ]
then
  segment=".text"
else
  segment=$3
fi
expand $original | sed 's/;.*$//' | grep "^$segment.*:" | sed -r "s/^$segment[0-9]*:.{8}//" | sed -r 's/^.{49}//' | sed 's/\r//' > $target
#expand $original | sed 's/;.*$//;/^$segment.*:/!d;s/^$segment[0-9]*:.{8}//;s/^.{49}//;s/\r//' > $target

