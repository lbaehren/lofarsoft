#!/bin/bash

#echo Doing mogrify script

#mogrify -crop 1080x1080+180+180 -format ppm *.png

echo converting to MPEG-2

cat *.ppm | ppmtoy4m -F 25:1 -S 420mpeg2 | mpeg2enc -f 13 -4 1 -2 1 -q7 -b 20000 -V 300 -P -g 6 -G 18 -I 0 -x 1080 -M 2 -o  VHECRanimation.m2v

