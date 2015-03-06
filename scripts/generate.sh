#!/bin/bash

set -e

scriptdir=$(dirname $(readlink -f $0))

format=$1
dir=$(readlink -f $2)
name=$3

if [ -z "$format" ]; then generator=$scriptdir/../src/asm2graphs;
                     else generator=$scriptdir/../src/asm2graphs-$format;
fi

original=$dir/$name.asm

rm -rf $name
mkdir $name
cd $name

$generator $original

cd ..

