/*! 	
	\class rmio
	
	\ingroup RM
	
	\brief I/O helper class for RM

	\author Sven Duscha (sduscha@mpa-garching.mpg.de)

	\date 02.08.09.
	
	\test trmio.cc
	
	<h3>Prerequisites</h3>
	
	<ul type="square">
	<li> rm
	</ul>

	<h3>Synopsis</h3>
	
	The need for testing several vector based RM algorithm and CLEAN classes
	demands for standardized vector I/O (this is currently present in the rm class)
	But due to branching of coding to different classes that only later get migrated
	into the development tree, this I/O class seems appropriate.

	It provides vector<double> and vector<complex<double> > reading and writing I/O.
	Most of the functions have been taken from the rm class.

	<h3>Example(s)<h3>

*/


#ifndef RMIO_H
#define RMIO_H

#include <vector>
#include <complex>

class rmio{
private:
	
	
public:
  //*************************************************************************************
  //
  // Read frequency / lambda squared distributions from text (or later FITS/HDF5) files
  //
  //**************************************************************************************

  //! read frequency distribution from a text file
  std::vector<double> readFrequencies(const std::string &);

  //! read lambda squareds from a text file
  std::vector<double> readLambdaSquareds(const std::string &);

  //! read frequencies from file and compute delta frequencies from differences
  std::vector<double> readFrequenciesDiffFrequencies(	const std::string &filename, 
																		std::vector<double> &deltafreqs);

  //! read frequencies and delta frequencies from a text file
  std::vector<double> readFrequenciesAndDeltaFrequencies(const std::string &, 
																			std::vector<double> &);

  //! read lambda squareds and delta squareds from a text file
  std::vector<double> readLambdaSquaredsAndDeltaSquareds(const std::string &, 
																			std::vector<double> &);

  //! read complete set of simulated data: lambdasq, delta lambda sq and complex intensity from file
  void readSimDataFromFile(const std::string &filename, 
									std::vector<double> &lambdasquareds, 
									std::vector<double> &delta_lambda_squareds, 
									std::vector<std::complex<double> > &intensities);

  //! read a complex RMSF from a file
  void readRMSFfromFile(	std::vector<std::complex<double> > &rmsf,
									const std::string &filename);

  //! write a complex RMSF to a file
  void writeRMSFtoFile( std::vector<std::complex<double> > &rmsf,
								const std::string &filename);

  //! Write a vector out to file
  void writeRMtoFile(std::vector<double>, 
							const std::string &filename);

  //! Write a complex vector out to a file
  void writeRMtoFile(std::vector<std::complex<double> > rm, 
							const std::string &filename);

  //! Write the lambda squareds ann the complex vector rm out to a file
  void writeRMtoFile(std::vector<double> &lambdasq, 
							std::vector<std::complex<double> > &rm, 
							const std::string &filename);
	
  //! Write single polarized intensity (Q or U) to file along with frequencies
  void writePolIntToFile(	std::vector<double> &frequencies, 
								 	std::vector<std::complex<double> > &polint, 
									const std::string &filename);

  //! Write complex polarized intensity to file along with frequencies
  void writeIntToFile(	std::vector<double> &frequencies, 
								std::vector<double> &intensities, 
								const std::string &filename);
};

#endif		// _RMIO_H_
