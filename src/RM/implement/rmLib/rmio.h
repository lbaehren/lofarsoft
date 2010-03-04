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
  void readFrequencies(const std::string &, std::vector<double> &);

  //! read lambda squareds from a text file
  void readLambdaSquareds(const std::string &,	std::vector<double> &);

  //! Compute deltaLambdaSquareds from lambdaSquareds
  void computeDeltaLambdaSquareds(const std::vector<double> &, std::vector<double> &);

  //! read frequencies from file and compute delta frequencies from differences
  void readFrequenciesDiffFrequencies(	const std::string &filename,
 													std::vector<double> &frequencies,
													std::vector<double> &deltafreqs);

  //! read frequencies and delta frequencies from a text file
  void readFrequenciesAndDeltaFrequencies(const std::string &, 
														std::vector<double> &,
														std::vector<double> &);

  //! read lambda squareds and delta squareds from a text file
  void readLambdaSquaredsAndDeltaSquareds(const std::string &, 
										  std::vector<double> &,
										  std::vector<double> &);

  //! read complete set of simulated data: lambdasq, delta lambda sq and complex intensity from file
  void readSimDataFromFile(const std::string &filename, 
						   std::vector<double> &lambdasquareds, 
						   std::vector<double> &delta_lambda_squareds, 
						   std::vector<std::complex<double> > &intensities);

  //! read a complex RMSF from a file
  void readRMSFfromFile(std::vector<std::complex<double> > &rmsf,
						const std::string &filename);

  //! read a Faraday simulation Faraday depths from file and compute deltaFaradayDepths
  void readFaradaySimulationFromFile(std::vector<double> &faradayDepths,
				     std::vector<double>  &deltaFaradayDepths,
				     const std::string &filename);
	
  //! read a Faraday simulation with Faraday emission as second column
  void readFaradaySimulationFromFile( std::vector<double> &faradayDepths,
				      std::vector<double> &deltaFaradayDepths,
				      std::vector<double> &FaradayEmissions,
				      const std::string &filename);				     

  //! read a real vector from a file
  void readVectorFromFile(std::vector<double> &vec, const std::string &filename);
	
  //! read two real vectors from a file
  void read2VectorsFromFile(std::vector<double> &vec1,
							std::vector<double> &vec2,
							const std::string &filename);
	
  //! write two real vectors to a file
  void write2VectorsToFile(const std::vector<double> &vec1,
						   const std::vector<double> &vec2,
						   const std::string &filename);
 
  //! read polarized intensities from file (used to read in simulations)
  void readPolIntensitiesFromFile(const std::string &filename, std::vector<std::complex<double> > &polIntensities);	
  void readPolIntensitiesFromFile(const std::string &filename, 
				  std::vector<double> &lambdaSquareds,
				  std::vector<std::complex<double> > &polIntensities);

  //! read a complex RMSF and its corresponding lambda squareds from a file
  void readRMSFfromFile(std::vector<double> &faradaydepths,
						std::vector<std::complex<double> > &rmsf,
						const std::string &filename);

  //! write a complex RMSF to a file
  void writeRMSFtoFile( const std::vector<std::complex<double> > &rmsf,
						const std::string &filename);

  //! write a complex RMSF and its corresponding lambda squareds to a file
  void writeRMSFtoFile( const std::vector<double> &faradaydepths,
						const std::vector<std::complex<double> > &rmsf, 
						const std::string &filename);

  //! Write a vector out to file
  void writeRMtoFile(const std::vector<double> &rm, 
					 const std::string &filename);

  //! Write a complex vector out to a file
  void writeRMtoFile(const std::vector<std::complex<double> > &rm, 
					 const std::string &filename);

  //! Write the lambda squareds ann the complex vector rm out to a file
  void writeRMtoFile(const std::vector<double> &lambdasq, 
					 const std::vector<std::complex<double> > &rm, 
					 const std::string &filename);
	
  //! Write complex polarized intensity to file along with frequencies
  void writePolIntToFile(const std::vector<double> &frequencies, 
						 const std::vector<std::complex<double> > &polint, 
						 const std::string &filename);

  void writeIntToFile(const std::vector<double> &frequencies, 
			     const std::vector<std::complex<double> > &polint, 
			     const std::string &filename);

  //! Write single polarized intensity (Q or U) to file along with frequencies
  void writeIntToFile(const std::vector<double> &frequencies, 
					  const std::vector<double> &intensities, 
					  const std::string &filename);


  //*************************************************************************************
  //
  // Image cube functions (internally only FITS implemented at first)
  //
  //**************************************************************************************

  //! Check if image cube is in correct format for RM-Synthesis
  bool checkImageCube(const std::string &filename, int hdu);

  //! Check if Q image cube is in correct format for RM-Synthesis  
  bool checkImageCubeQ(const std::string &filename, int hdu);

  //! Check if U image cube is in correct format for RM-Synthesis
  bool checkImageCubeU(const std::string &filename, int hdu);

  //! Read in a list of input/output files from a text file
  void readFileList(const std::string &filename, std::vector<std::string> &list);
};

#endif		// _RMIO_H_
