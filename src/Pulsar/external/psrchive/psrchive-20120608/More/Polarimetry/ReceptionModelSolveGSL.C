/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModelSolveGSL.h"
#include "Pulsar/CoherencyMeasurementSet.h"

#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_blas.h>

#include <iostream>
#include <assert.h>

using namespace std;
using Calibration::CoherencyMeasurementSet;

std::string Calibration::SolveGSL::get_name () const
{
  return "GSL";
}

Calibration::SolveGSL* Calibration::SolveGSL::clone () const
{
  return new SolveGSL;
}

static bool model_verbose = false;

static void model_set (Calibration::ReceptionModel* model, const gsl_vector* x)
{
  unsigned ifit=0;

  for (unsigned iparam=0; iparam < model->get_nparam(); iparam++)
  {
    if (model->get_infit(iparam))
    {
      assert (ifit < x->size);
      model->set_param( iparam, gsl_vector_get (x, ifit) );
      ifit ++;

      if (model_verbose)
	cerr << "set." << iparam << "=" << model->get_param(iparam) << endl;
    }
  }

  assert( ifit == model->get_solver()->get_nparam_infit() );
}

static void model_get (const Calibration::ReceptionModel* model, gsl_vector* x)
{
  unsigned ifit=0;

  for (unsigned iparam=0; iparam < model->get_nparam(); iparam++)
  {
    if (model->get_infit(iparam))
    {
      assert (ifit < x->size);
      gsl_vector_set( x, ifit, model->get_param (iparam) );
      ifit ++;
    }
  }

  assert( ifit == model->get_solver()->get_nparam_infit() );
}


static int model_fdf (const gsl_vector* x, void* data,
		      gsl_vector* f, gsl_matrix* J)
{
  // cerr << "model_fdf" << endl;

  Calibration::ReceptionModel* model = (Calibration::ReceptionModel*) data;

  model_set (model, x);

  // number of CoherencyMeasurementSets
  unsigned nset = model->get_ndata ();

  vector< Jones<double> > gradient;
  vector< Jones<double> >* grad_ptr = 0;
  if (J)
    grad_ptr = &gradient;

  unsigned idat = 0;

  vector<double> components;

  for (unsigned iset=0; iset < nset; iset++)
  {
    const CoherencyMeasurementSet& mset = model->get_data (iset);

    // set the independent variables for this set of measurements
    mset.set_coordinates();

    // set the signal path through which these measurements were observed
    model->set_transformation_index( mset.get_transformation_index() );

    for (unsigned istate=0; istate<mset.size(); istate++)
    {
      model->set_input_index( mset[istate].get_input_index() );

      Jones<double> result = model->evaluate (grad_ptr);

      unsigned idat_start = idat;

      /*

      Eq.(5) of van Straten (2004) or Eq.(10) of van Straten (2006)
      (summation over k is performed)

      */
      if (f)
      {
	Jones<double> delta = result - mset[istate].get_coherency();

	mset[istate].get_weighted_components (delta, components);

	for (unsigned i=0; i<components.size(); i++)
	{
	  assert (idat < f->size);
	  gsl_vector_set( f, idat, components[i] );
	  idat ++;
	}
      }

      if (J)
      {
	assert (gradient.size() == model->get_nparam());

	unsigned ifit=0;

	for (unsigned iparam=0; iparam < model->get_nparam(); iparam++)
        {
	  if (model->get_infit(iparam))
	  {
	    assert (ifit < J->size2);

	    /* 

	    Eq.(6) of van Straten (2004)
	    
	    */

	    mset[istate].get_weighted_components (gradient[iparam],components);

	    idat = idat_start;

	    for (unsigned i=0; i<components.size(); i++)
	    {
	      assert (idat < J->size1);
	      gsl_matrix_set( J, idat, ifit, components[i] );
	      idat ++;
	    }

	    ifit ++;
	  }
	}

	assert( ifit == model->get_solver()->get_nparam_infit() );
      }
    }
  }

  assert( idat == model->get_solver()->get_ndat_constraint() );

#ifdef _DEBUG
  if (f)
    cerr << "|f(x)|=" << gsl_blas_dnrm2 (f) << endl;
#endif
  
  return GSL_SUCCESS;
}

