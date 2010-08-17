/***************************************************************************/
/*                                                                         */
/* function create_labels                                                  */
/*                                                                         */
/* it creates the labels to be applied for the plot resulting from         */
/* the file outputfile                                                     */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"
#include <string.h>

void create_labels(char inpfile[], int plotnum,
		   char xlabel[],char ylabel[],char plottitle[])
{
  //if (strings_compare(inpfile,"bpm0.dat")) 
  //printf(" inp file %s %c ",inpfile, inpfile[29]);

  //if (inpfile[29] != "b"){ 
  //printf(" inp file %s %c ",inpfile, inpfile[29]);
  //}

  if ( strstr(inpfile,"bp0") != NULL ) {
    strcpy(xlabel,"Frequency channels");
    strcpy(ylabel,"Y-values (arbitrary units)");
    strcpy(plottitle,"Mean bandpass for pol 0 along the full data length");
  }
  //else if (strings_compare(inpfile,"bpm1.dat")) 
  else if (inpfile[29] == "b")
  {
    strcpy(xlabel,"Frequency channels");
    strcpy(ylabel,"Y-values (arbitrary units)");
    strcpy(plottitle,"Mean bandpass for pol 1 along the full data length");
  }
  else if (strings_compare(inpfile,"bp0.dat")) 
  {
    strcpy(xlabel,"Frequency channels");
    strcpy(ylabel,"Sub-integrations (1-sec long)");
    strcpy(plottitle,"Evolution of mean bandpass for pol 0");
  }
  else if (strings_compare(inpfile,"bp1.dat")) 
  {
    strcpy(xlabel,"Frequency channels");
    strcpy(ylabel,"Sub-integrations (1-sec long)");
    strcpy(plottitle,"Evolution of mean bandpass for pol 1");
  }
  else if (strings_compare(inpfile,"bp0rms.dat")) 
  {
    strcpy(xlabel,"Frequency channels");
    strcpy(ylabel,"Sub-integrations (1-sec long)");
    strcpy(plottitle,"Evolution of RMS bandpass for pol 0");
  }
  else if (strings_compare(inpfile,"bp1rms.dat")) 
  {
    strcpy(xlabel,"Frequency channels");
    strcpy(ylabel,"Sub-integrations(1-sec long) ");
    strcpy(plottitle,"Evolution of RMS bandpass for pol 1");
  }
  //else if (strings_compare(inpfile,"time0.dat")) 
  else if (inpfile[29] == "t")
  {
    if (plotnum==0) {
    strcpy(xlabel,"Time (sec)");
    strcpy(ylabel,"Amplitude/channel (arbitrary units)");
    strcpy(plottitle,"ZeroDM time series for pol 0");
    } else if (plotnum==1) {
    strcpy(xlabel,"Frequency (Hz)");
    strcpy(ylabel,"Spectral S/N");
 strcpy(plottitle,"Normalized power spectrum of ZeroDM time series for pol 0");
    }
  }
  //else if (strings_compare(inpfile,"time1.dat")) 
  else if (inpfile[29] == "t")
  {
    if (plotnum==0) {
    strcpy(xlabel,"Time (sec)");
    strcpy(ylabel,"Amplitude/channel (arbitrary units)");
    strcpy(plottitle,"ZeroDM time series for pol 1");
    } else if (plotnum==1) {
    strcpy(xlabel,"Frequency (Hz)");
    strcpy(ylabel,"Spectral S/N");
 strcpy(plottitle,"Normalized power spectrum of ZeroDM time series for pol 1");
    }
  }
  else if (strings_compare(inpfile,"rawstat0.dat")) 
  {
    if (plotnum==0) {
    strcpy(xlabel,"Time (sec)");
    strcpy(ylabel,"Amplitude (arbitrary units)");
    strcpy(plottitle,"Raw sample statistics at channel 512 for pol 0");
    } else if (plotnum==1) {
    strcpy(xlabel,"Frequency (Hz)");
    strcpy(ylabel,"Spectral S/N");
 strcpy(plottitle,"Normalized power spectrum of ch=512 time series for pol 0");
    }
  }
  else if (strings_compare(inpfile,"rawstat1.dat")) 
  {
    if (plotnum==0) {
    strcpy(xlabel,"Time (sec)");
    strcpy(ylabel,"Amplitude (arbitrary units)");
    strcpy(plottitle,"Raw sample statistics at channel 512 for pol 1");
    } else if (plotnum==1) {
    strcpy(xlabel,"Frequency (Hz)");
    strcpy(ylabel,"Spectral S/N");
 strcpy(plottitle,"Normalized power spectrum of ch=512 time series for pol 1");
    }
  }
  else
  {
    printf(" Not a recognized file name!");
    printf(" Applying standard settings to the plot. \n");
    strcpy(xlabel,"X-values (unknown units)");
    strcpy(ylabel,"Y-values (unknown units)");
    strcpy(plottitle,"Unknown plot");
  }
}

