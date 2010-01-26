
#include <fstream>
#include <iostream>
#include <vector>

float plotpa0,plotRM,plotRM_err;

std::vector<double> goodfreqs;
std::vector<double> goodpa;
std::vector<double> goodpa_stddev;

std::vector<double> goodi;
std::vector<double> goodirms;
std::vector<double> goodv;
std::vector<double> goodvrms;
std::vector<double> goodq;
std::vector<double> goodqrms;
std::vector<double> goodu;
std::vector<double> goodurms;

std::vector<double> goodl;
std::vector<double> goodlrms;


std::vector<double> delta_V;
std::vector<double> delta_Verr;
std::vector<double> delta_L;
std::vector<double> delta_Lerr;


void set_plotparams(float _plotpa0,float _plotRM,float _plotRM_err){
 
 plotpa0=_plotpa0;
 plotRM=_plotRM;
 plotRM_err=_plotRM_err;

}

std::vector<float> fbscr_RMs;
std::vector<float> fbscr_RM_errs;
std::vector<float> fbscr_RM_probmax;
//std::vector<float> fbscr_RM_chisqmin;

float best_fbscr_probmax;
//float best_fbscr_chisqmin;
float best_fbscr_RM;
float best_fbscr_RM_err;

bool outofrange_err;
bool zero_err;
bool anomalous_err;
 
float lthresh;
//bool good_lthresh;
bool good_fbscrunch;
bool plotv;

float minrm;
float maxrm;
double rmstep;
unsigned rmsteps;

float lookup_PA_err(float xint);

float lookup_RM_err(std::vector<float>& xint);

std::vector<int> ibin_vec;

void Kasterg_Fit(float& fRM_low,float& fRM_high,float& fRM_step,
                 const std::vector<double>& freqs,
                 const std::vector<double>& PAs,
		 const std::vector<double>& PA_stddevs,
                 double& RM,double& PA0,double& sigRM,double& sigPA0,
		 double& probmax,int& iRM);

