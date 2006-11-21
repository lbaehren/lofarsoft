#include <iostream>
#include <aomath/MAFilter.h>

/*!
	\file tMAFilter.cc
 
	\brief A number of tests for the MSFilter class
 
	This will test the basic functionality of the methods implemented by the
	MAFilter class.
 */

/*!
	\brief Main program function
 */
int main () {
	
	int retval (0);
	int size (100);

	std::cout << "[1] Testing constructor ..." << std::endl;
	{
		MAFilter<int> filterInt (size);
		MAFilter<unsigned int> filterUInt (size);
		MAFilter<short> filterShort (size);
		MAFilter<unsigned short> filterUShort (size);
		MAFilter<float> filterFloat (size);
		MAFilter<float> filterDouble (double);
	}
	
	std::cout << "[2] Testing variable feedback ..." << std::endl;
	{
		MAFilter<float> filter (size);
		
		std::cout << " - length      = " << filter.GetLength()     << std::endl;
		std::cout << " - max. length = " << filter.GetMaxLength()  << std::endl;
		std::cout << " - sum         = " << filter.Sum()           << std::endl;
		std::cout << " - mean        = " << filter.Mean()          << std::endl;
	}
	
	std::cout << "[3] Testing adding of data ..." << std::endl;
	{
		MAFilter<float> filter (size);
		float data (2.0);
		
		std::cout << " - length      = " << filter.GetLength()     << std::endl;
		std::cout << " - max. length = " << filter.GetMaxLength()  << std::endl;
		std::cout << " - sum         = " << filter.Sum()           << std::endl;
		std::cout << " - mean        = " << filter.Mean()          << std::endl;
		
		for (int n(1); n<20; n++) {
			filter.Add (data);
			
			std::cout
				<< "\t" << n
				<< "\t" << filter.GetLength()
				<< "\t" << filter.GetMaxLength()
				<< "\t" << filter.Sum()
				<< "\t" << filter.Mean()
				<< std::endl;
		}
	}
	
	return retval;
}
