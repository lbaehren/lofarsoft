//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/RemoveBaseline.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/09 10:44:47 $
   $Author: straten $ */

#ifndef _Pulsar_RemoveBaseline_H
#define _Pulsar_RemoveBaseline_H

#include "Pulsar/Transformation.h"
#include "Pulsar/Archive.h"

namespace Pulsar
{
  //! Algorithms that remove the off-pulse baseline
  class RemoveBaseline : public Transformation<Archive>
  {
  public:

    /* Two simple baseline removal algorithms
       (Not every child of RemoveBaseline needs to be nested.) */

    class Total;
    class Each;
  };

  //! Find the baseline from the total integrated total intensity profile
  /*! This algorithm assumes that both the timing model and the
    dispersion measure accurately reflect the phase of the pulsar in
    every sub-integration and frequency channel. */
  class RemoveBaseline::Total : public RemoveBaseline
  {
  public:

    //! Remove the baseline
    void transform (Archive*);
  };

  //! Find the baseline from each total intensity profile
  /*! This algorithm does not assume anything about the timing model
    or the dispersion measure.  The baseline is separately determined
    for each sub-integration and frequency channel. */
  class RemoveBaseline::Each : public RemoveBaseline
  {
  public:

    //! Remove the baseline
    void transform (Archive*);
  };
}

#endif

