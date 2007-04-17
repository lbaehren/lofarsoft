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

/* $Id: ShapeletsModel2D.h,v 1.3 2006/06/26 15:57:56 bahren Exp $ */

#ifndef SHAPELETSMODEL2D_H
#define SHAPELETSMODEL2D_H

#include <Math/Shapelets1D.h>

/*!
  \class ShapeletsModel2D

  \ingroup Math
  \ingroup Shapelets

  \brief Shapelet decomposition of a 2-dimensional function.

  \author Lars B&auml;hren

  \date 2005/04

  \test tShapeletsModel2D.cc

  <h3>Prerequisites</h3>

  <ul>
    <li>A. Refregier, <i>Shapelets - I. A method for image analysis</i>,
    MNRAS, 338 (1), pp. 35-47, 2003

    <li>Hermite1D   -- A class to implement Hermite Polynomials.
    <li>Shapelets1D -- A class to implement 1-dimensional Shapelet functions.
    <li>Shapelets2D -- A class to implement 2-dimensional Shapelet functions.
  </ul>

  <h3>Synopsis</h3>

  Shapelets are a complete, orthonormal set of 2D basis functions constructed
  from Laguerre or Hermite polynomials weighted by a Gaussian. A linear
  combination of these functions can be used to model any image, in a similar
  way to Fourier or wavelet synthesis. The shapelet decomposition is
  particularly efficient for images localised in space, and provide a high
  level of compression for individual galaxies in astronomical data. The basis
  has many elegant mathematical properties that make it convenient for image
  analysis and processing. The formalism was first introduced to astronomy by
  Alexandre Refregier & David Bacon, and a related method has also been
  independently suggested by Gary Bernstein & Mike Jarvis.

  This class implements the expansion of a function \f$ f \in \mathcal{L}^2
  (R^2) \f$ in terms of dimensional Shapelet basis functions,
  \f$ B_{\mathbf n}(\mathbf{x};\beta) \f$,
  \f[
    f(\mathbf{x}) = \sum_{n=0}^{\infty} f_{\mathbf{n}} \,
    B_{\mathbf n} (\mathbf{x}-\mathbf{x}_0;\beta) = \sum_{n_1,n_2=0}^{\infty}
    f_{n_1,n_2} B_{n_1} (x_{1}-x_{1,0};\beta) B_{n_2} (x_{2}-x_{2,0};\beta)
  \f]
  where the parameters of the model are:

  <table cellpadding="3" border="0">
    <tr>
      <td>\f$ \mathbf{n}_{\rm max} = (n_1,n_2)_{\rm max} \f$</td>
      <td>Maximum order of Shapelet functions included in the model.</td>
    </tr>
    <tr>
      <td>\f$ \beta \f$</td>
      <td>Width/scale of the Shapelet function.</td>
    </tr>
    <tr>
      <td>\f$ \mathbf{x}_0 = (x_1,x_2)_0 \f$</td>
      <td>Center position of the decomposed profile.</td>
    </tr>
    <tr>
      <td>\f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$</td>
      <td>Weighting coefficients of the Shapelet components.</td>
    </tr>
  </table>

*/

class ShapeletsModel2D : public Shapelets1D {

  // Center position of the model function
  vector<double> center_p;	

  // Coefficients for the individual shapelet functions
 double* coefficients_p;

  // Integral of the current model
  double integral_p;

 public:
  
  /*!
    \brief Empty constructor
  */
  ShapeletsModel2D ();
  
  /*!
    \brief Argumented constructor.
    
    Interface to Shapelets2D::Shapelets2D; uses default values for \f$ x_0 \f$
    and \f$ f_n \f$.
    
    \param order - Maximum order for which the function coefficients are
                   pre-computed.
    \param beta  - Shapelet scale parameter, \f$ \beta \f$.
   */
  ShapeletsModel2D (const int order,
		    const double beta);

