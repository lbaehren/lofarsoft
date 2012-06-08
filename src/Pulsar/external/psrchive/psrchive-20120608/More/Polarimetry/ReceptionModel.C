/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModel.h"
#include "Pulsar/CoherencyMeasurementSet.h"
#include "Pulsar/ReceptionModelSolveMEAL.h"

#include <assert.h>

using namespace std;

// #define _DEBUG 1

Calibration::ReceptionModel::ReceptionModel ()
{
  set_solver( new SolveMEAL );
}

Calibration::ReceptionModel::~ReceptionModel ()
{
#ifdef _DEBUG
  cerr << "Calibration::ReceptionModel::~ReceptionModel" << endl;
#endif
}

//! Return the name of the class
string Calibration::ReceptionModel::get_name () const
{
  return "ReceptionModel";
}

//! Set the algorithm used to solve the measurement equation
void Calibration::ReceptionModel::set_solver (Solver* s)
{
  solver = s;
  if (solver)
    solver->set_equation (this);
}

//! Get the algorithm used to solve the measurement equation
Calibration::ReceptionModel::Solver*
Calibration::ReceptionModel::get_solver ()
{
  return solver;
}

//! Get the algorithm used to solve the measurement equation
const Calibration::ReceptionModel::Solver*
Calibration::ReceptionModel::get_solver () const
{
  return solver;
}

//! Solve the measurement equation using the current algorithm
void Calibration::ReceptionModel::solve ()
{
  solver->solve();
}

//! Return true when solved
bool Calibration::ReceptionModel::get_solved () const
{
  return solver->get_solved();
}

void Calibration::ReceptionModel::copy_fit (const ReceptionModel* other)
{
  if (get_nparam() != other->get_nparam())
    throw Error (InvalidParam, "Calibration::ReceptionModel::copy_fit",
		 "this.nparam=%d != copy.nparam=%d",
		 get_nparam(), other->get_nparam());

  for (unsigned iparm=0; iparm < get_nparam(); iparm++)
  {
    if (get_infit(iparm) != other->get_infit(iparm))
      throw Error (InvalidParam, "Calibration::ReceptionModel::copy_fit",
		   "this.infit[%d]=%d != copy.infit[%d]=%d",
		   iparm, get_infit(iparm), iparm, other->get_infit(iparm));

    set_param (iparm, other->get_param(iparm));
  }
}

//! Checks that each MeasuredState has a valid source_index
void Calibration::ReceptionModel::add_data (CoherencyMeasurementSet& datum)
{
  if (datum.get_transformation_index() >= get_num_transformation())
      throw Error (InvalidParam,
		   "Calibration::ReceptionModel::add_data",
		   "path_index=%d >= npath=%d",
		   datum.get_transformation_index(), get_num_transformation());
 
  for (unsigned imeas=0; imeas<datum.size(); imeas++)  {

    if (datum[imeas].get_input_index() >= get_num_input())
      throw Error (InvalidParam,
		   "Calibration::ReceptionModel::add_data",
		   "source_index=%d >= nsource=%d",
		   datum[imeas].get_input_index(), get_num_input());

#if 0
    for (unsigned ipol=0; ipol<4; ipol++)
      if (datum[imeas].get_variance(ipol) <= 0.0) {
	Error error (InvalidParam, "Calibration::ReceptionModel::add_data");
	error << "source_index=" << datum[imeas].get_input_index()
	      << " ipol=" << ipol 
	      << " variance=" << datum[imeas].get_variance(ipol) << " <= 0";
        if (verbose)
          error << "\nstokes=" << datum[imeas].get_stokes();
        throw error;
      }
#endif

  }

  data.push_back (datum);
}

void Calibration::ReceptionModel::delete_data ()
{
  data.resize (0);
}

//! Get the number of CoherencyMeasurementSet
unsigned Calibration::ReceptionModel::get_ndata () const
{
  return data.size();
}

//! Get the specified CoherencyMeasurementSet
const Calibration::CoherencyMeasurementSet&
Calibration::ReceptionModel::get_data (unsigned idata) const
{
  range_check (idata, "Calibration::ReceptionModel::get_data");
  return data[idata];
}

void Calibration::ReceptionModel::range_check (unsigned idata, 
					       const char* method) const
{
  if (idata >= data.size())
    throw Error (InvalidRange, method, "idata=%d >= ndata=%d",
		 idata, data.size());
}

