/***************************************************************************/
/*                                                                         */
/* compute_margin                                                          */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

void compute_margin(long nvalues, float max_arr, float min_arr, float *margin)
{
  float span, step;
  const float factor=1.0;   // expansion factor for the margin

  span=max_arr-min_arr;
  step=span/(((float)nvalues)-1);
  *margin=factor*step;
}

