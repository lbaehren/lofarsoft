/***************************************************************************/
/*                                                                         */
/* function display_help                                                   */
/*                                                                         */
/* it displays the help on command line                                    */
/*                                                                         */
/* RB 06 Oct 2008:	revised 					   */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

void display_help(char *program) 
{
  puts("");
  printf("PROGRAM \"%s\" is a part of BPSRMON version: %.1f \n",
           program, BPSRMON_VERSION);
  printf("It produces various plots to monitor BPSR activity\n");
  puts("");
  puts("Usage: plot4mon [filename0] [filename1] {-h, -g, -zf, -zf, -G ...}");
  puts("");
  puts("filename0     -  data file [bandpass/time series] for polarisation 0");
  puts("filename1     -  data file [bandpass/time series] for polarisation 1");
  puts("-h            -  print this help page");
  puts("-zf <NNN-MMM> -  zoom on NNN-MMM Hz interval (eg: -zf 10-100)");
  puts("-zt <PP-QQ>   -  zoom on PP-QQ sec interval (eg: -zt 1.5-5.5)");
  puts("-G <AAAxBBB>  -  display resolution for plot (eg: -G 240x200)");  
  puts("-nolabel      -  omit plot labels and title ");
  puts("-nobox        -  omit plot box ");
  puts("-log          -  plot bandpass on log scale ");
  puts("-mmm          -  plot time series min, mean, and max ");
printf("-g <device>   -  display device for plot e.g. /xs  (def=%s)\n", STD_DEVICE);  
  puts("");
}
