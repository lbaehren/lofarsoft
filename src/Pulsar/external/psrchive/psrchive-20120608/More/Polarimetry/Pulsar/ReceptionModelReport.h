//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionModelReport.h,v $
   $Revision: 1.1 $
   $Date: 2007/11/05 00:26:17 $
   $Author: straten $ */

#ifndef __Calibration_ReceptionModelReport_H
#define __Calibration_ReceptionModelReport_H

#include "Reference.h"

namespace Calibration {

  class ReceptionModel;

  class ReceptionModelReport : public Reference::Able
  {

  public:

    //! Default constructor
    ReceptionModelReport (const std::string& filename);

    //! Write a report on the specified model
    bool report (ReceptionModel* model);

  protected:

    //! The name of the file to which text format data will be written
    std::string output_filename;

  };

}

#endif
