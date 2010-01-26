/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Types.h"
#include "Error.h"

using namespace std;

const char* Signal::state_string (State state)
{
  switch (state)  {
  case Nyquist:
    return "Baseband Nyquist";
  case Analytic:
    return "Baseband Quadrature";
  case Stokes:
    return "Stokes Parameters";
  case PseudoStokes:
    return "Pseudo-Stokes Parameters";
  case Coherence:
    return "Coherency Products";
  case PPQQ:
    return "Polarimetric Intensity";
  case Intensity:
    return "Total Intensity";
  case NthPower:
    return "Stokes I to some power";
  case Invariant:
    return "Invariant Interval";
  default:
    return "Invalid";
  }
}

const char* Signal::source_string(Source source)
{
  switch( source ){
  case Unknown:
    return "Unknown";
  case Pulsar:
    return "Pulsar";
  case PolnCal:
    return "PolnCal";
  case FluxCalOff:
    return "FluxCal-Off";
  case FluxCalOn:
    return "FluxCal-On";
  case Calibrator:
    return "Calibrator";
  default:
    return "Invalid";
  }
}

const char* Signal::basis_string(Basis basis)
{
  switch( basis ){
  case Circular:
    return "Circular";
  case Linear:
    return "Linear";
  case Elliptical:
    return "Elliptical";
  default:
    return "Invalid";
  }
}

unsigned Signal::State2npol (State s)
{
  if( s==Nyquist || s==Analytic || s==PPQQ )
    return 2;
  else if( s==Coherence || s==Stokes || s==PseudoStokes )
    return 4;
  else if( s==Intensity || s==Invariant )
    return 1;
  else if( s==NthPower )
    return 1;
  else if( s==PP_State || s==QQ_State )
    return 1;

  throw Error (InvalidState, "Signal::State2npol", "invalid state");
}

//! Tells you if your state is consistent with your npol and ndim
bool Signal::valid_state(Signal::State state,unsigned ndim,unsigned npol, string& reason){

  switch (state) {
  case Signal::Nyquist:
    if (ndim != 1)  {
      reason = "state=" + string(state_string(state)) + " and ndim!=1";
      return false;
    }
    break;

  case Signal::Analytic:
    if (ndim != 2) {
      reason = "state=" + string(state_string(state)) + " and ndim!=2";
      return false;
    }
    break;

  case Signal::Invariant:
  case Signal::Intensity:
  case Signal::NthPower:
    if (npol != 1) {
      reason = "state=" + string(state_string(state)) + " and npol!=1";
      return false;
    }
    break;

  case Signal::PPQQ:
    if (npol != 2) {
      reason = "state=" + string(state_string(state)) + " and npol!=2";
      return false;
    }
    break;

  case Signal::Coherence:
  case Signal::Stokes:
  case Signal::PseudoStokes:
    if (ndim*npol != 4) {
      reason = "state=" + string(state_string(state)) + " and ndim*npol!=4";
      return false;
    }
    break;
  case Signal::PP_State:
  case Signal::QQ_State:
    if( npol != 1 || ndim != 1 ) {
      reason = "state=" + string(state_string(state)) + " and (ndim or npol) is not 1";
      return false;
    }	
    break;

  case Signal::FourthMoment:
    if( npol != 10 )
      {
	reason = "state=FourthMoment and npol is not 10";
	return false;
      }
    break;

  default:
    reason = "unknown state";
    return false;
  }

  return true;
}

Signal::State Signal::pscrunch (State state)
{
  switch (state)
    {
    case Stokes:
    case PseudoStokes:
    case Coherence:
    case PPQQ:
      return Intensity;
    default:
      return state;
    }
}

// //////////////////////////////////////////////////////////////////////////
//
// Signal::State
//
// //////////////////////////////////////////////////////////////////////////

const string Signal::State2string (State state)
{
  switch (state)  {
  case Nyquist:
    return "Nyquist";
  case Analytic:
    return "Analytic";
  case Stokes:
    return "Stokes";
  case PseudoStokes:
    return "PseudoStokes";
  case Coherence:
    return "Coherence";
  case PPQQ:
    return "PPQQ";
  case Intensity:
    return "Intensity";
  case NthPower:
    return "NthPower";
  case Invariant:
    return "Invariant";
  case PP_State:
    return "PP";
  case QQ_State:
    return "QQ";
  default:
    return "Invalid";
  }
}

