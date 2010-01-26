//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Depolarizer.h,v $
   $Revision: 1.6 $
   $Date: 2009/03/16 12:57:36 $
   $Author: straten $ */

#ifndef __MEAL_Depolarizer_H
#define __MEAL_Depolarizer_H

#include "MEAL/Real4.h"

namespace MEAL
{

  //! Represents a pure depolarizer transformation
  /*! The pure depolarizer is parameterized as described in

  Lu, S.H. & Chipman, R.A., 1996, J. Opt. Soc. Am. A, 13, 1106

  \f$ {\bf M}_\Delta = {\bf I} + \left( \begin{array}{cc}
                                        0 & \mbf{0}^T \\
                                        \mbf{p}_\Delta & {\bf m}_\Delta
                                        \end{array}\right) \f$

  where \f$ {\bf I} \f$ is the \f$ 4\times4 \f$ identity matrix,
  \f$ \mbf{p}_\Delta \f$ is the 3-vector that describes the polarizance, 
  and \f$ {\bf m}_\Delta \f$ is the \f$ 3\times3 \f$ symmetric 
  depolarizer matrix.
  */

  class Depolarizer : public Real4
  {
  public:

    Depolarizer ();

    Depolarizer* clone () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Calculate the Mueller matrix and its gradient
    void calculate (Matrix<4,4,double>& result,
		    std::vector<Matrix<4,4,double> >*);

  };

}

#endif
