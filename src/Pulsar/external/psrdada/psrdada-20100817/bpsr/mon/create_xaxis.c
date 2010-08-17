/***************************************************************************/
/*                                                                         */
/* function create_xaxis                                                   */
/*                                                                         */
/* it fills the xaxis array with suitable values for 1-D plots             */
/*                                                                         */
/* RB 05 Oct 2008: X axis in proper units (e.g. MHz, sec, Hz)             */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

void create_xaxis(char inpfile[], int plotnum, long totvaluesread, 
		  long totvalues4plot, long *inivalue4plot, long *endvalue4plot, 
		  float zoomt_begin, float zoomt_end,
		  float zoomf_begin, float zoomf_end,
		  float fch1, float chbw,float xscale, int nchan, 
		  float tsamp, float *x_read)
{
  int exponent=0;
  long ii;
  unsigned long nfft;
  float frqbin;

  if ((strstr(inpfile,"bp")) != NULL)
  {
    for(ii=0; ii<=nchan-1; ii++) x_read[ii]=fch1-(float)ii*chbw;
    *inivalue4plot = 0;
    *endvalue4plot = totvalues4plot;
  }
  else if ((strstr(inpfile,"ts")) != NULL)
  {
    if (plotnum==0) 
     {
       for(ii=0; ii<=totvalues4plot-1; ii++) x_read[ii]=tsamp*((float)ii+1.0);
       *inivalue4plot = (long)(zoomt_begin/tsamp);
       *endvalue4plot = (long)(zoomt_end/tsamp);
       if (*inivalue4plot <= 0) *inivalue4plot = 0;
       if (*endvalue4plot > totvalues4plot) *endvalue4plot = totvalues4plot;
       if (*inivalue4plot >= totvalues4plot) {
	 printf(" Requested initial time %f sec is too high! \n ", zoomt_begin );
	 exit(-2);
         }
     }
    else if (plotnum==1) 
     {
       while( pow(2.0,(double)exponent) < totvaluesread) exponent++;
       nfft=pow(2.0, (double)exponent-1);       
       frqbin=1/(tsamp*(float)nfft);
       for(ii=0; ii<=totvalues4plot-1; ii++) x_read[ii]=frqbin*((float)ii+1.0);
       *inivalue4plot = (long)(zoomf_begin/frqbin);
       *endvalue4plot = (long)(zoomf_end/frqbin);  
       if (*inivalue4plot <= 0) *inivalue4plot = 0;
       if (*endvalue4plot > totvalues4plot) *endvalue4plot = totvalues4plot;
       if (*inivalue4plot >= totvalues4plot-1) {
	 printf(" Requested min frequency %f Hz is too high! \n ", zoomf_begin);
	 exit(-2);
         }
     }
  }
  else if (strings_compare(inpfile,"time1.dat")) 
  {
    if (plotnum==0) 
     {
       for(ii=0; ii<=totvalues4plot-1; ii++) x_read[ii]=tsamp*((float)ii+1.0);
       *inivalue4plot = (long)(zoomt_begin/tsamp);
       *endvalue4plot = (long)(zoomt_end/tsamp);
       if (*inivalue4plot <= 0) *inivalue4plot = 0;
       if (*endvalue4plot > totvalues4plot) *endvalue4plot = totvalues4plot;
       if (*inivalue4plot >= totvalues4plot) {
	 printf(" Requested initial time %f sec is too high! \n ", zoomt_begin );
	 exit(-2);
         }
    }
    else if (plotnum==1) 
     {
       while( pow(2.0,(double)exponent) < totvaluesread) exponent++;
       nfft=pow(2.0, (double)exponent-1);       
       frqbin=1/(tsamp*(float)nfft);
       for(ii=0; ii<=totvalues4plot-1; ii++) x_read[ii]=frqbin*((float)ii+1.0);
       *inivalue4plot = (long)(zoomf_begin/frqbin);
       *endvalue4plot = (long)(zoomf_end/frqbin);  
       if (*inivalue4plot <= 0) *inivalue4plot = 0;
       if (*endvalue4plot > totvalues4plot) *endvalue4plot = totvalues4plot;
       if (*inivalue4plot >= totvalues4plot-1) {
	 printf(" Requested min frequency %f Hz is too high! \n ", zoomf_begin);
	 exit(-2);
         }
    }
  }
  else if (strings_compare(inpfile,"rawstat0.dat")) 
  {
    if (plotnum==0) 
      {
	for(ii=0; ii<=totvalues4plot-1; ii++) x_read[ii]=tsamp*((float)ii+1.0);
       *inivalue4plot = (long)(zoomt_begin/tsamp);
       *endvalue4plot = (long)(zoomt_end/tsamp);
       if (*inivalue4plot <= 0) *inivalue4plot = 0;
       if (*endvalue4plot > totvalues4plot) *endvalue4plot = totvalues4plot;
       if (*inivalue4plot >= totvalues4plot) {
	 printf(" Requested initial time %f sec is too high! \n ", zoomt_begin );
	 exit(-2);
         }
      }
    else if (plotnum==1) 
      {
       while( pow(2.0,(double)exponent) < totvaluesread) exponent++;
       nfft=pow(2.0, (double)exponent-1);       
       frqbin=1/(tsamp*(float)nfft);
       for(ii=0; ii<=totvalues4plot-1; ii++) x_read[ii]=frqbin*((float)ii+1.0);
       *inivalue4plot = (long)(zoomf_begin/frqbin);
       *endvalue4plot = (long)(zoomf_end/frqbin);  
       if (*inivalue4plot <= 0) *inivalue4plot = 0;
       if (*endvalue4plot > totvalues4plot) *endvalue4plot = totvalues4plot;
       if (*inivalue4plot >= totvalues4plot-1) {
	 printf(" Requested min frequency %f Hz is too high! \n ", zoomf_begin);
	 exit(-2);
         } 
     }
  }
  else if (strings_compare(inpfile,"rawstat1.dat")) 
  {
    if (plotnum==0) 
      {
	for(ii=0; ii<=totvalues4plot-1; ii++) x_read[ii]=tsamp*((float)ii+1.0);
       *inivalue4plot = (long)(zoomt_begin/tsamp);
       *endvalue4plot = (long)(zoomt_end/tsamp);
       if (*inivalue4plot <= 0) *inivalue4plot = 0;
       if (*endvalue4plot > totvalues4plot) *endvalue4plot = totvalues4plot;
       if (*inivalue4plot >= totvalues4plot) {
	 printf(" Requested initial time %f sec is too high! \n ", zoomt_begin );
	 exit(-2);
         }
      }
    else if (plotnum==1) 
      {
       while( pow(2.0,(double)exponent) < totvaluesread) exponent++;
       nfft=pow(2.0, (double)exponent-1);       
       frqbin=1/(tsamp*(float)nfft);
       for(ii=0; ii<=totvalues4plot-1; ii++) x_read[ii]=frqbin*((float)ii+1.0);
       *inivalue4plot = (long)(zoomf_begin/frqbin);
       *endvalue4plot = (long)(zoomf_end/frqbin);  
       if (*inivalue4plot <= 0) *inivalue4plot = 0;
       if (*endvalue4plot > totvalues4plot) *endvalue4plot = totvalues4plot;
       if (*inivalue4plot >= totvalues4plot-1) {
	 printf(" Requested min frequency %f Hz is too high! \n ", zoomf_begin);
	 exit(-2);
         }
     }
  }
  else
  {
    printf(" Not a recognized file name!");
    printf(" Creating a standard X-axis array with %ld unitary values \n",
           totvalues4plot);
    for(ii=0; ii<totvalues4plot-1; ii++) x_read[ii]=((float)ii+1.0);
  }
}

