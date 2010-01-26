/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Fourier.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::PolnProfile* Pulsar::fourier_transform (const PolnProfile* input,
						FTransform::Plan* plan) try
{
  return new PolnProfile( input->get_basis(), input->get_state(),
			  fourier_transform(input->get_Profile(0),plan),
			  fourier_transform(input->get_Profile(1),plan),
			  fourier_transform(input->get_Profile(2),plan),
			  fourier_transform(input->get_Profile(3),plan) );
}
catch (Error& error) {
  throw error += "Pulsar::fourier_transform (PolnProfile)";
}

Pulsar::PolnProfile*
Pulsar::complex_fourier_transform (const PolnProfile* in,
				   FTransform::Plan* plan) try
{
  return new PolnProfile( in->get_basis(), in->get_state(),
			  complex_fourier_transform(in->get_Profile(0),plan),
			  complex_fourier_transform(in->get_Profile(1),plan),
			  complex_fourier_transform(in->get_Profile(2),plan),
			  complex_fourier_transform(in->get_Profile(3),plan) );
}
catch (Error& error) {
  throw error += "Pulsar::complex_fourier_transform (PolnProfile)";
}

Pulsar::Profile* Pulsar::fourier_transform (const Profile* input,
					    FTransform::Plan* plan) try
{
  Reference::To<Profile> fourier = new Profile (input->get_nbin() + 2);

  if (plan)
    plan->frc1d  (input->get_nbin(), fourier->get_amps(), input->get_amps());
  else
    FTransform::frc1d (input->get_nbin(),
		       fourier->get_amps(), input->get_amps());

  if (FTransform::get_norm() == FTransform::unnormalized)
    fourier->scale( 1.0 / sqrt(double(input->get_nbin())) );

  return fourier.release();
}
catch (Error& error) {
  throw error += "Pulsar::fourier_transform (Profile)";
}

Pulsar::Profile* Pulsar::complex_fourier_transform (const Profile* input,
						    FTransform::Plan* plan)
try
{
  unsigned nbin = input->get_nbin();

  Reference::To<Profile> fourier = new Profile (nbin * 2);

  // initialize vector to zero
  vector<float> complex_input( nbin*2, 0.0 );

  const float* real_input = input->get_amps();

  for (unsigned ibin=0; ibin<nbin; ibin++)
    complex_input[2*ibin] = real_input[ibin];

  if (plan)
    plan->fcc1d  (nbin, fourier->get_amps(), &(complex_input[0]));
  else
    FTransform::fcc1d(nbin, fourier->get_amps(), &(complex_input[0]));

  if (FTransform::get_norm() == FTransform::unnormalized)
    fourier->scale( 1.0 / sqrt(double(input->get_nbin())) );

  return fourier.release();
}
catch (Error& error) {
  throw error += "Pulsar::fourier_transform (Profile)";
}


void Pulsar::detect (PolnProfile* data) try
{
  unsigned npol = 4;
  for (unsigned ipol=0; ipol<npol; ipol++)
    detect( data->get_Profile(ipol) );
}
catch (Error& error) {
  throw error += "Pulsar::detect (PolnProfile)";
}


void Pulsar::detect (Profile* data) try
{
  unsigned nbin = data->get_nbin() / 2;
  float* amps = data->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++) {
    float re = amps[ibin*2];
    float im = amps[ibin*2+1];
    amps[ibin] = re*re + im*im;
  }

  data->resize(nbin);

  if (data->get_amps() != amps)
    throw Error (InvalidState, "Pulsar::detect (Profile)",
		 "Profile::resize does not preserve amps array");

}
catch (Error& error) {
  throw error += "Pulsar::detect (Profile)";
}
