/***************************************************************************
 *
 *   Copyright (C) 2006 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/counter_drift.h"
#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"

using namespace std;

template<class T> T SQR (T x) { return x*x; }

//! Rotates subints to counter a drifting pulse due to incorrect P/Pdot
void
Pulsar::counter_drift(Archive* archive,
		      double trial_p, double trial_pdot, MJD reference_time)
{
  unsigned midsub = unsigned(archive->get_nsubint()/2);
  double centre_period = archive->get_Integration(midsub)->get_folding_period();
  double delta_P  = trial_p - centre_period;

  // nu(t)  = nu0 + delta_nu + nu1*t + nu2*t*t/2 + ...
  // P(t)   = P0  + delta_P  + P1*t  + P2*t*t/2 + ...
  // Here we approximate as P(n>1)=0 and nu(n>1)=0
  
  // nu0    = 1/P0
  // nu(0)  = nu0 + delta_nu = 1/P(0) = 1/(P0 + delta_P)
  // nu(t)  = 1/P(t)
  // dnu/dt = -1/P(t)^2 * P1
  // nu1    = dnu/dt(0) = -P1/(P0 + delta_P)^2

  double nu0      = 1.0 / centre_period;
  double delta_nu = -nu0 + 1.0/(centre_period + delta_P);
  double nu1      = -trial_pdot / SQR(centre_period + delta_P);

  counter_frequency_drift(archive,nu0+delta_nu,nu1,reference_time);
}

//! Same as above, but inputs are in frequency domain
void
Pulsar::counter_frequency_drift(Archive* archive,
				double trial_nu0, double trial_nu1,
				MJD reference_time)
{
  if( reference_time == MJD::zero )
    reference_time =  archive->get_Integration(0)->get_start_time();

  unsigned midsub = unsigned(archive->get_nsubint()/2);

  double nu0      = 1.0 / archive->get_Integration(midsub)->get_folding_period(); 
  double delta_nu = trial_nu0 - nu0;
  double nu1      = trial_nu1;

  for (unsigned i=0;i<archive->get_nsubint();i++){
	 // I changed time to go in the conventional direction, i.e. forwards M.Keith 2013
    double subint_offset_seconds = (archive->get_Integration(i)->get_epoch() - reference_time).in_seconds();    
    // Number of pulses = integral of nu(t) dt
    //                  = (nu0 + delta_nu)*T + nu1*T*T/2
    // Extra pulses     = delta_nu*T + nu1*T*T/2
    double extra_phase  = delta_nu * subint_offset_seconds + nu1 * SQR(subint_offset_seconds)/2.0;
    archive->get_Integration(i)->expert()->rotate_phase( -extra_phase ); // rotate profile backewards M.Keith 2013
  }

}

//! Same as above, but takes multiple frequency derivatives
void
Pulsar::multi_counter_fdrift (Archive* archive,
			      vector<double> nus,
			      MJD reference_time)
{
  if( nus.empty() )
    throw Error(InvalidState,"Pulsar::multi_counter_fdrift()",
		"The input vector of frequency and its derivatives was empty!");

  if( reference_time == MJD::zero )
    reference_time =  archive->get_Integration(0)->get_start_time();

  unsigned midsub = unsigned(archive->get_nsubint()/2);

  double nu0      = 1.0 / archive->get_Integration(midsub)->get_folding_period(); 
  double delta_nu = nus[0] - nu0;

  for (unsigned i=0;i<archive->get_nsubint();i++){
    double t = (reference_time - archive->get_Integration(i)->get_epoch()).in_seconds();    

    //            nu(t) = nu0 + delta_nu + nu1*t + nu2*t^2/2 + ... + nu(n)*t^n/n! + ...
    // Number of pulses = integral of nu(t) dt   
    //                  = (nu0 + delta_nu)*T + nu1*T*T/2 + ... + nu(n) *T^(n+1)/(n+1)! + ...
    // Extra pulses     = delta_nu*T + nu1*T*T/2 + ... + nu(n) *T^(n+1)/(n+1)! + ...

    double extra_phase  = delta_nu * t;

    double power_of_t = t;
    unsigned factorial = 1;

    for( unsigned j=1; j<nus.size(); j++){
      factorial *= (j+1);
      power_of_t *= t;
      extra_phase += nus[j] * power_of_t/factorial;
    }

    archive->get_Integration(i)->expert()->rotate_phase( extra_phase );
  } // foreach subint

}
