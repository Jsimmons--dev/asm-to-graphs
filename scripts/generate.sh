#!/bin/bash

set -e

scriptdir=$(dirname $(readlink -f $0))

format=$1
dir=$(readlink -f $2)
name=$3

generator=$scriptdir/../src/asm2graphs-$format

original=$dir/$name.asm

rm -rf $name
mkdir $name
cd $name

echo "$generator $original"
$generator $original

cd ..

