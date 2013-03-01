/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfilePCA.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"

#include <gsl/gsl_eigen.h>

#include <fitsio.h>
#include "FITSError.h"

#include "Warning.h"
#include "FTransform.h"

using namespace std;

static Warning warn;

//! Default constructor
Pulsar::ProfilePCA::ProfilePCA ()
{
  nharm_cov=0;
  nharm_pca=0;
  cov=NULL;
  mean=NULL;
  nprof=0;
  wt_sum=0.0;
  wt2_sum=0.0;
  pc_values.resize(0);
  pc_vectors=NULL;
}

//! Destructor
Pulsar::ProfilePCA::~ProfilePCA()
{
  if (cov!=NULL) delete [] cov;
  if (mean!=NULL) delete [] mean;
  if (pc_vectors!=NULL) delete [] pc_vectors;
}

void Pulsar::ProfilePCA::set_nharm_cov(unsigned nharm) 
{

  // Check if nharm has been set before
  if (nharm_cov>0 && wt_sum>0.0) {
    // TODO: Implement resizing of cov matrix.  for now,
    // throw an error.
    throw Error (InvalidParam, "Pulsar::ProfilePCA::set_nharm_cov",
        "Resizing covariance matrix not yet implemented.");
  }

  // Alloc matrices
  nharm_cov = nharm;
  cov = new double[4*nharm_cov*nharm_cov]; // No DC terms
  mean = new double[2*nharm_cov + 2];      // Includes DC
  reset();
}

unsigned Pulsar::ProfilePCA::get_nharm_cov()
{
  return(nharm_cov);
}

void Pulsar::ProfilePCA::set_nharm_pca(unsigned nharm)
{
  nharm_pca = nharm;
}

unsigned Pulsar::ProfilePCA::get_nharm_pca()
{
  return(nharm_pca);
}

void Pulsar::ProfilePCA::reset()
{
  wt_sum=0.0;
  wt2_sum=0.0;
  mean[0]=mean[1]=0.0;
  for (unsigned i=0; i<2*nharm_cov; i++) 
    mean[i+2]=0.0;
  for (unsigned i=0; i<4*nharm_cov*nharm_cov; i++)
    cov[i]=0.0;
}

void Pulsar::ProfilePCA::add_Profile(const Profile *p) 
{
  // Check that we have enough time resolution
  // If not, continue with a warning.
  unsigned nfbins = p->get_nbin() + 2;
  if (nfbins-2<2*nharm_cov) 
    warn << "Profile::PCA::add_Profile WARNING " 
      <<"Not enough profile bins for requested number of harmonics." 
      << endl;

  // If weight==0.0, quit
  float wt = p->get_weight();
  if (wt==0.0) return;

  // FFT input profile
  // If we'll be adding a lot of profiles, it might
  // be more efficient to calc time domain cov matrix then
  // FFT it.  This is more straightforward, tho.
  float *fprof = new float[nfbins];
  FTransform::frc1d(p->get_nbin(), fprof, p->get_amps());
  
  // Integrate into cov matrix, mean prof
  // cov indices are offset by 2 since we're ignoring
  // the DC terms.
  unsigned lim = (nfbins-2<2*nharm_cov) ? nfbins-2 : 2*nharm_cov;
  for (unsigned i=2; i<=lim; i++) {
    mean[i] += wt*fprof[i];
    for (unsigned j=i; j<=lim; j++) {
      cov[2*nharm_cov*(i-2) + (j-2)] += wt*fprof[i]*fprof[j];
    }
  }

  // Free mem
  delete [] fprof;

  // Increment weight sums
  nprof++;
  wt_sum += wt;
  wt2_sum += wt*wt;
}

void Pulsar::ProfilePCA::add_Integration(const Integration *integ)
{
  // Loop over channels
  // Just take pol 0 for now
  for (unsigned ichan=0; ichan<integ->get_nchan(); ichan++) {
    add_Profile(integ->get_Profile(0,ichan));
  }
}

