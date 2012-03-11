//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Pulsar/Site.h,v $
   $Revision: 1.2 $
   $Date: 2009/03/03 05:03:49 $
   $Author: straten $ */

#ifndef __Pulsar_Site_h
#define __Pulsar_Site_h

#include "Reference.h"

namespace Pulsar
{

  //! Site coordinates
  class Site : public Reference::Able
  {
  public:

    //! Get the observatory name
    virtual std::string get_name () const = 0;

    //! Get the geocentric XYZ coordinates in metres
    virtual void get_xyz (double& x, double& y, double& z) const = 0;

    //! Get the latitude and longitude in radians, and radius in metres
    /*! longitude is in degrees East of Greenwich */
    virtual void get_sph (double& lat, double& lon, double& rad) const = 0;

    //! Get the location of the named antenna
    static const Site* location (const std::string& antenna);
  };

}

#endif

