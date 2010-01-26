//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/CalibratorType.h,v $
   $Revision: 1.3 $
   $Date: 2009/03/17 04:37:11 $
   $Author: straten $ */

#ifndef __CalibratorType_H
#define __CalibratorType_H

#include "Pulsar/Calibrator.h"

namespace Pulsar
{
  /*!  Children of this abstract base class store the minimal amount
    of calibrator information required for file I/O.  They also
    express the relationships between different calibrators and
    obviate the need to recompile all Calibrator-derived code whenever
    a new type is added.
  */
  class Calibrator::Type : public Reference::Able
  {
  public:

    //! Construct a new instance of Calibrator::Type, based on name
    static Type* factory (const std::string& name);

    //! Construct a new instance of Calibrator::Type, based on Calibrator
    static Type* factory (const Calibrator*);

    //! Return the name of the calibrator type
    virtual std::string get_name () const = 0;

    //! Return the number of parameters that describe the transformation
    virtual unsigned get_nparam () const = 0;

    //! Return true if that is a this
    virtual bool is_a (const Type* that) const;

    //! Return true if this is a T
    template<class T>
    bool is_a () const
    {
      return dynamic_cast<const T*> (this) != 0;
    }

  };
}

#endif