void Pulsar::ProfilePCA::compute()
{
  // If no data added, throw Error
  if (wt_sum==0.0)
    throw Error (InvalidState, "Pulsar::ProfilePCA::compute",
        "No data added before calling compute (weight=0.0)");

  // Convert full cov matrix to a (potentially) reduced size pca matrix
  double *pca = new double[4*nharm_pca*nharm_pca];
  for (unsigned i=0; i<2*nharm_pca; i++) {
    for (unsigned j=i; j<2*nharm_pca; j++) {
      pca[2*i*nharm_pca + j] = get_cov_value(i,j);
    }
  }

  // Mirror pca matrix (prob not really necessary)
  for (unsigned i=0; i<2*nharm_pca; i++) {
    for (unsigned j=0; j<i; j++) {
      pca[2*i*nharm_pca + j] = pca[2*j*nharm_pca + i];
    }
  }

  // Run eigenvalue/vector routine (gsl)
  gsl_matrix_view m = gsl_matrix_view_array(pca, 2*nharm_pca, 2*nharm_pca);
  gsl_vector *eval = gsl_vector_alloc(2*nharm_pca);
  gsl_matrix *evec = gsl_matrix_alloc(2*nharm_pca, 2*nharm_pca);
  gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(2*nharm_pca);
  gsl_eigen_symmv(&m.matrix, eval, evec, w);
  gsl_eigen_symmv_free(w);
  gsl_eigen_symmv_sort(eval, evec, GSL_EIGEN_SORT_VAL_DESC);
  
  // Organize results
  pc_values.resize(2*nharm_pca);
  for (unsigned i=0; i<2*nharm_pca; i++) {
    pc_values[i] = gsl_vector_get(eval, i);
  }
  if (pc_vectors!=NULL) delete [] pc_vectors;
  pc_vectors = new double[4*nharm_pca*nharm_pca];
  for (unsigned i=0; i<2*nharm_pca; i++) {
    for (unsigned j=0; j<2*nharm_pca; j++) {
      pc_vectors[2*i*nharm_pca + j] = gsl_matrix_get(evec, j, i);
    }
  }

  // Free temp mem
  delete [] pca;
  gsl_matrix_free(evec);
  gsl_vector_free(eval);
}

double Pulsar::ProfilePCA::get_cov_value(unsigned i, unsigned j)
{
  if ((i>2*nharm_cov) || (j>2*nharm_cov))
    throw Error (InvalidParam, "Pulsar::ProfilePCA::get_cov_value",
        "requested out of range component (%d,%d)", i, j);
  if (j<i) { unsigned tmp=j; j=i; i=tmp; }
  return cov[2*i*nharm_cov + j]/wt_sum 
    - mean[i+2]*mean[j+2]/(wt_sum*wt_sum);
}

double Pulsar::ProfilePCA::get_pc_value(unsigned i)
{
  if (i>2*nharm_pca)
    throw Error (InvalidParam, "Pulsar::ProfilePCA::get_pc_value",
        "requested out of range component (%d)", i);

  if (pc_values.size()==0) 
    compute();

  return(pc_values[i]);
}

Pulsar::Profile*
Pulsar::ProfilePCA::get_pc_vector(int i, unsigned nbin) 
{
  if ((i>=0) && ((unsigned)i>2*nharm_pca))
    throw Error (InvalidParam, "Pulsar::ProfilePCA::get_pc_vector",
        "requested out of range component (%d)", i);

  Profile *prof = new Profile(nbin);
  float *fprof = new float[nbin+2];
  for (unsigned ii=0; ii<nbin+2; ii++) fprof[ii]=0.0;

  // Component -1 is mean prof
  if (i<0) {
    for (unsigned ii=0; ii<2*nharm_pca; ii++) 
      fprof[ii+2] = mean[ii+2]/wt_sum;
    FTransform::bcr1d(nbin, prof->get_amps(), fprof);
    delete [] fprof;
    return(prof);
  }

  if (pc_values.size()==0) 
    compute();

  for (unsigned ii=0; ii<2*nharm_pca; ii++) {
    fprof[ii+2] = pc_vectors[2*i*nharm_pca + ii];
  }
  FTransform::bcr1d(nbin, prof->get_amps(), fprof);
  // TODO : set other Profile attributes?
  delete [] fprof;
  return(prof);
}

