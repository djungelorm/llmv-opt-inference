#!/bin/bash

./plot.r
convert Rplots.pdf output/plot.png
rm Rplots.pdf
