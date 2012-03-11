/***************************************************************************
 *
 *   Copyright (C) 2005 by Stephen Ord
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/VelocityModel.h"
#include <math.h>

using namespace std;

MEAL::VelocityModel::VelocityModel()  : parameters(this,8) {
  
    parameters.set_name (0,"scaling");
    parameters.set_name (1,"vpar");
    parameters.set_name (2,"vperp");
    parameters.set_name (3,"inc");
    parameters.set_name (4,"omega");
    parameters.set_name (5,"ecc");
    parameters.set_name (6,"pb");
    parameters.set_name (7,"a");

    parameters.set_infit (0,true);
    parameters.set_infit (1,true);
    parameters.set_infit (2,true);
    parameters.set_infit (3,false);
    parameters.set_infit (4,false);
    parameters.set_infit (5,false);
    parameters.set_infit (6,false);
    parameters.set_infit (7,false);

}
string MEAL::VelocityModel::get_name() const {

    return "VelocityModel";

}

void MEAL::VelocityModel::set_scaling(double _scale) {

   set_param(0,_scale);

}

double MEAL::VelocityModel::get_scaling () const {

   return get_param(0);

}

void MEAL::VelocityModel::set_vpar(double _vpar) {

   set_param(1,_vpar);

}

double MEAL::VelocityModel::get_vpar() const {

   return get_param(1);

}

void MEAL::VelocityModel::set_vperp(double _vperp) {

   set_param(2,_vperp);

}

double MEAL::VelocityModel::get_vperp() const {

   return get_param(2);

}

void MEAL::VelocityModel::set_incl(double _inclination) {
   // Inclination not set as fitable parameter because I can't evaluate the partial
   // Perhaps try a numerical differentiator?

   set_param(3,_inclination);
}

double MEAL::VelocityModel::get_incl() const {

   return get_param(3);

}

void  MEAL::VelocityModel::set_omega(double _omega) {

   // According to my notes there is a nasty covariance involving omega
   // I haven't checked this out yet. Just going to try and get the model
   // into the new scheme then i'll worry about the particulars

   set_param(4,_omega);

}

double MEAL::VelocityModel::get_omega () const {

   return get_param(4);

}

void MEAL::VelocityModel::set_ecc(double _ecc) {
   
   set_param(5,_ecc);
}

double MEAL::VelocityModel::get_ecc() const {

   return get_param(5);
}

void MEAL::VelocityModel::set_pb(double period) {

   set_param(6,period);

}

double MEAL::VelocityModel::get_pb() const {

   return get_param(6);

}

void MEAL::VelocityModel::set_a(double _a) {

   set_param(7,_a);

}

double MEAL::VelocityModel::get_a() const {

   return get_param(7);

}

void MEAL::VelocityModel::calculate_velocities (double theta, double& theta_vel, double& rad_vel) {

 
    // This is the routine that calculates the velocity in terms of a theta vel and radial vel
    // in the frame of the binary

    // Parameters from the model       
    double a = get_a();
    double ecc = get_ecc();
    double pb = get_pb();

    double v1,v2,v3,v4;

    bool verbose = false;
    // Not sure which to use at the moment needs checking Oct 05

    double speed_of_light = 299792458;  // m/s

    // v1 = (2*M_PI*a*_C)/sin(inclination); // <<<<< added sin(inc) as a is (x/c) sin(inc)
    v1 = (2*M_PI*a*speed_of_light); // <<<<< added sin(inc) as a is (x/c) sin(inc)
    v2 = (1-(ecc*ecc));
    v2 = sqrt(v2);
    v2 = v2*pb;
    v3 = (1+ecc*cos(theta));
    v4 = ecc*sin(theta);
  

	
    theta_vel = (v1/v2) * v3;
    rad_vel = (v1/v2) * v4;


    if (verbose) {
      cout << "Theta_vel= " << theta_vel << endl;
      cout << "Radial_vel= " << rad_vel << endl;
    }

}

void MEAL::VelocityModel::calculate (double& result, std::vector<double>* grad) {
   
    double angle;
    double theta_vel,rad_vel;

    // the current parameters
 
    double scale = get_scaling();
    double vpar = get_vpar();
    double vperp = get_vperp();
    double omega = get_omega();
    double incl = get_incl();
    double _theta = get_abscissa();

    // under normal situations we would call get_abscissa() need to check what for this will arrive in.
 
    double v1,v2,v3;
 
    angle =  (omega + _theta);
  
    calculate_velocities (_theta,theta_vel,rad_vel); 

    // ensures rad_vel and theta_vel are valid for the current _theta
    // remember this uses the inclination to calculate x ....
    
    /* 
       v1 = ((rad_vel*cos(angle)) - (theta_vel*sin(angle)) + param[1]);
       v2 = ((theta_vel*cos(angle) + rad_vel*sin(angle)) * cos(param[3])) + param[2];
    */ 
    /*
       v1 = (param[1] - (rad_vel*sin(angle) + theta_vel*cos(angle)) );
       v2 = ((theta_vel*sin(angle) - rad_vel*cos(angle))*cos(param[3])) + param[2];
    */
    
    v1 = ((theta_vel*sin(angle)) - (rad_vel*cos(angle))) + vpar;
    v2 = ((rad_vel*sin(angle) + theta_vel*cos(angle))*cos(incl)) + vperp;
    v3 = (v1*v1) + (v2*v2);
  
    double transverse = scale * sqrt(v3);
  
    if (verbose) { 
      cout << "Transverse " << transverse << endl;
      cout << "Velocity " << v3 << endl;
    }
    
    if (grad) {
      grad->resize (3);
      for (unsigned ig=0; ig<3; ig++)
        (*grad)[ig] = 0;
    }


    // Now to evaluate the partial derivatives
    (*grad)[0] = sqrt(v3); // the easy one
    
    (*grad)[1] = ((scale*scale)/transverse) * v1;

    (*grad)[2] = ((scale*scale)/transverse) * v2;
    
    /* THis is the partial for omega .. not implemented as yet
 
    double temp,temp1;
    
    temp = (theta_vel*cos(angle)) + (rad_vel*sin(angle));
    temp1 = (rad_vel*cos(angle)) - (theta_vel*sin(angle));
    
    _partials[4] =  ((param[0]*param[0])/transverse) * (v1*temp + (v2*cos(param[3])*temp1));
    
    */

    result = transverse;
    
}

 
MEAL::VelocityModel::~VelocityModel() {
  }
  
