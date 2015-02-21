#!/bin/bash

scriptdir=$(dirname $(readlink -f $0))

original=$(readlink -f $1)

filename=$(basename $1)
filename=${filename%.*}

if [ ! -e $scriptdir/../src/asm2graphs ]
then
  make -C $scriptdir/../src asm2graphs
fi

rm -rf $filename
mkdir $filename
cd $filename

expand $original | grep "^.text:" | sed -r 's/^.{14}//' | sed -r 's/^.{49}//' | sed 's/\r//' | grep -v "^;" > $filename.asm
if [ -e $scriptdir/../src/asm2graphs-$2 ]
then
  $scriptdir/../src/asm2graphs-$2 $filename.asm
else
  $scriptdir/../src/asm2graphs $filename.asm
fi

cd ..

