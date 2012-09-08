//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Types.h,v $
   $Revision: 1.29 $
   $Date: 2009/10/07 03:32:42 $
   $Author: straten $ */

#ifndef __Types_h
#define __Types_h

#include "Conventions.h"
#include <string>

//! Defines various signal types
namespace Signal {

  //! Dimensions of the data (so far)
  enum Dimension {
    //! Time given in MJD
    Time,
    //! Radio Frequency in MHz
    Frequency,
    //! Light
    Polarization,
    //! Pulsar Phase, for instance
    Phase,
    //! Mathematical space (real, complex, jones, etc.)
    Space
  };


  //! The scale in which units are measured
  enum Scale {
    //! Uncalibrated voltage
    EMF,
    //! Calibrated voltage
    Volts,
    //! Uncalibrated energy
    Energy,
    //! Calibrated energy
    Joules,
    //! Uncalibrated flux density
    FluxDensity,
    //! Reference flux density
    ReferenceFluxDensity,
    //! Calibrated flux density
    Jansky
  };
    
  //! Sources of observed signals
  enum Source {
    //! An unknown source
    Unknown,
    //! A pulsar
    Pulsar,
    //! A polarization calibrator, such as the linear noise diode
    PolnCal,
    //! A flux calibrator, such as Hydra A + the linear noise diode
    FluxCalOn,
    //! Near a flux calibrator, such as two degrees off Hydra A
    FluxCalOff,
    //! A calibrator solution
    Calibrator
  };

  //! Possible states of the data
  enum State
  {
    //! Nyquist sampled voltages (real)
    Nyquist,
    //! In-phase and Quadrature sampled voltages (complex)
    Analytic,
    //! Square-law detected total power
    Intensity,
    //! Square-law detected nth power
    NthPower,
    //! Square-law detected, two polarizations
    PPQQ,
    //! PP, QQ, Re[PQ], Im[PQ]
    Coherence,
    //! Stokes I,Q,U,V
    Stokes,
    //! PseudoStokes S0,S2,S2,S3
    PseudoStokes,
    //! Stokes invariant interval
    Invariant,
    //! Other
    Other,
    //! Just PP
    PP_State,
    //! Just QQ
    QQ_State,
    //! Fourth moment of the electric field (covariance of Stokes parameters)
    FourthMoment
  };
  
  unsigned State2npol (State s);
  
  const char* source_string (Source source);
  const char* state_string (State state);
  const char* basis_string (Basis basis);

  const std::string Source2string (Source source);
  const std::string State2string  (State state);
  const std::string Basis2string  (Basis basis);
  const std::string Scale2string  (Scale scale);

  Basis  string2Basis  (const std::string& ss);
  Source string2Source (const std::string& ss);
  State  string2State  (const std::string& ss);
  Scale  string2Scale  (const std::string& ss);

  //! Tells you if your state is consistent with your npol and ndim
  bool valid_state (State state, unsigned ndim, unsigned npol,
                    std::string& reason);

  //! Returns the state resulting from a pscrunch operation
  State pscrunch (State state);

}

std::ostream& operator << (std::ostream& ostr, Signal::Source source);
std::istream& operator >> (std::istream& is, Signal::Source& source);

std::ostream& operator << (std::ostream& ostr, Signal::State state);
std::istream& operator >> (std::istream& is, Signal::State& state);

std::ostream& operator << (std::ostream& ostr, Signal::Scale scale);
std::istream& operator >> (std::istream& is, Signal::Scale& scale);

/* note that Basis extraction and insertion operators are defined in
   Conventions.h */

#endif

