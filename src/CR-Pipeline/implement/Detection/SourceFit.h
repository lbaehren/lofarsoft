/***************************************************************************
 *   Copyright (C) 2005 by Sven Lafebre                                    *
 *   s.lafebre@astro.ru.nl                                                 *
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

#ifndef _SOURCEFIT_H_
#define _SOURCEFIT_H_

#include <sstream>
#include <fstream>
#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>

#include <Detection/PeakList.h>

using casa::Matrix;
using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class SourceFit
    
    \ingroup Analysis
    
    \brief Class for finding the position of a source
    
    \author Sven Lafebre
  */
  class SourceFit {
    //! Array of fit parameters
    Vector<double> fitParam;
    Vector<bool>   doFitParam;
    int            noFitParam;
    
    Matrix<double> covar;
    Matrix<double> alpha;
    double         chisq;
    double         alamda;
    double         chiSqr;
    double         chiSqrOld;
    
    Matrix<double> r;
    Vector<double> r0;
    Vector<double> F;
    Matrix<double> oneda;
    Vector<double> r0Try;
    Vector<double> beta;
    Vector<double> da;
    
  /* These functions were taken from:
     Numerical recipes in C++: the art of scientific computing
     William H. Press
     2002, Cambridge University Press, Cambridge
  */
  //! mrqmin
  void marquardtMinimize(PeakList*);
  //! mrqcof
  void marquardtMatrix(PeakList*,
		       Vector<double>&,
		       Matrix<double>&,
		       Vector<double>&);
  //! covsrt
  void expandCovariance(Matrix<double>&);
  //! gaussj
  void gaussJordan(Matrix<double>&,
		   Matrix<double>&);
  
  void init();
  void fitFunction(int,
		   Vector<double>,
		   double&,
		   Vector<double>&);	// funcs
  
 public:
  void minimize(PeakList*);
};

} // Namespace CR -- end

#endif /* _SOURCEFIT_H_ */
