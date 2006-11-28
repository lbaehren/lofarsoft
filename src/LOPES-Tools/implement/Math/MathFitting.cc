/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id: Statistics.h,v 1.5 2006/10/31 18:24:08 bahren Exp $*/

#include <Math/MathFitting.h>

namespace LOPES { // Namespace LOPES -- begin
  
  void LinearRegression (Vector<double>& x,
			 Vector<double>& y,
			 double& a,
			 double& b,
			 double& chisq)
  {
    int nelem (0);
    double xMean (0.0); 
    double yMean (0.0); 
    double bxy (0.0); 
    double bx (0.0); 
    
    // Check if the shape of the input vectors agree
    x.shape(nelem);
    if (nelem != y.shape()){
      cerr << "[LinearRegression] ";
      cerr << "Shape of the input vectors do not agree!" << endl;
    }
    
    xMean = mean(x);
    yMean = mean(y);
    
    b = bx = bxy = 0.0;
    for (int i=0; i<nelem; ++i ) {
      bx += (x(i)-xMean)*(x(i)-xMean);
      bxy += (x(i)-xMean)*(y(i)-yMean);
    }
    b = bxy/bx;
    a = yMean-b*xMean;
    
    // Evaluate the fitted linear regression on the values provided via 'x'
    for (int i=0; i<nelem; ++i ) {
      x(i) = a+b*x(i);
    }
    
    // Compute chisq for the fit
    chisq = 0.0;
    for (int i=0; i<nelem; ++i ) {
      chisq += (y(i)-x(i))*(y(i)-x(i));
    }
    
  }
  
} // Namespace LOPES -- end
