/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: svdTest.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"
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

static char *rcsId="@(#) $Id: svdTest.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#include <aomath/vector.h>
#include <aomath/matrix.h>
#include <aomath/svd.h>
#include <iostream>
#include <fstream>

extern "C" void svdTest()
{
    floatMatrix m(4,2);
    floatMatrix i(2,4);
    
    m(0,0)=0.05;
    m(1,0)=0.32;
    m(2,0)=0.59;
    m(3,0)=0.87;
    m(0,1)=1;
    m(1,1)=1;
    m(2,1)=1;
    m(3,1)=1;

    i.pinv(m);
    std::cout << i << std::endl;

    i.transpose(m);

    m.pinv(i);
    std::cout << m << std::endl;
}

extern "C" void svdTest2(char *filename,float conditionNumber,int iterations)
{
	std::ifstream matF(filename);

    floatMatrix m(60,60);
    floatMatrix c(60,60);
    floatMatrix U(60,60);
    floatMatrix UT(60,60);
    floatVector S(60);
    floatVector Sim(60);
    floatMatrix V(60,60);

    matF >> m;

    std::cout << m << std::endl;

    std::cout << "Start SVD" << std::endl;
    U=m;

    if (svd(U,S,V,iterations) != 0) {
		std::cerr << "Error computing SVD" << std::endl;
      return;
    }
    std::cout << "SVD OK" << std::endl;
    
    int eigenValueNum = 60;

    // ===================================================
    // filter the unwanted modes 
    // ===================================================
    float maxSVal = 0;

    std::cout << "Singular values:" << std::endl;
    for (int i=0;i<eigenValueNum;i++) {
      if (S(i)>maxSVal) maxSVal = S(i);
      Sim(i)=S(i);
      std::cout << S(i) << " ";
    }
    std::cout << std::endl;

    int filteredModes = 0;
    for (int i=0;i<eigenValueNum;i++) {
      if (maxSVal/S(i)>conditionNumber) {
        S(i)=0.0;
	Sim(i)=0;
	filteredModes++;
      }
    }
    std::cout << "Total filtered modes: " << filteredModes << std::endl;
    
    // ===================================================
    // now compute the control matrix
    // C = V*inv(S)*U'
    // ===================================================

    // ===================================================
    // Invert S
    // ===================================================
    
    for (int i=0;i<eigenValueNum;i++) {
      if (S(i)!=0.0) {
	S(i)=1.0/S(i);
      }
    }
    
    // ===================================================
    // multiply V with a matrix which has S on the diagonal 
    // and 0 elsewhere 
    // ===================================================

    for(unsigned int x=0;x<V.rows();x++) {
      for(unsigned int y=0;y<V.columns();y++) {
	V(x,y)=V(x,y)*S(y);
      }
    }
    
    // ===================================================
    // Transpose U 
    // ===================================================

    UT.transpose(U);
    
    // ===================================================
    // multiply C with U 
    // ===================================================

    c.multiply(V,UT);

	std::ofstream matOut("matout.dat");

    matOut << c;

}


/* (GML) doubleMatrix() constructors are not implemented in matrix.cc */
/*
extern "C" void svdTest3(char *filename,double conditionNumber,int iterations)
{
    ifstream matF(filename);

    doubleMatrix m(60,60);
    doubleMatrix c(60,60);
    doubleMatrix U(60,60);
    doubleMatrix UT(60,60);
    doubleVector S(60);
    doubleVector Sim(60);
    doubleMatrix V(60,60);

    matF >> m;

    std::cout << m << std::endl;

    std::cout << "Start SVD" << std::endl;
    U=m;

    if (svd(U,S,V,iterations) != 0) {
      cerr << "Error computing SVD" << std::endl;
      return;
    }
    std::cout << "SVD OK" << std::endl;
    
    int eigenValueNum = 60;

    // ===================================================
    // filter the unwanted modes 
    // ===================================================
    double maxSVal = 0;

    std::cout << "Singular values:" << std::endl;
    for (int i=0;i<eigenValueNum;i++) {
      if (S(i)>maxSVal) maxSVal = S(i);
      Sim(i)=S(i);
      std::cout << S(i) << " ";
    }
    std::cout << std::endl;

    int filteredModes = 0;
    for (int i=0;i<eigenValueNum;i++) {
      if (maxSVal/S(i)>conditionNumber) {
        S(i)=0.0;
	Sim(i)=0;
	filteredModes++;
      }
    }
    std::cout << "Total filtered modes: " << filteredModes << std::endl;
    
    // ===================================================
    // now compute the control matrix
    // C = V*inv(S)*U'
    // ===================================================

    // ===================================================
    // Invert S
    // ===================================================
    
    for (int i=0;i<eigenValueNum;i++) {
      if (S(i)!=0.0) {
	S(i)=1.0/S(i);
      }
    }
    
    // ===================================================
    // multiply V with a matrix which has S on the diagonal 
    // and 0 elsewhere 
    // ===================================================

    for(unsigned int x=0;x<V.rows();x++) {
      for(unsigned int y=0;y<V.columns();y++) {
	V(x,y)=V(x,y)*S(y);
      }
    }
    
    // ===================================================
    // Transpose U 
    // ===================================================

    UT.transpose(U);
    
    // ===================================================
    // multiply C with U 
    // ===================================================

    c.multiply(V,UT);

    ofstream matOut("matout.dat");

    matOut << c;
}
*/

int main()
{
  char filename[] = "";
  float conditionNumber = 0;
  int iterations = 0;

  svdTest();
  svdTest2(filename, conditionNumber, iterations);
  // (GML) doubleMatrix() constructors are not implemented in matrix.cc */
  // svdTest3(filename, conditionNumber, iterations);
}
