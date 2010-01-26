//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __linchi2_h_
#define __linchi2_h_

#include <vector>

// A simple linear chi-squared fit routine. Fits the given data
// to a line of the form y = a + b*x
// Parameters:
//
//    x, y          : the coordinates of the points
//    sig           : the std deviations of the points
//    siga and sigb : the respective probable uncertainties
//                    associated with a and b
//    chi2          : the chi-squared value
//    q             : goodness of fit probability
//
// If mwt is false, the standard deviations in y are assumed to be
// unknown. q is returned as 1 and chi2 is normalised to unit
// standard deviation on all points.

void lin_chi2_fit(const std::vector<double>& x, const std::vector<double>& y, 
                  const std::vector<double>& sig, 
		  bool mwt, double& a, double& b, double& siga, double& sigb, 
		  double& chi2, double& q);

#endif
