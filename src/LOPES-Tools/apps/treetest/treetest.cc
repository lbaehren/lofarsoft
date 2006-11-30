#include <iostream>
#include <sstream>

#include <lopes/LopesBase/StoredInputObject.h>
#include <lopes/LopesBase/StoredMultiplyObject.h>


using namespace std;

StoredInputObject<Double> *inp1,*inp2;
StoredInputObject<DComplex> *inp3;
StoredMultiplyObject<Double> *mult1, *mult2;
StoredMultiplyObject<DComplex> *mult3, *mult4;

Bool genTree(){

    inp1 = new StoredInputObject<Double>;
    inp2 = new StoredInputObject<Double>;
    inp3 = new StoredInputObject<DComplex>;

    mult1 = new StoredMultiplyObject<Double>(2);
    mult2 = new StoredMultiplyObject<Double>(2);

    mult3 = new StoredMultiplyObject<DComplex>(3);
    mult4 = new StoredMultiplyObject<DComplex>(3);

    mult1->setParent(0,inp1);
    mult1->setParent(1,inp2);

    mult2->setParent(0,inp2);
    mult2->setParent(1,inp1);

    mult3->setParent(0,inp2);
    mult3->setParent(1,inp3);
    mult3->setParent(2,inp1);

    mult4->setParent(0,mult1);
    mult4->setParent(1,mult2);
    mult4->setParent(2,mult3);

    return True;
};

int main (int argc, const char** argv) {

    cout << "treetest started" << endl;

    try {
	Bool erg;
	erg = genTree();
	cout << "tree generated:" << erg << endl;

	Vector<Double> vec(10);
	Int i;
	for (i=0;i<10;i++){ vec(i) = i+1 ;};
	erg = inp1->put(&vec);
	cout << "data into 1: " << erg << endl;
	for (i=0;i<10;i++){ vec(i) = 10. ;};
	erg = inp2->put(&vec);
	cout << "data into 2: " << erg << endl;

	Vector<DComplex> Dvec(10);
	for (i=0;i<10;i++){ Dvec(i) = 10. ;};
	erg = inp3->put(&Dvec);
	cout << "data into 3: " << erg << endl;


	erg = mult1->get(&vec);
	cout << "mult1: " << erg << "," << vec << endl;
	erg = mult2->get(&vec);
	cout << "mult2: " << erg << "," << vec << endl;


	erg = mult3->get(&Dvec);
	cout << "mult3: " << erg << "," << Dvec << endl;
	erg = inp3->get(&Dvec);
	cout << "input3: " << erg << "," << Dvec << endl;

	erg = mult4->get(&Dvec);
	cout << "mult4: " << erg << "," << Dvec << endl;
	erg = inp3->get(&Dvec);
	cout << "input3: " << erg << "," << Dvec << endl;


	for (i=0;i<10;i++){ vec(i) = i+1. ;};
	erg = inp1->put(&vec);
	erg = mult4->get(&Dvec);
	cout << "mult4: " << erg << "," << Dvec << endl;




 
    } catch (AipsError x) {
	cerr << x.getMesg() << endl;
	return 1;
    };

    return 0;
}
