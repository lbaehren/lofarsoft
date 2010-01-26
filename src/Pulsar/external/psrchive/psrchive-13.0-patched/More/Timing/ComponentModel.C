/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ComponentModel.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

#include "MEAL/ScaledVonMises.h"
#include "MEAL/ScaledVonMisesDeriv.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarConstant.h"

#include "MEAL/ChainRule.h"
#include "MEAL/SumRule.h"

#include "MEAL/Axis.h"
#include "MEAL/LevenbergMarquardt.h"

#include "FTransform.h"

#include <string.h>
#include <stdio.h>
#include <iomanip>

using namespace MEAL;
using namespace std;

// #define _DEBUG

void Pulsar::ComponentModel::init ()
{
  fit_derivative = false;
  threshold = 1e-3;
}

Pulsar::ComponentModel::ComponentModel ()
{
  init ();
}

Pulsar::ComponentModel::ComponentModel (const std::string& filename)
{
  init ();
  load (filename.c_str());
}

//! Return the shift estimate
Estimate<double> Pulsar::ComponentModel::get_shift () const try
{
  if (verbose)
    cerr << "Pulsar::ComponentModel::get_shift" << endl;

  if (!phase)
  {
    phase = new ScalarParameter (0.0);
    phase->set_value_name ("phase");
  }

  if (backup.size() == components.size())
  {
    if (verbose)
      cerr << "Pulsar::ComponentModel::get_shift restore backup" << endl;

    for (unsigned i=0; i<components.size(); i++)
    {
      components[i]->set_height( backup[i]->get_height() );
      components[i]->set_concentration( backup[i]->get_concentration() );
    }
  }

  try
  {
    const_cast<ComponentModel*>(this)->align (observation);
    const_cast<ComponentModel*>(this)->fit (observation);
  }
  catch (Error& error)
  {
    throw error += "Pulsar::ComponentModel::get_shift";
  }

  if (verbose)
    cerr << "Pulsar::ComponentModel::get_shift phase="
	 << phase->get_value() << " rad" << endl;

  return phase->get_Estimate(0) / (2*M_PI);
}
catch (Error& error)
{
  throw error += "Pulsar::ComponentModel::get_shift";
}

void Pulsar::ComponentModel::load (const char *fname)
{
  clear();

  FILE *f = fopen(fname, "r");
  if (!f)
    throw Error (FailedSys, "Pulsar::ComponentModel::load",
		 "fopen (%s)", fname);
  
  char line[1024];
  unsigned iline=0;

  while (fgets(line, 1023, f)!=NULL)
  {
    if (line[0]=='#') // # .. comment
      comments[iline] = line;
    else
    {
      bool allwhite=true;
      unsigned len = strlen(line);
      for (unsigned ic=0; ic < len && allwhite; ic++)
	if (!isspace(line[ic]))
	  allwhite = false;
      if (allwhite) // empty line
	comments[iline] = line;
      else
      {
	if (line[len-1]=='\n')
	  line[len-1]='\0';

	double centre, concentration, height;
	unsigned name_start;

	int scanned = sscanf (line, "%lf %lf %lf %n", 
			      &centre, &concentration, &height, &name_start);

	if (scanned !=3)
	  throw Error (InvalidState, "Pulsar::ComponentModel::load", 
		       "Could not parse file");

	components.push_back( new ScaledVonMises );
	components[components.size()-1]->set_centre(centre * 2*M_PI);
	components[components.size()-1]->set_concentration(concentration);
	components[components.size()-1]->set_height(height);
	component_names.push_back(line+name_start);
      }
    }
    iline++;
  }
  fclose(f);
}

void Pulsar::ComponentModel::unload (const char *fname) const
{
  FILE *f = fopen(fname, "w");
  if (!f)
    throw Error (FailedSys, "Pulsar::ComponentModel::unload",
		 "fopen (%s)", fname);
  
  unsigned iline, icomp = 0;
  bool done=false;
  map<unsigned, string>::const_iterator comment_it;
  for (iline=0; !done; iline++)
  {
    comment_it = comments.find(iline);
    if (comment_it!=comments.end())
      fputs(comment_it->second.c_str(), f);
    else if (icomp < components.size())
    {
      fprintf(f, "%12lg %12lg %12lg %s\n", 
	      components[icomp]->get_centre().val / (2*M_PI),
	      components[icomp]->get_concentration().val,
	      components[icomp]->get_height().val,
	      component_names[icomp].c_str());
      icomp++;
    }
    else
      done = true;
  }

  fclose(f);
}

void Pulsar::ComponentModel::add_component (double centre,
					    double concentration, 
					    double height,
					    const char *name)
{
  components.push_back( new ScaledVonMises );
  components[components.size()-1]->set_centre(centre * 2*M_PI);
  components[components.size()-1]->set_concentration(concentration);
  components[components.size()-1]->set_height(height);
  component_names.push_back(name);

  model = 0;
}

void Pulsar::ComponentModel::check (const char* method, unsigned icomp) const
{
  if (icomp >= components.size())
    throw Error (InvalidParam, "Pulsar::ComponentModel::" + string(method),
		 "icomponent=%u >= ncomponent=%u",
		 icomp, components.size());
}

