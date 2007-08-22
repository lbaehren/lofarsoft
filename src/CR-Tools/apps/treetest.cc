/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

/*!
  \file treetest.cc

  \ingroup Applications

  \brief A test program for setting up a processing tree

  \date 2006

  \author Andreas Horneffer
*/

#include <iostream>
#include <sstream>

#include <LopesLegacy/StoredInputObject.h>
#include <LopesLegacy/StoredMultiplyObject.h>

using namespace std;

using CR::StoredInputObject;
using CR::StoredMultiplyObject;

StoredInputObject<Double> *inp1,*inp2;
StoredInputObject<DComplex> *inp3;
StoredMultiplyObject<Double> *mult1, *mult2;
StoredMultiplyObject<DComplex> *mult3, *mult4;

/*!
  \brief Generate a new processing tree

  \return status -- Status of the operation; returns <i>False</i> in case an
                    error was encountered.
*/
int genTree()
{
  int nofFailedTests (0);

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
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  };
  
  return nofFailedTests;
};

// ------------------------------------------------------------------------------

int main (int argc, const char** argv)
{
  int nofFailedTests (0);
  bool status (true);

  nofFailedTests += genTree();
  
  if (nofFailedTests == 0) {
    Vector<Double> vec(10);
    int i;
    for (i=0;i<10;i++){ vec(i) = i+1 ;};
    status = inp1->put(&vec);
    cout << "data into 1: " << status << endl;
    for (i=0;i<10;i++){ vec(i) = 10. ;};
    status = inp2->put(&vec);
    cout << "data into 2: " << status << endl;
    
    Vector<DComplex> Dvec(10);
    for (i=0;i<10;i++){ Dvec(i) = 10. ;};
    status = inp3->put(&Dvec);
    cout << "data into 3: " << status << endl;
    
    
    status = mult1->get(&vec);
    cout << "mult1: " << status << "," << vec << endl;
    status = mult2->get(&vec);
    cout << "mult2: " << status << "," << vec << endl;
    
    
    status = mult3->get(&Dvec);
    cout << "mult3: " << status << "," << Dvec << endl;
    status = inp3->get(&Dvec);
    cout << "input3: " << status << "," << Dvec << endl;
    
    status = mult4->get(&Dvec);
    cout << "mult4: " << status << "," << Dvec << endl;
    status = inp3->get(&Dvec);
    cout << "input3: " << status << "," << Dvec << endl;
    
    
    for (i=0;i<10;i++){ vec(i) = i+1. ;};
    status = inp1->put(&vec);
    status = mult4->get(&Dvec);
    cout << "mult4: " << status << "," << Dvec << endl;
   }
  
  return 0;
}
