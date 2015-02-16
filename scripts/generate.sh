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

expand $original | sed -r 's/^.{14}//' | sed -r 's/^.{49}//' | sed 's/\r//' | grep -v "^;" > $filename.asm
$scriptdir/../src/asm2graphs $filename.asm
rm -rf $filename.asm

cd ..

tar czf $filename.tar.gz $filename

