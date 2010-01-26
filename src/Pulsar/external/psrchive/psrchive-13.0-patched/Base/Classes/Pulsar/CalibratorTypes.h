//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/CalibratorTypes.h,v $
   $Revision: 1.3 $
   $Date: 2009/03/17 04:37:11 $
   $Author: straten $ */

#ifndef __CalibratorTypes_H
#define __CalibratorTypes_H

#include "Pulsar/CalibratorType.h"

namespace Pulsar
{
  //! Contains the enumeration of all calibrator types
  /*! A separate namespace is used to keep the enumeration separate from
    the definition of the Calibrator::Type class */
  namespace CalibratorTypes
  {
    //! Flux calibrator
    class Flux : public Pulsar::Calibrator::Type
    {
    public:
      std::string get_name () const { return "flux"; }
      unsigned get_nparam () const { return 4; }
    };

    //! Corrections
    class Corrections : public Pulsar::Calibrator::Type
    {
    public:
      std::string get_name () const { return "corrections"; }
      unsigned get_nparam () const { return 0; }
    };
    
    //! Polarization calibrator
    class Poln : public Pulsar::Calibrator::Type
    {
    };
    
    //! Gain, differential gain and differential phase
    class SingleAxis : public Poln
    {
    public:
      std::string get_name () const { return "single"; }
      unsigned get_nparam () const { return 3; }
    };
    
    //! van Straten (2002; ApJ 568:436), equation A1
    class van02_EqA1 : public Poln
    {
    public:
      std::string get_name () const { return "van02eA1"; }
      unsigned get_nparam () const { return 6; }
    };
    
    //! Full 7 degrees of freedom parameterization of Jones matrix
    class CompleteJones : public Poln
    {
    public:
      bool is_a (const Type* that) const
      { return dynamic_cast<const CompleteJones*>(that) != 0; }

      std::string get_name () const
      { return "jones"; }

      unsigned get_nparam () const
      { return 7; }
    };

    //! van Straten (2004; ApJSS 152:129), equation 13
    class van04_Eq13 : public CompleteJones
    {
    public:
      bool is_a (const Type* that) const { return Type::is_a (that); }
      std::string get_name () const { return "van04e13"; }
    };

    //! Unpublished polar decomposition
    class van09_Eq : public CompleteJones
    {
    public:
      bool is_a (const Type* that) const { return Type::is_a (that); }
      std::string get_name () const { return "van09"; }
    };
    
    //! Phenomenological parameterizations of Jones matrix
    /*! Separate backend and frontend transformations enable variation of 
      backend to be modeled */
    class Phenomenological : public CompleteJones
    {
      bool is_a (const Type* that) const
      { return dynamic_cast<const Phenomenological*>(that) != 0; }
    };
    
    //! Britton (2000; ApJ 532:1240), equation 19
    class bri00_Eq19 : public Phenomenological
    {
    public:
      bool is_a (const Type* that) const { return Type::is_a (that); }
      std::string get_name () const { return "bri00e19"; }
    };
    
    //! van Straten (2004; ApJSS 152:129), equation 18
    class van04_Eq18 : public Phenomenological
    {
    public:
      bool is_a (const Type* that) const { return Type::is_a (that); }
      std::string get_name () const { return "van04e18"; }
    };

    //! Mixes a SingleAxis and Phenomenological parameterization
    class Hybrid : public Poln
    {
    public:
      bool is_a (const Type* that) const
      { return dynamic_cast<const Hybrid*>(that) != 0; }

      std::string get_name () const { return "hybrid"; }
      // 7 Phenomenological + 3 SingleAxis
      unsigned get_nparam () const { return 10; }
    };
    
    //! Ord, van Straten, Hotan & Bailes (2004; MNRAS 352:804), section 2.1
    class ovhb04 : public Hybrid
    {
    public:
      bool is_a (const Type* that) const { return Type::is_a (that); }
      std::string get_name () const { return "ovhb04"; }
    };
    
    //! Calibrate by brute force and unjustified assumptions
    class Nefarious : public Poln
    {
    public:
      unsigned get_nparam () const { return 0; }
    };
    
    //! Degree of Polarization Calibrator (P236)
    class DoP : public Nefarious
    {
    public:
      std::string get_name () const { return "dop"; }
    };
    
    //! Off-pulse Calibrator (P236)
    class OffPulse: public Nefarious
    {
    public:
      std::string get_name () const { return "off"; }
    };
  }
}

#endif