std::vector<double> 
Pulsar::ProfilePCA::decompose(const Profile *p, unsigned n_pc)
{
  if (n_pc>2*nharm_pca)
    throw Error (InvalidParam, "Pulsar::ProfilePCA::decompose",
        "requested out of range component (%d)", n_pc);

  if (pc_values.size()==0) 
    compute();

  // FFT input profile
  unsigned nfbins = p->get_nbin() + 2;
  float *fprof = new float[nfbins];
  FTransform::frc1d(p->get_nbin(), fprof, p->get_amps());

  std::vector<double> result;
  result.resize(n_pc);
  unsigned limit = (nfbins-2 < 2*nharm_pca) ? nfbins-2 : 2*nharm_pca;
  for (unsigned ipc=0; ipc<n_pc; ipc++) {
    result[ipc] = 0.0;
    for (unsigned ibin=0; ibin<limit; ibin++) {
      result[ipc] += fprof[ibin+2] * pc_vectors[2*ipc*nharm_pca + ibin];
    }
  }

  return(result);
}

void Pulsar::ProfilePCA::unload(const std::string& filename)
{
  // FITS status
  int status=0;

  // Create FITS file (overwrite if existing).
  fitsfile *f;
  char fname[256];
  sprintf(fname, "!%s", filename.c_str());
  if (fits_create_file(&f, fname, &status)) 
    throw FITSError (status, "Pulsar::ProfilePCA::unload",
        "fits_create_file(%s)", filename.c_str());

  // Primary HDU:
  // Basic keywords, cov matrix in image format
  fits_movabs_hdu(f, 1, NULL, &status);
  long naxes[2];
  naxes[0]=naxes[1]=2*nharm_cov;
  if (fits_create_img(f, DOUBLE_IMG, 2, naxes, &status))
    throw FITSError (status, "Pulsar::ProfilePCA::unload",
        "fits_create_img");
  fits_write_key(f, TUINT, "NHARMCOV", &nharm_cov, NULL, &status);
  fits_write_key(f, TUINT, "NHARMPCA", &nharm_pca, NULL, &status);
  fits_write_key(f, TUINT, "NPROF", &nprof, NULL, &status);
  fits_write_key(f, TDOUBLE, "WT_SUM", &wt_sum, NULL, &status);
  fits_write_key(f, TDOUBLE, "WT2_SUM", &wt2_sum, NULL, &status);
  // TODO : more info like source name, freq, etc..
  if (status) 
    throw FITSError (status, "Pulsar::ProfilePCA::unload",
        "fits_write_key");

  // COV matrix
  naxes[0]=naxes[1]=1;
  if (fits_write_pix(f, TDOUBLE, naxes, 4*nharm_cov*nharm_cov, 
        cov, &status))
    throw FITSError (status, "Pulsar::ProfilePCA::unload",
        "fits_write_pix");

  // Bintable extension with mean profile, and PCs (if computed)
  char *ttype[] = {"PC_VALUE", "PC_VECTOR"};
  char *tform[] = {"D", "1D"};
  if (fits_create_tbl(f, BINARY_TBL, 0, 2, ttype, tform, NULL, "PCA", 
        &status))
    throw FITSError (status, "Pulsar::ProfilePCA::unload",
        "fits_create_tbl");
  if (fits_modify_vector_len(f, 2, 2*nharm_cov, &status)) 
    throw FITSError (status, "Pulsar::ProfilePCA::unload",
        "fits_modify_vector_len");
  if (fits_write_comment(f, "First row contains unnormalized mean profile.", 
      &status))
    throw FITSError (status, "Pulsar::ProfilePCA::unload",
        "fits_write_comment");
  double dtmp=0.0;
  fits_write_col(f, TDOUBLE, 1, 1, 1, 1, &dtmp, &status);
  fits_write_col(f, TDOUBLE, 2, 1, 1, 2*nharm_cov, &mean[2], &status);
  if (pc_values.size()>0) {
    for (unsigned i=0; i<pc_values.size(); i++) {
      dtmp = pc_values[i];
      fits_write_col(f, TDOUBLE, 1, i+2, 1, 1, &dtmp, &status);
      fits_write_col(f, TDOUBLE, 2, i+2, 1, 2*nharm_pca, 
          &pc_vectors[2*nharm_pca*i], &status);
    }
  }
  if (status) 
    throw FITSError (status, "Pulsar::ProfilePCA::unload",
        "fits_write_col");

  if (fits_close_file(f, &status)) 
    throw FITSError (status, "Pulsar::ProfilePCA::unload",
        "fits_close_file");
}

