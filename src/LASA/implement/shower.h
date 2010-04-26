#include <strstream>
#include <TH1.h>
#include "constants.h"

/*!
  \class SHOWER
  \ingroup LASA
*/
class SHOWER
{
	private:
		float Det_X[NO_DETs],Det_Y[NO_DETs],Det_Z[NO_DETs] ;
		float Event_Size ;
		float x_core,y_core ;
		
	public:
		void Read_Detector_Cord() ;
		void Find_Core(long long unsigned*,float*) ;
		float Get_XCore() ;
		float Get_YCore() ;
		float Get_Size() ;
} ;
