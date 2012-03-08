/***************************************************************************
 *
 *   Copyright (C) 2011 by Stefan Oslowski
 *   Licensed under the Academic Free License version 2.1
 *   Inspired by Paul Demorest's ProfilePCA
 *   Described in Oslowski et al., 2011
 *
 ***************************************************************************/

//TODO:
// there is some overlap of code with pat.C, e.g. the diff profiles, this could be moved somewhere as a library, More/General I suppose.
// Loading of evecs // best predictor as in the Fourier domain version 

#include <iostream>
#include <fstream>
#include <iomanip>

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ArchiveExtension.h"

#include "Pulsar/TimeAppend.h"
#include "Pulsar/ArrivalTime.h"
#include "Pulsar/PhaseGradShift.h"
#include "toa.h"

#include "Pulsar/TimeDomainCovariance.h"
#include<gsl/gsl_blas.h>
#include<gsl/gsl_eigen.h>
#include<gsl/gsl_linalg.h>
#include<gsl/gsl_matrix.h>
#include<gsl/gsl_vector.h>
#include<gsl/gsl_statistics.h>

#include <RobustStats.h>

#if HAVE_PGPLOT
#include <cpgplot.h>
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/PlotOptions.h"
#endif

using namespace std;
using namespace Pulsar;

class psrpca : public Application
{
public:
  //! Default constructor:
  psrpca ();

  //! Initial setup
  void setup ();
  //! Process the given archive:
  void process ( Archive* );

  //! Final processing:
  void finalize ();

protected:
  //! add program options:
  void add_options ( CommandLine::Menu& );

  //! Set the standard
  void set_standard (string);

  //! Archive containing the standard
  Reference::To<Archive> std_archive;

  //! Timing machinery
  Reference::To<ArrivalTime> arrival;
  vector<Tempo::toa> toas;
  TimeAppend timeApp;

  //! Total archive
  Reference::To<Archive> total;
  unsigned total_count;

  //! Covariance calculations
  TimeDomainCovariance *t_cov;
  gsl_matrix *covariance;

  //! Output control
  string prefix;
  bool save_diffs;
  bool save_evecs;
  bool save_evals;
  bool save_covariance_matrix;
  bool save_decomps;

  //! Fit control
  bool apply_shift;
  bool apply_offset;
  bool apply_scale;
  bool prof_to_std;

  //! Regression
  bool unweighted_regr;
  string residuals_file;  

  //! Predictor
  double threshold;
  double threshold_sigma;
  unsigned consecutive_points;

  //! Baseline preprocessing control
  bool remove_arch_baseline;
  bool remove_std_baseline ;

  struct CastToDouble
  {
    double operator () (float value) const { return static_cast<double>(value);}
  };
};

psrpca::psrpca ()
	: Application ("psrpca", "perform PCA analysis and multiple regression")
{
  //add options
#if HAVE_PGPLOT
  add ( new PlotOptions );
#endif
  StandardOptions* preprocessor = new StandardOptions;
  add ( preprocessor );

  //initialise the timing machinery
  arrival = new ArrivalTime;
  toas.resize(0);
  arrival->set_shift_estimator(new PhaseGradShift);

  timeApp.must_match = true;

  total_count = 0;

  t_cov = new TimeDomainCovariance;
  covariance = NULL;
  save_diffs = save_evecs = save_evals = save_covariance_matrix = save_decomps = true;
  prefix = "psrpca";

  prof_to_std = apply_shift = apply_offset = apply_scale = true ;

  remove_arch_baseline = remove_std_baseline = true;

  unweighted_regr = true;

  threshold = -1.0;
  consecutive_points = 3;
  threshold_sigma = 3.0;
}

void psrpca::setup ()
{
  arrival->set_shift_estimator ( new PhaseGradShift );
  try
  {
    std_archive->fscrunch();
    std_archive->pscrunch();
    arrival->set_standard ( std_archive );
  } catch (Error& error)
  {
    cerr << error << endl;
    cerr << "psrpca::setup setting standard failed" << endl;
    exit ( -1 );
  }
}

