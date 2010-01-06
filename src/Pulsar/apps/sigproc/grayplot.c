
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cpgplot.h"
#include "sigproc.h"
int noffset=0;
int flip=0;
int verbose=0;
int maxpts=32768*32;
float xxmin=0.0,yymin=1.0e32,zzmin=1.0e32,grmin=1.0e32;
float xxmax=0.0,yymax=-1.0e32,zzmax=-1.0e32,grmax=-1.0e32;
void minmax(float *data, int npts, float *datamin, float *datamax)
{ 
  int i; 
  *datamin=data[0];
  *datamax=data[0];
  for (i=1; i<npts; i++) {
    *datamin = (data[i] < *datamin) ? data[i] : *datamin;
    *datamax = (data[i] > *datamax) ? data[i] : *datamax;
  }
}
main(int argc, char *argv[])
{
  float *x, *y, xmin, xmax, ymin, ymax, zmin, zmax, *twod, tr[6], deltat,f;
  int i,npts,nlines,nx,ny,pggray,ntwod,first,itwod,subints;
  char device[80], title[80];

  x = (float *) malloc(maxpts * sizeof(float));
  y = (float *) malloc(maxpts * sizeof(float));
  xmin=xmax=ymin=ymax=zmin=zmax=0.0;
  nx=ny=first=1;
  nlines=pggray=itwod=0;
  tr[0]=0.0;
  tr[1]=1.0;
  tr[2]=0.0;
  tr[3]=1.0;
  tr[4]=0.0;
  tr[5]=1.0;
  strcpy(device,"/xs");
  strcpy(filename,argv[1]);

  cpgbeg(0,device,1,1);
  cpgsch(1.5);
  cpgscf(2);
  cpgsvp(0.15,0.85,0.15,0.85);

  //  cpggray(twod,ntwod,nlines,1,ntwod,1,nlines,zmax,zmin,tr);
  //  cpgswin(xxmin,xxmax,yymin,yymax); 
  cpgbox("bcnst",0.0,0,"bvcnst",0.0,0);

  cpgend();
  exit(0);
}
