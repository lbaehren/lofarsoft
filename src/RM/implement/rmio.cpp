/* 	Implementation of RM I/O functions mainly needed for testing
	
		File:				rmio.cpp
		Author:			Sven Duscha (sduscha@mpa-garching.mpg.de)
		Date:				01-08-2009
		Last change:	02-08-2009
*/

#include <iostream>				// C++/STL iostream
#include <fstream>				// file stream I/O
#include "rmio.h"

using namespace std;

/*!
  \brief Read the distribution of measured frequencies from a text file
  
  \param filename -- name of txt file with frequency distribution

  \return frequencies -- vector with frequencies
*/
vector<double> rmio::readFrequencies(const std::string &filename)
{
  vector<double> frequencies;		// hold list of lambda squareds
  double frequency;			// individual lambda squared read per line

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use dalFITS table
  {
    // TODO
    // use dalFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rmio::readFrequencies failed to open file";
    }
  
	 unsigned int i=0;
    while(infile.good())	// as long as we can read from the file...
    {
      infile >> frequency;	// read double into temporary variable

		if(frequencies.size() > i)					// if there is sufficient space in frequencies vector...
			frequencies[i]=frequency;				// write to index i
		else												// otherwise		
      	frequencies.push_back (frequency);	// push back into lambdaSquareds vector
    }

    infile.close();		// close the text file
  }

  return frequencies;	 // return frequencies vector
}


/*!
  \brief Read the distribution of measured frequencies from a text file
  
  \param filename -- name of txt file with frequency distribution
  \param deltafreqs - vector to take delta frequencies (computed from difference)

  \return frequencies -- vector with frequencies
*/
vector<double> rmio::readFrequenciesDiffFrequencies(const std::string &filename, vector<double> &deltafreqs)
{
  vector<double> frequencies;		// hold list of lambda squareds
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
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use dalFITS table
  {
    // TODO
    // use dalFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rmio::readFrequencies failed to open file";
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

  return frequencies;	 // return frequencies vector
}



/*!
  \brief Read the distribution of measured lambda squareds from a text file
  
  \param filename -- name of txt file with lambda squared distribution

  \return lambdaSquareds -- vector with lambda squareds
*/
vector<double> rmio::readLambdaSquareds(const std::string &filename)
{
  vector<double> lambdaSquareds;	// hold list of lambda squareds
  double lambdaSq;			// individual lambda squared read per line

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use dalFITS table
  {
    // TODO
    // use dalFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rmio::readLambdaSquareds failed to open file";
    }
  
	 unsigned int i=0;		// vector index variable
    while(infile.good())	// as long as we can read from the file...
    {
      infile >> lambdaSq;	// read double into temporary variable
		
		if(lambdaSquareds.size() > i)					// if lambdaSquareds has sufficient size
			lambdaSquareds[i]=lambdaSq;				// write to index i in vector
		else
      	lambdaSquareds.push_back (lambdaSq);	// push back into lambdaSquareds vector
    }

    infile.close();		// close the text file
  }

  return lambdaSquareds;	 // return frequencies vector
}



/*!
  \brief Read the distribution of measured lambdaSquareds AND deltaLambdaSquareds from a text file
  
  \param filename - name of txt file with lambda squared distribution
  \param deltaFrequencies - vector to keep delta Frequencies

  \return lambdaSquareds - vector with frequencies and delta frequencies
*/
vector<double> rmio::readFrequenciesAndDeltaFrequencies(const std::string &filename, vector<double> &deltaFrequencies)
{
  double frequency=0;			// individual frequency read per line
  double deltaFrequency=0;		// individual delta frequency read per line  
  vector<double> frequencies;		// frequencies to be returned

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use dalFITS table
  {
    // TODO
    // use dalFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rmio::readFrequenciesAndDeltaFrequencies failed to open file";
    }

    unsigned int i=0;						// vector index variable if vector has sufficient size
    while(infile.good())					// as long as we can read from the file...
    {
      infile >> frequency;					// read frequency (first column)
      infile >> deltaFrequency;			// read delta Frequency (2nd coloumn)
 
		if(frequencies.size() > i)								// if frequencies vector has sufficient size
			frequencies[i]=frequency;							// store at index i
		else
     		frequencies.push_back (frequency);				// if delta frequencies vector has sufficient size
		if(deltaFrequencies.size() > i)						// store at index i
			deltaFrequencies[i]=deltaFrequency;
		else
      	deltaFrequencies.push_back (deltaFrequency);		// store in delta frequencies vector
    }

    infile.close();		// close the text file
  }

  return frequencies;	  // return frequencies vector
}


/*!
  \brief Read the distribution of measured lambdaSquareds AND deltaLambdaSquareds from a text file
  
  \param filename - name of txt file with lambda squared and delta lambda squared distribution
  \param deltaLambdaSquareds - vector with delta lambda squareds

  \return lambdaSquareds - vector with lambda squareds
*/
vector<double> rmio::readLambdaSquaredsAndDeltaSquareds(const std::string &filename,   vector<double> &deltaLambdaSquareds)
{
  vector<double> lambdaSquareds;		// lambda squareds to be returned
  double lambdaSq=0;						// individual frequency read per line
  double deltaLambdaSq=0;				// individual delta frequency read per line  

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use dalFITS table
  {
    // TODO
    // use dalFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rmio::readLambdaSquaredsAndDeltaSquareds failed to open file";
    }
  
	 int i=0;									// vector index variable if vector has sufficient size
    while(infile.good())					// as long as we can read from the file...
    {
      infile >> lambdaSq;					// read frequency (first column)
      infile >> deltaLambdaSq;			// read delta Frequency (2nd coloumn)

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
	      deltaLambdaSquareds.push_back (deltaLambdaSq);	// store in delta frequencies vector
		}
    }

    infile.close();		// close the text file
  }

  return lambdaSquareds;	  // return frequencies vector
}


/*!
	\brief Read lambda squareds, delta lambda squareds and complex data vector from a text file
	
	\param &filename - name of text file with simulated polarized emission data
	\param &lambdasquareds - vector to store lambda squared values in
	\param &delta_lambda_squareds - vector to store delta lambda squared values in
	\param &intensities				-	vector<complex<double> > to store complex polarized intensities
*/
void rmio::readSimDataFromFile(const std::string &filename, vector<double> &lambdasquareds, vector<double> &delta_lambda_squareds, vector<complex<double> > &intensities)
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
      throw "rn::readSimDataFromFile failed to open file";
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
  \brief Write a vector (RM) out to file on disk (mainly for debugging)
  
  \param rm - vector containing data (real double) to write to file
  \param filename - name of file to create or append to
  \param mode - write mode: overwrite, append
*/
void rmio::writeRMtoFile(vector<double> rm, const std::string &filename)
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
void rmio::writeRMtoFile(vector<complex<double> > rm, const std::string &filename)
{
  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(unsigned int i=0; i<rm.size(); i++)		// loop over vector
  {
    outfile << rm[i].real() << "   ";			// write real part of data
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
void rmio::writeRMtoFile(vector<double> &lambdasq, vector<complex<double> > &rm, const std::string &filename)
{
  if(lambdasq.size()!=rm.size())
	 throw "rmio::writeRMtoFile lambdasq and rm vector differ in size";
	
  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(unsigned int i=0; i<rm.size(); i++)		// loop over vector
  {
	 outfile << lambdasq[i] << "   ";			// write lambda squareds 
    outfile << rm[i].real() << "   ";			// write real part of data
	 outfile << rm[i].imag();						// write imaginary part of data
    outfile << endl;									// add endl
  }

  outfile.flush();						// flush output file
}
