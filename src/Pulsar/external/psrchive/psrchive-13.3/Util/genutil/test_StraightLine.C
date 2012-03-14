/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "StraightLine.h"
#include "BoxMuller.h"

int main () 
{
  double slope = 1.234;
  double intercept = 4.567;

  BoxMuller noise;

  unsigned ntrial = 10000;
  unsigned errors = 0;

  for (unsigned trial=0; trial < ntrial; trial++)
  {
    StraightLine<double> line;

    for (unsigned i=0; i<1000; i++)
      line.add_coordinate (i,Estimate<double>(i*slope+intercept+noise(),1.0));

    Estimate<double> Eslope = line.get_slope();

#ifdef _DEBUG
    cerr << "slope=" << Eslope << endl;
    cerr << "intercept=" << line.get_intercept() << endl;
#endif

    if ( fabs(slope-Eslope.get_value()) > Eslope.get_error() )
      errors ++;
  }

  float percentage_error = 100.0 * float(errors) / float(ntrial);

  cerr << "percentage error = " << percentage_error << endl;

  // can expect around 33% error
  if ( percentage_error > 40 )
    return -1;

  cerr << "test passed" << endl;
  return 0;
}
