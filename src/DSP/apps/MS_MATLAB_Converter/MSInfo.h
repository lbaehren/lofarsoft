#ifndef MSInfo_H
#define MSInfo_H

#include <string>
#include <vector>

#include <casa/Arrays.h>
#include <casa/aipstype.h>
#include <casa/complex.h>
#include <casa/BasicMath/Math.h>

#include "Antenna.h"

class Antenna;

class MSInfo
{
	public:
		MSInfo();
		~MSInfo();

		int nPolarizations;
		
		std::string sourceMS;
		int nChannels;

		casa::Vector<double> frequencies;

		int nAntennae;
		std::vector<Antenna> antennae;
				
		int nTimeSlots;
		std::vector<double> timeSlots;
		
		//int nPolarizations;
		bool validInfo;
};

#endif