Signal::State Signal::string2State (const string& ss)
{
  if(ss=="Nyquist")
    return Nyquist;
  if(ss=="Analytic")
    return Analytic;
  if(ss=="Intensity")
    return Intensity;
  if (ss=="NthPower")
    return NthPower;
  if(ss=="PPQQ")
    return PPQQ;
  if(ss=="Coherence")
    return Coherence;
  if(ss=="Stokes")
    return Stokes;
  if(ss=="PseudoStokes")
    return PseudoStokes;
  if(ss=="PP")
    return PP_State;
  if(ss=="QQ")
    return QQ_State;
  
  throw Error (InvalidState, "Signal::string2State",
	       "Unknown state '" + ss + "'");
}

std::ostream& operator<< (std::ostream& ostr, Signal::State state)
{
  return ostr << State2string(state);
}

std::istream& operator >> (std::istream& is, Signal::State& state)
{
  return extraction (is, state, Signal::string2State);
}

// //////////////////////////////////////////////////////////////////////////
//
// Signal::Basis
//
// //////////////////////////////////////////////////////////////////////////

const string Signal::Basis2string (Basis basis)
{
  return basis_string(basis);
}

Signal::Basis Signal::string2Basis (const string& ss)
{
  if(ss=="Circular")
    return Signal::Circular;
  if(ss=="Linear")
    return Signal::Linear;
  if(ss=="Elliptical")
    return Signal::Elliptical;

  throw Error (InvalidState, "Signal::string2Basis",
	       "Unknown basis '" + ss + "'");
}

// //////////////////////////////////////////////////////////////////////////
//
// Signal::Source
//
// //////////////////////////////////////////////////////////////////////////

const string Signal::Source2string (Source source)
{
  return source_string(source);
}

Signal::Source Signal::string2Source (const string& ss)
{
  if(ss=="Pulsar")
    return Pulsar;
  if(ss=="PolnCal")
    return PolnCal;
  if(ss=="FluxCal-Off")
    return FluxCalOff;
  if(ss=="FluxCal-On")
    return FluxCalOn;
  if(ss=="Calibrator")
    return Calibrator;

  throw Error (InvalidState, "Signal::string2Source",
	       "Unknown source '" + ss + "'");
}

std::ostream& operator<< (std::ostream& ostr, Signal::Source source)
{
  return ostr << Source2string(source);
}

std::istream& operator >> (std::istream& is, Signal::Source& source)
{
  return extraction (is, source, Signal::string2Source);
}

// //////////////////////////////////////////////////////////////////////////
//
// Signal::Scale
//
// //////////////////////////////////////////////////////////////////////////

const string Signal::Scale2string (Scale scale)
{
  switch( scale ) {
  case EMF:
    return "EMF";
  case Volts:
    return "Volts";
  case Energy:
    return "Energy";
  case Joules:
    return "Joules";
  case FluxDensity:
    return "FluxDensity";
  case ReferenceFluxDensity:
    return "ReferenceFluxDensity";
  case Jansky:
    return "Jansky";
  default:
    return "Unknown";
  }
}

Signal::Scale Signal::string2Scale (const string& ss)
{
  if( ss=="EMF" )
    return EMF;
  if( ss=="Volts" ) 
    return Volts;
  if( ss=="Energy" )
    return Energy;
  if( ss=="Joules" )
    return Joules;
  if( ss=="FluxDensity" )
    return FluxDensity;
  if( ss=="ReferenceFluxDensity" )
    return ReferenceFluxDensity;
  if( ss=="Jansky" )
    return Jansky;

  throw Error (InvalidState, "Signal::string2Scale",
	       "Unknown scale '" + ss + "'");
}

std::ostream& operator<< (std::ostream& ostr, Signal::Scale scale)
{
  return ostr << Scale2string(scale);
}

std::istream& operator >> (std::istream& is, Signal::Scale& scale)
{
  return extraction (is, scale, Signal::string2Scale);
}
