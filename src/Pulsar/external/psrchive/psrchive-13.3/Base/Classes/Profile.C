/***************************************************************************
 *
 *   Copyright (C) 2002-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Pulsar/Profile.h"
#include "Pulsar/DataExtension.h"

#include "FTransform.h"
#include "Physical.h"
#include "Error.h"
#include "typeutil.h"

#include <iostream>
#include <string>

#include <math.h>

using namespace std;

/*! 
  Default fractional pulse phase window used to calculate statistics
  related to the baseline.
 */
Pulsar::Option<float>
Pulsar::Profile::default_duty_cycle 
(
 "Profile::default_duty_cycle", 0.15,

 "Duty cycle of baseline [turns]",

 "The default width of the window used to find the off-pulse baseline\n"
 "and compute its statistics"
);

/*!  
  When true, Profile methods will output debugging information on cerr
*/
bool Pulsar::Profile::verbose = false;



/////////////////////////////////////////////////////////////////////////////
//
// nbinify - utility for correcting the indeces of a range
//
void nbinify (int& istart, int& iend, int nbin)
{
  if (istart < 0)
    istart += ( -istart/nbin + 1 ) * nbin;

  if (iend <= istart)
    iend += ( (istart-iend)/nbin + 1 ) * nbin;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::init
//
void Pulsar::Profile::init()
{
  weight = 1.0;
  centrefreq = -1.0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile default constructor
//
Pulsar::Profile::Profile (unsigned nbin) : ProfileAmps (nbin)
{
  init();
  if (!no_amps && nbin)
    zero ();
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::clone
//
Pulsar::Profile* Pulsar::Profile::clone () const
{
  Profile* retval = new Profile (*this);
  if (!retval)
    throw Error (BadAllocation, "Pulsar::Profile::clone");
  return retval;
}

void Pulsar::Profile::resize (unsigned new_nbin)
{
  ProfileAmps::resize (new_nbin);
  foreach<DataExtension> (this, &DataExtension::resize, new_nbin);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator =
//
/*!
  Sets all attributes of this Profile equal to that of the input Profile,
  resizes and copies the amps array.
*/
const Pulsar::Profile& Pulsar::Profile::operator = (const Profile& input)
{
  if (this == &input)
    return *this;

  try
  {
    resize( input.get_nbin() );
    set_amps( input.get_amps() );

    set_weight ( input.get_weight() );
    set_centre_frequency ( input.get_centre_frequency() );

    // copy the extensions
    extension.resize (0);
    for (unsigned iext=0; iext < input.get_nextension(); iext++)
      add_extension( input.get_extension(iext)->clone() );
  }
  catch (Error& error)
  {
    throw error += "Pulsar::Profile::operator =";
  }

  return *this;
}

//! set the weight of the profile
void Pulsar::Profile::set_weight (float w)
{
  weight = w;
  foreach<DataExtension> (this, &DataExtension::set_weight, weight);
}

//! Return the number of extensions available
unsigned Pulsar::Profile::get_nextension () const
{
  clean_dangling (extension);
  return extension.size ();
}

Pulsar::Profile::Extension::Extension (const char* _name)
{
  extension_name = _name;
}

Pulsar::Profile::Extension::~Extension ()
{}

string Pulsar::Profile::Extension::get_extension_name () const
{
  return extension_name;
}

string Pulsar::Profile::Extension::get_short_name () const
{
  return extension_name;
}

/*! Derived classes need only define this method, as the non-const version
  implemented by the Profile base class simply calls this method. */
const Pulsar::Profile::Extension*
Pulsar::Profile::get_extension (unsigned iext) const
{
  if ( iext >= extension.size() )
    throw Error (InvalidRange, "Pulsar::Profile::get_extension",
                 "index=%d >= nextension=%d", iext, extension.size());

  if ( !extension[iext] )
    return 0;

  return extension[iext];
}

/*! Simply calls get_extension const */
Pulsar::Profile::Extension*
Pulsar::Profile::get_extension (unsigned iext)
{
  if ( iext >= extension.size() )
    throw Error (InvalidRange, "Pulsar::Profile::get_extension",
                 "index=%d >= nextension=%d", iext, extension.size());

  if ( !extension[iext] )
    return 0;

  return extension[iext];
}

/*! Derived classes need only define this method, as the non-const version
  implemented by the Profile base class simply calls this method. */
void Pulsar::Profile::add_extension (Extension* ext)
{
  unsigned index = find( extension, ext );

  if (index < extension.size())
  {
    if (verbose)
      cerr << "Pulsar::Profile::add_extension replacing" << endl;
    extension[index] = ext;
  }
  else
  {
    if (verbose)
      cerr << "Pulsar::Profile::add_extension appending" << endl;
    extension.push_back(ext);
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator +=
//
const Pulsar::Profile& Pulsar::Profile::operator += (float factor)
{
  offset (factor);
  return *this;
}
 
/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator -=
//
const Pulsar::Profile& Pulsar::Profile::operator -= (float factor)
{
  offset (-factor);
  return *this;
}
 
/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::operator *=
//
const Pulsar::Profile& Pulsar::Profile::operator *= (float factor)
{
  scale (factor);
  return *this;
}

void Pulsar::Profile::offset (double factor)
{
  const unsigned nbin = get_nbin();
  float* amps = get_amps();
  for (unsigned i=0;i<nbin;i++)
    amps[i] += factor;

  foreach<DataExtension> (this, &DataExtension::offset, factor);
}

void Pulsar::Profile::scale (double factor)
{
  const unsigned nbin = get_nbin();
  float* amps = get_amps();
  for (unsigned i=0;i<nbin;i++)
    amps[i] *= factor;

  foreach<DataExtension> (this, &DataExtension::scale, factor);
}

void sumdiff (Pulsar::Profile* thiz, const Pulsar::Profile* that, float factor)
{
  unsigned nbin = thiz->get_nbin();

  if (nbin != that->get_nbin())
    throw Error (InvalidParam, "Pulsar::Profile::sumdiff",
		 "nbin=%u != other nbin=%u", nbin, that->get_nbin());

  float* amps = thiz->get_amps();
  const float* pamps = that->get_amps();
  
  for (unsigned i=0;i<nbin;i++)
    amps[i] += factor * pamps[i];
}

void Pulsar::Profile::sum (const Profile* that)
{
  sumdiff (this, that, 1);
}

void Pulsar::Profile::diff (const Profile* that)
{
  sumdiff (this, that, -1);
}

vector<float> Pulsar::Profile::get_weighted_amps () const
{
  vector<float> wamps;
  
  unsigned nbin = get_nbin();
  const float* amps = get_amps();

  if (weight == 0.0)
    for (unsigned i = 0; i < nbin; i++)
      wamps.push_back(0.0);
  else
    for (unsigned i = 0; i < nbin; i++)
      wamps.push_back(amps[i]);
  
  return wamps;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::zero
//
void Pulsar::Profile::zero()
{
  unsigned nbin = get_nbin();
  float* amps = get_amps();

  weight = 0;
  for (unsigned ibin = 0; ibin < nbin; ibin++)
    amps[ibin] = 0;

  foreach<DataExtension> (this, &DataExtension::zero);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::square_root
//
void Pulsar::Profile::square_root ()
{
  if (verbose)
    cerr << "Pulsar::Profile::square_root" << endl;
  
  unsigned nbin = get_nbin();
  float* amps = get_amps();

  for (unsigned ibin=0; ibin<nbin; ++ibin) {
    float sign = (amps[ibin]>0) ? 1.0 : -1.0;
    amps[ibin] = sign * sqrt(float(sign * amps[ibin]));
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::absolute
//
void Pulsar::Profile::absolute ()
{
  if (verbose)
    cerr << "Pulsar::Profile::absolute" << endl;
  
  unsigned nbin = get_nbin();
  float* amps = get_amps();

  for (unsigned ibin=0; ibin<nbin; ++ibin)
    amps[ibin] = fabs(amps[ibin]);

}

//! calculate the logarithm of each bin with value greater than threshold
void Pulsar::Profile::logarithm (double base, double threshold)
{
  if (verbose)
    cerr << "Pulsar::Profile::logarithm base=" << base 
	 << " threshold=" << threshold << endl;
  
  float log_threshold = log(threshold)/log(base);

  if (!finite(log_threshold))
    throw Error (InvalidParam, "Pulsar::Profile::logarithm",
		 "logarithm of threshold=%lf is not finite", threshold);

  unsigned nbin = get_nbin();
  float* amps = get_amps();

  for (unsigned ibin=0; ibin<nbin; ++ibin)
  {
    if (amps[ibin] > threshold)
      amps[ibin] = log(amps[ibin])/log(base);
    else
      amps[ibin] = log_threshold;
    if (!finite(amps[ibin]))
      throw Error (InvalidParam, "Pulsar::Profile::logarithm",
		   "logarithm of amps[%u]=%lf is not finite",
		   ibin, amps[ibin]);
  }
}

void Pulsar::Profile::pscrunch ()
{
  foreach<DataExtension> (this, &DataExtension::pscrunch);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::fold
//
void Pulsar::Profile::fold (unsigned nfold)
{
  if (verbose)
    cerr << "Pulsar::Profile::fold" << endl;
  
  unsigned nbin = get_nbin();
  float* amps = get_amps();

  if (nbin % nfold)
    throw Error (InvalidRange, "Pulsar::Profile::fold",
		 "nbin=%d %% nfold=%d != 0", nbin, nfold);
  
  unsigned newbin = nbin/nfold;
  float scale = 1.0/nfold;

  for (unsigned i=0; i<newbin; i++)
  {
    for (unsigned j=1; j<nfold; j++)
      amps[i] += amps[i+j*newbin];

    amps[i] *= scale;
  }

  foreach<DataExtension> (this, &DataExtension::fold, nfold);

  // note that ProfileAmps::resize will not lose data when newbin < nbin
  resize (newbin);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::bscrunch
//
void Pulsar::Profile::bscrunch (unsigned nscrunch) try
{
  if (verbose)
  {
    cerr << "Pulsar::Profile::bscrunch" << endl;
    cerr << "  Current nbin   = " << get_nbin() << endl;
    cerr << "  Scrunch factor = " << nscrunch << endl;
  }
  
  if (nscrunch < 2)
    throw Error (InvalidParam, "",
		 "nscrunch cannot be less than two");
  
  unsigned nbin = get_nbin();
  float* amps = get_amps();

  if (nbin % nscrunch)
    throw Error (InvalidRange, "",
		 "Scrunch factor does not divide number of bins");
  
  unsigned newbin = nbin/nscrunch;
  float scale = 1.0/nscrunch;

  for (unsigned i=0; i<newbin; i++)
  {
    amps[i] = amps[i*nscrunch];
    for (unsigned j=1; j<nscrunch; j++)
      amps[i] += amps[i*nscrunch+j];

    amps[i] *= scale;
  }

  foreach<DataExtension> (this, &DataExtension::bscrunch, nscrunch);

  // note that ProfileAmps::resize will not lose data when newbin < nbin
  resize (newbin);
}
catch (Error& error)
{
  throw error += "Pulsar::Profile::bscrunch";
}

void Pulsar::Profile::bscrunch_to_nbin (unsigned new_nbin) try
{
  if (new_nbin == get_nbin())
    return;

  if (new_nbin == 0)
    throw Error (InvalidParam, "",
		 "new nbin cannot be zero");
  
  else if (get_nbin() < new_nbin)
    throw Error (InvalidParam, "",
		 "current nbin=%u is less than new nbin=%u",
		 get_nbin(), new_nbin);

  else if (get_nbin() % new_nbin == 0)
    bscrunch(get_nbin() / new_nbin);

  else
  {
    foreach<DataExtension> (this, &DataExtension::bscrunch_to_nbin, new_nbin);

    unsigned orig_nbin = get_nbin();

    vector<float> temp( orig_nbin+2 );
    FTransform::frc1d (orig_nbin, &temp[0], get_amps());

    vector<float> solution (new_nbin);
    temp[new_nbin+1] = 0.0; // real-valued Nyquist

    FTransform::bcr1d (new_nbin, &solution[0], &temp[0]);

    set_amps (solution);

    if (FTransform::get_norm() == FTransform::unnormalized)
      scale( 1.0 / orig_nbin );
  }
}
catch (Error& error)
{
  throw error += "Pulsar::Profile::bscrunch_to_nbin";
}

/////////////////////////////////////////////////////////////////////////////
//
// minmax - worker function for Pulsar::Profile::<bin_>[min|max]
//
void minmax (int nbin, const float* amps, int* mi, float* mv, bool max,
	     int istart, int iend)
{
  nbinify (istart, iend, nbin);

  float val = 0;

  float best = amps[istart%nbin];
  int ibest = istart;

  for (int ibin=istart+1; ibin < iend; ibin++) {
    val = amps[ibin%nbin];
    if ( (max && val > best) || (!max && val < best) ) {
      best = val;
      ibest = ibin;
    }
  }
  if (mi)
    *mi = ibest;
  if (mv)
    *mv = best;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_max_bin
//
int Pulsar::Profile::find_max_bin (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::find_max_bin" << endl;
  
  int imax=0;
  minmax (get_nbin(), get_amps(), &imax, 0, true, istart, iend);
  return imax;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_min_bin
//
int Pulsar::Profile::find_min_bin (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::find_min_bin" << endl;
  
  int imin=0;
  minmax (get_nbin(), get_amps(), &imin, 0, false, istart, iend);
  return imin;
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::max
//
float Pulsar::Profile::max (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::max" << endl;
  
  float maxval=0;
  minmax (get_nbin(), get_amps(), 0, &maxval, true, istart, iend);
  return maxval;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::min
//
float Pulsar::Profile::min (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::min" << endl;
  
  float minval=0;
  minmax (get_nbin(), get_amps(), 0, &minval, false, istart, iend);
  return minval;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::sum
//
double Pulsar::Profile::sum (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::sum" << endl;

  unsigned nbin = get_nbin();
  const float* amps = get_amps();

  nbinify (istart, iend, nbin);

  double tot = 0;
  
  for (int ibin=istart; ibin < iend; ibin++)
    tot += (double) amps[ibin%nbin];

  return tot;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::sumfabs
//
double Pulsar::Profile::sumfabs (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::sum" << endl;

  unsigned nbin = get_nbin();
  const float* amps = get_amps();

  nbinify (istart, iend, nbin);

  double tot = 0;
  for (int ibin=istart; ibin < iend; ibin++)
    tot += fabs ((double) amps[ibin%nbin]);

  return tot;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::sumsq
//
double Pulsar::Profile::sumsq (int istart, int iend) const
{
  if (verbose)
    cerr << "Pulsar::Profile::sumsq" << endl;
  
  unsigned nbin = get_nbin();
  const float* amps = get_amps();

  nbinify (istart, iend, nbin);

  double tot = 0;
  for (int ibin=istart; ibin < iend; ibin++)  {
    double val = amps[ibin%nbin];
    tot += val * val;
  }

  return tot;
}

