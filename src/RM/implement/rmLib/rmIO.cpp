
#include <iostream>				// C++/STL iostream
#include <fstream>				// file stream I/O
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
  void rmIO::readFrequencies()
  {
    
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

  }
  
  

/*! prcedure gets from the Coordinates group of the image the
 *  the name of the coordinate group which contains the values of the 
 *  coordinates for the lamdasquared
 *  The name  of this group is returned in the string retName, the 
 *  returned boolean value shows up whether the lambda squared values are 
 *  read of the frequencies. If the return value is true, the coordinate
 *  contains already the lambda squared values, othervise it contains the 
 *  the frequency values
 *  \param grp_name complete name of the coordinate group 
 *  \param retName return value for the required group name */ 
// int rmIO::foundLambdaSQ(hid_t fileID,string &grp_name, string &retName) {
//   cout << "open subgroup " << grp_name << endl ;
//   int erg=0 ; // init the result to "no axe found"
//   hid_t grpID= H5Gopen (fileID,grp_name.c_str(),H5P_DEFAULT) ;
//   if (grpID != 0) {
//     set<string> coorNames;
//     h5get_names (coorNames,grpID,H5G_GROUP);
//     set<string>::iterator iter = coorNames.begin() ;
//     /* loop over the coordinate groups of the main coordinates group */
//     bool found = false ;
//     while (iter != coorNames.end()) {
// 	 // check for a frequence or lambda found 
// 	if ((*iter == freqConst) || (*iter == lambConst)) {
// 		if (*iter == lambConst) {
// 		  erg = 2 ;
// 		}
// 		else if (*iter == freqConst){
// 		  erg = 1 ;
// 		}
// 		found = true ;
// 		retName = grp_name+"/"+(*iter) ;
// 	}
// 	iter++ ;	    
//     }
//   }
//   return erg ;
// }


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
}
void rmIO::saveFaraLineToFits(string fileName, vector<complex<double> > &line, vector<double> &faras) {
  fitsfile *fout;
  int naxis = 5 ; // number of axes
  long *naxes = new long[naxis] ;
  long *indx = new long[naxis] ;
  /* specify the number of pixel for each axis*/
  naxes[2] = 2;
  naxes[1] = 4;
  naxes[0] = line.size() ;
  naxes[3] = 1 ;
  naxes[4] =1 ;
  int status = 0 ; 
  fits_create_file(&fout,fileName.c_str(),&status) ;
  fits_create_img(fout, DOUBLE_IMG, naxis,naxes, &status);
  if (!status) {
    /* define first point to set the values of the polarised antennasignal */
    indx[0] = 1 ;
    indx[1] = 2 ;
    indx[2] = 1 ;
    indx[3] = 1 ;
    indx[4] = 1 ;
    /* loop over all frequencies */
    for (uint i=0; i<line.size(); i++) {
      indx[1] = 2 ;
      double re = line[i].real() ;
      double im = line[i].imag() ;
      fits_write_pix(fout,TDOUBLE,indx,1,&re,&status) ;
      indx[1] = 3 ;
      fits_write_pix(fout,TDOUBLE,indx,1,&im,&status) ;
      indx[0]++ ;
    }	
  }
  /* create the frequency information table */
  uint num = faras.size() ;
  char *type[] = {(char*)"faradaydepth"} ;
  char *form[] = {(char*)"1D"} ;
  char *unit[] = {(char*)"rad/m^2"} ;
  fits_create_tbl(fout,BINARY_TBL,line.size(),1,type, form, unit,(char*)" RM_Information_Table",&status) ;
  fits_write_key(fout, TUINT, (char*)"NUM_FREQUENCY_POINTS", &num, (char*)"Number of frequency chanels", &status) ;
  /* loop over the intervals of frequency intervals 
     The array faras contains the interval boundaries,
     so we have one interval less than values */
  for (uint i=0; i<num;i++) {
    double center = faras[i] ;
    fits_write_col(fout,TDOUBLE,1,i+1,1,1,&center,&status) ;
  }
  fits_close_file(fout,&status) ;
}

