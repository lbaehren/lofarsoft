#ifndef AOMATH_H
#define AOMATH_H
/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: aomath.h,v 1.2 2004/05/10 15:41:32 loose Exp $"
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

#ifdef HAVE_VXWORKS
# include <vxWorks.h>
# if (CPU_FAMILY == PPC)
#  include <arch/ppc/vxPpcLib.h>
#  include <arch/ppc/archPpc.h>
# endif /* (CPU_FAMILY == PPC) */
# include <iostream.h>
#else
# include <iostream>
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <aomath/matrix.h>
#include <aomath/vector.h>
#include <aomath/svd.h>

#ifdef HAVE_VXWORKS
extern float nan;
extern double nanD;
extern "C" int isnan(float n);
extern "C" int isnanD(double n);
#endif

extern "C" void initMath();

double interpolate(double x0,double x1,double y0,double y1,double x);

#endif /*!AOMATH_H*/
