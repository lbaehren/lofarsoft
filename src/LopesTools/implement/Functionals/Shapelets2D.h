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

/* $Id: Shapelets2D.h,v 1.2 2005/07/15 10:06:05 bahren Exp $ */

#ifndef SHAPELETS2D_H
#define SHAPELETS2D_H

#include <Functionals/Shapelets1D.h>

/*!
  \class Shapelets2D

  \ingroup Shapelets

  \brief A class to implement 2-dimensional Shapelet functions.

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
      <img src="../docs/figures/Shapelets2D.png">
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
  Shapelets2D (int const &order);

  /*!
    \brief Argumented constructor.

    \param order - Maximum order for which the function coefficients are
                   pre-computed.
    \param beta  - Shapelet scale parameter, \f$ \beta \f$.
   */
  Shapelets2D (int const &order,
	       double const &beta);

  //! Destructor
  ~Shapelets2D ();

  // === Function evaluation =========================================

  /*!
    \brief Evaluate a two-dimensional Shapelet function.

    \param l --
    \param x --
    \param m --
    \param y --

    Evaluates \f$ B_{\mathbf n}(\mathbf x; \beta) = B_{n_1}(x_1; \beta)
    \, B_{n_2}(x_2; \beta) \f$ via Shapelets1D::eval
   */
  double fx (int const &l,
	     double const &x,
	     int const &m,
	     double const &y);
  
  // === Global function properties ==================================

  /*!
    \brief Integral of the 2D basis functions

    \param l --
    \param m --
  */
  double integral (int const &l,
		   int const &m);
  
  /*!
    \brief Integral of the 2D basis functions

    \param l --
   */
  double integral (vector<int> const &l);

};

#endif