void psrpca::add_options ( CommandLine::Menu& menu )
{
  CommandLine::Argument* arg;

  arg = menu.add ( remove_arch_baseline, "ra" );
  arg->set_help ( "Don't remove the baseline of input archives" );

  arg = menu.add ( remove_std_baseline, "rs" );
  arg->set_help ( "Don't remove the baseline of the template" );

  menu.add ("");
  menu.add ("Fitting options");

  arg = menu.add (this, &psrpca::set_standard, 's', "stdfile");
  arg->set_help ("Location of standard profile");

  arg = menu.add ( apply_shift, "fp" );
  arg->set_help ("Don't fit for phase shift");

  arg = menu.add ( apply_offset, "fb" );
  arg->set_help ("Don't fit for baseline offset");

  arg = menu.add ( apply_scale, "fs" );
  arg->set_help ("Don't fit for scale");

  arg = menu.add ( prof_to_std, "ts" );
  arg->set_help ( "Apply scaling, offset and shift to standard instead of profile");

  menu.add ("");
  menu.add ("Regression and Predictor Options");

  arg = menu.add (residuals_file, 'r', "residuals");
  arg->set_help ("File with residuals");
  arg->set_long_help ("As output by tempo2 in the following format: {sat} {post} {err}\n"
		  "They need to be in the same order as the input archives \n"
		  "When used, corrected residuals are output in a file prefix_residuals \n"
		  "The format of the output is: {sat} {input residual} {output residual} {input error}\n"
		  "Note that the output units are us\n");
  arg = menu.add (consecutive_points, 'c', "consecutive_points");
  arg->set_help ("Choose the number of consecutive points above threshold used during determination of significant eigenvectors");
  arg = menu.add (threshold_sigma,'t',"threshold_sigma");
  arg->set_help ("Choose the threshold in the units of standard deviation");

  menu.add ("");
  menu.add ("Output options:");

  arg = menu.add (save_diffs, "sd");
  arg->set_help ("Don't save the difference profiles");

  arg = menu.add (save_evecs, "se");
  arg->set_help ("Don't save the eigenvectors");

  arg = menu.add (save_evals, "sv");
  arg->set_help ("Don't save the eigenvalues");

  arg = menu.add (save_covariance_matrix, "sc");
  arg->set_help ("Don't save the covariance matrix");

  arg = menu.add (save_decomps, "sD");
  arg->set_help ("Don't save the decompositions");

  arg = menu.add (prefix, 'p', "prefix");
  arg->set_help ("Set prefix for all the output files");
  arg->set_long_help ("Default value: psrpca\n"
		  "The output files for differences profiles, eigenvectors, eigenvalues and covariance matrix\n"
		  "Are: prefix_diffs.ar, prefix_evecs.ar, prefix_evals.dat, prefix_covar.fits, respectively");
}

void psrpca::process (Archive* archive)
{
  archive->fscrunch();
  archive->pscrunch();
  if ( remove_arch_baseline )
    archive->remove_baseline ();
  if (total_count == 0)
  {
    total = archive;
    timeApp.init(total);
  }
  else
  {
    timeApp.append(total, archive);
  }
  total_count += archive->get_nsubint();
}

