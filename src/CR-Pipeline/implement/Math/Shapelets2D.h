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

/* $Id: Shapelets2D.h,v 1.4 2006/06/26 15:57:56 bahren Exp $ */

#ifndef SHAPELETS2D_H
#define SHAPELETS2D_H

#include <Math/Shapelets1D.h>

/*!
  \brief A class to implement 2-dimensional Shapelet functions.

  \ingroup Math
  \ingroup Shapelets

  \author Lars B&auml;hren

  \date 2004/08

  \test tShapelets2D.cc

  <h3>Prerequisites</h3>
  <ul>
    <li>A. Refregier, <i>Shapelets - I. A method for image analysis</i>,
    MNRAS, 338 (1), pp. 35-47, 2003

    <li>Hermite1D   -- A class to implement Hermite Polynomials.
    <li>Shapelets1D -- A class to implement 1-dimensional Shapelet functions.
  </ul>
  
  <h3>Synopsis</h3>

  <table border="0">
    <tr valign="top">
      <td>
      Using the 1-dimensional Shapelet basis functions, we can construct 
      basis functions that allow to describe 2-dimensional objects; this is done by
      simply taking the tensor product of two 1-dimensional basis functions,
      \f[ \phi_{\mathbf n}(\mathbf x) = \phi_{n_1}(x_1) \, \phi_{n_2}(x_2) \f]
      where \f$ \mathbf x = (x_1,x_2) \f$ and \f$ \mathbf n = (n_1,n_2) \f$. With
      this again we also can define dimensional basis functions as
      \f[ B_{\mathbf n}(\mathbf x;\beta) = \frac{1}{\beta}
      \phi_{\mathbf n}(\beta^{-1} \mathbf x) \f]
      </td>
      <td>
      <img src="figures/Shapelets2D.png">
      </td>
    </tr>
  </table>

*/

class Shapelets2D : public Shapelets1D {

 public:

  //! Empty constructor
  Shapelets2D ();

  /*!
    \brief Argumented constructor.

    \param order - Maximum order for which the function coefficients are
                   pre-computed.
   */
  Shapelets2D (const int);

  /*!
    \brief Argumented constructor.

    \param order - Maximum order for which the function coefficients are
                   pre-computed.
    \param beta  - Shapelet scale parameter, \f$ \beta \f$.
   */
  Shapelets2D (const int,
	       const double);

  //! Destructor
  ~Shapelets2D ();

  // === Function evaluation =========================================

  /*!
    \brief Evaluate a two-dimensional Shapelet function.

    Evaluates \f$ B_{\mathbf n}(\mathbf x; \beta) = B_{n_1}(x_1; \beta)
    \, B_{n_2}(x_2; \beta) \f$ via Shapelets1D::eval
   */
  double eval (const int,
	       const double,
	       const int,
	       const double);

  /*!
    \brief Evaluate a two-dimensional Shapelet function.

    Alternate interface accepting vector format input.
   */
  double eval (const vector<int>&,
	       const vector<double>&);

  // === Global function properties ==================================

  /*!
    \brief Integral of the 2D basis functions
    \todo Still needs to be implemented.
   */
  double integral (const int,
		   const int);

  /*!
    \todo Still needs to be implemented.
   */
  double integral (const vector<int>&);

};

#endif
