/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2004-2005                                               *
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

#ifndef SHAPELETS1D_H
#define SHAPELETS1D_H

#include "Hermite1D.h"

/*!
  \class Shapelets1D
  
  \ingroup contrib
  
  \brief A class to implement 1-dimensional Shapelet functions
  
  \author Lars B&auml;hren
  
  \date 2004/08
  
  \test tShapelets1D.cc
  
  <h3>Prerequisites</h3>
  <ul>
  <li>A. Refregier, <i>Shapelets - I. A method for image analysis</i>,
  MNRAS, 338 (1), pp. 35-47, 2003
  <li><a href="http://www.astro.caltech.edu/~rjm/shapelets/">The Shapelets
  Webpage</a>
  
  <li>Hermite1D -- A class to implement Hermite Polynomials.
  </ul>
  
  <h3>Synopsis</h3>
  
  Refregier (2003) defines a set of dimensionless basis functions
  \f[ \phi_{n}(x) = \frac{1}{\sqrt{2^n \sqrt{\pi} n!}} H_n(x) e^{-x^2/2} \f]
  where \f$n\f$ is a non-negative integer and \f$ H_n(x) \f$ is a Hermite 
  polynomial of order \f$ n \f$. These Shapelet basis functions are orthonormal,
  in the sense that for a scalar product in function space \f$ \mathcal{L}^2
  (R) \f$
  \f[ \int_{-\infty}^{\infty} dx \, \phi_n(x) \, \phi_m(x) = \delta_{nm} \f]
  where \f$ \delta_{nm} \f$ is the Krnonecker delta symbol.
  
  In order to describe an (imaged) object in practice from the above equation
  one defines a related set of dimensional basis functions,
  \f[ B_n(x;\beta) = \frac{1}{\sqrt{\beta}} \, \phi(\beta^{-1} x) \f]
  where the parameter \f$ \beta \f$ is a characteristic scale, which -- for
  efficient decomposition -- typically is chosen to be close to the size of 
  the object to be analyzed.
  
  Further properties:
  <ol>
  <li>Integral
  \f[
  \int_{-\infty}^{+\infty} dx \, B_n(x;\beta) \ =	\
  \left\{
  \begin{array}{ll}
  \sqrt{2^{1-n} \sqrt{\pi} \beta} \ \sqrt{\left( n \atop n/2 \right)} 
  & n \hbox{ even} \\[3mm]
  0 & n \hbox{ odd}
  \end{array}
  \right.
  \f]
  <li>r.m.s. radius
  \f[ \theta_{\rm max} = \beta\, \sqrt{n + \frac{1}{2}} \f]
  <li>Size of the smallest structure
  \f[ \theta_{\rm min} = \frac{\beta}{\sqrt{n + \frac{1}{2}}} \f]
  </ol>
*/
class Shapelets1D : public Hermite1D {

  // Shapelet scale parameter
  double beta_p;
  
  // The normalization factors of the Shapelet functions
  std::vector<double> coefficients_p;
  
  // Integral over x-axis, [-\infty,+\infty]
  std::vector<double> integral_p;
  
 public:
  
  // === Construction ===========================================================

  //! Default constructor
  Shapelets1D ();
  
  //! Argumented constructor.
  Shapelets1D (int const &order,
	       double const &beta=1.0);

  //! Argumented constructor.
  Shapelets1D (Hermite1D const &hermite,
	       double const &beta=1.0);

  // === Destruction ============================================================
  
  //! Destructor
  virtual ~Shapelets1D ();
  
  // === Parameter access =======================================================
  
  /*!
    \brief Set the maximum order of the Shapelets
    
    Setting the maximum order of the Shapelets function will cause automatic
    adjustment of both the coefficients and the integral value of a Shapelet
    of given order.
  */
  virtual void setOrder (int const &order);
  
  //! Get the scale factor, \f$ \beta \f$.
  inline double beta () const {
    return beta_p;
  }
  
  //! Set the scale factor, \f$ \beta \f$.
  virtual void setBeta (double const &beta=1.0);
  
  // === Methods ================================================================
  
  //! Provide a summary of the object's internal parameters and status
  void summary (std::ostream &os=std::cout);
  
  //! Evaluate the shapelet function.
  double fx (int const &l,
	     double const &x);
  
  //! Evaluate the shapelet function at a set of points.
  std::vector<double> fx (int const &l,
			  const std::vector<double>&);
  
  /*!
    \brief Integral of the basis function.
    
    \param order - Order if the Shapelet basis function for which the integral is
           evaluated.
  */
  double integral (int const &order);
  
  //! Get the r.m.s. radius of a shapelet function
  double rmsRadius (int const &order);
  
  //! Size of the smallest resolved feature
  double resolution (int const &n);
  
 private:
  
  //! Initialize member data
  void init ();
  //! Set up the internal array with the function coefficients
  void calcCoefficients ();
  //! Cache the integral over x for basis function of order n
  void setIntegral ();
  //! Logarithmic Gamma function (from NR)
  double gammln (double xx);
  //! Factorial of integer <i>n</i>.
  double factln (int n);
  //! Factorial of integer <i>n</i>.
  double factrl (int n);
  //! Binomial coefficient (from NR)
  double bico (int n, int k);
  
};

#endif
