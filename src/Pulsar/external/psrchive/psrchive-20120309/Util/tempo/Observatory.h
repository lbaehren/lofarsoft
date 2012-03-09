//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Observatory.h,v $
   $Revision: 1.5 $
   $Date: 2009/03/03 05:03:49 $
   $Author: straten $ */

#ifndef __Tempo_Observatory_h
#define __Tempo_Observatory_h

#include "Pulsar/Site.h"

namespace Tempo
{

  //! Observatory data available from TEMPO
  class Observatory : public Pulsar::Site
  {
  public:

    //! Default constructor
    Observatory ();

    //! Get the one-letter tempo code
    char get_code () const;
    void set_code (char);

    //! Get the two-letter ITOA code
    std::string get_itoa_code () const;
    void set_itoa_code (const std::string&);

    //! Get the observatory name
    std::string get_name () const;
    void set_name (const std::string&);

  protected:

    char code;
    std::string itoa_code;
    std::string name;

  };

  class ObservatoryITRF : public Observatory
  {
  public:

    //! Default constructor
    ObservatoryITRF (double x, double y, double z);

    //! Get the geocentric XYZ coordinates in metres
    virtual void get_xyz (double& x, double& y, double& z) const;

    //! Get the latitude and longitude in radians, radius in metres
    /*! longitude is in degrees East of Greenwich */
    virtual void get_sph (double& lat, double& lon, double& rad) const;

  protected:
    double x, y, z;
  };

  class ObservatoryIAU1976 : public Observatory
  {
  public:

    //! Default constructor
    ObservatoryIAU1976 (double alat, double alon, double elev);

    //! Get the geocentric XYZ coordinates in metres
    virtual void get_xyz (double& x, double& y, double& z) const;

    //! Get the latitude and longitude in radians, radius in metres
    /*! longitude is in degrees East of Greenwich */
    virtual void get_sph (double& lat, double& lon, double& rad) const;

  protected:
    double lat, lon, rad;
  };

}

#endif

