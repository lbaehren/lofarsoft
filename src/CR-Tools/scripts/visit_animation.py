
## -------------------------------------------------------------------
## $Id:: CMakeLists.txt 790 2007-09-19 14:12:22Z baehren             $
## ===================================================================
##
## [1] Run VisIt to create frames for the movie
##
##     visit -cli -s animation.py
##
## [2] Run mpeg_encode to combine frames into a movie
##
##     mpeg_encode movie.par
##
## ===================================================================

## import additional Python modules

from math import *

## -------------------------------------------------------------------
## Import saved session to get the basic parameters correct

RestoreSession("visit0004.session",0);

## -------------------------------------------------------------------
## Set attributes

## set attributes for saving a display window

s = SaveWindowAttributes();
#s.format = s.PPM
s.format = s.POSTSCRIPT
s.outputToCurrentDirectory = 0;
s.outputDirectory = "frames";
s.fileName = "frame";
#s.width  = 512
#s.height = 512
SetSaveWindowAttributes(s);

## -------------------------------------------------------------------
# Create control points for the view

nofFrames=360

v = GetView3D();

for i in range(nofFrames):
    v.viewNormal = (-sin(radians(i)),cos(radians(i)),0.6);
#    v.viewNormal = (-sin((i+20)*0.05),cos((i+20)*0.05),0.6);
    ## rotation combined with moving upwards along the z-axis
#    v.viewNormal = (-sin((i+20)*0.05),cos((i+20)*0.05),-0.6+i*0.005);
#    v.viewNormal = (-0.552867-sin(i*0.05), -0.830462+cos(i*0.05), -0.0683444+i*0.001);
    v.viewUp = (0,0,1);
    v.viewAngle = 30;
    SetView3D (v);
    SaveWindow();

## Store the session

ToggleCameraViewMode();
SaveSession("keyframe.session");

## exit when everything is done

quit();
