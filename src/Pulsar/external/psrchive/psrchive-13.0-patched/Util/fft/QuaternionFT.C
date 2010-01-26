/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "QuaternionFT.h"

#include "FTransform.h"

using namespace std;

bool QuaternionFT::verbose = false;

//! Null constructor
QuaternionFT::QuaternionFT ()
{
  // default axis
  for (unsigned i=0; i<3; i++)
    mu[i][i+1] = 1;

  // default direction
  sign = -1;

  extent = 1;
  power = 1;

  set_xform ();
}

//! Construct with spherical coordinates of imaginary axis
QuaternionFT::QuaternionFT (double theta, double phi)
{
  set_axis (theta, phi);
  sign = -1;
  extent = 1;
  power = 1;
}

//! Set the imaginary axis
void QuaternionFT::set_axis (Vector<3, float> axis1)
{
  // normalize the vector
  axis1 /= sqrt (axis1*axis1);

  // find the most perpendicular basis vector
  unsigned best_basis = 0;
  float minimum_projection = 1.0;
  
  for (unsigned basis=0; basis<3; basis++) {
    float projection = fabs(axis1 * Vector<3, float>::basis(basis));
    if (projection < minimum_projection) {
      minimum_projection = projection;
      best_basis = basis;
    }
  }

  cerr << "Most perpendicular basis = " << best_basis << endl;

  Vector<3, float> axis2 = cross (axis1, Vector<3, float>::basis(best_basis));
  axis2 /= sqrt (axis2*axis2);

  Vector<3, float> axis3 = cross (axis1, axis2);
  axis3 /= sqrt (axis3*axis3);

  // unit vectors defined by sperical coordinates theta and phi
  mu[0] = Quaternion<float> (0, axis1);
  mu[1] = Quaternion<float> (0, axis2);
  mu[2] = Quaternion<float> (0, axis3);

  set_xform ();
}

//! Set the imaginary axis
void QuaternionFT::set_axis (double theta, double phi)
{
  double cos_th = cos (theta);
  double sin_th = sin (theta);
  double cos_phi = cos (phi);
  double sin_phi = sin (phi);

  // unit vectors defined by sperical coordinates theta and phi
  mu[0] = Quaternion<float> (0, cos_th*cos_phi, sin_th*cos_phi, sin_phi);
  mu[1] = Quaternion<float> (0, -sin_th, cos_th, 0);
  mu[2] = Quaternion<float> (0, -cos_th*sin_phi, -sin_th*sin_phi, cos_phi);

  set_xform ();
}

//! Get the imaginary axis
Vector<3, float> QuaternionFT::get_axis () const
{
  return mu[0].get_vector();
}

void QuaternionFT::set_direction (Direction direction)
{
  if (direction == Forward)
    sign = -1;
  else
    sign = 1;
}

void QuaternionFT::set_power (double _power)
{
  power = _power;
}

void QuaternionFT::set_extent(double _extent)
{
  extent = _extent;;
}

void QuaternionFT::set_xform ()
{
  if (verbose) {
    cerr << "QuaternionFT::set_xform";
    for (unsigned i=0; i<3; i++)
      cerr << "\n    mu[" << i << "]=" << mu[i];
    cerr << "\n    mu[0]*mu[1]=" << mu[0]*mu[1] << endl;
  }

  Matrix <3, 3, float> inv_xform;

  for (unsigned i=0; i<3; i++)
    for (unsigned j=0; j<3; j++)
      inv_xform[i][j] = mu[j][i+1];

  xform = inv (inv_xform);
}

void QuaternionFT::fft (const vector<Quaternion<float> >& i,
			vector<Quaternion<float> >& o)
{
  o.resize (i.size());
  fft (i.size(), &(i[0]), &(o[0]));
}

void QuaternionFT::fft (unsigned npt, const Quaternion<float>* in,
			Quaternion<float>* out)
{
  const float* input = (const float*) in;
  float* output = (float*) out;
  fft (npt, input, output);
}

