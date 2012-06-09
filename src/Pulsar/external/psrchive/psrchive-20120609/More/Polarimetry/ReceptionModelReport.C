/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModelReport.h"
#include "Pulsar/CoherencyMeasurementSet.h"
#include "Pulsar/ReceptionModel.h"
#include "Pauli.h"

#include <fstream>
using namespace std;

Calibration::ReceptionModelReport::ReceptionModelReport (const string& name)
{
  output_filename = name;
}


bool Calibration::ReceptionModelReport::report (ReceptionModel* model)
{
  ofstream os ( output_filename.c_str() );
  if (!os)
    throw Error( FailedSys, "Calibration::ReceptionModel::Report::report",
		 "ofstream (" + output_filename + ")" );
  
  unsigned ndat = model->get_ndata ();

  Stokes<double> total_chisq = 0;
  unsigned total_count = 0;

  for (unsigned input = 0; input < model->get_num_input(); input++)
  {
    model->set_input_index (input);

    Stokes<double> chisq = 0;
    unsigned count = 0;

    for (unsigned idat=0; idat < ndat; idat++)
    {
      // get the specified CoherencyMeasurementSet
      const Calibration::CoherencyMeasurementSet& data 
	= model->get_data (idat);
    
      // set the independent variables for this observation
      data.set_coordinates();
      model->set_transformation_index( data.get_transformation_index() );
    
      unsigned mstate = data.size();
    
      for (unsigned jstate=0; jstate<mstate; jstate++)
      {
	if (data[jstate].get_input_index() != input)
	  continue;
      
	Stokes< Estimate<float> > datum = data[jstate].get_stokes();
      
	Stokes<double> ms = coherency( model->evaluate() );

#ifdef _DEBUG
	if (input == 0)
	  cerr << "model=" << ms << "\ndata=" << datum << endl;
#endif

        // report above 10 sigma
	double max_divergence = 100.0;

	for (unsigned ipol=0; ipol<4; ipol++)
	{
	  double diff = datum[ipol].get_value() - ms[ipol];
	  double divergence = diff * diff / datum[ipol].get_variance();

	  if (divergence > max_divergence)
	    os << "divergence[" << ipol << "]=" << sqrt(divergence) << " "
	       << data.get_identifier() << endl;

	  chisq[ipol] += divergence;
	}

	count ++;

      }

    }

    total_chisq += chisq;
    total_count += count;

    os << input << " " << count;

    for (unsigned ipol=0; ipol<4; ipol++)
      os << " " << chisq[ipol]/count;

    os << endl;
    
  }

  os << "TOTAL " << total_count;

  for (unsigned ipol=0; ipol<4; ipol++)
    os << " " << total_chisq[ipol]/total_count;
  
  os << endl;

  return true;
}

