/***************************************************************************/
/*                                                                         */
/* function whiten                                                         */
/*                                                                         */
/* It whitens a power spectrum of length npts by subtracting a running     */
/* median so that it has zero mean. After that the data are normalized     */
/* to unit rms so that the amplitude is directly the spectral S/N          */
/* (derived from an orignal version by Lorimer 2001)                       */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

void whiten(float *data, long npts, long nsegments) 
{
  int i,j,k;
  unsigned long nwrk;
  float *work, *median, sumsq, rms;

  /* allocate space to store medians and data segments */
  nwrk=npts/nsegments;
  median=(float *) malloc(nsegments*sizeof(float));
  work=(float *) malloc(nwrk*sizeof(float));

  /* find the median of each segment */
  for (i=0; i<nsegments; i++) {
    k=0;
    for (j=i*nwrk; j<(i+1)*nwrk; j++) work[k++]=data[j];
    median[i]=selectmed(nwrk/2,nwrk,work-1);
  }

  /* subtract running median and sum squares every npts/1024 samples */
  sumsq=0.0;  j=npts/1024;
  for (i=0; i<npts; i++) {
    if (data[i] != 0.0) data[i]-=median[i/nwrk];
    if (i%j == 0) sumsq+=data[i]*data[i];
  }

  /* normalize to unit rms */
  rms=sqrt(sumsq/1024.0);  
  for (i=0; i<npts; i++) data[i]/=rms;

  /* job done - free up space and return */
  free(median);  
  free(work);
}
