
#include <iostream>				// C++/STL iostream
#include <fstream>				// file stream I/O
#include "rmFITS.h"
#include "rmIO.h"

using namespace std;

namespace RM {  //  BEGIN -- namespace RM

  //_____________________________________________________________________________
  //                                                              readFrequencies
  
  /*!
    \brief Read the distribution of measured frequencies from a text file
    
    \param filename -- name of txt file with frequency distribution
    \param frequencies -- vector with frequencies
  */
  void rmIO::readFrequencies(const std::string &filename, vector<double> &frequencies)
  {
    //  vector<double> frequencies;		// hold list of lambda squareds
    double frequency;			// individual lambda squared read per line
    
    //----------------------------------------------------------
    // Check if filename is text file FITS file or HDF5 file
    if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
      {
	// TODO
	// use dal to read lambda Squareds and deltaLambdaSquareds from file
      }
    else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
      {
	// TODO
	// use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
      }
    else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
      {
	ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
	
	if(infile.fail())
	  {
	    throw "rmIO::readFrequencies failed to open file";
	  }
	
	//	 unsigned int i=0;
	frequencies.resize(0);
	while(infile.good())	// as long as we can read from the file...
	  {
	    infile >> frequency;	// read double into temporary variable
	    frequencies.push_back (frequency);	// push back into lambdaSquareds vector
	  }
	
	infile.close();		// close the text file
      }
  }
  
  //_____________________________________________________________________________
  //                                               readFrequenciesDiffFrequencies
  
  /*!
    \brief Read the distribution of measured frequencies from a text file
    
    \param filename -- name of txt file with frequency distribution
    \param frequencies -- vector with frequencies
    \param deltafreqs - vector to take delta frequencies (computed from difference)
  */
  void rmIO::readFrequenciesDiffFrequencies (const std::string &filename, 
					     vector<double> &frequencies,
					     vector<double> &deltafreqs)
  {
    //  vector<double> frequencies;		// hold list of lambda squareds
    double frequency=0;				// individual lambda squared read per line
    double prev_frequency=0;			// value of previous frequency read
    double difference=0;					// difference between current and previous frequency
    
    //----------------------------------------------------------
    // Check if filename is text file FITS file or HDF5 file
    if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
      {
	// TODO
	// use dal to read lambda Squareds and deltaLambdaSquareds from file
      }
    else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
      {
	// TODO
	// use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
      }
    else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
      {
	ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
	
	if(infile.fail())
	  {
	    throw "rmIO::readFrequencies failed to open file";
	  }
	
	unsigned int i=0;					// vector index variable
	while(infile.good())				// as long as we can read from the file...
	  {
	    infile >> frequency;				// read double into temporary variable
	    
	    if(frequencies.size() > i)					// if there is sufficient space in frequencies vector...
	      frequencies[i]=frequency;				// write to index i
	    else												// otherwise
	      frequencies.push_back (frequency);	// push back into lambdaSquareds vector
	    
	    if(prev_frequency!=0)
	      {
		difference=frequency-prev_frequency;
		deltafreqs.push_back (difference);			// write into delta vector
	      }
	    prev_frequency=frequency;							// keep as previous frequency
	  }
	deltafreqs.push_back (difference);					// write last diff into delta vector
	
	infile.close();		// close the text file
      }
    
    //  return frequencies;	 // return frequencies vector
  }
  
  
  
  /*!
    \brief Read the distribution of measured lambda squareds from a text file
    
    \param filename -- name of txt file with lambda squared distribution
    \param lambdaSquareds -- vector with lambda squareds
  */
void rmIO::readLambdaSquareds(const std::string &filename, vector<double> &lambdaSquareds)
{
//  vector<double> lambdaSquareds;	// hold list of lambda squareds
  double lambdaSq;			// individual lambda squared read per line

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  {
    // TODO
    // use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rmIO::readLambdaSquareds failed to open file";
    }
  
//	 unsigned int i=0;		// vector index variable
    lambdaSquareds.resize(0);
    while(infile.good())	// as long as we can read from the file...
    {
      infile >> lambdaSq;	// read double into temporary variable
     	lambdaSquareds.push_back (lambdaSq);	// push back into lambdaSquareds vector
    }

    infile.close();		// close the text file
  }

//  return lambdaSquareds;	 // return frequencies vector
}



