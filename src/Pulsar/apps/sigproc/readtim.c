#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "header.h"
#include "sigproc.h"

FILE *timfile;

void readhd_(char filename[80], float *rdm, float *rtsmp)
{
  int i;
  
  /* first fix filename */
  for (i=0; i<strlen(filename); i++) {
    if (filename[i]==' ') {
      filename[i]='\0';
      break;
    }
  }

  /* now open up the file and read in the header */
  refdm=tsamp=0.0;
  timfile=open_file(filename,"rb");
  read_header(timfile);
  *rtsmp=(float) tsamp;
  *rdm=(float) refdm;
  if (nbits != 32 && nbits != 8 && nbits != 16) {
    fprintf(stderr,"cannot read %d bit data..\n");
    exit(1);
  }
}

int skipsample_(int *nskip)
{
  long offset;
  if (feof(timfile)) {
    fclose(timfile);
    return 0;
  }
  offset = (long) (*nskip) * (nbits/8);
  return fseek(timfile,offset,SEEK_CUR);
}
int readsample_(float *sample) 
{
  unsigned short *twobytes;
  char *byte;

  if (feof(timfile)) {
    fclose(timfile);
    return 0;
  }
  switch (nbits) {
  case 32:
    fread(sample,nbits/8,1,timfile);
    break;
  case 16:
    fread(twobytes,nbits/8,1,timfile);
    *sample = (float) *twobytes;
    break;
  case 8:
    fread(byte,nbits/8,1,timfile);
    *sample = (float) *byte;
    break;
  }
  return(1);
}
