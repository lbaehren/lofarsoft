/***************************************************************************
 *
 *   Copyright (C) 1998 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* endian modification functions
 *
 * NOTE: for general use the macros in machine_endian.h are probably
 * the best thing to use since they do the preprocessor stuff. just
 * use one of
 *
 * [to|from][Big|Little]Endian(void *object, int nbytes)
 *
 */
#include "machine_endian.h"
#include <string.h>
#include <stdlib.h>

/* function to change endian-ness of a data, maximum 8 bytes long */
void changeEndian(void *num, int nbytes)
{
  unsigned char tmp[8];
  unsigned char *numPtr = (unsigned char *)num;
  int i;

  for (i=0; i < nbytes; i++)
    tmp[i] = numPtr[nbytes-i-1];
  
  memcpy((void *)num, (void *)tmp, nbytes);
}

void array_changeEndian (int count, void *p, int element_size)
{
  unsigned char *Ptr = (unsigned char *) p;
  int i;
  for (i=0; i < count; i++)  {
    changeEndian (Ptr, element_size);
    Ptr += element_size;
  }
}