void rmIO::saveFreqLineToFits(string fileName, vector<complex<double> > &line, vector<double> &freqs) {
  fitsfile *fout;
  int naxis = 5 ; // number of axes
  long *naxes = new long[naxis] ;
  long *indx = new long[naxis] ;
  /* specify the number of pixel for each axis*/
  naxes[2] = 2;
  naxes[1] = 4;
  naxes[0] = line.size() ;
  naxes[3] = 1 ;
  naxes[4] =1 ;
  int status = 0 ; 
  fits_create_file(&fout,fileName.c_str(),&status) ;
  fits_create_img(fout, DOUBLE_IMG, naxis,naxes, &status);
  if (!status) {
    /* define first point to set the values of the polarised antennasignal */
    indx[0] = 1 ;
    indx[1] = 2 ;
    indx[2] = 1 ;
    indx[3] = 1 ;
    indx[4] = 1 ;
    /* loop over all frequencies */
    for (uint i=0; i<line.size(); i++) {
      indx[1] = 2 ;
      double re = line[i].real() ;
      double im = line[i].imag() ;
      fits_write_pix(fout,TDOUBLE,indx,1,&re,&status) ;
      indx[1] = 3 ;
      fits_write_pix(fout,TDOUBLE,indx,1,&im,&status) ;
      indx[0]++ ;
    }	
  }
  /* create the frequency information table */
  uint num = freqs.size()-1 ;
  char *type[] = {(char*) "channel_center_frequency",(char*) "channel_bandwidth"} ;
  char *form[] = {(char*)"1D", (char*)"1D"} ;
  char *unit[] = {(char*)"Hz", (char*)"Hz"} ;
  fits_create_tbl(fout,BINARY_TBL,line.size(),2,type, form, unit,(char*)" Frequency_Information_Table",&status) ;
  fits_write_key(fout, TUINT, (char*)"NUM_FREQUENCY_POINTS", &num, (char*)"Number of frequency chanels", &status) ;
  /* loop over the intervals of frequency intervals 
     The array freqs contains the interval boundaries,
     so we have one interval less than values */
  for (uint i=0; i<num;i++) {
    double center = 0.5*(freqs[i+1]+freqs[i]) ;
    double width = freqs[i+1]-freqs[i] ;
    fits_write_col(fout,TDOUBLE,1,i+1,1,1,&center,&status) ;
    fits_write_col(fout,TDOUBLE,2,i+1,1,1,&width,&status) ;
  }
  fits_close_file(fout,&status) ;
}

/*! procedure reads the data of polarized receiving from the fits file.
    Additional it reads the frequencies of the receiving attenas and the intervals 
    for each receiver.
    \param fileName name of the fits file to read the frequencies from
    \param line 
    \param freqC center values of the frequencies
    \param freqI bandwidth of the frequency values 
    \param indFreq 
*/
void rmIO::readFreqLineFromFits(string fileName, cvec &line, vec &freqsC, vec &freqsI, int indFreq) {
  fitsfile *fptr;
  int indTime = 1 ;
  int indPulse = 1 ;
  int indQ = 2 ;  // index for the Q-values
  int indU = 3 ;  // index for the U-values
  int indErr = 1 ;
  if (indFreq == 1) indErr= 3 ;
  int status=0;
  char *axisN[] = {(char*)"naxis1",(char*)"naxis2",(char*)"naxis3",(char*)"naxis4",(char*)"naxis5",(char*)"naxis6",(char*)"naxis7",(char*) "naxis8"} ;
  fits_open_file(&fptr, fileName.c_str(), READONLY, &status);
  if (!status) { // opening for read worked correctly 
    uint naxis = 0 ;
    char comment[100] ;
    /* read the metadatas of the image */
    fits_read_key(fptr,TINT,(char*)"NAXIS",&naxis, comment, &status) ;
    uint *dims = new uint[naxis] ;
    /* loop over the number of axis */
    if (!status) {
      /* read the dimensions of the image object */
      for (uint i=0; i<naxis; i++) {
        fits_read_key(fptr,TINT,axisN[i],&dims[i], comment, &status) ;
      }
      /* if no error occured, read the Q and U values for each frequency */
      if (!status) {
        long *indx = new long[naxis] ;
	indx[indErr-1] = 1 ;
	indx[3]= indPulse ;
	indx[4]= indTime ;
    	line.set_size(dims[indFreq-1]) ;
    	freqsC.set_size(dims[indFreq-1]) ;
	freqsI.set_size(dims[indFreq-1]) ;
	/* loop over all frequencies */
	for (uint i=1; i<=dims[indFreq-1]; i++) {
	  indx[indFreq-1]=i ;
	  double q ;
	  double u ;
	  int anynul;
	  indx[1] = indQ ;
	  fits_read_pix(fptr, TDOUBLE, indx, 1, NULL, &q, &anynul, &status);
	  indx[1] = indU ;
	  fits_read_pix(fptr, TDOUBLE, indx, 1, NULL, &u, &anynul, &status);
	  complex<double> val(q,u) ;
	  line.set(i-1,val) ;
	}
      }
    }
  }
  readFreqsFromFits(fptr, freqsC, freqsI);
  fits_close_file(fptr, &status) ;
}

