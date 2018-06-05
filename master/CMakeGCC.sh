#!/bin/sh

rm -r bin
rm libSpider.so

mkdir bin
cd bin
mkdir make
cd make
# Generating the Makefile
# Run cmake gui to debug cmake problem
cmake ../..

make -j$(grep -c ^processor /proc/cpuinfo)

cp libSpider.so ../../
