/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: aomath.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  16/01/02  created 
*/

/************************************************************************
*   NAME
*   
* 
*   SYNOPSIS
*
*   
*   PARENT CLASS
*
* 
*   DESCRIPTION
*
*
*   PUBLIC METHODS
*
*
*   PUBLIC DATA MEMBERS
*
*
*   PROTECTED METHODS
*
*
*   PROTECTED DATA MEMBERS
*
*
*   PRIVATE METHODS
*
*
*   PRIVATE DATA MEMBERS
*
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

static char *rcsId="@(#) $Id: aomath.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#include <aomath/memory.h>

// Memory pools

memoryPool VectorMemoryPool(80000);
memoryPool MatrixMemoryPool(800000);

extern "C" void showMemoryPool()
{
    VectorMemoryPool.show();
    MatrixMemoryPool.show();
}

double interpolate(double x0,double x1,double y0,double y1,double x)
{
    if (x0>x1) {
       double tmp;

       tmp=x0;
       x0=x1;
       x1=tmp;
       tmp=y0;
       y0=y1;
       y1=tmp;
    }

    if (x<x0) { printf("too low: %f < %f ==> %f\n",x,x0,y0); return y0; }
    else if (x>x1) { printf("Too high: %f > %f ==> %f\n",x,x1,y1); return y1; }
    else if (x0==x1) { printf("Identical: %f == %f ==> %f\n",x0,x1,y0); return y0; }
    else {
       printf("%f %f , %f %f ==> %f\n",x0,x1,y0,y1,(y1-y0)/(x1-x0)*(x-x0)+y0);
       return (y1-y0)/(x1-x0)*(x-x0)+y0;
    }

}

/*___oOo___*/
