#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*!
	\class EVENT_TIME
	\ingroup LASA
*/
class EVENT_TIME
{
	private:
		unsigned long uncorrected[4] ;	//Store uncorrected event time (in units of 0.1 nsecs)
		unsigned long corrected[4] ;	//Store corrected event time (	,,	,,	,,)

	public:
		void Sort_Increase_Order(unsigned long*) ;
		void Cal_Event_Time(unsigned int,unsigned long) ;
		unsigned long Get_Event_Time(int) ;
} ;