//! Perform a fast quaternion fourier transform
/*! This method implements the procedure described in:

   Efficient implementation of quaternion Fourier transform, convolution, 
   and correlation by 2-D complex FFT.  
   Pei, Soo-Chang; Ding, Jian-Jiun; Chang, Ja-Han  
   IEEE Trans. Signal Process. 49 (2001), no. 11, 2783--2797, Math. Sci. Net.  

*/
void QuaternionFT::fft (unsigned npts, const float* input, float* output)
{
  if (verbose)
    cerr << "QuaternionFT::fft (float*)" << endl;

  float* h_a = new float [npts * 2];
  float* h_b = new float [npts * 2];

  const float* in = input;
  float* h_at = h_a;
  float* h_bt = h_b;

  // convert i,j,k to mu[0], mu[1], mu[2]
  Vector<3, float> input_unreal;
  Vector<3, float> h_unreal;

  if (verbose)
    cerr << "QuaternionFT::fft xform=" << xform << endl;

  unsigned ipt=0;
  for (ipt=0; ipt < npts; ipt++) {

    // copy the real part - start Equation 33
    *h_at = *in;  h_at ++;
    in ++;

    // transform the unreal part into the current basis - Equation 31
    for (unsigned ii=0; ii<3; ii++) {
      input_unreal[ii] = *in;
      in ++;
    }
    h_unreal = xform * input_unreal;

    // decompose the unreal part - end Equation 33
    *h_at = h_unreal[0];  h_at++;
    *h_bt = h_unreal[1];  h_bt++;

    // note: the Quaternion class implements a left-handed quaternion,
    // which means that there is a sign flip in k
    *h_bt = -h_unreal[2];  h_bt++;

  }

  // The rest of this method implements Equation 38  

  float* H_a = new float [npts * 2];
  float* H_b = new float [npts * 2];

  if (sign < 0) {
    FTransform::fcc1d (npts, H_a, h_a);
    FTransform::fcc1d (npts, H_b, h_b);
  }
  else {
    FTransform::bcc1d (npts, H_a, h_a);
    FTransform::bcc1d (npts, H_b, h_b);
  }

  Quaternion<float> result;
  float* out = output;

  h_at = H_a;
  h_bt = H_b;

  for (ipt=0; ipt < npts; ipt++) {
    result = *h_bt;          h_bt ++;
    result += *h_bt * mu[0]; h_bt ++;

    result *= mu[1];

    result += *h_at;         h_at ++;
    result += *h_at * mu[0]; h_at ++;

    for (unsigned i=0; i<4; i++)
      out[i] = result[i];

    out += 4;
  }

  delete H_a;
  delete H_b;
  delete h_a;
  delete h_b;
}

Quaternion<float> exp (const Quaternion<float>& axis, float phi)
{
  float sin_phi = sin (phi);
  float cos_phi = cos (phi);

  return Quaternion<float> (cos_phi, sin_phi*axis.s1,
			     sin_phi*axis.s2, sin_phi*axis.s3);
}

void QuaternionFT::ft (const vector<Quaternion<float> >& input,
			vector<Quaternion<float> >& output)
{
  output.resize (input.size());
  ft (input.size(), &(input[0]), &(output[0]));
}

//! Perform a slow quaternion fourier transform
void QuaternionFT::ft (unsigned npts, const float* input, float* output)
{
  const Quaternion<float>* in  = (const Quaternion<float>*) (input);
  Quaternion<float>* out = (Quaternion<float>*) (output);

  ft (npts, in, out);
}

//! Perform a slow quaternion fourier transform
void QuaternionFT::ft (unsigned npts, const Quaternion<float>* in,
		       Quaternion<float>* out)
{
  if (verbose)
    cerr << "QuaternionFT::ft (Quaternion<float>*)" << endl;

  unsigned ifr, it;

  float theta;

  Quaternion<double> acc;
  Quaternion<float> rot;

  for (ifr=0; ifr < npts; ifr++)  {
    acc = 0.0;
    for (it=0; it < npts; it++)  {
      theta = sign *2.0*M_PI *float(ifr)*it/float(npts);
      rot = exp (mu[0],theta);
      acc += rot * in[it];
    }
    out[ifr] = acc;
  }
}

#if 0

SPECIAL WAY OF DOING THINGS:

//! Perform a slow quaternion fourier transform
void QuaternionFT::ft (unsigned npts, const Quaternion<float>* in,
                       Quaternion<float>* out)
{
  if (verbose)
    cerr << "QuaternionFT::ft (Quaternion<float>*)" << endl;

  unsigned ifr, it;

  float theta;
  float scale = 1.0/npts; ///sqrt(npts);

  Quaternion<double> acc;
  Quaternion<float> rot;

  float t;
  float powscale = 1.0/(power*extent); // so d/dit[t^power] ~= 1/npts

  for (ifr=0; ifr < npts; ifr++)  {
    acc = 0.0;
    for (it=0; it < npts; it++)  {
      t = 1.0 + extent*(it - 0.5*npts)/npts;
      theta = sign *2.0*M_PI
        * (float(ifr)-0.5*npts)// npts/2 : do -ve to +ve frqs
        * (pow(t, float(power))-1.0) * powscale; // -1: ref phase to center
//       cerr << it << " " << theta << " " << (pow(t, float(power))-1.0) * powscale        << endl;
      rot = exp (mu[0],theta);
       acc += rot * in[it]; // * herm(rot);
//       acc += exp (mu[0],0.5*theta) * in[it] * exp (mu[0],-0.5*theta);
    }
//        exit(1);
    out[ifr] = acc * double(scale);
  }
}

#endif

