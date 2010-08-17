/***************************************************************************/
/*                                                                         */
/* compute_extremes                                                        */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

void compute_extremes(float array[], long nvalues, long inivalue,
                      float *max_array, float *min_array)
{
  long i;
  *max_array=-1e38;
  *min_array=+1e38;
  for (i=inivalue;i<=(nvalues-1);i++)
    {
      if (array[i]>(*max_array)) *max_array=array[i];
      if (array[i]<(*min_array)) *min_array=array[i];
    }
}
