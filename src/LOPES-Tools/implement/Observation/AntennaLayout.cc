
#include <Observation/AntennaLayout.h>

namespace LOPES {  // Namespace LOPES -- begin
  
  // ============================================================================
  //  Construction
  
  AntennaLayout::AntennaLayout ()
  {;}
  
  AntennaLayout::AntennaLayout (const String& tableFilename,
				const String& telescope,
				const Quantity& epoch)
  {
    obsInfo_p.setTelescope (telescope);
    obsInfo_p.setObsDate (epoch);
  }
  
  AntennaLayout::AntennaLayout (const String& tableFilename,
				const ObsInfo& obsInfo)
    : obsInfo_p(obsInfo)
  {;}
  
  
  // ============================================================================
  //  
  
  void AntennaLayout::setEpoch (const MEpoch& epoch) 
  {
    obsInfo_p.setObsDate (epoch);
  }
  
  void AntennaLayout::setTelescope (const String& telescope)
  {
    obsInfo_p.setTelescope (telescope);
  }
  
  // ============================================================================
  // 
  
  Vector<double> AntennaLayout::antennaPosition (const int& antenna)
  {
    Vector<double> position;
    
    return position;
  }
  
}  // Namespace LOPES -- end
