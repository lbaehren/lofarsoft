//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_ProfileShiftFit_h
#define __Pulsar_ProfileShiftFit_h

#include "Pulsar/Algorithm.h"
#include "FTransform.h"
#include "Estimate.h"
#include "toa.h"
#include "BoxMuller.h"

namespace Pulsar
{
  class Profile;
  class Integration;

  //! Calculates profile shifts by fitting to a template/standard.
  /*! 
   * This class essentially reimplements
   * the standard "Taylor fftfit" timing algorithm (used in PSRCHIVE's 
   * PhaseGradientShift method, etc).  This has several benefits over
   * the existing implementation in model_profile.C:
   *
   * <ol>
   * <li> It caches the FFT'd template, resulting in about half as many FFT
   *      calls (when timing many profiles) versus the old routines.
   * <li> It uses only the standard PSRCHIVE FFT interface, and avoids the 
   *      old Fortran "fccf" routines.
   * <li> The Chi2 minimization procedure is much more straightforward - at 
   *      least in the author's opinion ;)
   * <li> Additional information (fitted amplitude, profile sigma, etc) is
   *      returned in a cleaner, more extensible object oriented way.
   * <li> Supports multiple/alternate phase shift error calculation 
   *      algorithms (not implemented yet).
   * </ol>
   */
  class ProfileShiftFit : public Algorithm
  {   
  public:

    //! Method to use for calculating shift error
    enum Error_Method {
      Traditional_Chi2,     // Chi-squared 2nd derivatives
      MCMC_Variance         // Shift variance from MCMC
    };

    //! Default constructor
    ProfileShiftFit ();

    //! Destructor
    ~ProfileShiftFit();

    //! Reset everything to zero, free internal memory.
    void reset();

    //! Set the number of harmonics to use for fit.
    void set_nharm(unsigned nh);

    //! Get the number of harmonics currently in use.
    unsigned get_nharm();

    //! Set the error method
    void set_error_method(Error_Method e) { err_meth = e; }

    //! Set the standard or template profile to use
    void set_standard(Profile* p);

    //! Set the data profile to use
    void set_Profile(Profile* p);

    //! Run the fit
    void compute();

    //! Return a TOA object for the current fit.
    Tempo::toa toa(Integration *i);

    //! Get the resulting shift
    Estimate<double> get_shift();

    //! Get the resulting scale factor
    Estimate<double> get_scale();

    //! Get the resulting Mean Squared Error (per fit DOF)
    double get_mse();

    //! Get the resulting SNR
    double get_snr();

    //! Current MCMC accept stats
    int mcmc_trials;
    int mcmc_accept;

  protected:

    //! Initialize vars
    void init();

    //! Number of harmonics to use
    unsigned nharm;

    //! Current template profile
    Reference::To<Profile> std;

    //! Current template nbin
    unsigned nbins_std;

    //! R2C FFT of current template
    float *fstd;

    //! Template normalization factor
    double std_pow;

    //! Current data profile
    Reference::To<Profile> prof;

    //! Number of bins in profile
    unsigned nbins_prof;

    //! R2C FFT of current profile
    float *fprof;

    //! Current freq domain cross-correlation function
    float *fccf;

    //! Number of bins in ccf
    unsigned nbins_ccf;

    //! Evaluate ccf at phase shift phi
    double ccf(double phi);

    //! Evaluate d(ccf)/d(phi) at phase shift phi
    double dccf(double phi);

    //! Evaluate d^2(ccf)/d(phi)^2 at phase shift phi
    double d2ccf(double phi);

    //! Returns log of a value proportional to the posterior shift PDF
    double log_shift_pdf(double phi);

    //! Same as above, but with scale>0 prior
    double log_shift_pdf_pos(double phi);

    //! Error method to use
    Error_Method err_meth;

    //! Calculate "traditional" parameter uncertainties
    void error_traditional();

    //! Calculate shift uncertainty as posterior PDF variance using MCMC
    void error_mcmc_pdf_var();

    //! Current shift result
    double shift;

    //! Current shift error
    double eshift;

    //! Current scale result
    double scale;

    //! Current scale error
    double escale;

    //! Current sigma^2 estimate
    double sigma2;

    //! Current fit MSE
    double mse;

    //! Current fit Chi^2 (not reduced)
    double chi2;

    //! Current profile SNR
    double snr;

  private:

    //! Normal random number generator
    BoxMuller nrand;

    //! Current MCMC state
    double mcmc_state;

    //! Current MCMC probability value
    double mcmc_log_pdf;

    //! Init MCMC state
    void mcmc_init();

    //! Return next sample from MCMC sequence
    double mcmc_sample();

  };

}


#endif



