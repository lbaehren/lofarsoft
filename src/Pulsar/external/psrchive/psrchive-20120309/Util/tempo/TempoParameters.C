/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "psrParams.h"
#include "psrParameter.h"
#include "psrephem.h"
#include "ephio.h"
#include "strutil.h"

#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

// //////////////////////////////////////////////////////////////////////////
//
// STATIC MEMBERS
//
// //////////////////////////////////////////////////////////////////////////

bool  psrParams::verbose = false;

void psrParams::get_psrephem (psrephem& eph)
{
  eph.zero ();   // clear the data

  if (verbose)
    cerr << "psrParams::get_psrephem with " << params.size() 
	 << " parameters" << endl;

  for (unsigned iparm=0; iparm < params.size(); iparm++) {
    
    psrParameter* p = params[iparm];
    int ieph = p->get_ephind();

    if (verbose)
      cerr << "psrParams::get_psrephem parm[" << iparm << "]=" << ieph << endl;

    if (p->getFit())
      eph.parmStatus[ieph] = 2;
    else
      eph.parmStatus[ieph] = 1;

    switch (parmTypes[ieph]) {

    case 0:  // other strings (JNAME, TZRSITE, etc.)
      if (verbose)
	cerr << "psrParams::get_psrephem String:" << parmNames[ieph] << endl;
      eph.value_str[ieph] = p->getString();
      break;
    
    case 1:  // any double
      if (verbose)
	cerr << "psrParams::get_psrephem Double:" << parmNames[ieph] << endl;
      eph.value_double[ieph] = p->getDouble();
      break;

    case 2:  // RAs
      if (verbose)
	cerr << "psrParams::get_psrephem Angle (RA)" << endl;
      if (p->getAngle().getRadians() < 0.0)
	eph.value_double[ieph] = (p->getAngle().getRadians() + 2.0*M_PI)/(2.0*M_PI);
      else
        eph.value_double[ieph] = p->getAngle().getRadians() / (2.0 * M_PI);
      break;
    case 3:  // DECs
      if (verbose)
	cerr << "psrParams::get_psrephem Angle :" << parmNames[ieph] << endl;
      eph.value_double[ieph] = p->getAngle().getRadians() / (2.0 * M_PI);
      break;

    case 4:  { // MJDs
      if (verbose)
	cerr << "psrParams::get_psrephem MJD   :" << parmNames[ieph] << endl;
      MJD mjd = p -> getMJD();
      eph.value_integer[ieph] = mjd.intday();
      eph.value_double[ieph]  = mjd.fracday();
      break;
    }

    case 5:  // any integer
      if (verbose)
	cerr << "psrParams::get_psrephem Int:" << parmNames[ieph] << endl;
      eph.value_integer[ieph] = p->getInteger();
      break;
      
    default:
      cerr << "psrParams::get_psrephem error unhandled parmType=" 
	   << parmTypes[ieph] << endl;
      break;
    
    }
  }
}


void psrParams::set_psrephem (const psrephem& eph)
{
  // remove any old psrParameters
  destroy ();

  // load up the new ones
  if (verbose)
    cerr << "psrParams::loading new points" << endl;

  for (int ieph=0; ieph < EPH_NUM_KEYS; ieph++) {

    psrParameter* newparm = NULL;

    if (eph.parmStatus[ieph] == 0)
      continue;

    // parmTypes defined in ephio.h
    switch (parmTypes[ieph]) {

    case 0:  // string value
      if (verbose)
	cerr << "psrParams::load new String:" << parmNames[ieph] << endl;
      newparm = new psrString (ieph, eph.value_str[ieph]);
      break;

    case 1:  // double
      if (verbose)
	cerr << "psrParams::load new Double:" << parmNames[ieph] << endl;
      newparm = new psrDouble (ieph, eph.value_double[ieph],
			       eph.error_double[ieph]);
      break;

    case 2:  // RAs
    case 3:  // DECs
      if (verbose)
	cerr << "psrParams::load new Angle :" << parmNames[ieph] << endl;
      newparm = new psrAngle (ieph, eph.value_double[ieph], 
			      eph.error_double[ieph]);
      break;

    case 4:  // MJDs
      if (verbose)
	cerr << "psrParams::load new MJD   :" << parmNames[ieph] << endl;
      newparm = new psrMJD (ieph, eph.value_integer[ieph],
			    eph.value_double[ieph],
			    eph.error_double[ieph]);
      break;
    }

    if (newparm != NULL) {
      newparm -> setFit (eph.parmStatus[ieph] == 2);
      params.push_back (newparm);
    }
  }
}

void psrParams::destroy ()
{
  for (unsigned iparm=0; iparm < params.size(); iparm++)
    delete params[iparm];
  params.erase( params.begin(), params.end() );
}

psrParams& psrParams::operator = (const psrParams & p)
{
  if (this != &p) {
    destroy();
    for (unsigned iparm=0; iparm < p.params.size(); iparm++)
      params.push_back (p.params[iparm] -> duplicate());
  }
  return *this;
}
