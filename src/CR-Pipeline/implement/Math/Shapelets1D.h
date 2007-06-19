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

/* $Id$ */

#ifndef SHAPELETS1D_H
#define SHAPELETS1D_H

#include <Math/Hermite1D.h>

namespace CR {  // Namespace CR -- begin
  
  /*!
    \brief A class to implement 1-dimensional Shapelet functions
    
    \ingroup Math
    
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
    
    // Order of the Hermite Polynomial
    int order_p;
    
    // Number of normalization factors of the Shapelet function: [order+1]
    int nofCoeff_p;
    
    // Shapelet scale parameter
    double beta_p;
    
    // The normalization factors of the Shapelet functions
    vector<double> coefficients_p;
    
    // Integral over x-axis, [-\infty,+\infty]
    vector<double> integral_p;
    
  public:
    
    //! Empty constructor
    Shapelets1D ();
    
    /*!
      \brief Argumented constructor.
      
      \param order - Maximum order for which the function coefficients are
      pre-computed.
    */
    Shapelets1D (int const &order);
    
    /*!
      \brief Argumented constructor.
      
      \param order - Maximum order for which the function coefficients are
                     pre-computed.
      \param beta  - Shapelet scale parameter, \f$ \beta \f$.
    */
    Shapelets1D (int const &order,
		 double const &beta);
    
    /*!
      \brief Destructor
    */
    virtual ~Shapelets1D ();
    
    // === Shapelet function parameters ================================
    
    /*!
      \brief Set the maximum order of the Shapelets
      
      Setting the maximum order of the Shapelets function will cause 
      automatic adjustment of both the coefficients and the integral 
      value of a Shapelet of given order.
    */
    virtual void setOrder (const int);
    
    /*!
      \brief Get the scale factor, \f$ \beta \f$.
    */
    double beta ();
    
    /*!
      \brief Set the scale factor, \f$ \beta \f$.
    */
    virtual void setBeta (const double);
    
    // === Function evaluation =========================================
    
    /*!
      \brief Evaluate the shapelet function.
      
      \param l - Order of the shapelet function; integer number.
      \param x - Value for which the function is evaluated.
      
      \return y - Value of the shapelet function \f$ B_l (x;\beta) \f$.
    */
    double eval (const int, 
		 const double);
    
    /*!
      \brief Evaluate the shapelet function at a set of points.
      
      \param l - Order of the shapelet function; integer number.
      \param x - Vector of values for which the function is evaluated.
      
      \return y - Vector of values of the shapelet function \f$ B_l (x;\beta) \f$.
    */
    vector<double> eval (const int, 
			 const vector<double>&);
    
    // === Global characteristics ================================================
    
    /*!
      \brief Integral of the basis function.
      
      \param order - Order if the Shapelet basis function for which the integral is
                     evaluated.
    */
    double integral (int const &order);
    
    /*!
      \brief Get the r.m.s. radius of a shapelet function
    */
    double rmsRadius (int const &order);
    
    /*!
      \brief Size of the smallest resolved feature
    */
    double resolution (const int);
    
  private:
    
    //! Set default value for scale parameter
    void setBeta ();
    //! Initialize member data
    void initShapelets1D ();
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

}  // Namespace CR -- end


#endif
