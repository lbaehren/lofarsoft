/***************************************************************************/
/*                                                                         */
/* function check_file_exists                                              */
/*                                                                         */
/* it checks for the existence of a file                                    */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

int check_file_exists(char *filename) 
{
  if ((fopen(filename,"rb"))==NULL) {
        return(0);
  } else {
        return(1);
  }
}

