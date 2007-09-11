#include "mat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <casa/BasicMath/Math.h>
#include <casa/aipstype.h>
#include <casa/complex.h>
#include <tables/Tables.h>

#define BUFSIZE 256


int main() {

  MATFile *pmat;
  mxArray *pa;

	double data[4]= {1,2,3,4};

  const char *file = "mattest.mat";
  char str[BUFSIZE];
  int status; 

  pmat = matOpen(file, "w");
  if (pmat == NULL) {
    printf("Error creating file %s\n", file);
    printf("(Do you have write permission in this directory?)\n");
    return(EXIT_FAILURE);
  }
	const int dimensions = 4;
	const int dimensionLengths[dimensions] = {10,10,10,10};

	std::cout << "Create " << dimensions << "d array..." << std::flush;
	//pa = mxCreateDoubleMatrix(18,18,mxCOMPLEX);
	//pa = mxCreateCellArray(dimensions, dimensionLengths);
	pa = mxCreateNumericArray(dimensions, dimensionLengths, mxDOUBLE_CLASS, mxCOMPLEX);
  if (pa == NULL) {
      printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
      printf("Unable to create mxArray.\n");
      return(EXIT_FAILURE);
  }
	printf("Done.\n");

	std::cout << "Filling " << dimensions << "d array, with data..." << std::flush;
	int multiDimensionalIndex[4] = {0,0,0,0};
	double counter=0;

	for(int t=0; t<dimensionLengths[3]; t++){
		for(int f=0; f<dimensionLengths[2]; f++){
			for(int j=0; j<dimensionLengths[1]; j++){
				for(int i=0; i<dimensionLengths[0]; i++){
					multiDimensionalIndex[0]=i;
					multiDimensionalIndex[1]=j;
					multiDimensionalIndex[2]=f;
					multiDimensionalIndex[3]=t;
					int indexInMatlabArray = mxCalcSingleSubscript(pa, dimensions, multiDimensionalIndex);
					std::cout << "3D index(i,j,f,t):" << i << "," << j << "," << f << "," << t <<  " index 1D: " << indexInMatlabArray << std::endl;
					counter++;
					memcpy((void *)(mxGetPr(pa)+indexInMatlabArray), (void *)&counter, sizeof(double));
					memcpy((void *)(mxGetPi(pa)+indexInMatlabArray), (void *)&counter, sizeof(double));
				}
			}
		}
	}

// 	int totalLength = dimensionLengths[0]*dimensionLengths[1]*dimensionLengths[2];
// 
// 	for(int i=0; i< totalLength; i++){
// 		double temp = i;
// 		memcpy((void *) (mxGetPr(pa)+i), (void *)&temp, sizeof(double));
// 	}

// 	int a =0;
// 	for(int c= 0; c< dimensionLengths[2]; c++){
// 		for(int b=0; b< dimensionLengths[1]; b++){
// 					multiDimensionalIndex[0]=a;
// 					multiDimensionalIndex[1]=b;
// 					multiDimensionalIndex[2]=c;
// 
// 					int indexInMatlabArray = mxCalcSingleSubscript(pa, dimensions, multiDimensionalIndex);
// 					std::cout << "3D index(a,b,c):" << a << "," << b << "," << c << " index 1D: " << indexInMatlabArray << std::endl;
// 
// 					memcpy((void *)(mxGetPr(pa)+indexInMatlabArray), (void *)data, sizeof(data));
// 					memcpy((void *)(mxGetPi(pa)+indexInMatlabArray), (void *)data, sizeof(data));
// 		}
// 	}


	printf("Done.\n");

	std::cout << "Saving..." <<std::flush;
  status = matPutVariableAsGlobal(pmat, "testMatrix", pa);
  if (status != 0) {
      printf("Error using matPutVariableAsGlobal\n");
      return(EXIT_FAILURE);
  }

	if (matClose(pmat) != 0) {
    printf("Error closing file %s\n",file);
    return(EXIT_FAILURE);
  }
	printf("Done.\n");

	std::cout << "Freeing memory..." << std::flush;
	mxDestroyArray(pa);
	printf("Done.\n");
  return(EXIT_SUCCESS);
  //return(0);
}
  //pa2 = mxCreateDoubleMatrix(18,18,mxCOMPLEX);

//   /*
//    * Re-open file and verify its contents with matGetVariable
//    */
//
  //memcpy((void *)(mxGetPr(pa2)), (void *)data,  sizeof(data));
  //memcpy((void *)(mxGetPi(pa2)), (void *)iData, sizeof(iData));

//   printf("Reopen...");
// 
//   pmat = matOpen(file, "r");
//   if (pmat == NULL) {
//     printf("Error reopening file %s\n", file);
//     return(EXIT_FAILURE);
//   }
// 	printf("Done.\n");
// 
//   /*
//    * Read in each array we just wrote
//    */
//   printf("Read matrix...");
// 
//   pa2 = matGetVariable(pmat, "GlobalDouble");
//   if (pa2 == NULL) {
//     printf("Error reading existing matrix GlobalDouble\n");
//     return(EXIT_FAILURE);
//   }
//   if (!(mxIsFromGlobalWS(pa2))) {
//     printf("Error saving global matrix: result is not global\n");
//     return(EXIT_FAILURE);
//   }
// 	printf("Done.\n");
// 
//   /* clean up before exit */
// 	printf("Cleaning again...");
// 
//   mxDestroyArray(pa2);
// 
//   if (matClose(pmat) != 0) {
//     printf("Error closing file %s\n",file);
//     return(EXIT_FAILURE);
//   }
//   printf("Done\n");
//   return(EXIT_SUCCESS);