void Pulsar::ComponentModel::remove_component (unsigned icomp)
{
  check ("remove_component", icomp);
  components.erase (components.begin()+icomp);
}

ScaledVonMises* Pulsar::ComponentModel::get_component (unsigned icomp)
{
  check ("get_component", icomp);
  return components[icomp];
}

unsigned Pulsar::ComponentModel::get_ncomponents() const
{
  return components.size();
}


void Pulsar::ComponentModel::align (const Profile *profile)
{
  Profile modelprof(*profile);

  evaluate (modelprof.get_amps(), modelprof.get_nbin());

  Estimate<double> shift = profile->shift (modelprof);

  if (verbose)
    cerr << "Pulsar::ComponentModel::align shift=" << shift << endl;

  if (phase)
  {
    float normalization = profile->sum() / modelprof.sum();
    
    if (verbose)
      cerr << "Pulsar::ComponentModel::align normalization="
	   << normalization << endl;

    for (unsigned icomp=0; icomp < components.size(); icomp++)
    {
      Estimate<double> height = components[icomp]->get_height();
      components[icomp]->set_height ( height * normalization );
    }

    phase->set_value (phase->get_value() + shift.get_value() * 2*M_PI);
    return;
  }

  // Dick requires this
  cout << setprecision(6) << fixed << "Shift= " << shift.val << endl;

  for (unsigned icomp=0; icomp < components.size(); icomp++)
  {
    Estimate<double> centre = components[icomp]->get_centre() + shift * 2*M_PI;
    if (centre.val >= M_PI)
      centre -= 2*M_PI;
    if (centre.val < -M_PI)
      centre += 2*M_PI;
    components[icomp]->set_centre(centre);
  }
}


void Pulsar::ComponentModel::set_infit (unsigned icomponent,
					unsigned iparam,
					bool infit)
{
  check ("set_infit", icomponent);

  if (iparam > 2)
    throw Error(InvalidParam, "Pulsar::ComponentModel::set_infit",
		"Parameter index out of range");

  components[icomponent]->set_infit (iparam, infit);
}

bool get_boolean (char c)
{
  return c=='1' || c=='t' || c=='T' || c=='y' || c=='Y';
}

void Pulsar::ComponentModel::set_infit (const char *fitstring)
{
  int ic=0, nc=strlen(fitstring);

  for (unsigned icomp=0; icomp < components.size(); icomp++)
    for (unsigned iparam=0; iparam < 3; iparam++)
    {
      if (ic==nc)
	return;

      bool fit = get_boolean (fitstring[ic]);

      components[icomp]->set_infit(iparam, fit);

      if (verbose)
	cerr << "Pulsar::ComponentModel::set_infit icomp=" << icomp
	     << " iparam=" << iparam << " fit=" << fit << endl;
      ic++;
    }
}

void Pulsar::ComponentModel::build () const
{
  if (model)
    return;

  SumRule< Univariate<Scalar> >* sum = new SumRule< Univariate<Scalar> >; 
  model = sum;

  if (fit_derivative)
  {
    derivative.resize (components.size());

    // setup model using derivative components
    for (unsigned icomp=0; icomp < components.size(); icomp++)
    {
      derivative[icomp] = new ScaledVonMisesDeriv;

      derivative[icomp]->set_centre(components[icomp]->get_centre());
      derivative[icomp]->set_concentration
	(components[icomp]->get_concentration());
      derivative[icomp]->set_height(components[icomp]->get_height());
      sum->add_model( derivative[icomp] );
    }
  }
  else
  {
    for (unsigned icomp=0; icomp < components.size(); icomp++)
      sum->add_model( components[icomp] );
  }

  if (phase)
  {
    if (verbose)
      cerr << "Pulsar::ComponentModel::build single phase" << endl;

    backup.resize (components.size());

    ChainRule<Univariate<Scalar> >* chain = new ChainRule<Univariate<Scalar> >;
    chain->set_model (sum);

    model = chain;

    for (unsigned icomp=0; icomp < components.size(); icomp++)
    {
      if (sum->get_param_name(icomp*3) != "centre")
	throw Error (InvalidState, "Pulsar::ComponentModel::fit",
		     "iparam=%u name='%s'", icomp*3,
		     sum->get_param_name(icomp*3).c_str());

      if (sum->get_param_name(icomp*3+1) != "concentration")
	throw Error (InvalidState, "Pulsar::ComponentModel::fit",
		     "iparam=%u name='%s'", icomp*3,
		     sum->get_param_name(icomp*3).c_str());

      // don't allow the widths to vary
      sum->set_infit(icomp*3+1, false);

      backup[icomp] = components[icomp]->clone();

      SumRule<Scalar>* psum = new SumRule<Scalar>;
      psum->add_model (phase);
      psum->add_model (new ScalarConstant (sum->get_param(icomp*3)));

      chain->set_constraint (icomp*3, psum);
    }
  }
}


