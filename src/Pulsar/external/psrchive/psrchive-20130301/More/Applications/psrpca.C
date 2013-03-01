/***************************************************************************
 *
 *   Copyright (C) 2011 by Stefan Oslowski
 *   Licensed under the Academic Free License version 2.1
 *   Inspired by Paul Demorest's ProfilePCA
 *   Described in Oslowski et al. 2011 & Oslowski et al. 2012
 *
 * evecs contains the evectors in columns
 * profiles contains  the input profiles in columns
 * WARNING the proj_covariance (denoted by alfa in the paper) matrix is transposed w.r.t. to the paper 
 ***************************************************************************/


//TODO:
// there is some overlap of code with pat.C, e.g. the diff profiles, this could be moved somewhere as a library, More/General I suppose.
// the total intensity mode is broken

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <iomanip>

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/ArchiveExtension.h"

#include "Pulsar/TimeAppend.h"
#include "Pulsar/ArrivalTime.h"
#include "Pulsar/PhaseGradShift.h"
#include "Pulsar/MatrixTemplateMatching.h"
#include "toa.h"

#include "Pulsar/TimeDomainCovariance.h"
#include<gsl/gsl_blas.h>
#include<gsl/gsl_eigen.h>
#include<gsl/gsl_linalg.h>
#include<gsl/gsl_matrix.h>
#include<gsl/gsl_vector.h>
#include<gsl/gsl_permutation.h>
#include<gsl/gsl_permute_vector.h>
#include<gsl/gsl_statistics.h>

#include <RobustStats.h>


#ifdef HAVE_CULA
#include <cula_lapack.hpp>
#include <cula_blas.hpp>
#include <cula.hpp>
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
#ifdef HAVE_CULA
  int MeetsMinimumCulaRequirements();
  void checkStatus(culaStatus);
#endif 

protected:
  //! add program options:
  void add_options ( CommandLine::Menu& );

  //! Set the standard
  void set_standard (string);

  //! use the matrix template matching
  bool enable_MTM;

  //! worker functions for finalize:
  void fit_data( Reference::To<Profile> );
  void get_covariance_matrix();
  void solve_eigenproblem();
  void decompose_profiles();
  void read_and_correct_residuals();
  void get_toas_and_shift_profiles();
  void populate_profiles_matrix();
  gsl_matrix *profiles;
  gsl_matrix *decompositions;
  gsl_matrix *evec;
  gsl_vector *eval;
  gsl_vector *mean_calc_vector;
  float nbin;

#ifdef HAVE_CULA
  //! Cula variables:
  culaStatus status;
#endif

  //! Archive containing the standard
  Reference::To<Archive> std_archive;

  //! Timing machinery
  Reference::To<ArrivalTime> arrival;
  Reference::To<MatrixTemplateMatching> full_poln;
  vector<Tempo::toa> toas;
  TimeAppend timeApp;

  //! Total archive
  Reference::To<Archive> total;
  unsigned total_count;

  Reference::To<Archive> evecs_archive;
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
  bool save_proj;
  bool save_res_decomp_corel;

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
  unsigned last_eigen;
  unsigned consecutive_points;

  //! Load previous results
  string load_prefix;

  //! Baseline preprocessing control
  bool remove_arch_baseline;
  bool remove_std_baseline ;

  //! Polarisation extension
  unsigned which_pol;
  bool full_stokes_pca;

  struct CastToDouble
  {
    double operator () (float value) const { return static_cast<double>(value);}
  };
};

psrpca::psrpca ()
	: Application ("psrpca", "perform PCA analysis and multiple regression")
{
  //add options
  StandardOptions* preprocessor = new StandardOptions;
  add ( preprocessor );

  //initialise the timing machinery
  arrival = new ArrivalTime;
  enable_MTM = false;
  toas.resize(0);

  timeApp.must_match = true;

  total_count = 0;

  t_cov = new TimeDomainCovariance;
  covariance = NULL;
  save_diffs = save_evecs = save_evals = save_covariance_matrix = save_decomps = save_proj = save_res_decomp_corel = true;
  prefix = "psrpca";
  load_prefix = "";

  prof_to_std = apply_shift = apply_offset = apply_scale = true ;

  remove_arch_baseline = remove_std_baseline = true;

  unweighted_regr = true;

  threshold = -1.0;
  consecutive_points = 3;
  threshold_sigma = 3.0;
  last_eigen = 0;

  which_pol = 0;
  full_stokes_pca = false;
}

