#ifndef MatWriter_H
#define MatWriter_H

#include "mat.h"
#include "MSInfo.h"

#include <casa/Arrays.h>
#include <casa/BasicMath/Math.h>
#include <casa/aipstype.h>
#include <casa/complex.h>

using namespace std;
using namespace casa;

class MatWriter{
	public:
		MatWriter();
		~MatWriter();
		int openFile(const string& fileName);
		int writeCube(const Cube<complex<float> >& cube);
		int closeFile(const string& varName);
		int writeInfoFile(const string& fileName, MSInfo& msInfo);
		
	private:
		MATFile *matlabDocument;
		mxArray *pa;
		int initMatCube();
		void setCubeSize(int dim1, int dim2, int dim3);

		int dimensions;
//		int* dimensionLengths[];
		int dimensionLengths[3];
};

#endif


