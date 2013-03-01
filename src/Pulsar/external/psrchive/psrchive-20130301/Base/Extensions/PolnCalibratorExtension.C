/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorType.h"
#include "templates.h"

#include <string.h>
#include <assert.h>

using namespace std;
using namespace Pulsar;

//! Default constructor
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension ()
  : CalibratorExtension ("PolnCalibratorExtension")
{
  init ();
}

void Pulsar::PolnCalibratorExtension::init ()
{
  nparam = 0;
  has_covariance = false;
  has_solver = false;
}

//! Copy constructor
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension
(const PolnCalibratorExtension& copy)
  : CalibratorExtension (copy)
{
  operator = (copy);
}

//! Operator =
const Pulsar::PolnCalibratorExtension&
Pulsar::PolnCalibratorExtension::operator= 
(const PolnCalibratorExtension& copy)
{
  if (this == &copy)
    return *this;

  if (Archive::verbose == 3)
    cerr << "Pulsar::PolnCalibratorExtension::operator=" << endl;

  type = copy.get_type();
  epoch = copy.get_epoch();
  nparam = copy.get_nparam();
  has_covariance = copy.get_has_covariance();
  has_solver = copy.get_has_solver();

  unsigned nchan = copy.get_nchan();
  set_nchan (nchan);

  for (unsigned ichan = 0; ichan < nchan; ichan++)
    response[ichan] = copy.response[ichan];

  return *this;
}

//! Destructor
Pulsar::PolnCalibratorExtension::~PolnCalibratorExtension ()
{
}

//! Set the type of the instrumental response parameterization
void Pulsar::PolnCalibratorExtension::set_type (const Calibrator::Type* _type)
{
  type = _type;
  nparam = type->get_nparam();
}


//! Set the number of frequency channels
void Pulsar::PolnCalibratorExtension::set_nchan (unsigned _nchan)
{
  CalibratorExtension::set_nchan( _nchan );
  response.resize( _nchan );
  construct ();
}

void PolnCalibratorExtension::remove_chan (unsigned first, unsigned last)
{
  CalibratorExtension::remove_chan (first, last);
  remove (response, first, last);
}

//! Set the weight of the specified channel
void Pulsar::PolnCalibratorExtension::set_weight (unsigned ichan, float weight)
{
  set_valid (ichan, weight != 0.0);
}

//! Set the weight of the specified channel
float Pulsar::PolnCalibratorExtension::get_weight (unsigned ichan) const
{
  if (get_valid (ichan))
    return 1.0;
  else
    return 0.0;
}

//! Get the weight of the specified channel

bool Pulsar::PolnCalibratorExtension::get_valid (unsigned ichan) const
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_valid");
  return response[ichan].get_valid();
}

void Pulsar::PolnCalibratorExtension::set_valid (unsigned ichan, bool valid)
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::set_valid");

  if (!valid)
    weight[ichan] = 0;
  else
    weight[ichan] = 1.0;

  response[ichan].set_valid (valid);
}

unsigned Pulsar::PolnCalibratorExtension::get_nparam () const
{
  return nparam;
}

bool Pulsar::PolnCalibratorExtension::get_has_covariance () const
{
  return has_covariance;
}

void Pulsar::PolnCalibratorExtension::set_has_covariance (bool has)
{
  has_covariance = has;
}

//! Get if the covariances of the transformation parameters
bool Pulsar::PolnCalibratorExtension::get_has_solver () const
{
  return has_solver;
}

//! Set if the covariances of the transformation parameters
void Pulsar::PolnCalibratorExtension::set_has_solver (bool has)
{
  has_solver = has;
}

//! Get the transformation for the specified frequency channel
Pulsar::PolnCalibratorExtension::Transformation* 
Pulsar::PolnCalibratorExtension::get_transformation (unsigned ichan)
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_transformation");
  return &response[ichan];
}

//! Get the transformation for the specified frequency channel
const Pulsar::PolnCalibratorExtension::Transformation*
Pulsar::PolnCalibratorExtension::get_transformation (unsigned ichan) const
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_transformation");
  return &response[ichan];
}

void Pulsar::PolnCalibratorExtension::construct ()
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::PolnCalibratorExtension::construct nchan="
         << response.size() << " type=" << get_type()->get_name() << endl;

  for (unsigned ichan=0; ichan<response.size(); ichan++)
  {
    response[ichan].set_nparam (nparam);
    weight[ichan] = 1.0;
  }
}

PolnCalibratorExtension::Transformation::Transformation ()
{
  valid = true;
  chisq = 0.0;
  nfree = 0;
}

unsigned
PolnCalibratorExtension::Transformation::get_nparam() const
{
  return params.size();
}

//! Get the name of the specified model parameter
string
PolnCalibratorExtension::Transformation::get_param_name (unsigned i) const
{
  return names[i];
}