/*!
  \brief Read the distribution of measured lambdaSquareds AND deltaLambdaSquareds from a text file
  
  \param filename - name of txt file with lambda squared distribution
  \param frequencies - vector with frequencies
  \param deltaFrequencies - vector to keep delta Frequencies
*/
void rmIO::readFrequenciesAndDeltaFrequencies(	const std::string &filename,
 																vector<double> &frequencies,
																vector<double> &deltaFrequencies)
{
  double frequency=0;			// individual frequency read per line
  double deltaFrequency=0;		// individual delta frequency read per line  
//  vector<double> frequencies;		// frequencies to be returned

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  {
    // TODO
    // use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rmIO::readFrequenciesAndDeltaFrequencies failed to open file";
    }

//    unsigned int i=0;						// vector index variable if vector has sufficient size
    frequencies.resize(0); 
	deltaFrequencies.resize(0);

    while(infile.good())					// as long as we can read from the file...
    {
      infile >> frequency;					// read frequency (first column)
      infile >> deltaFrequency;			// read delta Frequency (2nd coloumn)
 
		/*
		if(frequencies.size() > i)								// if frequencies vector has sufficient size
			frequencies[i]=frequency;							// store at index i
		else
     		frequencies.push_back (frequency);				// if delta frequencies vector has sufficient size
		if(deltaFrequencies.size() > i)						// store at index i
			deltaFrequencies[i]=deltaFrequency;
		else
      	deltaFrequencies.push_back (deltaFrequency);		// store in delta frequencies vector
    	*/
	}

    infile.close();		// close the text file
  }

//  return frequencies;	  // return frequencies vector
}


/*!
  \brief Read the distribution of measured lambdaSquareds AND deltaLambdaSquareds from a text file
  
  \param filename - name of txt file with lambda squared and delta lambda squared distribution
  \param lambdaSquareds - vector with lambda squareds
  \param deltaLambdaSquareds - vector with delta lambda squareds
*/
void rmIO::readLambdaSquaredsAndDeltaSquareds(	const std::string &filename,  
																vector<double> &lambdaSquareds,
 																vector<double> &deltaLambdaSquareds)
{
//  vector<double> lambdaSquareds;		// lambda squareds to be returned
  double lambdaSq=0;						// individual frequency read per line
  double deltaLambdaSq=0;				// individual delta frequency read per line  

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  {
    // TODO
    // use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rmIO::readLambdaSquaredsAndDeltaSquareds failed to open file";
    }
  
	 lambdaSquareds.resize(0);
	 deltaLambdaSquareds.resize(0);
//	 int i=0;									// vector index variable if vector has sufficient size
    while(infile.good())					// as long as we can read from the file...
    {
      infile >> lambdaSq;					// read frequency (first column)
      infile >> deltaLambdaSq;			// read delta Frequency (2nd coloumn)

		lambdaSquareds.push_back(lambdaSq);
	   deltaLambdaSquareds.push_back (deltaLambdaSq);	// store in delta lambda squared vector
		/*
		if(static_cast<int>(lambdaSquareds.size()) > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
		{ 
			lambdaSquareds[i]=lambdaSq;
		}
		else
		{
			lambdaSquareds.push_back (lambdaSq);		// store in frequencies vector
		}	
		if(static_cast<int>(deltaLambdaSquareds.size()) > i)
		{
			deltaLambdaSquareds[i]=deltaLambdaSq;	
		}	
		else
		{
	      deltaLambdaSquareds.push_back (deltaLambdaSq);	// store in delta lambda squared vector
		}
		*/
    }

    infile.close();		// close the text file
  }

//  return lambdaSquareds;	  // return frequencies vector
}