static int model_f (const gsl_vector* x, void* data, gsl_vector* f)
{
  // cerr << "model_f" << endl;

  int ret = model_fdf (x, data, f, 0);

  // cerr << "|f(x)|=" << gsl_blas_dnrm2 (f) << endl;

  return ret;
}

static int model_df (const gsl_vector* x, void* data, gsl_matrix* J)
{
  // cerr << "model_df" << endl;

  return model_fdf (x, data, 0, J);
}

void unpack_covariance (Calibration::ReceptionModel* model,
			vector< vector<double> >& covariance,
			gsl_matrix* covar)
{
  unsigned nparam = model->get_nparam();

  covariance.resize (nparam);
  for (unsigned iparam=0; iparam < nparam; iparam++)
    covariance[iparam].resize (nparam);

  unsigned idat = 0;

  for (unsigned iparam=0; iparam < nparam; iparam++)
  {
    unsigned jdat = idat;

    for (unsigned jparam=iparam; jparam < nparam; jparam++)
    {
      double val = 0;

      if (model->get_infit(iparam) && model->get_infit(jparam))
      {
	val = gsl_matrix_get( covar, idat, jdat );
	jdat ++;
      }

      covariance[iparam][jparam] = covariance[jparam][iparam] = val;
    }

    // cerr << endl;

    if (model->get_infit(iparam))
    {
      assert( jdat == covar->size2 );
      idat ++;
    }
  }

  assert( idat == covar->size1 );

#ifdef _DEBUG
  for (unsigned iparam=0; iparam < nparam; iparam++)
  {
    for (unsigned jparam=0; jparam < nparam; jparam++)
      cerr << covariance[iparam][jparam] << " ";
    cerr << endl;
  }
#endif
}

void Calibration::SolveGSL::fit ()
{
  gsl_multifit_function_fdf function;
  function.f = &model_f;
  function.df = &model_df;
  function.fdf = &model_fdf;
  function.n = get_ndat_constraint ();
  function.p = get_nparam_infit ();
  function.params = equation;

  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::gsl_solve"
      " nfit=" << function.p << " ndat=" << function.n << endl;

  gsl_vector* initial_guess = gsl_vector_alloc (function.p);
  model_get (equation, initial_guess);

  const gsl_multifit_fdfsolver_type* type = gsl_multifit_fdfsolver_lmsder;

  gsl_multifit_fdfsolver* solver
    = gsl_multifit_fdfsolver_alloc (type, function.n, function.p);

  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::gsl_solve set solver" << endl;

  gsl_multifit_fdfsolver_set (solver, &function, initial_guess);

  int status = 0;
  iterations = 0;

  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::gsl_solve enter loop" << endl;

  do 
  {
    iterations ++;

    status = gsl_multifit_fdfsolver_iterate (solver);

    if (status)
      break;

#ifdef _DEBUG
    cerr << " |dx|=" << gsl_blas_dnrm2 (solver->dx) 
	 << " |x|=" << gsl_blas_dnrm2 (solver->x) << endl;
#endif

    status = gsl_multifit_test_delta (solver->dx, solver->x, 0, 5e-2);
  }
  while ( (status == GSL_CONTINUE) && (iterations < maximum_iterations) );

#ifdef _DEBUG
  cerr << iterations << " status: " << gsl_strerror (status) << endl;
#endif

  // unpack the final solution
  model_set (equation, solver->x);

  best_chisq = pow( gsl_blas_dnrm2 (solver->f), 2.0 );

  gsl_matrix *covar = gsl_matrix_alloc (function.p, function.p);
  gsl_multifit_covar (solver->J, 0.0, covar);

  unpack_covariance (equation, covariance, covar);

  gsl_multifit_fdfsolver_free (solver);
  gsl_matrix_free (covar);
  gsl_vector_free (initial_guess);
}