//! Set the name of the specified model parameter
void
PolnCalibratorExtension::Transformation::set_param_name (unsigned i,
							 const string& n)
{
  names[i] = n;
}

//! Get the description of the specified model parameter
string
PolnCalibratorExtension::Transformation::get_param_description (unsigned i)
  const
{
  return descriptions[i];
}

//! Set the description of the specified model parameter
void
PolnCalibratorExtension::Transformation::set_param_description 
(unsigned i, const string& n)
{
  descriptions[i] = n;
}

void PolnCalibratorExtension::Transformation::set_nparam (unsigned s)
{
  params.resize(s);
  names.resize(s);
  descriptions.resize(s);
}

double 
PolnCalibratorExtension::Transformation::get_param (unsigned i) const
{
  return params[i].get_value();
}

void PolnCalibratorExtension::Transformation::set_param 
(unsigned i, double value)
{
  params[i].set_value(value);
}

double
PolnCalibratorExtension::Transformation::get_variance (unsigned i) 
const
{
  return params[i].get_variance();
}

void PolnCalibratorExtension::Transformation::set_variance
(unsigned i, double var)
{
  params[i].set_variance(var);
}

Estimate<double>
PolnCalibratorExtension::Transformation::get_Estimate (unsigned i) const
{
  return params[i];
}

void PolnCalibratorExtension::Transformation::set_Estimate
(unsigned i, const Estimate<double>& e)
{
  params[i] = e;
}

bool PolnCalibratorExtension::Transformation::get_valid () const
{
  return valid;
}

void PolnCalibratorExtension::Transformation::set_valid (bool flag)
{
  valid = flag;
}

double PolnCalibratorExtension::Transformation::get_chisq () const
{
  return chisq;
}

void PolnCalibratorExtension::Transformation::set_chisq (double c)
{
  chisq = c;
}

unsigned PolnCalibratorExtension::Transformation::get_nfree() const
{
  return nfree;
}

void PolnCalibratorExtension::Transformation::set_nfree (unsigned n)
{
  nfree = n;
}

//! Get the covariance matrix of the model paramters
vector< vector<double> >
PolnCalibratorExtension::Transformation::get_covariance () const
{
  unsigned nparam = get_nparam();

  unsigned size = nparam * (nparam+1) / 2;
  if (size != covariance.size())
    throw Error (InvalidState,
		 "PolnCalibratorExtension::Transformation::get_covariance",
		 "covariance vector has incorrect length = %u (expect %u)",
		 covariance.size(), size);

  vector<vector<double> > matrix (nparam, vector<double>(nparam));

  unsigned count = 0;
  for (unsigned i=0; i<nparam; i++)
    for (unsigned j=i; j<nparam; j++)
    {
      matrix[i][j] = matrix[j][i] = covariance[count];
      count ++;
    }

  assert (count == covariance.size());

  return matrix;
}

//! Set the covariance matrix of the model paramters
void PolnCalibratorExtension::Transformation::set_covariance 
(const vector< vector<double> >& covar)
{
  unsigned nparam = get_nparam();

  assert (nparam == covar.size());

  covariance.resize( nparam * (nparam+1) / 2 );

  unsigned count = 0;
  for (unsigned i=0; i<nparam; i++)
  {
    assert (nparam == covar[i].size());
    for (unsigned j=i; j<nparam; j++)
    {
      covariance[count] = covar[i][j];
      count ++;
    }
  }

  assert (count == covariance.size());
}

//! Get the covariance matrix efficiently
void PolnCalibratorExtension::Transformation::get_covariance 
(vector<double>& covar) const
{
  covar = covariance;
}

//! Set the covariance matrix efficiently
void PolnCalibratorExtension::Transformation::set_covariance
(const vector<double>& covar)
{
  covariance = covar;

  if (covar.size() == 0) {
    valid = false;
    return;
  }

  unsigned nparam = get_nparam();
  unsigned expect = nparam * (nparam+1) / 2;
  if (covar.size() != expect)
    throw Error (InvalidParam,
		 "PolnCalibratorExtension::Transformation::set_covariance",
		 "covariance vector length=%u != expected=%u=%u*(%u+1)/2",
		 covar.size(), expect, nparam, nparam);

  unsigned icovar = 0;

  // set the variance stored in the transformation
  for (unsigned i=0; i<nparam; i++)
    for (unsigned j=i; j<nparam; j++)
    {
      if (i==j)
      {
#ifdef _DEBUG
	cerr << j << " " << covar[icovar] << endl;
#endif
	set_variance (j,covar[icovar]);
      }
      icovar++;
    }

  if (icovar != covar.size())
    throw Error (InvalidState,
		 "PolnCalibratorExtension::Transformation::set_covariance",
		 "covariance vector length=%u != icovar=%u",
		 covar.size(), icovar);

}

//! Get the text interface 
TextInterface::Parser* PolnCalibratorExtension::get_interface()
{
  return new Interface( this );
}
