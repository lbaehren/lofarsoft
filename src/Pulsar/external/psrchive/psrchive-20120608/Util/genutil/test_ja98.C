#include "JenetAnderson98A5.h"
#include <iostream>
using namespace std;

int main ()
{
  JenetAnderson98 ja98;

  double mean_variance = ja98.A4( ja98.get_mean_Phi() );

  cerr << "<sigma^2> = " << mean_variance << endl;
  cerr << "<Phi> = " << ja98.get_mean_Phi() << endl;

  unsigned ipt, npt = 256;
  double min_variance = mean_variance * 0.95;
  double max_variance = mean_variance * 1.05;

  double interval = (max_variance - min_variance) / (npt - 1);

  vector<float> mark2_A (npt);
 
  for (ipt = 0; ipt < npt; ipt++) {
    double variance = min_variance + ipt * interval;
    ja98.set_sigma_n( variance / mean_variance );
    mark2_A[ipt] = ja98.get_A ();
  }

  vector<float> mark3_A (npt);
 
  for (ipt = 0; ipt < npt; ipt++) {
    double variance = min_variance + ipt * interval;
    double Phi = ja98.invert_A4 ( variance );
    ja98.set_Phi (Phi);
    mark3_A[ipt] = ja98.get_A ();
  }

  vector<float> mark4_A (npt);
 
  for (ipt = 0; ipt < npt; ipt++) {
    double variance = min_variance + ipt * interval;
    double Phi = ja98.invert_A14 ( variance );
    ja98.set_Phi (Phi);
    mark4_A[ipt] = ja98.get_A ();
  }

  vector<float> mark5_A (npt);
 
  JenetAnderson98::EquationA5 a5;
  a5.set_nsamp(512);

  for (ipt = 0; ipt < npt; ipt++) {
    double variance = min_variance + ipt * interval;
    double Phi = a5.invert ( variance );
    ja98.set_Phi (Phi);
    mark5_A[ipt] = ja98.get_A ();
  }

  cout.precision(20);
  for (ipt = 0; ipt < npt; ipt++) {
    double variance = min_variance + ipt * interval;
    cout << variance/mean_variance
	 << " " << 1-mark2_A[ipt]
	 << " " << 1-mark3_A[ipt]
	 << " " << 1-mark4_A[ipt]
 	 << " " << 1-mark5_A[ipt] << endl;
  }

  return 0;
}
