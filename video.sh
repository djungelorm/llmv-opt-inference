#!/bin/bash

rm samples/*.png
for f in samples/*.txt; do convert -size 800x600 xc:black -font "FreeMono" -pointsize 60 -fill white -draw @$f $f.png; echo $f; done
ffmpeg -y -framerate 1000 -i samples/%08d.txt.png output/samples.mkv
cp samples/data.csv ./output/
