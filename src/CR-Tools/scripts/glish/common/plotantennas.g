# INSTRUCTIONS #
# start gui
# include this file
# open antenna layout

include 'matrix.g'

#-----------------------------------------------------------------------
#Name: plotantf
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Plots the current antenna layout 
#
#Ret: Boolean
#
#Par: rec	= 	- data record, where data is stored
#Par: index	= 1	- Index of data set to be used
#Par: kascade = T       - Flag describing whether the LOPES
#                         detectors should be plotted as well
#Par: source ='sun'     - draw and arrow in the direction of 
#                         an astronomical object
#                         Set to F (bool) if no object desired
#Par: azel =[az,el]     - Describing the direction in radians
#-----------------------------------------------------------------------

#-97.5

plotantf := function (ref plotant, ref rec, index=-1, kascade=T, antpos = 0,
                      source='sun', azel=-1, xy=F, observatory='LOPES')
{

  if (observatory=='LOPES') {size:=135; offset:=45; dia:=7.5};
  if (observatory=='LOFAR-ITS') {size:=135; offset:=0; dia:=7.5};
  if (observatory=='LORUN') {size:=30; offset:=30; dia:=2.5};
  x1:=-size-offset; x2:=size; y1:=-size; y2:=size;

  if (!is_record(plotant)) { val plotant:=[=] }
    
  if (!has_field(plotant,'pg') || len(plotant.pg)<=1) {
    plotant.pg := pgplotter(size=[800,600],foreground='black',background='white')
  } else {
    plotant.pg.gui(); plotant.pg.clear();
  }

  plotant.pg.title(spaste('Antenna Layout for LOPES-Tools: ',observatory));
  plotant.pg.sch(1.5);
  plotant.pg.slw(2);
  plotant.pg.env(x1,x2,y1,y2,1,0)
  plotant.pg.lab('W->E Direction','S->N Direction','Antenna Layout')
  plotant.pg.sch(2);

if (kascade && observatory=='LOPES') {
#define a rectangle and rotate it 
angle:=-15.
dist:=13.
rect:=array(0,4,2); rot:=rect; xypos:=rect
rect[1,]:=[-1,1]; rect[2,]:=[1,1]; rect[3,]:=[1,-1]; rect[4,]:=[-1,-1]; 
for (i in seq(shape(rect)[1])) rot[i,]:=mx.rotate(rect[i,]*3,angle,T)

plotant.pg.sci(1)
for (iy in seq(16)) {
  for (ix in seq(16)) {
    x:=(ix-1)*dist-97.5
    y:=(iy-1)*dist-97.5
    pos:=mx.rotate([x,y],angle,T)
    for (i in seq(shape(xypos)[1])) xypos[i,]:=pos+rot[i,]
#plot station outlines
plotant.pg.sfs(2);
    if (!(ix>=8 && ix<=9 && iy>=8 && iy<=9)) plotant.pg.poly(xypos[,1],xypos[,2])
#plot all stations hatched
plotant.pg.sfs(3);
    if (!(ix>=8 && ix<=9 && iy>=8 && iy<=9)) plotant.pg.poly(xypos[,1],xypos[,2])

#plot inner clusters stations cross-hatched
plotant.pg.sfs(4);
    if ((ix>4 && ix<13 && iy>4 && iy<13 )&&!(ix>=8 && ix<=9 && iy>=8 && iy<=9)) plotant.pg.poly(xypos[,1],xypos[,2])
    }
  }
}

if (kascade && observatory=='LOFAR-ITS') {
#define a rectangle and rotate it 
angle:=-45.; xs:=3; ys:=8;
rect:=array(0,4,2); xypos:=rect
rect[1,]:=[-1,1]; rect[2,]:=[1,1]; rect[3,]:=[1,-1]; rect[4,]:=[-1,-1]; 
xpos:=0; ypos:=2;
for (i in seq(shape(rect)[1])) xypos[i,]:=[xpos,ypos]+mx.rotate(rect[i,]*[xs,ys],angle,T)
plotant.pg.sci(1)
plotant.pg.sfs(2);
plotant.pg.slw(3);
plotant.pg.poly(xypos[,1],xypos[,2])
}


if (index==-1) {idx:=data.selant()} else {idx:=index}

if (!globalpar.plotantennas.phasecenter) {refant:=0} else {refant:=1};
if (!is_record(antpos))
{antpos:=CalcAntPos(rec,idx,refant=refant,phasecenter=globalpar.plotantennas.phasecenter)};

#outline fill area mode for circle
plotant.pg.sfs(2);

#x and y is interchanged in antenna position!
plotant.pg.sci(4)
plotant.pg.slw(3);

#get characterheight in world coordinate units
plotant.pg.sch(1);
cht:=plotant.pg.qcs(4)[2];

for (a in antpos) {
  pos:=a.position
  plotant.pg.ptxt(pos[2],pos[1]-cht/2.8,0,0.5,as_string(a.antenna))
  plotant.pg.circ(pos[2],pos[1],dia)
}

if (source) {
  rec.doframe()
  sdir:=dm.measure(dm.direction(source),'AZEL')
  az:=sdir.m0.value; el:=sdir.m1.value
  if (el>=0) {
    x:=sin(az)*size*cos(el)
    y:=cos(az)*size*cos(el)
    xto:=sin(az)*cht/1.9
    yto:=cos(az)*cht/1.9
    plotant.pg.sci(3)
    plotant.pg.sfs(1);
    plotant.pg.arro(0,0,x,y)
    txan:=az/degree-90.
#  if (x<0) txan:=txan-180
#  if (y>0) txan:=txan+180
    plotant.pg.ptxt(x+xto,y+yto,txan,0.5,as_string(source))
 }
}
x0:=0; y0:=0;
if (len(xy)==2 && xy != [0,0]) {
  plotant.pg.sfs(2);
  plotant.pg.sci(2)
  x0:=xy[1];y0:=xy[2];
#  pos:=mx.rotate([x0,y0],angle,T)
#  x0:=pos[1]; y0:=pos[2];
  plotant.pg.ptxt(x0,y0-cht/2.8,0,0.5,'X')
  plotant.pg.circ(x0,y0,7.5)
}

if (len(azel)==2 && azel[2]<((pi/2.)/(1.+1e-6))) {
  az:=azel[1]; el:=azel[2];
  x:=sin(az)*size*cos(el)
  y:=cos(az)*size*cos(el)
  xto:=sin(az)*cht/1.9
  yto:=cos(az)*cht/1.9
  plotant.pg.sci(2)
  plotant.pg.sfs(1);
  plotant.pg.arro(x0,y0,x0+x,y0+y)
  txan:=az/degree-90.
# if (x<0) txan:=txan-180
# if (y>0) txan:=txan+180
  plotant.pg.ptxt(x0+x+xto,y0+y+yto,txan,0.5,as_string('dir'))
}
}


#dm.gui()
