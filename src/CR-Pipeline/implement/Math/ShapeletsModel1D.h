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

/* $Id: ShapeletsModel1D.h,v 1.3 2006/06/26 15:57:56 bahren Exp $ */

#ifndef SHAPELETSMODEL1D_H
#define SHAPELETSMODEL1D_H

// Standard header files
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
using namespace std;

// Custom header files
#include <Math/Hermite1D.h>
#include <Math/Shapelets1D.h>

/*!
  \brief Shapelet decomposition of a 1-dimensional profile.

  \ingroup Math
  \ingroup Shapelets

  \author Lars B&auml;hren

  \date 2005/03

  \test tShapeletsModel1D.cc

  <h3>Prerequisites</h3>

  <ul>
    <li>Hermite1D   -- A class to implement Hermite Polynomials.
    <li>Shapelets1D -- A class to implement 1-dimensional Shapelet functions.
  </ul>

  <h3>Synopsis</h3>

  Expansion of a function \f$ f \in \mathcal{L}^2 (R) \f$ in terms of
  dimensional Shapelet basis functions, \f$ B_n(x;\beta) \f$,
  \f[ f(x) = \sum_{n=0}^{\infty} f_n \, B_n(x-x_0;\beta) \f]
  where the parameters of the model are:

  <table cellpadding="3" border="0">
    <tr>
      <td>\f$ n_{\rm max} \f$</td>
      <td>Maximum order of Shapelet functions included in the model.</td>
    </tr>
    <tr>
      <td>\f$ \beta \f$</td>
      <td>Width/scale of the Shapelet function.</td>
    </tr>
    <tr>
      <td>\f$ x_0 \f$</td>
      <td>Center position of the decomposed profile.</td>
    </tr>
    <tr>
      <td>\f$ f_n \f$</td>
      <td>Weighting coefficients of the Shapelet components.</td>
    </tr>
  </table>

  <h4>Global properties of the Shapelet decomposition</h4>

  <ol>
    <li>The integral of the model function over the x-axis -- corresponding e.g.
    to the the integrated flux -- can be computed using the integral relation 
    for the dimensional basis functions:
    \f[ 
        F = \int^{-\infty}_{+\infty} dx\, \sum_{n=0}^{\infty} f_n\, 
            B_n ((x-x_0);\beta) 
          = \sum_{n=0}^{\infty} f_n\, F_n
    \f]
    where \f$ F_n \f$ is the itegral value of the n-th order dimensional Shapelet
    basis function.
	</ol>
*/

class ShapeletsModel1D : public Shapelets1D {
  
  // Center position of the model function
  double center_p;	

  // Coefficients for the individual shapelet functions
  vector<double> coefficients_p;

  // Integral of the current model
  double integral_p;

 public:
  
  /*!
    \brief Empty constructor
  */
  ShapeletsModel1D ();
  
  /*!
    \brief Argumented constructor.
    
    Interface to Shapelets1D::Shapelets1D; uses default values for \f$ x_0 \f$
    and \f$ f_n \f$.
    
    \param order - Maximum order for which the function coefficients are
                   pre-computed.
    \param beta  - Shapelet scale parameter, \f$ \beta \f$.
   */
  ShapeletsModel1D (const int order,
		    const double beta);
  
  /*!
    \brief Argumented constructor.
    
    This provides custom parameters to Shapelet-based model.
    
    \param center - Center position of the profile, \f$ x_0 \f$.
    \param f      - Decomposition coefficients, \f$ f_n \f$.
  */
  ShapeletsModel1D (const double center,
		    const vector<double>& f);
  
  /*!
    \brief Argumented constructor.
    
    The maximum order of the Shapelets is set using the number of elements in
    coefficients vector.
    
    \param order  - Maximum order for which the function coefficients are
                    pre-computed.
    \param beta   - Shapelet scale parameter, \f$ \beta \f$.
    \param center - Center position of the profile, \f$ x_0 \f$.
    \param f      - Decomposition coefficients, \f$ f_n \f$.
  */
  ShapeletsModel1D (const int order,
		    const double beta,
		    const double center,
		    const vector<double>& f);
  
  //! Destructor
  ~ShapeletsModel1D ();
  
  // === Access to the model parameters ========================================
  
  /*!
    \brief Set the maximum order of the Shapelet components.
    
    \param order  - Maximum order for which the function coefficients are
           pre-computed.
  */
  void setOrder (const int order);
  
  /*!
    \brief Set a new value to the scale parameter.
    
    \param beta   - Shapelet scale parameter, \f$ \beta \f$.
  */
  void setBeta (const double beta);
  
  /*!
    \brief Get the center, \f$ x_0 \f$, of the model function.
    
    \return center - Center position of the profile, \f$ x_0 \f$.
  */
  double center ();
  
  /*!
    \brief Set the center, \f$ x_0 \f$, of the model function.
    
    \param center - Center position of the profile, \f$ x_0 \f$.
  */
  void setCenter (const double& center);
  
  /*!
    \brief Get the coefficients for the current model.

    \param coeff - Decomposition coefficients, \f$ f_n \f$.
  */
  vector<double> coefficients ();
  
  /*!
    \brief Set the coefficients of the model.
    
    \param coeff - Decomposition coefficients, \f$ f_n \f$.
  */
  void setCoefficients (const vector<double>& coeff);
  
  // === Globale properties of the model =============================
  
  /*!
    \brief Integral over the model function.
  */
  double integral ();
  
  // === Evaluation of the model function ============================
  
  /*!
    \brief Evaluate the Shapelet model at a position \f$ x \f$.
  */
  double eval (const double x);
  
  /*!
    \brief Evaluate the Shapelet model at a set of positions.
  */
  vector<double> eval (const vector<double>& x);
  
 private:

  // === Private function for internal usage =========================
  
  //! Initialize the internal model values
  void initModel ();
  
  /*!
    \brief Update interna after update of model parameters.
   */
  void updateGlobals ();

  //! Compute the integral [-\infty,\infty] of the current model
  void calcIntegral ();	
  
};

#endif
