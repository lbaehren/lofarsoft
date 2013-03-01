//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionModelSolveGSL.h,v $
   $Revision: 1.1 $
   $Date: 2008/06/03 04:57:18 $
   $Author: straten $ */

#ifndef __ReceptionModel_SolveGSL_H
#define __ReceptionModel_SolveGSL_H

#include "Pulsar/ReceptionModelSolver.h"

namespace Calibration
{
  //! Solve the measurement equation using GNU Scientific Library
  class SolveGSL : public ReceptionModel::Solver
  {

    //! Return the name of this solver
    std::string get_name () const;

    //! Return a new, copy-constructed clone
    SolveGSL* clone () const;

  protected:

    //! Solve the measurement equation using GSL
    void fit ();

  };

}

#endif

