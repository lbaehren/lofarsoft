/***************************************************************************/
/*                                                                         */
/* function work_on_data                                                   */
/*                                                                         */
/* it operates in various ways on the input array readstream, producing    */
/* a new array newstream which will be what plotted by following routines  */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

void work_on_data(char inpfile[], float *readstream, float *newstream, 
		  long totvaluesread, long *totvalues4plot, float tsamp,
		  float yscale, int plotnum, char add_work[], 
		  int dolog, int dommm)
{
  long jj;

  if (dolog == 1)
   {
     printf(" \n taking log of bandpass \n");
     for (jj=0; jj<=totvaluesread-1; jj++) { 
	if ( readstream[jj] != 0.0 ) readstream[jj]=log(readstream[jj]); }
     *totvalues4plot=totvaluesread;
   }

  if (plotnum==0)
  {
    if (dommm && (strstr(inpfile,"ts") != NULL))
    {
         printf(" \n Doing the max min ...\n");
	 do_mmm (readstream,newstream,
		    totvaluesread,totvalues4plot,tsamp);
    }

    else
      {
	for (jj=0; jj<=totvaluesread-1; jj++) 
	  newstream[jj]=readstream[jj]/yscale;
	*totvalues4plot=totvaluesread;
      }
  }
  else if (plotnum==1) 
   {
      if (strings_compare(add_work,"fft"))
       {
         printf(" \n Doing the power spectrum...\n");
	 do_powerspec(&readstream[0],&newstream[0],
		      totvaluesread,totvalues4plot,tsamp);

	 if (dolog == 1)
	   {
	     printf(" \n taking log of fluctuation power spectrum \n");
	     for (jj=0; jj<*totvalues4plot; jj++) { 
	       if ( newstream[jj] > 1.0 )
		 {
		   newstream[jj]=log(newstream[jj])/log(10.0);
		 }
	       else
		 newstream[jj]=0;
	     }
	   }

	 printf(" Obtained a power spectrum with %ld bins \n",*totvalues4plot);
       } 
      else
       {
	 printf(" To be implemented yet !\n");
       }
   }
  else
   {
      printf(" Not an allowed value for plotnum=%d \n",plotnum);
   }
}

	 
