/***************************************************************************
 *   Copyright (C) 2004,2005                                               *
 *   Andreas Horneffer                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id: treetest.cc,v 1.3 2007/05/02 08:14:14 bahren Exp $ */

/*!
  \file treetest.cc

  \ingroup Applications

  \brief A test program for setting up a processing tree

  \date 2006

  \author Andreas Horneffer
*/

#include <iostream>
#include <sstream>

#include <lopes/LopesBase/StoredInputObject.h>
#include <lopes/LopesBase/StoredMultiplyObject.h>

using namespace std;

StoredInputObject<Double> *inp1,*inp2;
StoredInputObject<DComplex> *inp3;
StoredMultiplyObject<Double> *mult1, *mult2;
StoredMultiplyObject<DComplex> *mult3, *mult4;

/*!
  \brief Generate a new processing tree

  \return status -- Status of the operation; returns <i>False</i> in case an
                    error was encountered.
*/
Bool genTree(){

  try {
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
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    return False;
  };
  
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
