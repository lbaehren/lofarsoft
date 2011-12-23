
#ifndef RMIO_H
#define RMIO_H

#include <vector>
#include <complex>
#include <fitsio.h>
#include <fitsio2.h>
//#include "psrfits.h"
#include "rmNumUtils.h"
using namespace std;

namespace RM {  //  BEGIN -- namespace RM
  
  /*! 	
    \class rmIO
    \ingroup RM
    
    \brief I/O helper class for RM to read rm-cubes from
    hdf5 files and write them into hdf5 files
    
    
    <h3>Example(s)<h3>
    
  */
  typedef struct psrfits psr;
  class rmIO {
  public:
     string grp_name ;
     string data_name ;
     string fara_name;
//      string freq_name;
     string fara_unit ;
     string freq_unit ;
     string freqConst ;
     string lambConst ;
     string namePhi ;
     string nameTheta ;
     string nameFara ;
     string theta_unit ;
     string phi_unit ;

     string nameQ ;
     string nameU ;
     string q_name ;
     string u_name ;
     string currentMainGroup;
     string currentImage ;
     string currentData ;
     string lambSQ_unit ;
     bool write ;  // id for the data group of a image 
     /* additional coordinates for the dimensions of the sky */
     vector<double> theta ;
     vector<double> phi ;

     /* attribute shows the type of the axe, which is used for rm-synthesis third dimension
        rmType = 0 : no maching third axe found
	rmType = 1 : frequencies are used for this axe
	rmType = 2 : lambda squared are used for this axe */
     uint rmType;   
    rmIO() {
	grp_name = "/" + string("Coordinates") ; // name of the group containing the values of lambda squared  
	data_name = "/Data" ;
	freqConst = string("Freqs");
	lambConst = string("LambdaSQ");
	namePhi = string("Phi");
	nameTheta = string("Theta");
	nameFara = string("FaradayDepths");
	fara_name = string("faraday_depths");
/*	freq_name = string("frequencies");
	lamb_name = string("lambda_squared") ;*/
	fara_unit = string("rad/m^2");
	freq_unit = string("1/s");
	lambSQ_unit = string("m^2") ;
	nameQ = string("Q") ;
	nameU = string("U") ;
  }

  ~rmIO() {

  }

  /*! constuctor of a file io object for reading hdf5 files.
      @param name of the file to open 
    */
  rmIO(string fileName, bool write) {
// 	psr *test = new psr();
/*	const char *name = fileName.c_str() ;
	for (uint i=0; i<fileName.length(); i++) {
	  test->filename[i] = name[i] ;
	}*/
  }

    // ***************************************************************************
    //
    // Read frequency / lambda squared distributions from text (or later FITS/HDF5) files
    //
    // ***************************************************************************
    
    //! Read frequency distribution from a text file
    void readFrequencies ();
    /*! saves a lines of sight data into a fits file in the format,
        defined by James Anderson http://usg.lofar.org/forum/index.php?topic=676.0
	The datastructure has a five dimensional image

	AXIS1 VALUE_ERROR       NAXIS1 must always be 2 (0: data-point value
	                                                 1: error estimate)
	AXIS2 Stokes(I,Q,U,V) , NAXIS2 should always be 4 (0:I, 1:Q, 2:U, 3:V)
	AXIS3 Frequency
	AXIS4 Pulse phase       NAXIS4 may be 1
	AXIS5 Time              NAXIS5 may be 1
	*/
    void saveFreqLineToFits(string fileName, vector<complex<double> > &line, vector<double> &freqs) ;
    void saveFaraLineToFits(string fileName, vector<complex<double> > &line, vector<double> &freqs) ;
    /*! reads a line of sight data into a fits file in the format,
        defined by James Anderson http://usg.lofar.org/forum/index.php?topic=676.0
	The datastructure has a five dimensional image

	AXIS1 VALUE_ERROR       NAXIS1 must always be 2 (0: data-point value
	                                                 1: error estimate)
	AXIS2 Stokes(I,Q,U,V) , NAXIS2 should always be 4 (0:I, 1:Q, 2:U, 3:V)
	AXIS3 Frequency
	AXIS4 Pulse phase       NAXIS4 may be 1
	AXIS5 Time              NAXIS5 may be 1
	*/
    void readFreqLineFromFits(string fileName, cvec &line, vec &freqsC, vec &freqsI, int indFreq) ;
    void readFreqLineFromAscii(string fileName, cvec &line, vec &freqsC, vec &freqsI, int indFreq) ;
    void readFreqLine(string fileName, cvec &line, vec &freqsC, vec &freqsI, int indFreq,int fits) ;
    
    /*! procedure to read the frequency intervals from the fitsfile 
        out of the table of the seccond hdu */
    void readFreqsFromFits(fitsfile *fptr, vec &freqsC, vec &freqsI) ;
    //! Read lambda squareds from the hdf5 file
//     int foundLambdaSQ(hid_t fileID,string &grp_name, string &retName) ;

