#include <stdio.h>
#include <stdlib.h>
#include <strstream>

class DETECTOR
{
	private:
		float X_Cor[100], Y_Cor[100], Z_Cor[100] ;
		float Xmin,Xmax,Ymin,Ymax ;

	public:
		void Read_Cord(char*,int) ;
		float Get_XCord(int) ;
		float Get_YCord(int) ;
		float Get_ZCord(int) ;
		float Get_Xmin() ;
		float Get_Xmax() ;
		float Get_Ymin() ;
		float Get_Ymax() ;
} ;
