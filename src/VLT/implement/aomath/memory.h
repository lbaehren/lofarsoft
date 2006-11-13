#ifndef MEMORY_H
#define MEMORY_H
/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: memory.h,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  16/01/02  created
*/

/************************************************************************
 *
 *----------------------------------------------------------------------
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

class memoryPool {

  public:

    memoryPool(int size);
    ~memoryPool();

    void *malloc(int size);
    void free(void *ptr);
    void show(int type = 0);

  private:
    
#ifdef HAVE_VXWORKS
    char *memoryBlock;
    PART_ID partition;
#endif
    bool constructed;
};

#endif /*!MEMORY_H*/
