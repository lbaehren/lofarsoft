/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: testMath.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* macaomgr  27/02/03  created
*/

/************************************************************************
*   NAME
*   
* 
*   SYNOPSIS
*   
* 
*   DESCRIPTION
*
*   FILES
*
*   ENVIRONMENT
*
*   COMMANDS
*
*   RETURN VALUES
*
*   CAUTIONS 
*
*   EXAMPLES
*
*   SEE ALSO
*
*   BUGS   
* 
*------------------------------------------------------------------------
*/
#define _POSIX_SOURCE 1

// Uncomment this if you are using the VLT environment
// #include "vltPort.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *rcsId="@(#) $Id: testMath.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#include <aomath/MAStdFilter.h>

extern "C" int testMath()
{
    MAFilterFloat filter1(10);
    MAStdFilterFloat filter2(10);

    for(int x=0;x<20;x++) {
      filter1.Add((float)x);
      filter2.Add((float)x);
      
      printf("%d: Filter1 mean: %f\n",x,filter1.Mean());
      printf("%d: Filter2 mean: %f std: %f\n",x,filter2.Mean(),filter2.Std());

    }

    return 0;

}


int main()
{
  testMath();
  return 0;
}
