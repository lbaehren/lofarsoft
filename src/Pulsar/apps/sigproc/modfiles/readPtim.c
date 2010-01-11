#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "header.h"
#include "sigproc.h"
#include "makeinf.h"
#include "vectors.h"

FILE *timfile;

void readphd_(char filename1[80], float *rdm, float *rtsmp)
{
   int i;
   char filename[80];
   char filename2[80];
   infodata idata;
    for (i=0; i<strlen(filename1); i++) {
	if (filename1[i]=='.' && filename1[i+1]=='i' && filename1[i+2]=='n' && filename1[i+3]=='f') {
           filename[i]='\0';
	   break;
        } else {
           filename[i]=filename1[i];
	}
    }
    nbits=32;
    readinf(&idata, filename);
    *rdm=(float) idata.dm;
    *rtsmp=(float) idata.dt;
    sprintf(filename2, "%s.dat", filename);
    timfile=open_file(filename2,"rb");
}
