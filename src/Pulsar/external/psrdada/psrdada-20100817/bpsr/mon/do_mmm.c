/****************************************************************/
/*                                                              */
/* function do_minmax                                        */
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

void do_mmm (float timeseries[], float mmm[], 
		long nvalues, long *totvalues4plot, float tsamp)
{
  long reduction_factor = MMM_REDUCTION;
  long result_nvalues = nvalues / reduction_factor;

  float* current_reduction = timeseries;

  long i=0, v=0;
  float max=0, min=0, tot=0;

  for (i=0; i < result_nvalues; i++)
    {
      max = min = tot = current_reduction[0];
     
      for (v=1; v < reduction_factor; v++)
	{
	  tot += current_reduction[v];
	  if (current_reduction[v] > max)
	    max = current_reduction[v];
	  if (current_reduction[v] < min)
	    min = current_reduction[v];
	}

      mmm[i*3+0] = min;
      mmm[i*3+1] = tot / (float)reduction_factor;
      mmm[i*3+2] = max;

      current_reduction += reduction_factor;
    }

  *totvalues4plot = result_nvalues;
}