void Pulsar::ComponentModel::fit (const Profile *profile)
{
  build ();

  unsigned nbin = profile->get_nbin(); 
  vector<float> data (nbin);

  float mean = 0.0;
  float variance = 1.0;

  if (fit_derivative)
  {
    vector<complex<float> > spec(nbin/2+2);

    // FFT
    FTransform::frc1d(nbin, (float*)&spec[0], profile->get_amps());

    unsigned ic, nc=nbin/2+1;
    float scale = 1.0/nbin;
    for (ic=0; ic < nc; ic++)
      spec[ic] *= complex<float>(0.0f, ic) // scale by i*frequency
	* scale; // correction for DFT normalisation

    // IFFT
    FTransform::bcr1d (nbin, &data[0], (float*)&spec[0]);
  }
  else // normal profile
  {
    Reference::To<PhaseWeight> baseline = profile->baseline();
    variance = baseline->get_variance().get_value();
    mean = baseline->get_mean().get_value();

    if (verbose)
      cerr << "Pulsar::ComponentModel::fit mean=" << mean
	   << " variance=" << variance << endl;

    // copy data
    for (unsigned i=0; i < nbin; i++)
      data[i] = profile->get_amps()[i] - mean;
  }

  Axis<double> argument; 
  model->set_argument (0, &argument);

  vector< Axis<double>::Value > xval;  
  vector< Estimate<double> > yval;

  unsigned nfree = 0;

  for (unsigned i=0; i < nbin; i++)
  {
    double radians = (i+0.5)/nbin * 2*M_PI;

    xval.push_back( argument.get_Value(radians) );
    yval.push_back( Estimate<double>(data[i], variance) );

    nfree ++;
  }
  
  for (unsigned i=0; i<model->get_nparam(); i++)
    if (model->get_infit(i))
    {
      if (nfree == 0)
	throw Error (InvalidState, "Pulsar::ComponentModel::fit",
		     "more free parameters than data");
      nfree --;
    }

  // fit

  LevenbergMarquardt<double> fit;
  fit.verbose = Function::verbose;
    
  chisq = fit.init (xval, yval, *model);

  // fit.singular_threshold = 1e-15; // dodge covariance problems
  unsigned iter = 1;
  unsigned not_improving = 0;

  string result = "RMS";
  if (variance)
    result = "chisq";

  unsigned max_iter = 100;

  while (not_improving < 25 && iter < max_iter)
  { 
    if (verbose)
      cerr << "iteration " << iter;

    float nchisq = fit.iter (xval, yval, *model);

    if (verbose)
      cerr << "     " << result << " = " << nchisq << endl;
    
    if (nchisq < chisq)
    {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;

      if (diffchisq/chisq < threshold && diffchisq > 0)
      {
	if (verbose)
	  cerr << "converged, delta " << result << " = " << diffchisq << endl;
	break;
      }
    }
    else
      not_improving ++;
    
    iter ++;
  }
  
  std::vector<std::vector<double> > covar;
  fit.result (*model, covar);
  
  for (unsigned iparam=0; iparam < model->get_nparam(); iparam++)
  {
    double variance = covar[iparam][iparam];

    if (verbose)
      cerr << "Pulsar::ComponentModel::fit"
	" variance[" << iparam << "]=" << variance << endl;

    if (!finite(variance))
      throw Error (InvalidState, 
		   "Pulsar::ComponentModel::fit",
		   "non-finite variance "
		   + model->get_param_name(iparam));

    if (!model->get_infit(iparam) && variance != 0)
      throw Error (InvalidState,
		   "Pulsar::ComponentModel::fit",
		   "non-zero unfit variance "
		   + model->get_param_name(iparam)
		   + " = " + tostring(variance) );

    if (variance < 0)
      throw Error (InvalidState,
		   "Pulsar::ComponentModel::fit",
		   "invalid variance " + model->get_param_name(iparam)
		   + " = " + tostring(variance) );

    // cerr << iparam << ".var=" << variance << endl;

    model->set_variance (iparam, variance);
  }

  if (fit_derivative) // copy best-fit parameters back
  {
    for (unsigned icomp=0; icomp < components.size(); icomp++)
    {
      components[icomp]->set_centre(derivative[icomp]->get_centre());
      components[icomp]->set_concentration
	(derivative[icomp]->get_concentration());
      components[icomp]->set_height(derivative[icomp]->get_height());
    }
  }
}

void Pulsar::ComponentModel::evaluate (float *vals,
				       unsigned nvals, int icomp_selected) 
{
  //Construct the summed model
  SumRule< Univariate<Scalar> > m; 
  if (icomp_selected >= 0)
  {
    check ("evaluate", icomp_selected);
    m += components[icomp_selected];
  }
  else
  {
    for (unsigned icomp=0; icomp < components.size(); icomp++)
      m += components[icomp];
  }

  // evaluate
  Axis<double> argument;  
  m.set_argument (0, &argument);

  for (unsigned i=0; i < nvals; i++)
  { 
    argument.set_value( (i+0.5)/nvals * 2*M_PI );
    vals[i] = m.evaluate();
  }
}

void Pulsar::ComponentModel::clear()
{
  components.clear();
  component_names.clear();
  comments.clear();
  model = 0;
}
