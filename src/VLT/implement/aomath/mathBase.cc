/*******************************************************************************
* E.S.O. - VLT project
*
* 
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  29/11/02  created
*/

/************************************************************************
*   NAME
* 
*   SYNOPSIS
* 
*   DESCRIPTION
*
*   FILES
*
*   ENVIRONMENT
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

static char *rcsId="@(#) $Id: mathBase.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#include <aomath/aomath.h>
#include <string.h>

/* 
 * 
 */

#ifdef HAVE_VXWORKS
float nan = 0.0;
double nanD = 0.0;

extern "C" int isnan(float n)
{
    return memcmp((char*)&n,(char*)&nan,sizeof(float))==0;
}

extern "C" int isnanD(double n)
{
    return memcmp((char*)&n,(char*)&nan,sizeof(double))==0;
}
#endif

extern "C" void initMath()
{
#ifdef HAVE_VXWORKS
#if (CPU_FAMILY == PPC)

    UINT32 fpscr,oldFpscr;

    /*
     * For logging sake get Floating Point Status and Control Register
     */
    fpscr = vxFpscrGet();
    oldFpscr = fpscr;

    /*
     * Disable floating point exceptions.
     * Results that would eventually trigger an exception are defaulted
     * to their corresponding IEEE value (like NaN).
     */

    fpscr &= ~( _PPC_FPSCR_UE | _PPC_FPSCR_XE | _PPC_FPSCR_ZE | _PPC_FPSCR_VE 
		| _PPC_FPSCR_OE);
    vxFpscrSet(fpscr);

#endif /* (CPU_FAMILY == PPC) */

    float zero = 0;
    double zeroD = 0;
    
    nan = zero/zero;
    nanD = zeroD/zeroD;
#endif

}

/*___oOo___*/
