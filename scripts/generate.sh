#!/bin/bash

set -e

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

if [ -e $scriptdir/../src/asm2graphs-$2 ]
then
  $scriptdir/../src/asm2graphs-$2 $original
else
  $scriptdir/../src/asm2graphs $original
fi

cd ..

