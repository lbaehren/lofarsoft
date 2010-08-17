/***************************************************************************/
/*                                                                         */
/* function strings_compare                                                */
/*                                                                         */
/* it compares strings                                                     */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

int strings_compare(char *string1, char *string2) 
{
  if (!strcmp(string1,string2)) {
    return 1;
  } else {
    return 0;
  }
}

