#!/bin/sh

rm -r bin

mkdir bin
cd bin
# Generating the Makefile
# Run cmake gui to debug cmake problem
cmake ../

make -j$(grep -c ^processor /proc/cpuinfo)

./runTests

cd ..
rm -rf bin
