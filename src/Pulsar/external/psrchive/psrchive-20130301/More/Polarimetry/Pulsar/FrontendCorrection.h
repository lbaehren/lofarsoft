//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FrontendCorrection.h,v $
   $Revision: 1.3 $
   $Date: 2009/03/01 18:04:41 $
   $Author: straten $ */

#ifndef __Pulsar_FrontendCorrection_H
#define __Pulsar_FrontendCorrection_H

#include "Pulsar/Calibrator.h"
#include "Pulsar/BasisCorrection.h"
#include "Pulsar/ProjectionCorrection.h"

namespace Pulsar
{
  //! Corrects the recorded properties of the frontend

  /*! Frontend corrections include basis corrections and projection
    corrections, as implemented by the BasisCorrection and
    ProjectionCorrection classes */

  class FrontendCorrection : public Calibrator
  {

  public:

    //! Return true if the archive needs to be corrected
    bool required (const Archive*);

    //! Set the archive for which corrections will be calculated
    void set_archive (const Archive*);

    //! Return the transformation from sky to receptor basis
    Jones<double> get_transformation (unsigned isub);

    //! Return the transformation to the receptor basis
    Jones<double> get_basis ();

    //! Return the transformation from sky to receptors
    Jones<double> get_projection (unsigned isub);

    //! Return a summary of parameters relevant to get_transformation
    std::string get_summary () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Calibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calibrate the Pulsar::Archive
    void calibrate (Archive* archive);

    //! Get the number of frequency channels in the calibrator
    unsigned get_nchan () const { return 0; }

    //! Returns a new CalibratorExtension
    CalibratorExtension* new_Extension () const { return 0; }

  protected:

    //! The basis correction
    BasisCorrection basis_correction;

    //! The basis correction transformation applies to all sub-integrations
    Jones<double> basis_transformation;

    //! The projection correction
    ProjectionCorrection projection_correction;

  };

}

#endif
