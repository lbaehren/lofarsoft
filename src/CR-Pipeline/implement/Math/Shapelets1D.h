/***************************************************************************
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

/* $Id: Shapelets1D.h,v 1.3 2005/08/03 07:31:45 bahren Exp $ */

#ifndef SHAPELETS1D_H
#define SHAPELETS1D_H

#include <complex>
#include <blitz/array.h>

#include <Math/Hermite1D.h>

using blitz::Array;

namespace CR {  // Namespace CR -- BEGIN
  
  /*!
    \class Shapelets1D
    
    \ingroup Math
    
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
    
    <img src="../docs/figures/Shapelets1D.png">
    
    Further properties:
    <ol>
      <li>Derivatives
      \f[
      \partial_x \phi_n(x) = \sqrt{2n} \phi_{n-1}(x) - x \phi_{n}(x)
      \f]
      \f[
      \partial_{x^2} \phi_n(x) = 2 \sqrt{n (n-1)} \phi_{n-2}(x) - \sqrt{8n}
      x \phi_{n-1}(x) + (x^2 - 1) \phi_{n}(x)
      \f]
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
    Array<double,1> coefficients_p;
    
    // Integral over x-axis, [-\infty,+\infty]
    Array<double,1> integral_p;
    
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
    
    // === Shapelet function parameters =========================================
    
    /*!
      \brief Set the maximum order of the Shapelets
      
      Setting the maximum order of the Shapelets function will cause 
      automatic adjustment of both the coefficients and the integral 
      value of a Shapelet of given order.
    */
    virtual void setOrder (int const &order);
    
    /*!
      \brief Get the scale factor, \f$ \beta \f$.

      \return beta -- The scale factor, \f$ \beta \f$.
    */
    inline double beta () const {
      return beta_p;
    }
    
    /*!
      \brief Set the scale factor, \f$ \beta \f$.

      \param beta -- The scale factor, \f$ \beta \f$.
    */
    virtual void setBeta (const double& beta);
    
    // === Function evaluation ==================================================
    
    /*!
      \brief Evaluate the shapelet function.
      
      This function implements 
      \f[ B_n(x;\beta) = \frac{1}{\sqrt{\beta}} \, \phi(\beta^{-1} x) \f]
      
      \param l - Order of the shapelet function; integer number.
      \param x - Value for which the function is evaluated.
      
      \return y - Value of the shapelet function \f$ B_l (x;\beta) \f$.
    */
    double fx (int const &l, 
	       double const &x);
    
    /*!
      \brief Evaluate the shapelet function at a set of points.
      
      \param l - Order of the shapelet function; integer number.
      \param x - Vector of values for which the function is evaluated.
      
      \return y - Vector of values of the shapelet function \f$ B_l (x;\beta) \f$.
    */
    Array<double,1> fx (int const &l, 
			Array<double,1> const &x);
    
    /*!
      \brief Evaluate the shapelet function at a set of points.
      
      \param l - Order of the shapelet function; integer number.
      \param x - Vector of values for which the function is evaluated.
      
      \return y - Vector of values of the shapelet function \f$ B_l (x;\beta) \f$.
    */
    Array<double,1> fx (Array<int,1> const &l, 
			Array<double,1> const &x);
    
    /*!
      \brief First order derivative w.r.t. \f$ x \f$.
      
      This function implements
      \f[ \frac{d}{dx} B_n(x;\beta) = \frac{1}{\sqrt{\beta}} \frac{d}{dx}
      \phi_{n}(\beta^{-1}x) = \frac{\sqrt{2n}}{\beta} B_{n-1}(x;\beta) -
      \frac{2x}{\beta^{2}} B_{n}(x;\beta) \f]
      
      
      \param l - Order of the shapelet function; integer number.
      \param x - Vector of values for which the function is evaluated.
    */
    double dfdx (int const &l, 
		 double const &x);
    
    /*!
      \brief First order derivative w.r.t. \f$ x \f$.
      
      \todo not yet implemented
      
      \param l - Order of the shapelet function; integer number.
      \param x - Vector of values for which the function is evaluated.
    */
    Array<double,1> dfdx (int const &l, 
			  Array<double,1> const &x);
    
    /*!
      \brief First order derivative w.r.t. \f$ x \f$.
      
      \todo not yet implemented
      
      \param l - Order of the shapelet function; integer number.
      \param x - Vector of values for which the function is evaluated.
    */
    Array<double,1> dfdx (Array<int,1> const &l, 
			  Array<double,1> const &x);
    
    /*!
      \brief Second order derivative w.r.t. \f$ x \f$.
      
      \todo Still needs to be implemeted.
      
      \param l - Order of the shapelet function; integer number.
      \param x - Vector of values for which the function is evaluated.
    */
    double d2dx2 (int const &l, 
		  double const &x);
    
    /*!
      \brief Evaluate the Fourier transform of the shapelet function.
      
      This function implements
      \f[ \{ \mathcal{F} B_{n} \} (k;\beta) \equiv \widetilde{B_{n}}(k;\beta)
      = i^{n} B_{n}(k;\beta^{-1}) = i^{n} \sqrt{\beta}\, \phi(\beta x) \f]
      
      \param l - Order of the shapelet function; integer number.
      \param x - Value for which the function is evaluated.
      
      \return y - Value of the shapelet function \f$ \widetilde{B_{n}}(k;\beta) \f$.
    */
    std::complex<double> fft (int const &l, 
			      double const &x);
    
    // === Global characteristics ===============================================
    
    /*!
      \brief Integral of the basis function.
      
      \param l - Order if the Shapelet basis function for which the integral is
                 evaluated.
    */
    virtual double integral (int const &l);
    
    /*!
      \brief r.m.s. radius
    */
    double rmsRadius (const int);
    
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
    /*!
      \brief Evaluation of the dimensionless basis function
      
      This function implements
      \f[ \phi_{n}(x) = \frac{1}{\sqrt{2^n \sqrt{\pi} n!}} H_n(x) e^{\xi}
      \quad {\rm where} \qquad \xi = -x^2/2 \f]
    */
    double eval (int const &l, 
		 double const &x);
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
  
}  // Namespace CR -- END

#endif
