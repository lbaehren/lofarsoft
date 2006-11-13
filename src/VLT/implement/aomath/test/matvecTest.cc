/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: matvecTest.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* macaomgr  28/02/03  created
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

#if defined(HAVE_VLT)
#include "vltPort.h"
#endif

#if defined(HAVE_VXWORKS)
#include <vxWorks.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <string>

static char *rcsId="@(#) $Id: matvecTest.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#include <aomath/vector.h>
#include <aomath/matrix.h>
#include <iostream>

extern "C" void matvecTest()
{
    floatMatrix m(2,4);

    m(0,0)=-1.10204;
    m(0,1)=-0.367347;
    m(0,2)=0.367347;
    m(0,3)=1.10204;
    m(1,0)=0.755102;
    m(1,1)=0.418367;
    m(1,2)=0.0816327;
    m(1,3)=-0.255102;
	std::cout << m << std::endl;

    floatVector v(4);
    v(0)=380;
    v(1)=464;
    v(2)=540;
    v(3)=610;
	std::cout << v << std::endl;

    floatVector b(2);

    b.multiply(m,v);

	std::cout << b << std::endl;
}

int main()
{
  matvecTest();
}
