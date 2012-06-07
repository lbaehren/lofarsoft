//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/psrephem.h,v $
   $Revision: 1.49 $
   $Date: 2009/06/06 11:24:31 $
   $Author: straten $ */

#ifndef __PSREPHEM_H
#define __PSREPHEM_H

#include "Pulsar/Parameters.h"

#include "MJD.h"
#include "sky_coord.h"

#include "Error.h"

#include <vector>
#include <string>

namespace Legacy {

class psrephem : public Pulsar::Parameters
{
  friend class psrParams;

 public:

  //! Return a new, copy constructed instance of self
  Parameters* clone () const;

  //! Return true if *this == *that
  bool equals (const Parameters* that) const;

  //! Load from an open stream
  void load (FILE*);

  //! Unload to an open stream
  void unload (FILE*) const;

  //! Return the name of the source
  std::string get_name () const;
  
  //! Return the coordinates of the source
  sky_coord get_coordinates () const;
  
  //! Return the dispersion measure
  double get_dispersion_measure () const;
  
  //! Return the rotation measure
  double get_rotation_measure () const;

  // makes tex_descriptor return short_tex_descriptor
  static bool short_tex;

  int*    parmStatus;
  std::string* value_str;
  double* value_double;
  int*    value_integer;
  double* error_double;

  bool   tempo11;
  std::string nontempo11;

  psrephem();
  ~psrephem();

  psrephem (const psrephem &);

  psrephem (const char* filename);

  psrephem& operator = (const psrephem &);

  void zero ();

  int load   (const std::string& filename);
  int unload (const std::string& fname) const;

  int load   (std::string* str);
  int unload (std::string* str) const;

  // set fitting attributes
  void   nofit();
  void   fitall();

  // update all parameters to the new_epoch
  void   set_epoch (const MJD& new_epoch, bool binary = false);

  // multiply all errors by efac
  void   efac (float efac);

  // return the value
  std::string get_string  (int ephind);
  double get_double  (int ephind);
  MJD    get_MJD     (int ephind);
  Angle  get_Angle   (int ephind);
  int    get_integer (int ephind);

  // set value
  void set_string  (int ephind, const std::string&);
  void set_double  (int ephind, double );
  void set_MJD     (int ephind, const MJD&);
  void set_Angle   (int ephind, const Angle&);
  void set_integer (int ephind, int);

  double p() const;
  double p_err() const;

  // return some values
  std::string psrname() const;

  double get_dm() const;
  void   set_dm (double dm);
  
  double jra() const;
  double jdec() const;
  double omega() const;
  double omdot() const;
  double ecc() const;
  double t0() const;
  double a1() const;  
  double pb() const;  

  // these functions return 0 if no error, -1 on error
  //
  // return galactic latitude and longitude in radians (slalib)
  int galactic (double& l, double& b);
  // return galactic height (z) in parsec
  int galactic_z (double& z);

  // returns the acceleration along the line of sight arising
  // from differential galactic rotation in km/s
  int acc_diffrot (double& beta);

  // returns the sine of the inclination angle
  int sini (double& si, double& si_err) const;
  // returns the simple calculation based on Pb and x
  int mass_function (double& mf, double& mf_err) const;
  // returns m1 mass based on mass function, sin(i), and m2
  int m1 (double& m1, double& m1_err) const;
  // inverts the above to return m2, given m1
  int m2 (double& m2, double m1) const;

  // returns the composite proper motion
  int pm (double& pm, double& pm_err) const;
  // returns the proper motion celestial position angle
  int phi (double& phi, double& phi_err) const;

  // returns the proper motion contribution to xdot (Kopeikin 1996)
  int pm_x_dot (double& xdot, double& xdot_err) const;
  // returns the proper motion contribution to omdot in degrees per year
  int pm_omega_dot (double& omdot, double& omdot_err) const;

  // returns the orbital period in seconds
  int P (double& p, double& p_err) const;
  // returns the orbital period derivative
  int P_dot (double& pdot, double& pdot_err) const;
  // returns the second orbital period derivative in seconds^-1
  int P_ddot (double& pddot, double& pddot_err) const;

  // stability parameter of antt94
  int Delta_t (double& delta_t) const;

  // returns the transverse quadratic Doppler shift due to the apparent
  // acceleration along the line of sight that arises from proper motion
  int quadratic_Doppler (double& beta, double& beta_err) const;

  int cubic_Doppler (double& gamma, double& gamma_err,
		     double pmrv, double pmrv_err) const;

  int Doppler_P_dotdot (double& P_dotdot, double& P_dotdot_err,
			double pmrv, double pmrv_err) const;

  int Doppler_F2 (double& f2, double& f2_err,
		  double pmrv, double pmrv_err) const;

  int intrinsic_P_dotdot (double& P_dotdot, double& P_dotdot_err,
			  float braking_index=3) const;

  // returns the intrinsic x_dot due to gravitational wave emission
  int GR_x_dot (double& x_dot) const;

  // returns the intrinsic Pb_dot due to gravitational wave emission
  int GR_Pb_dot (double& Pb_dot) const;

  // returns the intrinsic Pb_dot due to gravitational wave emission
  int GR_omega_dot (double& w_dot) const;

  // given omega_dot from par file, returns the total mass
  int GR_omega_dot_mtot (double& mtot, double& mtot_err) const;

  // uses the above and sin(i) and f(M) to derive m2
  int GR_omega_dot_m2 (double& m2, double& m2_err) const;

  // returns the general relativistic prediction of gamma
  int GR_gamma (double& gamma) const;

  // used by the above
  int GR_f_e (double& f_e) const;

  // returns spin-orbit coupling rate of precession
  int GR_Omega_p (double& Omp) const;

  // //////////////////////////////////////////////////////////////////////
  // to use the following two functions, you will need to link with 
  // -lpsrinfo

  // returns a block of LaTeX formatted text suitable for use in tables
  std::string tex () const;
  // returns the LaTeX formatted pulsar name
  std::string tex_name () const;
  // returns the a LaTeX formatted string for the parameter at ephind
  std::string tex_val (int ephind, double fac=1.0, unsigned precision=1) const;

  // returns a LateX formatted string suitable for labelling the parameter
  static const char* tex_descriptor (int ephind);

  // returns a LateX formatted string suitable for short labels
  static const char* short_tex_descriptor (int ephind);

  // returns a block of LaTeX formatted text suitable for use in tables
  static std::string tex (std::vector<psrephem>& vals, bool dots=false);

  // returns the directory path in which psrinfo will be run
  std::string get_directory ();

  // this is specifically for the older style ephemeris file
  // inserts the ephemeris values into the val_str, val_double,
  // and val_int depending on the type of each ephemeris value.
  void read_old_ephem_str (std::vector<std::string>& lines, 
			   int *pstatus,
			   std::string *val_str, 
			   double *val_double, 
			   int *val_int);

 protected:

  void init ();
  void size_dataspace();
  void destroy ();
  void zero_work ();

  int old_load (const std::string& filename);
  int old_unload (const std::string& filename) const;

};

}

bool operator == (const Legacy::psrephem&, const Legacy::psrephem&);
bool operator != (const Legacy::psrephem&, const Legacy::psrephem&);

std::ostream& operator<< (std::ostream& ostr, const Legacy::psrephem& eph);

#endif