    //! Compute deltaLambdaSquareds from lambdaSquareds
    void computeDeltaLambdaSquareds (const vector<double> &,
				     vector<double> &);
    
    //! read frequencies from file and compute delta frequencies from differences
    void readFrequenciesDiffFrequencies (const std::string &filename,
					 vector<double> &frequencies,
					 vector<double> &deltafreqs);
    
    //! read frequencies and delta frequencies from a text file
    void readFrequenciesAndDeltaFrequencies (const std::string &, 
					     vector<double> &,
					     vector<double> &);
    
    //! read lambda squareds and delta squareds from a text file
    void readLambdaSquaredsAndDeltaSquareds (const std::string &, 
					     vector<double> &,
					     vector<double> &);
    
    //! read complete set of simulated data: lambdasq, delta lambda sq and complex intensity from file
    void readSimDataFromFile (const std::string &filename, 
			      vector<double> &lambdasquareds, 
			      vector<double> &delta_lambda_squareds, 
			      vector<std::complex<double> > &intensities);
    
    //! read a complex RMSF from a file
    void readRMSFfromFile (vector<std::complex<double> > &rmsf,
			   const std::string &filename);
    
    //! read a Faraday simulation Faraday depths from file and compute deltaFaradayDepths
    void readFaradaySimulationFromFile (vector<double> &faradayDepths,
					vector<double>  &deltaFaradayDepths,
					const std::string &filename);
    
    //! read a Faraday simulation with Faraday emission as second column
    void readFaradaySimulationFromFile (vector<double> &faradayDepths,
					vector<double> &deltaFaradayDepths,
					vector<double> &FaradayEmissions,
					const std::string &filename);				     
    
    //! read a real vector from a file
    void readVectorFromFile (vector<double> &vec,
			     const std::string &filename);
    
    //! read two real vectors from a file
    void read2VectorsFromFile (vector<double> &vec1,
			       vector<double> &vec2,
			       const std::string &filename);
    
    //! write two real vectors to a file
    void write2VectorsToFile (const vector<double> &vec1,
			      const vector<double> &vec2,
			      const std::string &filename);
    
    //! read polarized intensities from file (used to read in simulations)
    void readPolIntensitiesFromFile (const std::string &filename,
				     vector<std::complex<double> > &polIntensities);	
    void readPolIntensitiesFromFile (const std::string &filename, 
				     vector<double> &lambdaSquareds,
				     vector<std::complex<double> > &polIntensities);
    
    //! read a complex RMSF and its corresponding lambda squareds from a file
    void readRMSFfromFile (vector<double> &faradaydepths,
			   vector<std::complex<double> > &rmsf,
			   const std::string &filename);
    
    //! write a complex RMSF to a file
    void writeRMSFtoFile (const vector<std::complex<double> > &rmsf,
			  const std::string &filename);
    
    //! write a complex RMSF and its corresponding lambda squareds to a file
    void writeRMSFtoFile (const vector<double> &faradaydepths,
			  const vector<std::complex<double> > &rmsf, 
			  const std::string &filename);
    
    //! Write a vector out to file
    void writeRMtoFile (const vector<double> &rm, 
			const std::string &filename);
    
    //! Write a complex vector out to a file
    void writeRMtoFile (const vector<std::complex<double> > &rm, 
			const std::string &filename);
    
    //! Write the lambda squareds ann the complex vector rm out to a file
    void writeRMtoFile (const vector<double> &lambdasq, 
			const vector<std::complex<double> > &rm, 
			const std::string &filename);
    
    //! Write complex polarized intensity to file along with frequencies
    void writePolIntToFile (const vector<double> &frequencies, 
			    const vector<std::complex<double> > &polint, 
			    const std::string &filename);
    
    void writeIntToFile (const vector<double> &frequencies, 
			 const vector<std::complex<double> > &polint, 
			 const std::string &filename);
    
    //! Write single polarized intensity (Q or U) to file along with frequencies
    void writeIntToFile (const vector<double> &frequencies, 
			 const vector<double> &intensities, 
			 const std::string &filename);
    
    // === Image cube functions (internally only FITS implemented at first ======

    //! Check if image cube is in correct format for RM-Synthesis
    bool checkImageCube (const std::string &filename,
			 int hdu);
    
    //! Check if Q image cube is in correct format for RM-Synthesis  
    bool checkImageCubeQ (const std::string &filename,
			  int hdu);
    
    //! Check if U image cube is in correct format for RM-Synthesis
    bool checkImageCubeU (const std::string &filename,
			  int hdu);
    
    //! Read in a list of input/output files from a text file
    void readFileList (const std::string &filename,
		       vector<std::string> &list);
  };
  
}  //  END -- namespace RM

#endif		// _RMIO_H_
