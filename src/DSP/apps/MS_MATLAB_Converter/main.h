#include <casa/Arrays.h>
#include <casa/aipstype.h>
#include <casa/complex.h>
#include <casa/BasicMath/Math.h>

using namespace std;
class test{
	public:
void test::countZeros(const Cube<complex<float> >& cube) const
{
	uInt nAntenae1 = cube.nrow();
	uInt nAntenae2 = cube.ncolumn();
	uInt nFreq = cube.nplane();


	double counterZero=0;
	double counterNotZero=0;
	
	for(uInt i=0; i<nAntenae1; i++){
		for(uInt j=0; j<nAntenae2; j++){
			for(uInt f=0; f<nFreq; f++)
			{
				complex<float> complexFloat = cube(i,j,1);

				double Re = real(complexFloat);
				double Im = imag(complexFloat);

				if(Re==0)
				{
					counterZero++;
				}
				else
				{
					counterNotZero++;
				}
			}
		}
	}

	cout << "In coutZeros" << endl;
	cout << "Amount of zero numbers: " << counterZero << endl;
	cout << "Amount of numbers: " << counterNotZero << endl;
	cout << "Cube(4,5,34)" << cube(4, 5, 34) << endl;
	cout << endl;
};
}

