#include <casa/Arrays.h>
#include <casa/aipstype.h>

#include "MSInfo.h"
#include "Antenna.h"

MSInfo::MSInfo():
	antennae(),
	timeSlots()
{
	validInfo = false;
	
	
}

MSInfo::~MSInfo()
{

}