  /*!
    \brief Argumented constructor.
    
    \param center       - Center position of the profile, \f$ \mathbf{x}_0 \f$.
    \param coefficients - Weighting coefficients for the Shapelet components,
                          \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.
   */
  ShapeletsModel2D (const vector<double>& center,
		    const double coefficients[]);

  /*!
    \brief Argumented constructor.
    
    \param order        - Maximum order for which the function coefficients are
                          pre-computed.
    \param beta         - Shapelet scale parameter, \f$ \beta \f$.
    \param center       - Center position of the profile, \f$ \mathbf{x}_0 \f$.
    \param coefficients - Weighting coefficients for the Shapelet components,
                          \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.
   */
  ShapeletsModel2D (const int order,
		    const double beta,
		    const vector<double>& center,
		    const double coefficients[]);
  
  /*!
    \brief Destructor
  */
  ~ShapeletsModel2D ();

  // === Order of the Shaplet components =======================================
  
  /*!
    \brief Set the maximum order of the Shapelet components.
    
    \param order  - Maximum order for which the function coefficients are
    pre-computed.
  */
  void setOrder (const int order);

  // === Scale parameter =======================================================
  
  /*!
    \brief Set a new value to the scale parameter.
    
    \param beta   - Shapelet scale parameter, \f$ \beta \f$.
  */
  void setBeta (const double beta);

  // === Center position of the profile ========================================
  
  /*!
    \brief Get the center, \f$ x_0 \f$, of the model function.
    
    \return center - Center position of the profile, \f$ \mathbf{x}_0 \f$.
  */
  vector<double> center ();
  
  /*!
    \brief Set the center, \f$ \mathbf{x}_0 \f$, of the model function.
    
    \param center - Center position of the profile, \f$ \mathbf{x}_0 \f$.
  */
  void setCenter (const vector<double>& center);

  // === Weighting coefficients of the Shapelet components =====================

  /*!
    \brief Get the model coefficient \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.

    \param n1 - \f$ n_1 \f$ index for \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.
    \param n2 - \f$ n_2 \f$ index for \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.
    
    \return coeff - Coefficient \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.
  */
  double coefficients (const int n1,
		       const int n2);

  /*!
    \brief Set the model coefficients.

    \param coeff - Coefficients \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.

    \todo Finish implementation.
  */
  void setCoefficients (const double coeff[]);

  /*!
    \brief Set the model coefficient \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.

    \param coeff - Coefficient \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.
    \param n     - Index \f$ \mathbf{n} \f$ of \f$ f_{\mathbf{n}} =
           f_{n_1,n_2} \f$.
  */
  void setCoefficients (const double coeff,
			const vector<int>& n);

  /*!
    \brief Set the model coefficient \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.

    \param coeff - Coefficient \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.
    \param n1    - \f$ n_1 \f$ index for \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.
    \param n2    - \f$ n_2 \f$ index for \f$ f_{\mathbf{n}} = f_{n_1,n_2} \f$.
  */
  void setCoefficients (const double coeff,
			const int n1,
			const int n2);

  // === Evaluation of the model function ======================================

  /*!
    \brief Evaluate the Shapelet model at a position \f$ \mathbf{x} \f$.

    \param x - Position \f$ \mathbf{x} = (x_1,x_2) \f$ where to evaluate the 
           model function.
  */
  double eval (const vector<double>& x);

  /*!
    \brief Evaluate the Shapelet model at a position \f$ \mathbf{x} \f$.

    \param x1 - \f$ x_1 \f$ component of \f$ \mathbf{x} \equiv (x_1,x_2) \f$
    \param x2 - \f$ x_2 \f$ component of \f$ \mathbf{x} \equiv (x_1,x_2) \f$
  */
  double eval (const double x1,
	       const double x2);

  // === Globale properties of the model =============================
  
  /*!
    \brief Integral over the model function.
  */
  double integral ();
  
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