void psrpca::setup ()
{
#ifdef HAVE_CULA
  if ( MeetsMinimumCulaRequirements() != 1 )
    exit(-1);
#endif
  if ( enable_MTM )
  {
    arrival = full_poln = new MatrixTemplateMatching;
    full_poln->set_choose_maximum_harmonic( true );
  }
  else
    arrival->set_shift_estimator ( new PhaseGradShift );
  try
  {
    std_archive->fscrunch();
    if ( which_pol == 0 && !full_stokes_pca )
    {
      std_archive->pscrunch();
    }
    else
    {
      std_archive->convert_state ( Signal::Stokes );
    }
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

  arg = menu.add (load_prefix, 'L', "load prefix");
  arg->set_help ("Use previous solution, load it from input files with chosen prefix");

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
  arg = menu.add ( last_eigen, 'E', "last eigenvector" );
  arg->set_help( "Choose the last used eigenvector manually" );

  menu.add ("");
  menu.add ("Polarisation pca");

  arg = menu.add ( enable_MTM, "MTM" );
  arg->set_help ("Enable matrix template matching" );
  arg->set_long_help ("This options can only be used in the polarisation pca mode" );

  arg = menu.add ( which_pol, "P", "stokes" );
  arg->set_help ( "which Stokes parameters to perform PCA on?" );

  arg = menu.add ( full_stokes_pca, "S" );
  arg->set_help ( "Perform full Stokes PCA" );

  menu.add ("");
  menu.add ("Output otpions:");

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

  arg = menu.add (save_proj, "sp" );
  arg->set_help ("Don't save the projection coefficients covariances" );

  arg = menu.add (save_res_decomp_corel, "sc" );
  arg->set_help ("Don't save the correlations between the decompositions and residuals" );
  
  arg = menu.add (prefix, 'p', "prefix");
  arg->set_help ("Set prefix for all the output files");
  arg->set_long_help ("Default value: psrpca\n"
		  "The output files for differences profiles, eigenvectors, eigenvalues and covariance matrix\n"
		  "Are: prefix_diffs.ar, prefix_evecs.ar, prefix_evals.dat, prefix_covar.fits, respectively");
}

void psrpca::process (Archive* archive)
{
  if ( verbose )
    cerr << "psrpca::process () entered" << endl;
  archive->fscrunch();
  if ( which_pol == 0 && !full_stokes_pca )
  {
    archive->pscrunch();
  }
  else
  {
    archive->convert_state ( Signal::Stokes );
  }
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
  if ( verbose )
    cerr << "psrpca::process () finished" << endl;
}

void psrpca::finalize ()
{
  evecs_archive = total->clone();
  if ( verbose )
    cerr << "psrpca::finalize () entered" << endl;
  arrival->set_observation( total );
  arrival->get_toas( toas );

  if ( remove_std_baseline )
    std_archive -> remove_baseline ();

  Reference::To<Profile> std_prof;
  nbin = std_archive->get_Profile ( 0, 0, 0 )->get_nbin ();
  if ( ! full_stokes_pca )
  {
    std_prof = std_archive->get_Profile ( 0, which_pol, 0 );
  }
  else
  {
    std_prof = new Profile ( (unsigned)(4 * nbin) );
    float *std_amps = new float[ (unsigned)(4 * nbin) ];
    std_amps = std_prof->get_amps();
    for ( unsigned i_pol = 0; i_pol < 4; i_pol++ )
      memcpy ( std_amps + (unsigned)nbin*i_pol, std_archive->get_Profile ( 0, i_pol, 0 )->get_amps(), (unsigned)nbin*sizeof(float) );
    std_prof->set_amps ( std_amps );
  }

  FILE *out;
  
  fit_data( std_prof );
  out = fopen( (prefix+"_profiles.dat").c_str(), "w" );
  gsl_matrix_fprintf( out, profiles, "%g" );
  fclose( out );

  if ( full_stokes_pca )
    nbin = 4 * nbin;
  get_covariance_matrix();

  // write the covariance matrix and difference profiles
  if ( save_covariance_matrix )
  {
    out = fopen ( (prefix+"_covariance.dat").c_str(), "w" );
    gsl_matrix_fprintf(out, covariance, "%g");
    fclose ( out );
  } // save covariance matrix

  if ( save_diffs ) 
    total->unload ( prefix+"_diffs.ar" );

#ifdef HAVE_CULA
  status = culaInitialize();
  checkStatus(status);
#endif

  solve_eigenproblem();
  decompose_profiles();
  read_and_correct_residuals();
}

void psrpca::set_standard ( string std_path )
{
  std_archive = Archive::load ( std_path );
}

#ifdef HAVE_CULA
int psrpca::MeetsMinimumCulaRequirements() {
  int cudaMinimumVersion = culaGetCudaMinimumVersion();
  int cudaRuntimeVersion = culaGetCudaRuntimeVersion();
  int cudaDriverVersion = culaGetCudaDriverVersion();
  int cublasMinimumVersion = culaGetCublasMinimumVersion();
  int cublasRuntimeVersion = culaGetCublasRuntimeVersion();
  if(cudaRuntimeVersion < cudaMinimumVersion) {
    printf("CUDA runtime version is insufficient; " "version %d or greater is required while %d is present\n", cudaMinimumVersion, cudaRuntimeVersion);
    return 0;
  }
  if(cudaDriverVersion < cudaMinimumVersion) {
    printf("CUDA driver version is insufficient; " "version %d or greater is required while %f is present\n", cudaMinimumVersion, cudaDriverVersion);
    return 0;
  }
  if(cublasRuntimeVersion < cublasMinimumVersion) {
    printf("CUBLAS runtime version is insufficient; " "version %d or greater is required while %f is present\n", cublasMinimumVersion, cublasRuntimeVersion );
    return 0;
  }
  return 1;
}

void psrpca::checkStatus(culaStatus status)
{
  char buf[256];

  if(!status)
    return;

  culaGetErrorInfoString(status, culaGetErrorInfo(), buf, sizeof(buf));
  printf("%s\n", buf);

  culaShutdown();
  exit(EXIT_FAILURE);
}
#endif

void psrpca::fit_data( Reference::To<Profile> std_prof )
{
  double scale, offset, snr;
  if ( total_count < nbin )
    cerr << "WARNING: psrpca::finalize - not enough observations provided, "
	    "covariance matrix will not have full rank" << endl;
  //total->remove_baseline();

  profiles = gsl_matrix_alloc ( full_stokes_pca ? 4 * (unsigned)nbin : (unsigned)nbin, total_count );
  for (unsigned i_subint = 0; i_subint < total->get_nsubint(); i_subint++ )
  {
    //cerr << "Handling subint " << i_subint << endl;
    Reference::To<Profile> prof;
    if ( ! full_stokes_pca )
    {
      prof = total->get_Profile ( i_subint, which_pol, 0 );
    }
    else
    {
      prof = new Profile ( (unsigned)(4 * nbin) );
      float *prof_amps = new float[ (unsigned)(4 * nbin) ];
      // the memcpy below only needed if not applying shift, otherwise the memcpy will happen after shifting
      if ( !apply_shift )
      {
	prof_amps = prof->get_amps();
	for ( unsigned i_pol = 0; i_pol < 4; i_pol++ )
	{
	  memcpy ( prof_amps + (unsigned)nbin*i_pol, total->get_Profile ( i_subint, i_pol, 0 )->get_amps(), (unsigned)nbin*sizeof(float) );
	}
	for (int iii = 0; iii < 4; iii++) {
	}
	prof->set_amps ( prof_amps );
      }
      //cerr << "set amps for prof" << endl;
    } // full_stokes_pca is true
    if ( apply_shift )
    {
      total->get_Integration( i_subint )->expert()->rotate_phase( toas[i_subint].get_phase_shift() );
      if ( full_stokes_pca )
      {
	float *prof_amps = new float[ (unsigned)(4 * nbin) ];
	prof_amps = prof->get_amps();
	for ( unsigned i_pol = 0; i_pol < 4; i_pol++) {
	  memcpy ( prof_amps + (unsigned)nbin*i_pol, total->get_Profile ( i_subint, i_pol, 0 )->get_amps(), (unsigned)nbin*sizeof(float) ); 
	}
	prof->set_amps ( prof_amps );
      }
    }
    // if full_stokes_pca, use total intensity S/N. Else use the chosen pol
    snr = total->get_Profile ( i_subint, full_stokes_pca ? 0 : which_pol, 0 )->snr();

    //calculate the scale
    float *p_amps = prof->get_amps ();
    scale = 0.0;

    // fit only to Stokes I even when full_stokes_pca is true
    for ( unsigned i_bin = 0; i_bin < nbin; i_bin++ )
    {
      scale += std_archive->get_Profile( 0, 0, 0 )->get_amps()[i_bin] * total->get_Profile( i_subint, 0, 0 )->get_amps()[i_bin];
    }
    scale = (nbin * scale - total->get_Profile ( i_subint, 0, 0 )->sum() * std_archive->get_Profile ( 0, 0, 0 )->sum()) /
	    (nbin * std_archive->get_Profile ( 0, 0, 0 )->sumsq() - std_archive->get_Profile ( 0, 0, 0 )->sum() * std_archive->get_Profile ( 0, 0, 0 )->sum());

    // calculate the baseline offset
    // while the shift and scale are the same for all the Stokes parameters, the baseline offset will be different for each one of them
    if ( !full_stokes_pca )
    {
      offset = (scale * std_archive->get_Profile ( 0, which_pol, 0 )->sum() - total->get_Profile ( i_subint, which_pol, 0 )->sum()) / nbin;
    }
    else
    {
      offset = -1; // TODO why am I postponing the offset calculation? I think the only reason is not to store 4 offsets, still, pretty dumb
      //cerr << "Baseline calculation postponed as full stokes mode is on" << endl;
    }


    if ( full_stokes_pca )
    {
      nbin = 4 * nbin; // TODO: this is kinda confusing, maybe best to stick with original NBIN and just adjust it by a factor of 4 where necessary
    }
    if ( prof_to_std )
    {
      //cerr << " prof to std " << endl;
      //match the profile to standard and subtract the standard
      if ( apply_offset )
      {
	if ( !full_stokes_pca )
	  prof->offset  ( offset );
	else
	{
	  // calculate offset for each Stokes parameter separately
	  float* amps = prof->get_amps();
	  //cerr << "pre-offset amps for pols 0 and 1 are: " << amps[0] << " " << amps[1024] << endl;
	  for (unsigned i_pol = 0; i_pol < 4; i_pol++) {
	    offset = (scale * std_archive->get_Profile ( 0, i_pol, 0 )->sum() - total->get_Profile ( i_subint, i_pol, 0 )->sum()) / nbin * 4.0; 
	    //cerr << "Otained an offset of " << offset << " for pol " << i_pol << endl;
	    //cerr << "Looping through the subints with nbin = " << nbin << endl;
	    for (unsigned i_bin = (unsigned)(nbin/4)*i_pol; i_bin < (unsigned)(nbin/4)*(i_pol + 1 ); i_bin++) { //nbin/4 cause when full_stokes_pca is true I adjust the nbin to be 4*nbin but here I'm actually interested in the original nbin
	      amps[i_bin] += offset;
	    }
	  }
	}
      }
      //cerr << "offset applied" << endl;
      if ( apply_scale )
	prof->scale ( 1.0/scale );
      //cerr << "scale applied " << scale << endl;
      prof->diff ( std_prof );
      //cerr << "std_prof subtracted" << endl;
      //cerr << "reported nbins for prof and std_prof are: " << prof->get_nbin() << " " << std_prof->get_nbin() << endl;

      // Copy the 4*nbin profile back into a normal nbin * 4 pol packages:
      if ( full_stokes_pca )
      {
	for (unsigned i_pol = 0; i_pol < 4; i_pol++)
	{
	  // I think the nbin is still pumped up to 4 times the original value
	  total->get_Profile( i_subint, i_pol, 0 )->set_amps( prof->get_amps() + i_pol * (unsigned)(nbin / 4) );
	}
      }

      double* damps;
      damps = new double [ (unsigned)nbin ];

      transform( prof->get_amps(), prof->get_amps() + ( (unsigned)nbin), damps, CastToDouble() );
      gsl_vector_const_view view = gsl_vector_const_view_array( damps, (unsigned)nbin );
      gsl_matrix_set_col ( profiles, i_subint, &view.vector );

      t_cov->add_Profile ( prof, snr );
    }
    else
    {// prof_to_std is false
      Reference::To<Profile> diff = prof->clone ();
      diff->set_amps ( std_prof->get_amps () );
      if ( apply_offset ) 
      {
	if ( !full_stokes_pca )
	  diff->offset( -offset );
	else
	{
	  // calculate offset for each Stokes parameter separately
	  float* amps = prof->get_amps();
	  for (unsigned i_pol = 0; i_pol < 4; i_pol++) {
	    offset = (scale * std_archive->get_Profile ( 0, i_pol, 0 )->sum() - total->get_Profile ( i_subint, i_pol, 0 )->sum()) / nbin;
	    for (unsigned i_bin = (unsigned)nbin*i_pol; i_bin < (unsigned)nbin*(i_pol + 1 ); i_bin++) {
	      amps[i_bin] -= offset;
	    }
	  }
	}
      }
      if ( apply_scale ) 
	diff->scale (scale);
      diff->diff ( prof );
      diff->scale (-1);

      double* damps;
      damps = new double [ (unsigned)nbin ];
      transform( diff->get_amps(), diff->get_amps() + (unsigned)nbin, damps, CastToDouble() );

      gsl_vector_const_view view = gsl_vector_const_view_array( damps, (unsigned)nbin );
      gsl_matrix_set_col ( profiles, i_subint, &view.vector );

      t_cov->add_Profile ( diff, snr );
      prof->set_amps ( diff->get_amps() );
    }
    if ( full_stokes_pca )
      nbin = nbin / 4;
  }
}

void psrpca::get_covariance_matrix()
{
  covariance = gsl_matrix_alloc ( (int) nbin, (int) nbin );
  if ( load_prefix.size() == 0 )
  {
    t_cov->get_covariance_matrix_gsl ( covariance );
  }
  else
  {
    if ( verbose )
      cerr << "psrpca: loading the " << nbin << " by " << nbin << " covariance matrix from " << load_prefix << "_covariance.dat" << endl;
    FILE *in;
    int status;
    in = fopen( (load_prefix+"_covariance.dat").c_str(), "r" );
    status = gsl_matrix_fscanf( in, covariance );
    fclose( in );
    if ( status == GSL_EFAILED )
    {
      cerr << "psrpca: Reading the covariance matrix from " << load_prefix <<  "_covariance.dat failed" << endl;
      exit( -1 );
    }
  }
}

void psrpca::solve_eigenproblem()
{
  if ( verbose )
    cerr << "psrpca::solve_eigenproblem () entered" << endl;
  //solve the eigenproblem
  gsl_matrix_view m = gsl_matrix_submatrix ( covariance, 0, 0, (int)nbin, (int)nbin );
  eval = gsl_vector_alloc ( (int)nbin );
  evec = gsl_matrix_alloc ( (int)nbin, (int)nbin );

  if ( load_prefix.size() == 0 )
  {
#ifdef HAVE_CULA
    if ( verbose )
      cerr << "psrpca: Running cula implementation" << endl;
    //CULA implementation of the eigenproblem:
    gsl_matrix_transpose( covariance ); // transposing as CULA uses column-major while GSL uses row-major ordering
    status = culaDsyev( 'V', 'U', nbin, covariance->data, nbin, eval->data );

    gsl_matrix_transpose( covariance ); 
    checkStatus(status);
    // Invert the evals (cula produces them in ascending order):
    gsl_permutation *p_reverse = gsl_permutation_calloc( nbin );
    gsl_permutation_reverse( p_reverse );
    gsl_permute_vector( p_reverse, eval );
    // transposing as CULA uses column-major while GSL uses row-major ordering
    // ..and swap the columns of the evectors, to get back the correspondence to evals
    for ( unsigned i_column = 0; i_column < nbin/2; i_column++) {
      gsl_matrix_swap_columns( covariance, i_column, nbin-i_column-1 );
    }
    // copy the evecs as calculated by CULA to the gsl matrix evec:
    gsl_matrix_memcpy( evec, covariance );

#else
    //original GSL implementation:
    if ( verbose )
      cerr << "psrpca: Running gsl implementation" << endl;
    gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc ( (int)nbin );
    gsl_eigen_symmv ( &m.matrix, eval, evec, w );
    gsl_eigen_symmv_free ( w );
    gsl_eigen_symmv_sort ( eval, evec, GSL_EIGEN_SORT_VAL_DESC );
#endif

    // save evectors
    if ( save_evecs )
    {
      //Reference::To<Archive> evecs_archive = total->clone();
      // TODO remove the hardwired name of the file
      //Reference::To<Archive> evecs_archive = Archive::load ( "all.FcalibP" );
      gsl_vector *evec_copy = gsl_vector_alloc ( (int)nbin );

      /*for (unsigned iext=0; iext < evecs_archive->get_nextension(); iext++)
      {
      delete evecs_archive->get_extension(iext);
      }*/
      evecs_archive->resize ( (unsigned)nbin, 0, 0, 0 );
      for (unsigned i_evec = 0; i_evec < (unsigned)nbin; i_evec++ )
      {
	gsl_vector_view view = gsl_matrix_column(evec, i_evec);

	gsl_vector_memcpy( evec_copy, &view.vector ); // this memcopy is necessary, as the evec matrix is stored in row-major order
	evecs_archive->get_Profile( i_evec, 0, 0 ) -> set_amps( evec_copy->data );
	if ( full_stokes_pca )
	{
	  for ( unsigned i_pol = 1; i_pol < 4; i_pol++ )
	  {
	    evecs_archive->get_Profile( i_evec, i_pol, 0 )->set_amps( evec_copy->data + (unsigned)(nbin/4.0)*i_pol );
	  }
	}
      }
      evecs_archive->unload ( prefix+"_evecs.ar" );
    } 
  } //calculating eigenvectors / values
  else 
  {
    if ( verbose )
      cerr << "psrpca: loading the eigenvectors and values from the previous solutions in files " << load_prefix << "_evecs.ar and " << load_prefix << "_evals.dat" << endl;
    // load eigenvectors
    Reference::To<Archive> evecs_archive = Archive::load( load_prefix + "_evecs.ar" );
    gsl_vector *evec_copy = gsl_vector_alloc ( (int)nbin );

    double *damps = new double [ (unsigned)nbin ];
    float *famps = new float [ (unsigned)nbin ];
    gsl_vector_view view;

    for ( unsigned i_evec = 0; i_evec < (unsigned)nbin; i_evec++ )
    {
      //evecs_archive->get_Profile( i_evec, 0, 0 ) -> set_amps( evec_copy->data );
      if ( full_stokes_pca )
      {
	for ( unsigned i_pol = 0; i_pol < 4; i_pol++ )
	{
	  memcpy ( famps + i_pol*((unsigned)nbin/4), evecs_archive->get_Profile( i_evec, i_pol, 0 )->get_amps(), (unsigned)(nbin/4)*sizeof(float) );
	}
      }
      else
      {
	famps  = evecs_archive->get_Profile( i_evec, 0, 0 )->get_amps();
      }
      transform( famps, famps+(unsigned)nbin, damps, CastToDouble() );
      view = gsl_vector_view_array( damps, (unsigned)nbin );
      gsl_matrix_set_col( evec, i_evec, &view.vector );
    }

    // load eigenvalues
    FILE *in;
    int status;
    in = fopen( (load_prefix+"_evals.dat").c_str(), "r" );
    status = gsl_vector_fscanf( in, eval );
    fclose( in );
    if ( status == GSL_EFAILED )
    {
      cerr << "psrpca: Reading the covariance matrix from " << load_prefix <<  "_covariance.dat failed" << endl;
      exit( -1 );
    }
  } // loading eigenvectors / values

  if ( save_evals )
  {
    FILE *out;
    out = fopen ( (prefix+"_evals.dat").c_str(), "w" );
    gsl_vector_fprintf ( out, eval, "%g" );
    fclose ( out );
  } // save_evals
}

void psrpca::decompose_profiles()
{
  if ( verbose )
    cerr << "psrpca::decompose_profiles() entered" << endl;
  // decompose profiles onto eigenvectors
  decompositions = gsl_matrix_alloc ( total_count, (unsigned)nbin );

#ifdef HAVE_CULA
  // transpose for cula's ordering
  gsl_matrix *profiles_T = gsl_matrix_alloc ( total_count, (unsigned)nbin );
  gsl_matrix *decompositions_T = gsl_matrix_alloc ( (unsigned)nbin, total_count );
  //cerr << "Transpose memcpy profiles " << profiles->size1 << " " << profiles->size2 << " vs " << profiles_T->size1 << " " << profiles_T->size2 << endl;
  gsl_matrix_transpose_memcpy( profiles_T, profiles );
  gsl_matrix_transpose( evec );
  status = culaGemm( 'T', 'N', (int)total_count, (int)nbin, (int)nbin, (double)1.0, profiles_T->data, (int)nbin, evec->data, (int)nbin, (double)0.0, decompositions_T->data, (int)total_count ); // this is probably wrong
  checkStatus( status );
  //cerr << "Transpose memcpy decompositions" << endl;
  gsl_matrix_transpose_memcpy( decompositions, decompositions_T );
  //cerr << "Decomposing residual profiles finished!" << endl;
  culaShutdown();
#else
  gsl_blas_dgemm ( CblasTrans, CblasNoTrans, 1.0, profiles, evec, 0.0, decompositions );
#endif

  FILE *out;
  if ( save_decomps )
  {
    out = fopen ( (prefix + "_decomposition.dat").c_str(), "w" );
    gsl_matrix_fprintf ( out, decompositions, "%g");
    fclose ( out );
  } // save decompositions
}

void psrpca::read_and_correct_residuals()
{
  if ( verbose )
    cerr << "psrpca::read_and_correct_residuals() entered" << endl;
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

    // the beta_vector 
    gsl_vector *beta_vector_used;
    double beta_zero_used;

    FILE *out;
    gsl_vector *decompositions_means = gsl_vector_alloc( nbin ); // vector of means of the projections
    mean_calc_vector = gsl_vector_alloc( total_count );
    gsl_vector_set_all( mean_calc_vector, 1.0/total_count ); // previously this vector was filled with ones, now we use it to calculate the means 

    // generate the mean projection coefficients
    gsl_blas_dgemv( CblasTrans, 1.0, decompositions, mean_calc_vector, 0.0, decompositions_means );

    gsl_vector_view tmp_v1;

    // calculating the multiple regression from scratch:
    if ( load_prefix.size() == 0 )
    {
      // multiple regression
      // variables:
      gsl_vector *res_decomp_covar = gsl_vector_alloc((unsigned)nbin); // covariance between residuals and decomposition coefficients, used for calculating the predictor 
      gsl_vector *res_decomp_corel = gsl_vector_alloc((unsigned)nbin); // this one is used to determine how many eigenvectors to use
      gsl_matrix *proj_covariance = gsl_matrix_alloc((unsigned)nbin, (unsigned)nbin); // covariance of projections
      gsl_vector *proj_sd_vector = gsl_vector_alloc ( nbin ); // used for res_decomp_corel

      // subtract the mean from decompositions and calculate the standard deviations
      for (unsigned i_col = 0; i_col < nbin; i_col++) {
	tmp_v1 = gsl_matrix_column( decompositions, i_col );
	gsl_vector_set( proj_sd_vector, i_col, gsl_stats_sd( (&tmp_v1.vector)->data, 1, total_count ) );
	gsl_vector_add_constant( &tmp_v1.vector, -decompositions_means->data[i_col] );
      }
      gsl_blas_dgemm( CblasTrans, CblasNoTrans, 1.0, decompositions, decompositions, 0.0, proj_covariance );
      gsl_matrix_scale( proj_covariance, 1.0/total_count );

      // output the projection covariance matrix
      if ( save_proj )
      {
	out = fopen ( (prefix+"_proj_covariance.dat").c_str(), "w" );
	gsl_matrix_fprintf( out, proj_covariance, "%g" );
	fclose(out);
      }

      // calculate res_decomp_covar and res_decomp_corel:
      gsl_blas_dgemv( CblasTrans, 1.0, decompositions, residuals, 0.0, res_decomp_covar );
      gsl_vector_scale( res_decomp_covar, 1.0/total_count);
      double residual_sd = gsl_stats_sd( residuals->data, 1, residuals->size );
      for (unsigned i_el = 0; i_el < nbin; i_el++) {
	gsl_vector_set( res_decomp_corel, i_el, gsl_vector_get( res_decomp_covar, i_el ) / residual_sd / gsl_vector_get( proj_sd_vector, i_el ));
      }

      // determine the number of significant eigenvectors
      double robust_sigma = robust_stddev ( res_decomp_corel->data, res_decomp_corel->size );
      threshold = threshold_sigma * robust_sigma;
      if ( save_res_decomp_corel )
      {
	out = fopen( (prefix+"_res_decomp_corel.dat").c_str(), "w" );
	gsl_vector_fprintf( out, res_decomp_corel, "%g" );
	fclose(out);
      }

      //open the log file and determine the number of significant eigenvectors
      ofstream logFile ( ( prefix+".log" ).c_str() );
      if ( last_eigen == 0 )
      {
	last_eigen = (unsigned)nbin; 
	unsigned over_count = 0;
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
	    break;
	  }
	}
      }
      logFile  << "Using " << last_eigen << " eigenvectors." << endl << "Based on robust standard deviation equal " << robust_sigma << " and " << consecutive_points << " consecutive points " << threshold_sigma << " sigma away from zero." << endl;

      // invert the D matrix
      int signum = 0;
      double beta_zero;
      gsl_matrix_view proj_covariance_used = gsl_matrix_submatrix(proj_covariance, 0, 0, last_eigen, last_eigen);
      gsl_matrix *inverse_used = gsl_matrix_alloc(last_eigen, last_eigen);
      gsl_permutation *p_used = gsl_permutation_alloc (last_eigen);
      gsl_linalg_LU_decomp ( &proj_covariance_used.matrix, p_used, &signum);
      gsl_linalg_LU_invert ( &proj_covariance_used.matrix, p_used, inverse_used);

      gsl_vector_view res_decomp_covar_used = gsl_vector_subvector (res_decomp_covar, 0, last_eigen);
      gsl_vector_view mean_vector_used = gsl_vector_subvector (decompositions_means, 0, last_eigen);

      beta_vector_used = gsl_vector_alloc( last_eigen );
      gsl_blas_dgemv( CblasNoTrans, 1.0, inverse_used, &res_decomp_covar_used.vector, 0.0, beta_vector_used);
      // the beta_zero equals mu_y - beta^T cdot mu_z 
      gsl_blas_ddot ( beta_vector_used, &mean_vector_used.vector, &beta_zero_used );

      beta_zero_used += gsl_stats_mean ( residuals->data, 1, residuals->size );
      double multiple_correlation_coeff_used;
      tmp_v1 = gsl_matrix_row(decompositions, 0);
      gsl_blas_ddot( &res_decomp_covar_used.vector, beta_vector_used, &multiple_correlation_coeff_used );
      logFile << "Multiple correlation coefficient = " <<  sqrt( multiple_correlation_coeff_used / gsl_stats_variance(residuals->data, 1, total_count ) ) << endl;

    } // calculating the multiple regression from scratch:
    else // load the multiple regression from files
    {
      if ( verbose )
	cerr << "psrpca: loading the beta_zero from " << load_prefix << "_beta_zero.dat" << endl;
      FILE *in;
      int status;
      in = fopen ( (load_prefix+"_beta_zero.dat").c_str(), "r" );
      status = fscanf( in, "%lf", &beta_zero_used );
      fclose(in);
      if ( status !=1 )
      {
	cerr << "psrpca: something went wrong reading beta zero from " << load_prefix << "beta_zero.dat: fscanf status = " << status << endl;
	exit( -1 );
      }

      // read in the last_eigen from log
      if ( verbose )
	cerr << "psrpca: reading last_eigen from " << load_prefix << ".log" << endl;
      in = fopen ( (load_prefix+".log").c_str(), "r" );
      status = fscanf( in, "%*s %u %*s", &last_eigen );
      fclose( in );
      // DEBUG
      cerr << "got last_eigen = " << last_eigen << endl;
      if ( status != 1 )
      {
	cerr << "psrpca: something went wrong when reading last_eigen from " << load_prefix << ".log: fscanf status = " << status << endl;
	exit( -1 );
      }

      beta_vector_used = gsl_vector_alloc (last_eigen);
      in = fopen ( (load_prefix+"_beta_vector_used.dat").c_str(), "r" );
      status = gsl_vector_fscanf( in, beta_vector_used );
      fclose( in );
      if ( status == GSL_EFAILED )
      {
	cerr << "psrpca: Reading the beta vector from " << load_prefix <<  "_beta_vector_used.dat failed" << endl;
	exit( -1 );
      }

      // if the correction scheme is loaded, we need to subtract the means of decompositions as they were saved pre-mean subtraction
      for (unsigned i_col = 0; i_col < nbin; i_col++) {
	tmp_v1 = gsl_matrix_column( decompositions, i_col );
	//gsl_vector_set( proj_sd_vector, i_col, gsl_stats_sd( (&tmp_v1.vector)->data, 1, total_count ) );
	gsl_vector_add_constant( &tmp_v1.vector, -decompositions_means->data[i_col] );
      }
    }

    out = fopen ( (prefix+"_beta_zero.dat").c_str(), "w" );
    fprintf(out, "%g\n",beta_zero_used);
    fclose(out);
    out = fopen ( (prefix+"_beta_vector_used.dat").c_str(), "w" );
    gsl_vector_fprintf( out, beta_vector_used, "%g" );
    fclose(out);

    ofstream outFile ( ( prefix+"_residuals.dat" ).c_str() );
    if ( outFile.is_open() )
    {
      gsl_vector *residuals_corr = gsl_vector_alloc(total_count);
      for (unsigned ires = 0 ; ires < total_count; ires++)
      {
	double correction = 0.0;
	for (unsigned ieigen = 0 ; ieigen < last_eigen; ieigen++)
	{
	  correction += gsl_vector_get(beta_vector_used, ieigen) * ( gsl_matrix_get(decompositions, ires, ieigen) + gsl_vector_get( decompositions_means, ieigen ) ); // need to add the mean as I've subtracted it previously.
	}
	correction -= beta_zero_used;
	gsl_vector_set(residuals_corr, ires, gsl_vector_get(residuals, ires) - correction);
	ios_base::fmtflags originalFormat = outFile.flags();
	outFile << setiosflags(ios::fixed) << setprecision ( 15 ) << gsl_vector_get(mjds, ires);
	outFile.flags ( originalFormat );
	outFile	<< " " <<  gsl_vector_get(residuals, ires) << " " <<  gsl_vector_get(residuals_corr, ires) << " " << gsl_vector_get(residuals_err, ires) << endl;
      }
      outFile.close ();
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

int main (int argc, char** argv)
{
  psrpca program;
  return program.main (argc, argv);
}
