/***************************************************************************
 *
 *   Copyright (C) 2005 by Stephen Ord
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
//-*
#ifndef __VELOCITYMODEL
#define __VELOCITYMODEL

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"
#include "MEAL/Parameters.h"

#include <vector>

namespace MEAL {
  
  class VelocityModel : public Univariate<Scalar> {
  
    public: 


      VelocityModel(); 
  
      //! Scaling constant    
      void set_scaling(double scaling);

      double get_scaling () const;

      //! Velocity parallel to the line of nodes
      void set_vpar(double vpar);

      double get_vpar () const;
      
      //! Velocity perpendicular to the line of nodes
      void set_vperp(double vperp);

      double get_vperp () const;

      //! System inclination
      void set_incl(double inclination);

      double get_incl() const;

      //! Omega
      void set_omega(double omega);

      double get_omega() const;

      //! Eccentricity
      void set_ecc(double eccentricity);

      double get_ecc() const;

      //! Binary Period
      void set_pb(double period);

      double get_pb() const;

      //! a
      void set_a(double a);

      double get_a() const;
 
      std::string get_name () const;

      ~VelocityModel();

    protected:

      void calculate (double& x, std::vector<double>* grad=0);  

      //! Velocity in the frame of the binary 

      void calculate_velocities(double phase_angle, double& theta_velocity, double& radial_velocity);
    
     
      Parameters parameters;

  };
}

#endif

