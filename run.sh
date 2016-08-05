#!/bin/bash
set -e

rm -rf samples
mkdir -p bin cache samples output
g++ -O3 -std=c++11 compile.cpp -o bin/compile
gcc -c utils.c -o bin/utils.o
webppl main.wppl --require webppl-fs --require llvm

echo "Generating animation..."
./video.sh

echo "Plotting graph..."
./plot.sh
