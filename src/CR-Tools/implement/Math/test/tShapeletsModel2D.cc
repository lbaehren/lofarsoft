/*-------------------------------------------------------------------------*
 | $Id                                                                     |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#include <cmath>

#include <Math/ShapeletsModel2D.h>

using CR::ShapeletsModel2D;

/*!
  \file tShapeletsModel2D.cc

  \ingroup Math
  
  \brief A collection of tests for ShapeletsModel2D.
  
  \author Lars B&auml;hren

  \date 2005/04
*/

// === Display of the model parameters =========================================

/*!
  \brief Show current values of the 2-dim Shapelet model.
*/
void printModel (ShapeletsModel2D& model)
{
  vector<double> center (2);
  int order (0);
  int nx (0);
  int ny (0);

  order = model.order();
  center = model.center();

  cout << endl;

  cout << " - Maximum shapelet order     : " << order << endl;
  cout << " - Shapelet scale parameter   : " << model.beta() << endl;
  cout << " - Center position of profile : " << center[0] << " , "
       << center[1] << endl;
  
  cout << " - Weighting coefficients ... : " << endl;
  for (nx=0; nx<order; nx++) {
    cout << "\t";
    for (ny=0; ny<order; ny++) {
      cout << " " << model.coefficients(nx,ny);
    }
    cout << endl;
  }

//   cout << " - Integration over x-axis    : " << model.integral() << endl;

  cout << endl;
}

// === Misc. tests =============================================================

/*!
  \brief Test handling of 2-dimensional arrays.

  \param a  - 
  \param n1 - 
  \param n2 - 
*/
void test_arrays (float a[], int n1, int n2) {

  for (int j=0; j<n1; j++) {
    for (int i=0; i<n2; i++) {
      cout << i << "\t" << j << "\t" << a[i*n1+j] << endl;
    }
  }

}


// === Test constructors =======================================================

/*!
  \brief Test constructors of ShapeletsModel2D.

  \param order  - Maximum order for which the function coefficients are
                  pre-computed.
  \param beta   - Shapelet scale parameter, \f$ \beta \f$.
  \param center - Center position of the profile, \f$ \mathbf{x}_{0} \f$.

  \return ok - Status.
*/
bool test_ShapeletsModel2D (int order, 
			    double beta,
			    vector<double>& center)
{
  cout << "\n[ShapeletsModel2D] Testing ShapeletsModel2D constructors\n"
       << endl;
  
  // Test 1 : 
  cout << " [1] Empty constructor" << endl;
  {
    ShapeletsModel2D model;
    //
    printModel (model);
  }

  // Test 2 : 
  cout << " [2] Construction providing Shapelet parameters" << endl;
  {
    ShapeletsModel2D model (order,beta);
    //
    printModel (model);
  }

  return true;
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main () {

  bool ok (true);
  int order (15);
  double beta (2.0);
  vector<double> center (2);

  center[0] = 1.0;
  center[1] = 5.0;

//   {
//     int n1 (5);
//     int n2 (5);
//     float *a;
    
//     a = new float[n1*n2];
    
//     test_arrays (a,n1,n2);
//   }

  // Test constructors of ShapeletsModel2D
  {
    ok = test_ShapeletsModel2D (order, beta, center);
  }

  return 0;

}
