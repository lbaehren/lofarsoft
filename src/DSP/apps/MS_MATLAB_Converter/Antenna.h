#ifndef Antenna_H
#define Antenna_H

#include <string>

class Antenna
{
	public:
		Antenna();
		~Antenna();
		
		std::string name;
		double x;
		double y;
		double z;
};

#endif
