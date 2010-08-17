/***************************************************************************/
/*                                                                         */
/* function set_array_dim                                                  */
/*                                                                         */
/* it determines the number of rows and columns in the array for 2-D plots */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

void set_array_dim(long totvalues4plot, int nchan, int *nbin_x, int *nsub_y)
{
  *nbin_x=nchan;
  *nsub_y=totvalues4plot/nchan;
  printf(" The 2-D array has dimension = [ %d x %d ] \n", *nbin_x, *nsub_y);
  if ((long)((*nbin_x)*(*nsub_y)) != totvalues4plot)
    {
      printf(" Mismatch between number of read and plotted values!\n");
      printf(" %ld values in the 2-D array wrt %ld read values \n",
	     (long)((*nbin_x)*(*nsub_y)),totvalues4plot);
      printf(" Lost in plotting the final tail of the data \n");
    }
}

