/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BasisCorrection.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/SingleAxis.h"
#include "Pulsar/SingleAxisSolver.h"
#include "Pulsar/Parallactic.h"
#include "Pauli.h"

using namespace std;

double min_cyclic (double diff)
{
  double min = diff;

  if (fabs(diff+2*M_PI) < fabs(diff)) 
    min = diff + 2*M_PI;

  if (fabs(diff-2*M_PI) < fabs(diff))
    min = diff - 2*M_PI;

  return min;
}

int main (int argc, char** argv) try
{
  bool verbose = false;

  /*
    Simulate 21 on-pulse phase bins, with longitude = atan(U/Q)
     1) going from negative to positive values;
     2) spanning 270 degrees in total; and
     3) equal to zero in the eighth bin.
  */
  const unsigned nstate = 21;
  const unsigned zero = 7;

  Stokes<double> states [nstate];
  for (unsigned istate=0; istate < nstate; istate++)
  {
    double radian = M_PI*1.5 * double(int(istate)-int(zero))/nstate;
    states[istate] = Stokes<double> (1, cos(radian), sin(radian), 0);
  }

  Calibration::Parallactic para;
  Pulsar::Receiver receiver;
  Calibration::SingleAxis backend;

  Calibration::SingleAxisSolver solver;
  Calibration::SingleAxis solution;

  // The naive calibrator will assume that the noise diode has Stokes U = 1

  Estimate<double> one (1,1);
  Estimate<double> nil (0,1);
  Stokes< Estimate<double> > assume_cal (one, nil, one, nil);
  
  if (verbose)
    cerr << "assume cal=" << assume_cal << endl;

  cout << "\nhand\tcalU\t f(phi)\t f(PA)\t offset" << endl;

  // Start with a right-handed receiver
  Signal::Hand hand = Signal::Right;

  Pulsar::BasisCorrection correction;

  for (unsigned i=0; i<2; i++)
  {
    receiver.set_hand (hand);
    Jones<double> actual_rcvr = correction( &receiver );

    // start with a noise diode with Stokes U = 1
    Angle xyph = 0;

    for (unsigned j=0; j<2; j++)
    {
      receiver.set_reference_source_phase( xyph );
      Stokes<double> actual_cal = receiver.get_reference_source ();

      if (verbose)
	cerr << "actual cal=" << actual_cal << endl;

      // count changes in longitude between pulse phase bins
      unsigned up_count = 0;
      unsigned down_count = 0;

      // count changes in longitude as a function of parallactic angle
      unsigned pro_count = 0;
      unsigned con_count = 0;

      // count alignment of zero point when parallactic angle is zero
      unsigned zero_count = 0;
      unsigned pi_count = 0;

      // repeat the simulation as a function of differential phase

      for (double phi=-M_PI*.99/2; phi < M_PI/2; phi += M_PI/20)
      {
	backend.set_diff_phase (phi);
	Jones<double> be = backend.evaluate();

	// apply the differential phase transformation to the actual CAL

	Stokes< Estimate<double> > obs_cal = transform (actual_cal, be);
	for (unsigned i=0; i<4; i++)
	  obs_cal[i].set_variance( 1.0 );

	if (verbose)
	  cerr << "observed cal=" << obs_cal << endl;

	/*
	  Solve for the differential phase, given the actual observed
	  Stokes parameters of the noise diode, and using the assumed
	  noise diode polarization.
	*/

	solver.set_input (assume_cal);
	solver.set_output (obs_cal);
	solver.solve (&solution);

	double previous_zero = 500;

	for (double va = -M_PI/2; va <= M_PI/2; va += M_PI/30)
	{
	  para.set_phi (va);
	  
	  Jones<double> R = para.evaluate ();

	  /*
	    The naive calibrator uses the solution based on the cal =
	    Stokes U assumption and the known parallactic angle
	    rotation.
	  */
	  Jones<double> calib = inv ( solution.evaluate() * R );

	  /*
	    The observed pulsar is subjected to the parallactic
	    rotation, the simulated receiver and backend, followed
	    by the naive calibrator
	  */
	  Jones<double> total = calib * be * actual_rcvr * R;
	  
	  double longitude[nstate];
	  
	  for (unsigned istate=0; istate < nstate; istate++)
	  {
	    Stokes<double> result = transform( states[istate], total );
	    longitude[istate] = atan2 (result[2], result[1]);
	  }

	  if (fabs(va) < 1e-15)
	  {
	    /*
	      When parallactic angle is zero, count the number of times
	      the zero point is zero or +/- pi
	    */
	    if (fabs(longitude[zero]) < 1e-15)
	      zero_count ++;
	    if (fabs(min_cyclic (longitude[zero]-M_PI)) < 1e-15)
	      pi_count ++;
	  }

	  /*
	    Count the number of times that the zero point is greater
	    than or less than the zero point from the previous simulated
	    parallactic angle.  This is used to describe the longitude trend
	    as a function of parallactic angle.
	  */
	  if (previous_zero != 500)
	  {
	    double diff = min_cyclic (longitude[zero] - previous_zero);
	      
	    if (diff > 1e-15)
	      pro_count ++;
	      
	    if (diff < -1e-15)
	      con_count ++;
	  }

	  previous_zero = longitude[zero];

	  /*
	    Count the number of times that the longitude of a given input
	    state is greater than or less than the longitude of the 
	    preceding input state.  This is used to describe the longitude
	    trend as a function of "pulse phase"
	  */
	  for (unsigned istate = 1; istate < nstate; istate++)
	  {
	    double diff = min_cyclic (longitude[istate] - longitude[istate-1]);
	      
	    if (diff > 0)
	      up_count ++;
	      
	    if (diff < 0)
	      down_count ++;
	  }
	}
      }

      string slope = "undef";

      if (up_count && !down_count)
	slope = "up";
      if (!up_count && down_count)
	slope = "down";

      string trend = "none";

      if (pro_count && !con_count)
	trend = "pro";
      if (!pro_count && con_count)
	trend = "con";
      if (pro_count && con_count)
	trend = "mix";

      string pi = "undef";

      if (zero_count && !pi_count)
	pi = "0";
      if (!zero_count && pi_count)
	pi = "pi";

      cout << hand << "\t" << 1-int(j)*2 
	   << "\t " << slope << "\t " << trend << "\t " << pi << endl;

      // switch to a noise diode with Stokes U = -1
      xyph = M_PI;
    }

    // switch to a left-handed receiver
    hand = Signal::Left;

  }
  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}
