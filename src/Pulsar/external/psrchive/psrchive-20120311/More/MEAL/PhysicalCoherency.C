/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/PhysicalCoherency.h"
#include "MEAL/Parameters.h"
#include "Pauli.h"
#include "Error.h"

using namespace std;

MEAL::PhysicalCoherency::PhysicalCoherency ()
{
  Parameters* parameters = new Parameters (this, 4);

  parameters->set_name (0, "log_Inv");
  parameters->set_name (1, "StokesQ");
  parameters->set_name (2, "StokesU");
  parameters->set_name (3, "StokesV");
}

//! Return the name of the class
string MEAL::PhysicalCoherency::get_name () const
{
  return "PhysicalCoherency";
}


//! Calculate the Jones matrix and its gradient
void MEAL::PhysicalCoherency::calculate (Jones<double>& result,
					 std::vector<Jones<double> >* grad)
{

  Stokes<double> stokes;

  for (unsigned i=1; i<4; i++)
    stokes[i] = get_param (i);

  // b = param[0] = log(invariant)
  double invariant = exp(get_param(0));

  // I = sqrt (p^2 + invariant)
  stokes[0] = sqrt( stokes.sqr_vect() + invariant );

  // 1/I
  double one_over_I = 1.0/stokes[0];

  result = convert(stokes);

  if (grad) {

    grad->resize (4);

    // drho/dI
    Stokes<double> unpol (1,0,0,0);
    Jones<double> drho_dI = convert (unpol);

    // dI/db
    double dI_db = 0.5 * invariant * one_over_I;

    // drho/db = drho/dI * dI/db
    (*grad)[0] = drho_dI * dI_db;

    for (unsigned i=1; i<4; i++) {

      Stokes<double> param;
      param[i] = 1.0;
      (*grad)[i] = convert (param);

      // drho/dSi += drho/dI * dI/dSi
      (*grad)[i] += drho_dI * stokes[i] * one_over_I;

    }

  }

  if (verbose) {
    cerr << "MEAL::PhysicalCoherency::get_stokes result\n"
      "   " << result << endl;
    if (grad) {
      cerr << "MEAL::PhysicalCoherency::get_stokes gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}

//! Set the Stokes parameters of the model
void MEAL::PhysicalCoherency::set_stokes (const Stokes<double>& stokes)
{
  double invariant = stokes.invariant();

  if (invariant < 0)
    throw Error (InvalidParam, "MEAL::PhysicalCoherency::set_stokes",
		 "invariant = %lf < 0", invariant);

  set_param (0, log(invariant));

  for (unsigned i=1; i<4; i++)
    set_param (i, stokes[i]);
}

//! Set the Stokes parameters of the model
void MEAL::PhysicalCoherency::set_stokes (const Stokes<Estimate<double> >& s)
{
  Estimate<double> invariant = s.invariant();
  if (invariant.val < 0)
    throw Error (InvalidParam, "MEAL::PhysicalCoherency::set_stokes",
		 "invariant = %lf < 0", invariant.val);

  set_Estimate (0, log(invariant));

  for (unsigned i=1; i<4; i++)
    set_Estimate (i, s[i]);
}

//! Set the Stokes parameters of the model
Stokes< Estimate<double> > MEAL::PhysicalCoherency::get_stokes () const
{
  Stokes< Estimate<double> > stokes;
  for (unsigned i=1; i<4; i++)
    stokes[i] = get_Estimate (i);

  stokes[0] = sqrt( stokes.sqr_vect() + exp(get_Estimate(0)) );

  return stokes;
}
