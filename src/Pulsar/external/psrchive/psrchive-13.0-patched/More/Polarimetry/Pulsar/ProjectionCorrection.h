//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ProjectionCorrection.h,v $
   $Revision: 1.2 $
   $Date: 2008/05/28 08:32:34 $
   $Author: straten $ */

#ifndef __Pulsar_ProjectionCorrection_H
#define __Pulsar_ProjectionCorrection_H

#include "Pulsar/Parallactic.h"
#include "Reference.h"
#include "Jones.h"

namespace Pulsar
{
  class Archive;
  class Receiver;
  class Telescope;
  class Integration;
  class Pointing;

  //! Correct the backend convention
  class ProjectionCorrection : public Reference::Able
  {
  public:

    //! Trust Pointing parallactic angle more than computed value
    static bool pointing_over_computed;

    //! Default constructor
    ProjectionCorrection ();
    //! Destructor
    ~ProjectionCorrection ();

    //! Set the archive for which projection corrections will be computed
    void set_archive (const Archive*);

    //! Return the projection correction for the given sub-integration
    Jones<double> operator () (unsigned isub) const;

    //! Return true if the given sub-integration requires correction
    bool required (unsigned isub) const;

    //! Return a summary of parameters relevant to get_rotation
    std::string get_summary () const;

  protected:

    //! Return the basis rotation correction
    Jones<double> get_rotation () const;

    /** @name Correction Information
     *  These attributes are set during the call to required and
     *  are used by the correct method
     */
    //@{

    //! The archive for which projection corrections will be computed
    Reference::To<const Archive, false> archive;

    //! The telescope extension from the archive
    Reference::To<const Telescope, false> telescope;
    
    //! The receiver extension from the archive
    Reference::To<const Receiver, false> receiver;

    //! The sub-integration for which projection corrections will be computed
    mutable Reference::To<const Integration, false> integration;

    //! The pointing extension from the integration
    mutable Reference::To<const Pointing, false> pointing;

    //! The parallactic (vertical) angle rotation
    mutable Calibration::Parallactic para;
    
    //! Flag set when the receiver vertical angle should be corrected
    mutable bool should_correct_vertical;

    //! Flag set when the receiver projection should be corrected
    mutable bool should_correct_projection;

    //! Flag set when either of the above corrections have not been performed
    mutable bool must_correct_platform;

    //! Summary of relevant parameters
    mutable std::string summary;

    //@}

  };
}

#endif