Pulsar::ProfilePCA* Pulsar::ProfilePCA::load(const std::string& filename)
{
  // Open file
  int status=0;
  fitsfile *f;
  if (fits_open_file(&f, filename.c_str(), READONLY, &status))
    throw FITSError (status, "Pulsar::ProfilePCA::load",
        "fits_open_file(%s)", filename.c_str());

  // Init new pca object
  Reference::To<ProfilePCA> pca = new ProfilePCA;

  // Main HDU params
  unsigned uitmp;
  fits_movabs_hdu(f, 1, NULL, &status);
  fits_read_key(f, TUINT, "NHARMCOV", &uitmp, NULL, &status);
  pca->set_nharm_cov(uitmp);  // allocs mem, etc.
  fits_read_key(f, TUINT, "NHARMPCA", &pca->nharm_pca, NULL, &status);
  fits_read_key(f, TUINT, "NPROF", &pca->nprof, NULL, &status);
  fits_read_key(f, TDOUBLE, "WT_SUM", &pca->wt_sum, NULL, &status);
  fits_read_key(f, TDOUBLE, "WT2_SUM", &pca->wt2_sum, NULL, &status);
  // TODO : more info like source name, freq, etc..
  if (status) 
    throw FITSError (status, "Pulsar::ProfilePCA::load",
        "fits_read_key");

  // COV matrix
  long fpixel[2] = {1,1};
  if (fits_read_pix(f, TDOUBLE, fpixel, 4*pca->nharm_cov*pca->nharm_cov,
        NULL, pca->cov, NULL, &status))
    throw FITSError (status, "Pulsar::ProfilePCA::load",
        "fits_read_pix(cov)");

  // move to PCA table
  if (fits_movnam_hdu(f, BINARY_TBL, "PCA", 0, &status)) 
    throw FITSError (status, "Pulsar::ProfilePCA::load",
        "fits_movnam_hdu(pca)");

  // Read mean vector
  int vec_col=0, val_col=0;
  fits_get_colnum(f, CASEINSEN, "PC_VECTOR", &vec_col, &status);
  fits_read_col(f, TDOUBLE, vec_col, 1, 1, 2*pca->nharm_cov, 
      NULL, &pca->mean[2], NULL, &status);
  if (status) 
    throw FITSError (status, "Pulsar::ProfilePCA::load",
        "fits_read_col(mean)");

  // Read components
  long num_pc=0;
  double dbltmp;
  fits_get_num_rows(f, &num_pc, &status);
  num_pc--;
  if (num_pc>0) {
    pca->pc_values.resize(num_pc);
    pca->pc_vectors = new double[4*pca->nharm_pca*pca->nharm_pca];
    fits_get_colnum(f, CASEINSEN, "PC_VALUE", &val_col, &status);
    for (int i=0; i<num_pc; i++) {
      fits_read_col(f, TDOUBLE, val_col, i+2, 1, 1, NULL, &dbltmp, NULL, 
          &status);
      pca->pc_values[i] = dbltmp;
      fits_read_col(f, TDOUBLE, vec_col, i+2, 1, 2*pca->nharm_pca, 
          NULL, &pca->pc_vectors[2*pca->nharm_pca*i], NULL, &status);
    }
    if (status) 
      throw FITSError (status, "Pulsar::ProfilePCA::load",
          "fits_read_col(pca)");
  }

  // All done
  if (fits_close_file(f, &status)) 
    throw FITSError (status, "Pulsar::ProfilePCA::load",
        "fits_close_file");

  return pca.release();
}