void rmIO::readFreqLineFromAscii(string fileName, cvec &line, vec &freqsC, vec &freqsI, int indFreq) {
  ifstream inpfile (fileName.c_str());
  double freq, dfreq,Ival,Ierr,Qval,Qerr,Uval,Uerr,Vval,Verr;
  while(inpfile.good()) {
    inpfile >> dfreq >> freq >> Ival >> Ierr >> Qval >> Qerr >> Uval >> Uerr >> Vval >> Verr ;
    if (inpfile.good()) {
      freqsC.data.push_back(1e6*freq) ;
      freqsI.data.push_back(1e3*dfreq) ;
      complex<double> val(Qval,Uval);
      line.data.push_back(val);
//       cout << "test: " << 1e6*freq << " " << 1e3*dfreq << " " << Qval << " " << Uval << endl ;
    }
  }
  cout << "number of lines: " << freqsC.size() << endl ;
  inpfile.close();
}

void rmIO::readFreqLine(string fileName, cvec &line, vec &freqsC, vec &freqsI, int indFreq,int fits) {
  if (fits==1) {
    readFreqLineFromFits(fileName, line, freqsC,freqsI,indFreq) ;
  }
  else if (fits == 0 ) {
    readFreqLineFromAscii(fileName, line, freqsC,freqsI,indFreq) ;
  }
}

void rmIO::readFreqsFromFits(fitsfile *fptr, vec &freqsC, vec &freqsI) {
  int status=0;
  uint nfreqs;
  char comment[100] ;
  int hduTyp ;
  int freHdu=2 ; // index of the hdu for the frequency table 
  fits_movabs_hdu(fptr, freHdu, &hduTyp, &status);
  if (!status) { // move to hdu was sucessful 
    fits_read_key(fptr,TINT,(char*)"NAXIS2",&nfreqs, comment, &status) ;
    if (!status) { // number of frequencies read 
       for (unsigned long i=1; i<=nfreqs; i++) {
         double center ;
         double width ;
	 int anynul ;
         fits_read_col(fptr, TDOUBLE, 1, i, 1, 1, NULL, &center,&anynul, &status) ;
         fits_read_col(fptr, TDOUBLE, 2, i, 1, 1, NULL, &width,&anynul, &status) ;
	 freqsC.set(i-1,center) ;
	 freqsI.set(i-1,width) ;
       }
    }
  }
}


/*!
	\brief Read a complex RMSF from a file
	
	\param rmsf - complex vector containing rmsf intensities
	\param filename - file to read from (can be FITS/HDF5 or txt file)
*/
void rmIO::readRMSFfromFile(	vector<complex<double> > &rmsf, const string &filename)
{
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
}


/*!
 \brief Read a vector<double> from a file
 
 \param v - vector to read into
 \param filename - name of file to read from (FITS, HDF5 or TXT)
*/
void rmIO::readVectorFromFile(std::vector<double> &v, const std::string &filename)
{
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
}


/*!
 \brief Write two real vectors to a file

 \param vec1 - real vector 1 to write to first column of file
 \param vec2 - real vector 1 to write to second column of file 
 \param filename - fileanme to write to (can be FITS/HDF5 or txt file)
*/
void rmIO::write2VectorsToFile(const vector<double> &vec1, const vector<double> &vec2, const string &filename)
{
}


/*!
	\brief Write a complex RMSF to a file

	\param rmsf - complex vector with RMSF intensities in Q and U
	\param filename - fileanme to write to (can be FITS/HDF5 or txt file)
*/
void rmIO::writeRMSFtoFile(const vector<complex<double> > &rmsf, const string &filename)
{
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
}


/*!
  \brief Write a vector (RM) out to file on disk (mainly for debugging)
  
  \param rm - vector containing data (real double) to write to file
  \param filename - name of file to create or append to
  \param mode - write mode: overwrite, append
*/
void rmIO::writeRMtoFile(const vector<double> &rm, const std::string &filename)
{
}


/*!
  \brief Write a vector (RM) out to file on disk (mainly for debugging)
  
  \param rm - vector containing data (real double) to write to file
  \param filename - name of file to create or append to
*/
void rmIO::writeRMtoFile(const vector<complex<double> > &rm, const std::string &filename)
{
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
  return true ;
}








/*!
	\brief Read a list of files
	
  	\param filename - text file containing input list with 2-D FITS files
	\param list - vector of strings to contain 2-D FITS filenames
*/
void rmIO::readFileList(const string &listfilename, vector<string> &list)
{
}
  
}  //  END -- namespace RM
