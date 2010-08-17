/****************************************************************/
/*                                                              */
/* function do_powerspec                                        */
/*                                                              */
/* calculate the power spectrum of a set of "nfft" real-data    */
/* points stored in the float array "timeseries".               */
/*                                                              */
/* Since "nfft" MUST be a power of two, the power spectrum is   */
/* calculated on the first nfft (over "nvalues") elements in    */
/* the array, with "nfft" being the largest power-of-2 value    */
/* smaller than "nvalues"                                       */
/*                                                              */
/****************************************************************/

#include "plot4mon.h"

void do_powerspec(float timeseries[], float powsp[], 
		  long nvalues, long *totvalues4plot, float tsamp)
{
  long i;
  int isign=1;
  float spd;
  int exponent=0;
  long minbin,maxbin,nsegments;
  unsigned long nfft; 

    /* obtain the nearest power of two number to the integer passed down */
  while( pow(2.0,(double)exponent) < nvalues) exponent++;
  nfft=pow(2.0, (double)exponent-1);

   /* finally run the FFT */
  realft(timeseries,nfft,isign);

  /* calculate the idx of the min interesting bin (i.e. the FREQmin=1/Pmax) */
  minbin=(long)((float)(1.0/MAX_PERIOD)*nfft*tsamp);
  /* calculate the idx of the max interesting bin (i.e. the FREQmax=1/Pmin) */
  maxbin=(long)((float)(1.0/MIN_PERIOD)*nfft*tsamp);
  if (maxbin > (nfft/2)) {
    printf(" Not enough available spectral bins! \n");
    printf(" %ld bin requested vs %ld available. \n",maxbin,nfft/2);
    exit(-1);
  } else {
    *totvalues4plot=maxbin;
  }
  for (i=0; i<=(nfft/2)-1; i++) 
   {
       if (i <= minbin) 
	{ powsp[0]=0.0; }
       else
        {
          spd=2.0*(timeseries[2*i+1]*timeseries[2*i+1]+
                   timeseries[2*i]*timeseries[2*i]);
          powsp[i]=spd;
        }
   }

  /*  whiten the power spectrum */
  nsegments=(long)pow(2.0, (double)exponent-8); // segments are 2^7=28 bin long
  whiten(&powsp[0], (long)nfft/2, nsegments);

  /* calculate the one sided (hence the factor 2) power spectral density */

  printf(" Created and whitened the power spectrum over ");
  printf("a 2^%d=%ld sample timeseries\n",exponent-1,nfft);
  printf(" from the %ld-th to the %ld-th spectral bin, corresponding to \n",
             minbin,maxbin);
  printf(" a minimum and maximum frequencies 1/%.1f Hz and %.1f Hz \n",
	     MAX_PERIOD, 1/MIN_PERIOD);
  printf(" whereas the Nyquist frequencies is %.1f Hz \n",
	     0.5/tsamp);
}

