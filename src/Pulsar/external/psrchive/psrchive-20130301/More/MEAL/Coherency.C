/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Coherency.h"
#include "MEAL/Parameters.h"
#include "Pauli.h"
#include "Error.h"

using namespace std;

MEAL::Coherency::Coherency ()
{
  Parameters* parameters = new Parameters (this, 4);

  parameters->set_name (0, "StokesI");
  parameters->set_name (1, "StokesQ");
  parameters->set_name (2, "StokesU");
  parameters->set_name (3, "StokesV");
}

//! Return the name of the class
string MEAL::Coherency::get_name () const
{
  return "Coherency";
}

//! Calculate the Jones matrix and its gradient
void MEAL::Coherency::calculate (Jones<double>& result,
				 std::vector<Jones<double> >* grad)
{

  Stokes<double> stokes;
  for (unsigned i=0; i<4; i++)
    stokes[i] = get_param (i);

  result = convert(stokes);

  if (grad)
  {
    grad->resize (4);

    for (unsigned i=0; i<4; i++)
    {
      Stokes<double> param;
      param[i] = 1.0;
      (*grad)[i] = convert (param);
    }
  }

  if (verbose)
  {
    cerr << "MEAL::Coherency::get_stokes result\n"
      "   " << result << endl;
    if (grad)
    {
      cerr << "MEAL::Coherency::get_stokes gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}

//! Set the Stokes parameters of the model
void MEAL::Coherency::set_stokes (const Stokes<double>& stokes)
{
  for (unsigned i=0; i<4; i++)
    set_param (i, stokes[i]);
}

//! Set the Stokes parameters of the model
void MEAL::Coherency::set_stokes (const Stokes<Estimate<double> >& s)
{
  for (unsigned i=0; i<4; i++) {
    set_param (i, s[i].val);
    set_variance (i, s[i].var);
  }
}

//! Set the Stokes parameters of the model
Stokes< Estimate<double> > MEAL::Coherency::get_stokes () const
{
  Stokes< Estimate<double> > stokes;
  for (unsigned i=0; i<4; i++)
    stokes[i] = get_Estimate (i);

  return stokes;
}

void MEAL::Coherency::set_param_name_prefix (const string& prefix)
{
  Parameters* current = dynamic_kast<Parameters>(parameter_policy);
  if (!current)
    return;

  const char* iquv = "IQUV";

  for (unsigned i=0; i<4; i++)
    current->set_name (i, prefix + iquv[i]);
}

