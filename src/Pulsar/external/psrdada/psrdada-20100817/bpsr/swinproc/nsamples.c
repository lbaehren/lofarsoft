#include <math.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

long long nsamples(char *filename,int headersize, int nbits, int nifs, int nchans) /*includefile*/
{
  long long datasize,numsamps;
  struct stat stbuf;

  long long filesize;

  if (stat(filename,&stbuf) == -1){
    fprintf(stderr,"Cannot determine status of %s\n",filename);
    exit(1);
  }
  filesize = stbuf.st_size;

  datasize=filesize-headersize;
  numsamps=(long long) (long double) (datasize)/ (((long double) nbits) / 8.0) 
		 /(long double) nifs/(long double) nchans;
  return(numsamps);
}
