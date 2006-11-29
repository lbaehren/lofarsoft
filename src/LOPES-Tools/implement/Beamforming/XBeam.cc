
#include <Beamforming/XBeam.h>

using std::cout;
using std::cerr;
using std::endl;

namespace LOPES {  // namespace LOPES -- begin
  
  // =============================================================================
  //
  //  Construction
  //
  // =============================================================================
  
  XBeam::XBeam ()
    : method_p(method3),
      sigma_p (1.0),
      blockaverage_p(3)
  {}
  
  XBeam::XBeam (XBeam::method which,
		const double& sigma,
		const int& blockaverage)
    : method_p(which),
      sigma_p (sigma),
      blockaverage_p(blockaverage)
  {}
  
  
  // =============================================================================
  //
  //  Destruction
  //
  // =============================================================================
  
  XBeam::~XBeam () {}
  
  
  // =============================================================================
  //
  //  Access to the computation parameters
  //
  // =============================================================================
  
  XBeam::method XBeam::methodType ()
  {
    return method_p;
  }
  
  String XBeam::methodName ()
  {
    return XBeam::methodName(method_p);
  }
  
  String XBeam::methodName (XBeam::method method)
  {
    switch (method) {
    case method1: return String ("method1");
    case method2: return String ("method2");
    case method3: return String ("method3");
    case method4: return String ("method4");
    default: 
      cerr << "[XBeam::methodName] Unsupported method" << endl;
    }
    return String ("UNKNOWN");
  }
  
  
  // =============================================================================
  //
  //  Computation of the X-Beam
  //
  // =============================================================================
  
  Vector<double> XBeam::xbeam (const Matrix<double>& antennaData)
  {
    bool verbose (true);
    IPosition shape (antennaData.shape());
    Vector<double> powerAC (shape(1));
    Vector<double> powerCC (shape(1));
    int nofCC (0);
    int nofAC (0);
    int ant1 (0);
    int ant2 (0);
    
    for (ant1=0; ant1<shape(0)-1; ant1++) {
      for (ant2 = ant1+1; ant2<shape(0); ant2++) {
	powerCC += antennaData.row(ant1)*antennaData.row(ant2);
	nofCC++;
      }
      powerAC += antennaData.row(ant1)*antennaData.row(ant1);
      nofAC++;
    }
    
    // do not forget about the last AC contribution:
    powerAC += antennaData.row(shape(0)-1)*antennaData.row(shape(0)-1);
    
    // normalization of the sums
    {
      Vector<double> norm (powerCC.shape());
      //
      norm = nofCC;
      powerCC /= norm;
      //
      norm = nofAC;
      powerAC /= norm;
    }
    
    // Get statistical properties
    double meanCC (mean(powerCC));
    double meanAC (mean(powerAC));
    double stddevCC (stddev(powerCC));
    double stddevAC (stddev(powerAC));
    
    if (verbose) {
      cout << "[XBeam::xbeam]" << endl;
      cout << " - Auto-correlation power - mean    : " << meanAC << endl;
      cout << " - Auto-correlation power - stddev  : " << stddevAC << endl;
      cout << " - Cross-correlation power - mean   : " << meanCC << endl;
      cout << " - Cross-correlation power - stddev : " << stddevCC << endl;
    }
    
    return powerCC;
  }
  
}  // namespace LOPES -- end