/*!
	\brief Read lambda squareds, delta lambda squareds and complex data vector from a text file
	
	\param &filename - name of text file with simulated polarized emission data
	\param &lambdasquareds - vector to store lambda squared values in
	\param &delta_lambda_squareds - vector to store delta lambda squared values in
	\param &intensities				-	vector<complex<double> > to store complex polarized intensities
*/
void rmIO::readSimDataFromFile(	const std::string &filename, 
											vector<double> &lambdasquareds, 
											vector<double> &delta_lambda_squareds, 
											vector<complex<double> > &intensities)
{
	ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds
	
	double lambdasq=0;		// temporary variable for lambda sqaured per line
   double deltalambdasq=0;	// temporary variable for delta lambda squared per line
	double real=0;				// temporary variable for real part per line
	double imag=0;				// temporary variable for imaginary part per line
	complex<double> intensity;					// temporary complex intensity
	int i=0;					// loop variable
	
	// Temporary vectors
//	vector<complex<double> > intensities;

   if(infile.fail())
   {
      throw "rmIO::readSimDataFromFile failed to open file";
   }
	
	while(infile.good())
	{
		infile >> lambdasq >> deltalambdasq >> real >> imag;


		if(static_cast<int>(lambdasquareds.size()) > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
		{
			lambdasquareds[i]=lambdasq;
			delta_lambda_squareds[i]=deltalambdasq;
			intensities[i]=complex<double>(real, imag);		
		}
		else		// otherwise use push back function to append at the end of the vector
		{
		   lambdasquareds.push_back(lambdasq);
			delta_lambda_squareds.push_back(deltalambdasq);			
			intensities.push_back(complex<double>(real, imag));		
		}

		i++;										// increment index into data vector
	}
	
	infile.close();
}


/*!
	\brief Read a complex RMSF from a file
	
	\param rmsf - complex vector containing rmsf intensities
	\param filename - file to read from (can be FITS/HDF5 or txt file)
*/
void rmIO::readRMSFfromFile(	vector<complex<double> > &rmsf,
										const string &filename)
{
	double real=0, imag=0;		// real and imaginary part of RMSF intensity to read per line
	unsigned int i=0;				// index / loop variable
	
  	//----------------------------------------------------------
  	// Check if filename is text file FITS file or HDF5 file
  	if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  	{
    	// TODO
    	// use dal to read lambda Squareds and deltaLambdaSquareds from file
  	}
  	else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  	{
    	// TODO
    	// use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  	}
  	else if(filename.find(".txt", 1)!=string::npos || filename.find(".dat", 1)!=string::npos)	// if it is text file
  	{
		ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds and RMSF complex intensities	

	   if(infile.fail())
	   {
	      throw "rmIO::readRMSFfromFile failed to open file";
	   }
	
		while(infile.good())
		{
			infile >> real >>  imag;
			
			if(static_cast<unsigned int>(rmsf.size()) > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
			{
				rmsf[i]=complex<double>(real, imag);		
			}
			else		// otherwise use push back function to append at the end of the vector
			{
				rmsf.push_back(complex<double>(real, imag));		
			}

			i++;										// increment index into data vector
		}
	
		infile.close();
	}
	else		// otherwise, if file extension was not recognized
	{
		throw "rmIO::readRMSFfromFile file extension was not recognized";
	}
}


/*!
	\brief Read a complex RMSF and its corresponding lambdasquareds from a file
	
	\param faradaydepths - vector with faraday depths the RMSF is based on
	\param rmsf - complex vector containing rmsf intensities
	\param filename - file to read from (can be FITS/HDF5 or txt file)
*/
void rmIO::readRMSFfromFile(	vector<double> &faradaydepths,
										vector<complex<double> > &rmsf,
										const string &filename)
{
	double faraday=0, real=0, imag=0;		// faraday,  real and imaginary part of RMSF intensity to read per line
	unsigned int i=0;								// index / loop variable
	
  	//----------------------------------------------------------
  	// Check if filename is text file FITS file or HDF5 file
  	if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  	{
    	// TODO
    	// use dal to read lambda Squareds and deltaLambdaSquareds from file
  	}
  	else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  	{
    	// TODO
    	// use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  	}
  	else if(filename.find(".txt", 1)!=string::npos || filename.find(".dat", 1)!=string::npos)	// if it is text file
  	{
		ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds and RMSF complex intensities	

	   if(infile.fail())
	   {
	      throw "rmIO::readRMSFfromFile failed to open file";
	   }
	
		while(infile.good())
		{
			infile >> faraday >> real >>  imag;

			if(static_cast<unsigned int>(rmsf.size()) > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
			{
				faradaydepths[i]=faraday;		
			}
			else		// otherwise use push back function to append at the end of the vector
			{
				faradaydepths.push_back(faraday);		
			}
			
			if(static_cast<unsigned int>(rmsf.size()) > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
			{
				rmsf[i]=complex<double>(real, imag);		
			}
			else		// otherwise use push back function to append at the end of the vector
			{
				rmsf.push_back(complex<double>(real, imag));		
			}

			i++;										// increment index into data vector
		}
	
		infile.close();
	}
	else		// otherwise, if file extension was not recognized
	{
		throw "rmIO::readRMSFfromFile file extension was not recognized";
	}
}


/*!
 \brief Read a vector<double> from a file
 
 \param v - vector to read into
 \param filename - name of file to read from (FITS, HDF5 or TXT)
*/
void rmIO::readVectorFromFile(std::vector<double> &v, const std::string &filename)
{
	double value=0;					// value to read per line
	
  	//----------------------------------------------------------
  	// Check if filename is text file FITS file or HDF5 file
  	if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  	{
    	// TODO
    	// use dal to read lambda Squareds and deltaLambdaSquareds from file
  	}
  	else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  	{
    	// TODO
    	// use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  	}
  	else if(filename.find(".txt", 1)!=string::npos || filename.find(".dat", 1)!=string::npos)	// if it is text file
  	{
		ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds and RMSF complex intensities	
		
		if(infile.fail())
		{
			throw "rmIO::readVectorFromFile failed to open file";
		}
		
		v.clear();
		while(infile.good())
		{
			infile >>  value;	
			v.push_back(value);		
		}
		
		infile.close();
	}
	else		// otherwise, if file extension was not recognized
	{
		throw "rmIO::readVectorFromFile file extension was not recognized";
	}
}


/*!
 \brief Read two real vectors from a file
 
 \param vec1 - real vector 1 to read from first column of file
 \param vec2 - real vector 1 to read from second column of file
 \param filename - file to read from (can be FITS/HDF5 or txt file)
*/
void rmIO::read2VectorsFromFile(vector<double> &vec1,
								vector<double> &vec2,
								const string &filename)
{
	double value1=0, value2=0;		// value1 (1st column) and value2 (2nd column) to read per line
	unsigned int i=0;				// index / loop variable
	
  	//----------------------------------------------------------
  	// Check if filename is text file FITS file or HDF5 file
  	if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  	{
    	// TODO
    	// use dal to read lambda Squareds and deltaLambdaSquareds from file
  	}
  	else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  	{
    	// TODO
    	// use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  	}
  	else if(filename.find(".txt", 1)!=string::npos || filename.find(".dat", 1)!=string::npos)	// if it is text file
  	{
		ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds and RMSF complex intensities	
		
		if(infile.fail())
		{
			throw "rmIO::readRMSFfromFile failed to open file";
		}
		
		while(infile.good())
		{
			infile >>  value1 >>  value2;
			
			if(static_cast<unsigned int>(vec1.size()) > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
			{
				vec1[i]=value1;		
			}
			else		// otherwise use push back function to append at the end of the vector
			{
				vec1.push_back(value1);		
			}
			
			if(static_cast<unsigned int>(vec2.size()) > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
			{
				vec2[i]=value2;		
			}
			else		// otherwise use push back function to append at the end of the vector
			{
				vec2.push_back(value2);		
			}
			
			i++;										// increment index into data vector
		}
		
		infile.close();
	}
	else		// otherwise, if file extension was not recognized
	{
		throw "rmIO::readRMSFfromFile file extension was not recognized";
	}
}


/*!
 \brief Write two real vectors to a file

 \param vec1 - real vector 1 to write to first column of file
 \param vec2 - real vector 1 to write to second column of file 
 \param filename - fileanme to write to (can be FITS/HDF5 or txt file)
*/
void rmIO::write2VectorsToFile(const vector<double> &vec1, const vector<double> &vec2, const string &filename)
{
	// Check data integrity
	if(vec1.size()==0)
		throw "rmIO::write2VectorsToFile vec1 has size 0";
	if(vec2.size()==0)
		throw "rmIO::write2VectorsToFile vec2 has size 0";
	
	
	//----------------------------------------------------------
	// Check if filename is text file FITS file or HDF5 file
	if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
	{
  		// TODO
		// use dal to read lambda Squareds and deltaLambdaSquareds from file
	}
	else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
	{
		// TODO
		// use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
	}
	else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
	{
		
		ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);		// file with lambda squareds and RMSF complex intensities	
		
		if(outfile.fail())
		{
			throw "rmIO::writeRMSFtoFile failed to open file";
		}
		
		for(unsigned int i=0; i < vec1.size(); i++)
		{
			outfile << vec1[i] << "\t";
			outfile << vec2[i] << endl;
		}
		
		outfile.flush();
	}	
}


/*!
	\brief Write a complex RMSF to a file

	\param rmsf - complex vector with RMSF intensities in Q and U
	\param filename - fileanme to write to (can be FITS/HDF5 or txt file)
*/
void rmIO::writeRMSFtoFile(const vector<complex<double> > &rmsf, const string &filename)
{
	// Check data integrity
	if(rmsf.size()==0)
		throw "rmIO::writeRMSFtoFile";
		
	//----------------------------------------------------------
	// Check if filename is text file FITS file or HDF5 file
	if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
	{
  		// TODO
	   // use dal to read lambda Squareds and deltaLambdaSquareds from file
	}
	else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
	{
		// TODO
	   // use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
	}
	else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
	{
	
		ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);		// file with lambda squareds and RMSF complex intensities	

		if(outfile.fail())
		{
			throw "rmIO::writeRMSFtoFile failed to open file";
		}
	
		for(unsigned int i=0; i < rmsf.size(); i++)
		{
			outfile << rmsf[i].real() << "\t";
			outfile << rmsf[i].imag() << endl;
		}
		
		outfile.flush();
	}	
}


/*!
	\brief Write a complex RMSF and its corresponding lambdasquareds to a file

	\param faradaydepths - vector with faraday depths the RMSF is based on
	\param rmsf - complex vector with RMSF intensities in Q and U
	\param filename - fileanme to write to (can be FITS/HDF5 or txt file)
*/
void rmIO::writeRMSFtoFile(	const vector<double> &faradaydepths,
							const vector<complex<double> > &rmsf, 
							const string &filename)
{
	// Check data integrity
	if(rmsf.size()==0)
		throw "rmIO::writeRMSFtoFile";
		
	//----------------------------------------------------------
	// Check if filename is text file FITS file or HDF5 file
	if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
	{
  		// TODO
	   // use dal to read lambda Squareds and deltaLambdaSquareds from file
	}
	else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
	{
		// TODO
	   // use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
	}
	else if(filename.find(".txt", 1)!=string::npos || filename.find(".dat", 1)!=string::npos)	// if it is text file
	{
	
		ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);		// file with lambda squareds and RMSF complex intensities	

		if(outfile.fail())
		{
			throw "rmIO::writeRMSFtoFile failed to open file";
		}
	
		for(unsigned int i=0; i < rmsf.size(); i++)
		{
			outfile << faradaydepths[i] << "\t";
			outfile << rmsf[i].real() << "\t";
			outfile << rmsf[i].imag() << endl;
		}
		
		outfile.flush();
	}
	else
		throw "rmIO::writeRMSFtoFile unrecognized file extension";
}


/*!
  \brief Write a vector (RM) out to file on disk (mainly for debugging)
  
  \param rm - vector containing data (real double) to write to file
  \param filename - name of file to create or append to
  \param mode - write mode: overwrite, append
*/
void rmIO::writeRMtoFile(const vector<double> &rm, const std::string &filename)
{
  unsigned int i=0;	// loop variable
 
  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(i=0; i<rm.size(); i++)		// loop over vector
  {
    outfile << rm[i];				// write out data
    outfile << endl;					// add endl
  }

  outfile.flush();					// flush output file
}


/*!
  \brief Write a vector (RM) out to file on disk (mainly for debugging)
  
  \param rm - vector containing data (real double) to write to file
  \param filename - name of file to create or append to
*/
void rmIO::writeRMtoFile(const vector<complex<double> > &rm, const std::string &filename)
{
  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(unsigned int i=0; i<rm.size(); i++)		// loop over vector
  {
    outfile << rm[i].real() << "\t";			// write real part of data
	 outfile << rm[i].imag();						// write imaginary part of data
    outfile << endl;									// add endl
  }

  outfile.flush();						// flush output file
}


/*!
  \brief Write a vector (RM) out to file on disk (mainly for debugging)
  
  \param lambdasq - vector containing the lambda squared wavelengths
  \param rm - vector containing data (real double) to write to file
  \param filename - name of file to create or append to
*/
void rmIO::writeRMtoFile(const vector<double> &lambdasq, 
						 const vector<complex<double> > &rm, 
						 const std::string &filename)
{
  if(lambdasq.size()!=rm.size())
	 throw "rmIO::writeRMtoFile lambdasq and rm vector differ in size";
	
  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(unsigned int i=0; i<rm.size(); i++)		// loop over vector
  {
	 outfile << lambdasq[i] << "\t";			// write lambda squareds 
    outfile << rm[i].real() << "\t";			// write real part of data
	 outfile << rm[i].imag();						// write imaginary part of data
    outfile << endl;									// add endl
  }

  outfile.flush();						// flush output file
}


/*!
	\brief Write complex polarized intensities along side frequencies to file (generated from forward Transform)

	\param frequencies - vector containing frequencies the polarized intensities are given
	\param polint - vector containing complex polarized intensities
	\param filename - name of text file to write to
*/
void rmIO::writePolIntToFile(const std::vector<double> &frequencies, 
							 const std::vector<std::complex<double> > &polint, 
							 const std::string &filename)
{
  if(frequencies.size()!=polint.size())
	 throw "rmIO::writePolIntToFile frequencies and polint vector differ in size";

  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(unsigned int i=0; i<polint.size(); i++)		// loop over vector
  {
	 outfile << frequencies[i] << "\t";		// write lambda squareds 
    outfile << polint[i].real() << "\t";		// write real part of data
	 outfile << polint[i].imag();					// write imaginary part of data
    outfile << endl;									// add endl
  }

  outfile.flush();									// flush output file
}


/*!
	\brief Write single polarized intensities (Q or U) along side frequencies to file (generated from forward Transform)

	\param frequencies - vector containing frequencies the polarized intensities are given
	\param intensities - vector containing single polarized intensities (Q or U)
	\param filename - name of text file to write to
*/
void rmIO::writeIntToFile(const std::vector<double> &frequencies, 
						  const std::vector<double> &intensities, 
						  const std::string &filename)
{
  if(frequencies.size()!=intensities.size())
	 throw "rmIO::writePolIntToFile frequencies and polint vector differ in size";

  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(unsigned int i=0; i<intensities.size(); i++)		// loop over vector
  {
	 outfile << frequencies[i] << "\t";					// write lambda squareds 
    outfile << intensities[i];								// write real part of data
    outfile << endl;												// add endl
  }

  outfile.flush();						// flush output file
}



//*************************************************************************************
//
// Image cube functions (internally only FITS implemented at first)
//
//**************************************************************************************


/*!
	\brief Check if the image cube is in correct format for RM-Synthesis

	\param filename - name of image cube to check
	\param hdu - optional give a HDU to check (default=1)
	
	\return check - true if image format is correct, false if not
*/
bool rmIO::checkImageCube(const std::string &filename, int hdu=1)
{
	if(filename.size()==0 || filename=="")
		throw "rmIO::checkImageCube no filename provided";
		
	//----------------------------------------------------------
	// Check if filename is text file FITS file or HDF5 file
	if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
	{
  		// TODO
	}
	else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
	{
		// Open FITS image
		
		
		// 3 axis

		// 32-Bit double format (-32)

		// complex value for P=Q+iU
	}	
	else
	{
		throw "rmIO::checkImageCube unknown file extension";
	}
	
	return false;
}


/*
	\brief Check if Q image cube is in correct format for RM-Synthesis

	\param filename - name of image cube to check
	\param hdu - optional give a HDU to check (default=1)
	
	\return check - true if image format is correct, false if not
*/
/*
bool rmIO::checkImageCubeQ(const std::string &filename, int hdu=1)
{
	if(filename.size()==0 || filename=="")
		throw "rmIO::checkImageCube no filename provided";
			
	//----------------------------------------------------------
	// Check if filename is text file FITS file or HDF5 file
	if(filename.find(".hdf5", 1)!=string::npos || filename.find(".HDF5", 1)!=string::npos)			// if HDF5 use dal
	{
  		// TODO
	}
	else if(filename.find(".fits", 1)!=string::npos || filename.find(".FITS", 1)!=string::npos)	// if FITS file  use rmFITS
	{
		DAL::rmFITS fitsimage(filename, READONLY);				// create and open rmFITS image object
		
		if(fitsimage.getImgDim()!=3)							// must have 3 axes
			throw "rmIO::checkImageCubeQ image cube has wrong dimensions";
		
		if(fitsimage.getImgType()!=-32) 						// 32-Bit double format (-32)
			throw "rmIO::checkImageCubeQ pixel values are not in 32 bit";

		fitsimage.close();										// close FITS image

		return true;
	}	
	else
	{
		throw "rmIO::checkImageCube unknown file extension";
	}	
	
	return false;
}
*/


/*
	\brief Check if U image cube is in correct format for RM-Synthesis

	\param filename - name of image cube to check
	\param hdu - optional give a HDU to check (default=1)
	
	\return check - true if image format is correct, false if not
*/
/*
bool rmIO::checkImageCubeU(const std::string &filename, int hdu=1)
{
	if(filename.size()==0 || filename=="")
		throw "rmIO::checkImageCube no filename provided";
		
	//----------------------------------------------------------
	// Check if filename file FITS file or HDF5 file
	if(filename.find(".hdf5", 1)!=string::npos || filename.find(".HDF5", 1)!=string::npos)				// if HDF5 use dal
	{
  		// TODO
	}
	else if(filename.find(".fits", 1)!=string::npos || filename.find(".FITS", 1)!=string::npos)		// if FITS file  use rmFITS 
	{
		DAL::rmFITS fitsimage(filename, READONLY);			// create and open rmFITS image object

		if(fitsimage.getImgDim()!=3)			// must have 3 axes
			throw "rmIO::checkImageCubeU image cube has wrong dimensions";
		
		if(fitsimage.getImgType()!=-32) 		// 32-Bit double format (-32)
			throw "rmIO::checkImageCubeU pixel values are not in 32 bit";

		fitsimage.close();										// close FITS image

		return true;
	}	
	else
	{
		throw "rmIO::checkImageCube unknown file extension";
	}
	
	return false;
}
*/


/*!
	\brief Read a list of files
	
  	\param filename - text file containing input list with 2-D FITS files
	\param list - vector of strings to contain 2-D FITS filenames
*/
void rmIO::readFileList(const string &listfilename, vector<string> &list)
{
        string filenameitem;    // local variable for filename

        ifstream infile(listfilename.c_str(), ifstream::in);    // input filestream

		filenameitem.resize(100);                       // resize filenameitem string (needed on OS X)
        list.clear();				// empty vector

        if(infile.fail())
			throw "fitsmerge::readList failed to open file";

        while(infile.good())                            // as long as we can read from the text file
        {
          infile >> filenameitem;                 // read one line into filename    
		  list.push_back(filenameitem);           // push back into vector
		}

      infile.close();
}
  
}  //  END -- namespace RM
