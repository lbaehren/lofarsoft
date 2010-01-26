/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Receiver.h"
#include <iostream>

void parse_test (const char* filename)
{
  Reference::To<Pulsar::Receiver> receiver;
  receiver = Pulsar::Receiver::load (filename);
    
  cerr << "\n\n\nReceiver parsed:\n" << endl;
  cerr << "Name = " << receiver->get_name() << endl;
  cerr << "Basis = " << Signal::basis_string(receiver->get_basis()) << endl;
  cerr << endl;
  cerr << "Tracking mode = " << receiver->get_tracking_mode_string() << endl;
  cerr << "Tracking angle = " << receiver->get_tracking_angle().getDegrees()
       << " deg" << endl;
  cerr << endl;
  cerr << "X offset = " << receiver->get_X_offset().getDegrees()
       << " deg" << endl;
  cerr << "Y offset = " << receiver->get_Y_offset().getDegrees()
       << " deg" << endl;
  cerr << "CAL offset = " << receiver->get_calibrator_offset().getDegrees()
       << " deg" << endl;
  cerr << endl;
}

//! Return true if two matrices are equal up to a scalar constant
template <class T>
bool parallel (const Jones<T>& a, const Jones<T>& b)
{
  Jones<T> test = inv(a)*b;

  double mean = 0.5 * trace(test).real();
  double tnorm = norm (test - Jones<T>(mean));

  /*
    cerr << "test=" << test << endl;
    cerr << "mean=" << mean << endl;
    cerr << "norm=" << tnorm << endl;
  */

  return tnorm < 1e-15 * fabs(mean);
}

void test_parkes_multibeam ()
{
  Pulsar::Receiver receiver;

  receiver.set_X_offset (0);
  receiver.set_Y_offset (M_PI);
  receiver.set_calibrator_offset (-0.5 * M_PI);

  complex<double> i (0,1);
  Jones<double> expected (i, 0,
			  0, -i);

  if (!parallel(receiver.get_transformation(),expected)) {
    cerr << "Receiver::get_transformation="
	 << receiver.get_transformation()
	 << " != " << expected << endl;
    throw Error (InvalidState, "test_parkes_multibeam");
  }

  Stokes<double> expected_cal (1,0,1,0);

  if (receiver.get_reference_source() != expected_cal) {
    cerr << "Receiver::get_reference_source=" 
	 << receiver.get_reference_source()
	 << " != " << expected_cal << endl;
    throw Error (InvalidState, "test_parkes_multibeam");
  }

  Pulsar::Receiver copy (receiver);

  if (!parallel(copy.get_transformation(),expected)) {
    cerr << "Receiver::get_transformation="
	 << copy.get_transformation()
	 << " != " << expected << endl;
    throw Error (InvalidState, "test_parkes_multibeam");
  }

  if (copy.get_reference_source() != expected_cal) {
    cerr << "Receiver::get_reference_source=" 
	 << copy.get_reference_source()
	 << " != " << expected_cal << endl;
    throw Error (InvalidState, "test_parkes_multibeam");
  }

}

void test_copy ()
{
}

int main (int argc, char** argv) try {

  Pulsar::Archive::set_verbosity (3);

  if (argc == 2) {
    parse_test (argv[1]);
    return 0;
  }

  cerr << "Testing Parkes Multibeam" << endl;
  test_parkes_multibeam ();

  cerr << "Testing Receiver Extension copy" << endl;
  test_copy ();

  cerr << "All Receiver Extension tests passed" << endl;
  return 0;

}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}

