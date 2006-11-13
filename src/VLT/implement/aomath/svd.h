#ifndef SVD_H
#define SVD_H
/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: svd.h,v 1.2 2004/05/10 15:41:32 loose Exp $"
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

#include <aomath/aomath.h>

class floatMatrix;
class floatVector;
class doubleMatrix;
class doubleVector;

#define MAX_ITERATIONS 30 

int svd(floatMatrix &U, 
	floatVector &S,       
	floatMatrix &V,
	int maxIterations = MAX_ITERATIONS);

void svdSort(floatMatrix &U, 
	     floatVector &S,       
	     floatMatrix &V);

int svd(doubleMatrix &U, 
	doubleVector &S,       
	doubleMatrix &V,
	int maxIterations = MAX_ITERATIONS);

void svdSort(doubleMatrix &U, 
	     doubleVector &S,       
	     doubleMatrix &V);

#endif /*!SVD_H*/
