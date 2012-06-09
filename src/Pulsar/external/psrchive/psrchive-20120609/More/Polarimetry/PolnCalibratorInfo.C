/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/InstrumentInfo.h"
#include "Pulsar/BrittonInfo.h"

using namespace std;

Pulsar::PolnCalibrator::Info* 
Pulsar::PolnCalibrator::Info::create (const Pulsar::PolnCalibrator* calibrator)
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::Info::create type="
	 << calibrator->get_type()->get_name() << endl;

  if (calibrator->get_type()->is_a<CalibratorTypes::SingleAxis>())
    return new SingleAxisCalibrator::Info (calibrator);

  if (calibrator->get_type()->is_a<CalibratorTypes::van02_EqA1>() ||
      calibrator->get_type()->is_a<CalibratorTypes::van09_Eq>())
    return new PolarCalibrator::Info (calibrator);

  if (calibrator->get_type()->is_a<CalibratorTypes::van04_Eq18>())
    return new InstrumentInfo (calibrator);

  if (calibrator->get_type()->is_a<CalibratorTypes::bri00_Eq19>())
    return new BrittonInfo (calibrator);

  return new PolnCalibrator::Info (calibrator);
}

//! Constructor
Pulsar::PolnCalibrator::Info::Info (const PolnCalibrator* cal)
{
  nparam = 0;

  if (!cal)
    return;

  calibrator = cal;
  
  unsigned nchan = cal->get_nchan ();

  // find the first valid transformation
  const MEAL::Complex2* xform = 0;
  for (unsigned ichan = 0; ichan < nchan; ichan++)
    if ( cal->get_transformation_valid (ichan) )
    {
      xform = cal->get_transformation (ichan);
      break;
    }

  if (!xform)
    return;

  nparam = xform->get_nparam();
}

unsigned Pulsar::PolnCalibrator::Info::get_nchan () const
{
  return calibrator->get_nchan();
}

/*! Each parameter of the Transformation is treated as a separate class. */
unsigned Pulsar::PolnCalibrator::Info::get_nclass () const
{
  return nparam;
}

/*! The name of each parameter is unknown */
string Pulsar::PolnCalibrator::Info::get_title () const
{
  return "Unknown Calibrator Parameters";
}

/*! The name of each parameter is unknown */
string Pulsar::PolnCalibrator::Info::get_name (unsigned iclass) const
{
  return "unknown";
}

/*! Each parameter of the Transformation is treated as a separate class. */
unsigned Pulsar::PolnCalibrator::Info::get_nparam (unsigned iclass) const
{
  return 1;
}
      
//! Return the estimate of the specified parameter
Estimate<float> Pulsar::PolnCalibrator::Info::get_param (unsigned ichan, 
							 unsigned iclass,
							 unsigned iparam) const
{
  if (! calibrator->get_transformation_valid(ichan) )
  {
    if (verbose > 2) cerr << "Pulsar::PolnCalibrator::Info::get_param"
		   " invalid ichan=" << ichan << endl;
    return 0;
  }

  unsigned offset = 0;
  for (unsigned jclass=1; jclass<=iclass; jclass++)
    offset += get_nparam (jclass-1);

  return calibrator->get_transformation(ichan)->get_Estimate(iparam+offset);
}


//! Return the colour index
int Pulsar::PolnCalibrator::Info::get_colour_index (unsigned iclass,
						    unsigned iparam) const
{
  unsigned colour_offset = 1;
  if (get_nparam (iclass) == 3)
    colour_offset = 2;

  return colour_offset + iparam;
}

//! Return the graph marker
int Pulsar::PolnCalibrator::Info::get_graph_marker (unsigned iclass, 
						    unsigned iparam) const
{
  if (iparam == 0 || iparam > 3)
    return -1;

  iparam --;

  int nice_markers[3] = { 2, 5, 4 };

  return nice_markers[iparam];
}