void psrpca::finalize ()
{
  arrival->set_observation ( total );
  arrival->get_toas(toas);

  if ( remove_std_baseline )
    std_archive -> remove_baseline ();
  Reference::To<Profile> std_prof = std_archive->get_Profile(0, 0, 0);
  float *s_amps = std_prof->get_amps ();
  const float nbin = std_prof->get_nbin ();

  double scale, offset, snr;

  if ( total_count < nbin )
    cerr << "WARNING: psrpca::finalize - not enough observations provided, "
	    "covariance matrix will not have full rank" << endl;

  //total->remove_baseline();

  gsl_matrix *profiles = gsl_matrix_alloc ( (unsigned)nbin, total_count );

  for (unsigned i_subint = 0; i_subint < total->get_nsubint(); i_subint++ )
  {
    Reference::To<Profile> prof = total->get_Profile ( i_subint, 0, 0 );
    if ( apply_shift )
      prof->rotate_phase ( toas[i_subint].get_phase_shift() );
    snr = prof->snr ();

    //calculate the scale
    float *p_amps = prof->get_amps ();
    scale = 0.0;

    for ( unsigned i_bin = 0; i_bin < nbin; i_bin++ )
    {
      scale += s_amps[i_bin] * p_amps[i_bin];
    }
    scale = (prof->get_nbin()* scale - prof->sum() * std_prof->sum()) /
	    (prof->get_nbin()* std_prof->sumsq() - std_prof->sum() * std_prof->sum());

    // calculate the baseline offset
    offset = (scale * std_prof->sum() - prof->sum()) / nbin;

    if ( prof_to_std )
    {
      //match the profile to standard and subtract the standard
      if ( apply_offset )
	prof->offset  ( offset );
      if ( apply_scale )
	prof->scale ( 1.0/scale );
      prof->diff ( std_prof );

      double* damps;
      damps = new double [ (unsigned)nbin ];
      transform( prof->get_amps(), prof->get_amps() + (unsigned)nbin, damps, CastToDouble() );
      gsl_matrix_set_col ( profiles, i_subint, &gsl_vector_const_view_array( const_cast<double*> (damps), (unsigned)nbin ).vector );
      t_cov->add_Profile ( prof, snr );
    }
    else
    {// prof_to_std is false
      Reference::To<Profile> diff = prof->clone ();
      diff->set_amps ( std_prof->get_amps () );
      if ( apply_offset ) 
	diff->offset( -offset );
      if ( apply_scale ) 
	diff->scale (scale);
      diff->diff ( prof );
      diff->scale (-1);

      double* damps;
      damps = new double [ (unsigned)nbin ];
      transform( diff->get_amps(), diff->get_amps() + (unsigned)nbin, damps, CastToDouble() );
      gsl_matrix_set_col ( profiles, i_subint, &gsl_vector_const_view_array( const_cast<double*> (damps), (unsigned)nbin ).vector );
      t_cov->add_Profile ( diff, snr );
      prof->set_amps ( diff->get_amps() );
    }
  }

  covariance = gsl_matrix_alloc ( (int) nbin, (int) nbin );
  t_cov->get_covariance_matrix_gsl ( covariance );

  // write the covariance matrix and difference profiles
  FILE *out;
  if ( save_covariance_matrix )
  {
    out = fopen ( (prefix+"_covariance.dat").c_str(), "w" );
    gsl_matrix_fprintf(out, covariance, "%g");
    fclose ( out );
  } // save covariance matrix

  if ( save_diffs ) 
    total->unload ( prefix+"_diffs.ar" );

  //solve the eigenproblem
  gsl_matrix_view m = gsl_matrix_submatrix ( covariance, 0, 0, (int)nbin, (int)nbin );
  gsl_vector *eval = gsl_vector_alloc ( (int)nbin );
  gsl_matrix *evec = gsl_matrix_alloc ( (int)nbin, (int)nbin );

  gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc ( (int)nbin );
  gsl_eigen_symmv ( &m.matrix, eval, evec, w );
  gsl_eigen_symmv_free ( w );
  gsl_eigen_symmv_sort ( eval, evec, GSL_EIGEN_SORT_VAL_DESC );

  // save evectors
  if ( save_evecs )
  {
    Reference::To<Archive> evecs_archive = total->clone();
    gsl_vector *evec_copy = gsl_vector_alloc ( (int)nbin );

    for (unsigned iext=0; iext < evecs_archive->get_nextension(); iext++)
    {
      delete evecs_archive->get_extension(iext);
    }

    evecs_archive->resize ( (unsigned)nbin, 1, 1, (unsigned)nbin );

    for (unsigned i_evec = 0; i_evec < (unsigned)nbin; i_evec++ )
    {
      gsl_vector_memcpy ( evec_copy, &gsl_matrix_column( evec, i_evec).vector );
      evecs_archive->get_Profile ( i_evec, 0, 0 ) -> set_amps ( evec_copy->data );
    }
    evecs_archive->unload ( prefix+"_evecs.ar" );
  } // save evectors

  if ( save_evals )
  {
    out = fopen ( (prefix+"_evals.dat").c_str(), "w" );
    gsl_vector_fprintf ( out, eval, "%g" );
    fclose ( out );
  } // save_evals

  // decompose profiles onto eigenvectors
  gsl_matrix *decompositions = gsl_matrix_alloc ( (unsigned)nbin, total_count );

  gsl_blas_dgemm ( CblasTrans, CblasNoTrans, 1.0, evec, profiles, 0.0, decompositions );

  if ( save_decomps )
  {
    out = fopen ( (prefix + "_decomposition.dat").c_str(), "w" );
    gsl_matrix_fprintf ( out, decompositions, "%g");
    fclose ( out );
  } // save decompositions

  if ( !residuals_file.empty() )
  {
    //read in the residuals:
    double tmp;
    unsigned residual_count = 0;

    gsl_vector *mjds = gsl_vector_alloc ( total->get_nsubint() );
    gsl_vector *residuals = gsl_vector_alloc ( total->get_nsubint() );
    gsl_vector *residuals_err = gsl_vector_alloc ( total->get_nsubint() );
    //TODO check the format of the input
    ifstream inFile( residuals_file.c_str() );
    if ( inFile.is_open() )
    {
      while ( inFile.good() )
      {
	inFile >> tmp;
	if ( !inFile.good() )
	  break;
	gsl_vector_set(mjds, residual_count, tmp);

	inFile >> tmp;
	gsl_vector_set(residuals, residual_count, tmp*1e6);

	inFile >> tmp;
	gsl_vector_set(residuals_err, residual_count, tmp);

	residual_count ++ ;
      }
      inFile.close ();
      if ( residual_count != total->get_nsubint() )
      {
	cerr << "psrpca::finalize wrong number of residuals provided. Got " << residual_count 
		<< " while needed " << total->get_nsubint()<< endl;
	exit (-1) ;
      }

    }
    else
    {
      cerr << "psrpca::finalize couldn't open the residuals" << endl;
    }

    // multiple regression
    gsl_vector *res_decomp_covar = gsl_vector_alloc((unsigned)nbin); // covariance between residuals and decomposition coefficients

    gsl_vector *res_decomp_corel = gsl_vector_alloc((unsigned)nbin); // this one is used to determine how many eigenvectors to use

    gsl_vector *mean_vector= gsl_vector_alloc ((unsigned)nbin); // vector of means of the projections
    gsl_matrix *proj_covariance = gsl_matrix_alloc((unsigned)nbin, (unsigned)nbin); // covariance of projections
    gsl_matrix *proj_covariance_org = gsl_matrix_alloc((unsigned)nbin, (unsigned)nbin); // copy of the above
    gsl_matrix *inverse = gsl_matrix_alloc((unsigned)nbin, (unsigned)nbin); // inverse of the above

    // calculate res_decomp_ covar and corel
    gsl_vector_view tmp_v1, tmp_v2;
    for (unsigned i=0; i<nbin; i++)
    {
      tmp_v1 = gsl_matrix_row(decompositions, i);
      for (unsigned j=0; j<nbin; j++)
      {
	tmp_v2 = gsl_matrix_row(decompositions, j);
	gsl_matrix_set( proj_covariance, i, j, gsl_stats_covariance( (&tmp_v1.vector)->data, 1, (&tmp_v2.vector)->data, 1, (&tmp_v1.vector)->size) );
	gsl_matrix_set( proj_covariance, j, i, gsl_matrix_get( proj_covariance, i, j));
      }
      gsl_vector_set(res_decomp_covar, i, gsl_stats_covariance (residuals->data, 1, (&tmp_v1.vector)->data, 1, (&tmp_v1.vector)->size) );
      gsl_vector_set(res_decomp_corel, i, gsl_vector_get(res_decomp_covar, i) / gsl_stats_sd(residuals->data, 1, residuals->size) / gsl_stats_sd( (&tmp_v1.vector)->data, 1, (&tmp_v1.vector)->size) );
      gsl_vector_set( mean_vector, i, gsl_stats_mean( (&tmp_v1.vector)->data, 1, (&tmp_v1.vector)->size ) );
    }

    // determine the number of significant eigenvectors
    threshold = threshold_sigma * robust_stddev ( res_decomp_corel->data, res_decomp_corel->size ) ;
    unsigned last_eigen = (unsigned)nbin; 
    unsigned over_count = 0;
    ofstream logFile ( ( prefix+".log" ).c_str() );
    if ( !logFile.is_open() )
      cerr << "psrpca::finalize couldn't open the log file" << endl;
    for (unsigned ieigen = (unsigned)nbin-1; ieigen > 0; ieigen--) 
    {
      if (ieigen<2)
	printf("ieigen %d\n",ieigen);
      if (fabs(gsl_vector_get(res_decomp_corel, ieigen)) >= threshold )
	over_count ++;
      else
	over_count = 0;
      if (over_count == consecutive_points)
      {
	last_eigen = ieigen + consecutive_points; 
	logFile  << "Using " << last_eigen << " eigenvectors." << endl;
	break;
      }
    }

    // invert the D matrix
    int result;
    int signum = 0;
    double beta_zero;
    gsl_matrix_view proj_covariance_used = gsl_matrix_submatrix(proj_covariance, 0, 0, last_eigen, last_eigen);
    gsl_matrix *inverse_used = gsl_matrix_alloc(last_eigen, last_eigen);
    gsl_permutation *p_used = gsl_permutation_alloc (last_eigen);
    gsl_linalg_LU_decomp ( &proj_covariance_used.matrix, p_used, &signum);
    gsl_linalg_LU_invert ( &proj_covariance_used.matrix, p_used, inverse_used);

    gsl_vector_view res_decomp_covar_used = gsl_vector_subvector (res_decomp_covar, 0, last_eigen);
    gsl_vector_view mean_vector_used = gsl_vector_subvector (mean_vector, 0, last_eigen);
    // the beta_vector 
    gsl_vector *beta_vector_used = gsl_vector_alloc (last_eigen);
    gsl_blas_dgemv( CblasNoTrans, 1.0, inverse_used, &res_decomp_covar_used.vector, 0.0, beta_vector_used);
    // the beta_zero equals mu_y - beta^T cdot mu_z 
    double beta_zero_used;
    gsl_blas_ddot ( beta_vector_used, &mean_vector_used.vector, &beta_zero_used );

    beta_zero = gsl_stats_mean ( residuals->data, 1, residuals->size ) - beta_zero_used ;
    double multiple_correlation_coeff_used;
    tmp_v1 = gsl_matrix_row(decompositions, 0);
    gsl_blas_ddot( &res_decomp_covar_used.vector, beta_vector_used, &multiple_correlation_coeff_used );
    logFile << "Multiple correlation coefficient = " <<  sqrt( multiple_correlation_coeff_used / gsl_stats_variance(residuals->data, 1, total_count ) ) << endl;

    out = fopen ( (prefix+"_beta_zero.dat").c_str(), "w" );
    fprintf(out, "%g\n",beta_zero_used);
    out = fopen ( (prefix+"_beta_vector_used.dat").c_str(), "w" );
    gsl_vector_fprintf( out, beta_vector_used, "%g" );
    fclose(out);

    ofstream outFile ( ( prefix+"_residuals.dat" ).c_str() );
    if ( outFile.is_open() )
    {
      //    out = fopen( (prefix+"_residuals.dat").c_str(), "w");
      gsl_vector *residuals_corr = gsl_vector_alloc(total_count);
      for (unsigned ires = 0 ; ires < total_count; ires++)
      {
	double correction = 0.0;
	for (unsigned ieigen = 0 ; ieigen < last_eigen; ieigen++)
	{
	  correction += gsl_vector_get(beta_vector_used, ieigen) * gsl_matrix_get(decompositions, ieigen, ires);
	}
	correction -= beta_zero_used;
	gsl_vector_set(residuals_corr, ires, gsl_vector_get(residuals, ires) - correction);
	ios_base::fmtflags originalFormat = outFile.flags();
	outFile << setiosflags(ios::fixed) << setprecision ( 15 ) << gsl_vector_get(mjds, ires);
	outFile.flags ( originalFormat );
        outFile	<< " " <<  gsl_vector_get(residuals, ires) << " " <<  gsl_vector_get(residuals_corr, ires) << " " << gsl_vector_get(residuals_err, ires) << endl;
	//fprintf(out,"%lf %g %g %g\n",gsl_vector_get(mjds, ires), gsl_vector_get(residuals, ires), gsl_vector_get(residuals_corr, ires), gsl_vector_get(residuals_err, ires));
      }
      outFile.close ();
      //fclose(out);
    }
    else
    {
      cerr << "psrpca::finalize error opening the file " << prefix << "_residuals.dat for corrected residuals" << endl;
    }
  }
  else
  {
    cerr << "psrpca: No residuals provided, not performing multiple regression" << endl;
  }
}

void psrpca::set_standard ( string std_path )
{
  std_archive = Archive::load ( std_path );
}

int main (int argc, char** argv)
{
  psrpca program;
  return program.main (argc, argv);
}
