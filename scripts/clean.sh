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
expand $original | grep "^$segment:" | sed -r "s/^$segment.{9}//" | sed -r 's/^.{49}//' | sed 's/\r//' | grep -v "^;" > $target

