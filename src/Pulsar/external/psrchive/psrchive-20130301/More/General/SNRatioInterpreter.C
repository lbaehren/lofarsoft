/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SNRatioInterpreter.h"
#include "Pulsar/FourierSNR.h"
#include "Pulsar/StandardSNR.h"
#include "Pulsar/AdaptiveSNR.h"
#include "Pulsar/SquareWave.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::SNRatioInterpreter::SNRatioInterpreter ()
{
  add_command 
    ( &SNRatioInterpreter::adaptive,
      "adaptive", "install AdaptiveSNR algorithm",
      "usage: adaptive \n" );

  add_command 
    ( &SNRatioInterpreter::fourier,
      "fourier", "install FourierSNR algorithm",
      "usage: fourier \n" );

  add_command 
    ( &SNRatioInterpreter::square,
      "square", "install SquareWave algorithm",
      "usage: square \n" );

  add_command 
    ( &SNRatioInterpreter::standard,
      "standard", "install StandardSNR algorithm",
      "usage: standard filename.ar \n" );

  add_command 
    ( &SNRatioInterpreter::phase,
      "phase", "install Profile::snr_phase algorithm (default)",
      "usage: phase \n" );

}

string Pulsar::SNRatioInterpreter::adaptive (const string& args) try
{
  if (!adaptive_functor)
    adaptive_functor.set( new AdaptiveSNR, &AdaptiveSNR::get_snr );

  Profile::snr_strategy = adaptive_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}

string Pulsar::SNRatioInterpreter::fourier (const string& args) try
{
  if (!fourier_functor)
    fourier_functor.set( new FourierSNR, &FourierSNR::get_snr );

  Profile::snr_strategy = fourier_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}

string Pulsar::SNRatioInterpreter::square (const string& args) try
{
  if (!square_functor)
    square_functor.set( new SquareWave, &SquareWave::get_snr );

  Profile::snr_strategy = square_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}

string Pulsar::SNRatioInterpreter::standard (const string& args) try
{
  if (!standard_snratio)
    standard_snratio = new StandardSNR;

  if (!standard_functor)
    standard_functor.set( standard_snratio.get(), &StandardSNR::get_snr );

  Reference::To<Archive> archive = Archive::load ( args );
  standard_snratio->set_standard( archive->get_Profile (0,0,0) );

  Profile::snr_strategy = standard_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}

string Pulsar::SNRatioInterpreter::phase (const string& args) try
{
  if (!phase_functor)
    phase_functor.set( &Pulsar::snr_phase );

  Profile::snr_strategy = phase_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}

string Pulsar::SNRatioInterpreter::empty ()
{ 
  if (Profile::snr_strategy == adaptive_functor)
    return "adaptive";

  if (Profile::snr_strategy == fourier_functor)
    return "fourier";

  if (Profile::snr_strategy == square_functor)
    return "square";

  if (Profile::snr_strategy == standard_functor)
    return "standard";

  if (Profile::snr_strategy == phase_functor)
    return "phase";

  return "unknown";
}

