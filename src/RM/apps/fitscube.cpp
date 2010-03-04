/*
 Small helper tool that displays 
 list
 into one 3-D (spectral / faraday) FITS cube
 
 File:                 fitscube.cpp
 Author:               Sven Duscha (sduscha@mpa-garching.mpg.de)
 Date:                 01-09-2009
 Last change:		   08-09-2009
 */


#include <iostream>
#include <fstream>                      // file stream functions
#include <vector>                       // vector used to keep list of input images
#include <stdio.h>
#include "fitsio.h"


using namespace std;


void readList(const string &listfilename, vector<string> &list);
bool checkFiles(const vector<string> &list);
void merge2Dto3D(const vector<string> &list, const string  
				 &outfilename);
void preemptivelyDeleteFITS(const string &filename);
void correctFreqHeader(vector<string> &list, vector<double> &freqs);
void readPlane(fitsfile *fptr, float *plane, const unsigned long z, void *nulval, int &fitsstatus);
void writePlane (fitsfile *fptr, float *plane, const long x, const  
				 long y, const long z, void *nulval, int  &fitsstatus);
void printPlane(float *plane, unsigned int x, unsigned int y);


int main (int argc, const char * argv[]) {
	string listfilename, outfilename;
	
	//-------------------------------------
	// Command line argument parsing
	if(argc!=3)
	{
		cout << "usage: fitsmerge <list.txt> <output.fits>" << endl;
		cout << endl;
		cout << "This program reads in a list of 2-D FITS images from <list.tx>" << endl;
		cout << "and merges them into a 3-D cube." << endl;
	}
	else
	{
		listfilename=argv[1];					// 1st cmd argument is filename for input list
		outfilename="!" + (string) argv[2];    // 2nd cmd argument is output filename, exclamation mark for overwriting with fits_create_file
	}
	
	//-------------------------------------
	try {
		vector<string> list;            // vector to contain list of FITS files
		
		readList(listfilename, list);
		merge2Dto3D(list, outfilename);
	}
	
	catch (const char* s) {
		cout << s << endl;
	}
	
	return 0;
}


/*!
 \brief Read a text file and put filenames into a vector
 
 \param filename - text file containing input list with 2-D FITS files
 \param list - vector of strings to contain 2-D FITS filenames
 */
void readList(const string &listfilename, vector<string> &list)
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


/*!
 \brief Merge a list of 2D images into a 3-D cube
 
 \param list - vector containing a list of 2-D FITS files
 \param outfilename - filename of 3-D FITS file to be created
 // \param freqlist - text file the frequencies of the input files is written to
 */
void merge2Dto3D(const vector<string> &list, const string  
				 &outfilename)
{
	fitsfile *infptr=NULL, *outfptr=NULL;                           // fileptr for current input  and (one) output file
	//      ofstream outfile(const_cast<const char*>(freqlist.c_str()),  ofstream::out); // output filestream to create frequency list file
	int fitsstatus=0;                                                                       // status returned from cfitsio functions
	int naxis=0;													// number of axes for output image
	long *naxes;													// array holding dimensions for each axis
	float nulval;													// null value to be used if nan is encountered in FITS file
	char errortext[255];											// char string to contain fits error message
	
	// Check input parameters
	if(list.size()==0)
		throw "fitsmerge::merge2Dto3D file list has length 0";
	if(outfilename=="")
		throw "fitsmerge::merge2Dto3D no filename given";
	
	//-----------------------------------------------
	checkFiles(list);		
	
	fits_open_image(&infptr, list[0].c_str(), READONLY, &fitsstatus);
	fits_get_img_dim(infptr, &naxis, &fitsstatus);
	
	// Prepare creation of 3D FITS image
	naxis=3;
	naxes=new long[naxis];
	fits_get_img_size(infptr, naxis, naxes, &fitsstatus);					// check  img  dimensions of input FITS
	
	naxes[2]=list.size();                   // Length of list = z-axis of output  FITS
 	
	fits_create_file(&outfptr, outfilename.c_str(),  &fitsstatus);      // create output fits image
	if(fitsstatus)
	{
		fits_get_errstatus(fitsstatus, errortext);	
		cout << "fitserror: " << errortext << endl;
		throw "fitsmerge::merge2Dto3D could not create output file";    //  throw exception
	}
	
	// Create the primary array image (32-bit float pixels)
	fits_create_img(outfptr, FLOAT_IMG, naxis, naxes, &fitsstatus);
	if(fitsstatus)
	{
		fits_get_errstatus(fitsstatus, errortext);	
		cout << "fitserror: " << errortext << endl;
		throw "fitsmerge::merge2Dto3D could not create image extension";    //  throw exception
	}
	
	if(fitsstatus)
	{
		fits_get_errstatus(fitsstatus, errortext);	
		cout << "fitserror: " << errortext << endl;
		throw "fitsmerge::merge2Dto3D could not copy header from source to destination";    //  throw exception
	}
	
	float *plane=new float[naxes[0]*naxes[1]]; 	// create buffer to read plane into
	
	nulval=0.0; // set nulval
	
	// Loop over list and copy 2-D FITS files into output file
	for(unsigned long int i=0; i<list.size(); i++)
	{
		fits_open_image(&infptr, list[i].c_str(), READONLY , &fitsstatus);			// update infptr
		
		cout << "i = " << i << "\t" << list[i].c_str() << "\t" << fits_file_name(infptr, errortext, &fitsstatus) << endl;
		
		readPlane(infptr, plane, 1, &nulval, fitsstatus);							// read plane from input file: FITS start with index 1!							
		
		writePlane(outfptr, plane, naxes[0], naxes[1], i+1, &nulval, fitsstatus);	// write to i-th plane in output file
		
		fits_close_file(infptr, &fitsstatus);
	}
	
	// Close input and output files
	//        fits_close_file(infptr, &fitsstatus);
	fits_close_file(outfptr, &fitsstatus);
    //    outfile.close();
	delete[] naxes;
	delete[] plane;
}



/*!
 \brief Check images for consistency
 
 \param &list - vector containing list of FITS files
 */
bool checkFiles(const vector<string> &list)
{
	fitsfile *infptr=NULL;											// fileptr for current input file
	//      ofstream outfile(const_cast<const char*>(freqlist.c_str()),  ofstream::out); // output filestream to create frequency list file
	int fitsstatus=0;                                                                       // status returned from cfitsio functions
	unsigned int prevx=0, prevy=0;                                  // x and y dimensions of previous  image in list
	int naxis=1;													// number of axes for output image
	long naxes[3];                                                  // array holding dimensions for each axis
	//	void *nulval;													// null value to be used if nan is encountered in FITS file
	char errortext[255];											// char string to contain fits error message
	
	// Check input parameters
	if(list.size()==0)
		throw "fitsmerge::merge2Dto3D file list has length 0";
	
	//-----------------------------------------------
	// For each image in list... (a) check if it is an image extension  (b) check dimensions
	for(unsigned int i=0; i < list.size(); i++)
	{
		int hdutype=0;          // hdutype variable needed to check we have an image  HDU
		//		double freq=0;          // frequency to be read from keyword
		string filename;        // name of current file
		
		//-----------------------------------------------
		filename=list[i];       // get filename from list
		
		if(filename=="")
			throw "fitsmerge::merge2Dto3D filename is empty";
		
		
		fits_open_image(&infptr, filename.c_str(), READONLY, &fitsstatus);    // Open Image
		
		if(fitsstatus)
		{
			fits_get_errstatus(fitsstatus, errortext);
			throw "fitsmerge::merge2Dto3D could not open image " + filename;
        }                
		fits_get_hdu_type(infptr, &hdutype ,&fitsstatus);                       // Check if  current HDU is an image extension
		if(fitsstatus)
			throw "fitsmerge::merge2Dto3D";
		
		fits_get_img_dim(infptr, &naxis, &fitsstatus);
		fits_get_img_size(infptr, naxis, naxes, &fitsstatus);					// check  img  dimensions of input FITS
		
		if((prevx!=naxes[0] || prevy!=naxes[1]) && i>0)                         // only compare  2nd image to 1st and so on...
			throw "fitsmerge::merge2Dto3D input images differ in size";
		else
		{
			prevx=naxes[0];         // keep current x dimension as previous for later  comparison
			prevy=naxes[1];         // keep current y dimension as previous for later  comparison
		}
	}
	
	fits_close_file(infptr, &fitsstatus);
	
	return true;
}



/*!
 \brief Preemptively delete an existing FITS file
 
 \param filename - name of file to check for
 */
void preemptivelyDeleteFITS(const string &filename)
{
	int exists=0;
	int fitsstatus=0;
	fitsfile *outfptr;
	char errortext[255];											// char string to contain fits error message	
	
	
	//------------------------------------------------------------------
	// Preemptively delete FITS file
	fits_file_exists(filename.c_str(), &exists, &fitsstatus);
	if(exists)				// Check if it exists...
	{
		fits_open_file(&outfptr, filename.c_str(), READWRITE, &fitsstatus);	// need to open it, since delete only works on fileptr not filename		
		if(fitsstatus)
		{
			fits_get_errstatus(fitsstatus, errortext);
			cout << "fitserror: " << errortext << endl;
			throw "fitsmerge::merge2Dto3D could not open output file for deletion";			//  throw exception				
		}
		fits_delete_file(outfptr, &fitsstatus);									// ... then delete it
		if(fitsstatus)
		{
			fits_get_errstatus(fitsstatus, errortext);	
			cout << "fitserror: " << errortext << endl;
			throw "fitsmerge::merge2Dto3D could not delete old output file";    //  throw exception				
		}			
	}	
	
	
}



void readPlane(fitsfile *fptr, float *plane, const unsigned long z, void *nulval, int &fitsstatus)
{
    long fpixel[3];		// read vector where reading starts
    int hdutype=0;		// HDU type which is checked for
    int nelements=0;	// number of elements to read
    int anynul=0;		// indicator if any nul value has been read
    int naxis=0;		// number axes present in image
    long naxes[3];		// dimensions of these axes
	
	
	//-------------------------------------------------------------
	if (fits_get_hdu_type(fptr, &hdutype ,&fitsstatus)!=IMAGE_HDU)	// Check if current HDU is an image extension
    {
        throw "fitsmerge::readPlane CHDU is not an image";
    }
	
    // Read from FITS file one plane at depth z
    fpixel[0]=1;
    fpixel[1]=1;
    fpixel[2]=z;
	
	fits_get_img_dim(fptr, &naxis, &fitsstatus);
    fits_get_img_size(fptr, naxis, naxes, &fitsstatus); // get image dimensions
	
    nelements=naxes[0]*naxes[1];	// compute number of elements in plane
	
    if (plane!=NULL)	// only if valid pointer is given
    {
        fits_read_pix(fptr, TFLOAT, fpixel, nelements, nulval, plane, &anynul, &fitsstatus);
	}
    else
    {
        throw "fitsmerge::readPlane pointer is NULL";
    }
}


/*!
 \brief Write an image plane to a FITS file at z position
 
 \param fptr - file pointer of FITS file
 \param x - X dimension of image
 \param y - Y dimension of image
 \param z - z coordinate plane to write plane to
 \param nulval - pointer to nulval which is used for NULL pixels
 \parm fitsstatus - fits status variable
 */
void writePlane (fitsfile *fptr,
				 float *plane,
				 const long x,
				 const long y,
				 const long z,
				 void *nulval,
				 int &fitsstatus)
{
	long fpixel[3];          // first pixel position to read
	long nelements=0;        // number of elements to write
	int naxis=0;			// number of axes in FITS file
	long naxes[3];			// dimensions of these axes
	char errortext[255];
	
	//-------------------------------------------------------------	
	fpixel[0]=1;
	fpixel[1]=1;
	fpixel[2]=z;
	
	fits_get_img_dim(fptr, &naxis, &fitsstatus);	
	fits_get_img_size(fptr, naxis, naxes, &fitsstatus); // get image dimensions 
	nelements=naxes[0]*naxes[1];        // compute nelements  in plane
	
	if(x*y!=nelements)
		throw "fitsmerge::writePlane dimensions of plane does not fit image size";
	
	// Write to FITS file
	if (plane==NULL)
	{
		throw "fitsmerge::writePlane NULL pointer";
	}
	else if (nulval==NULL)
	{
		fits_write_pix(fptr, TFLOAT, fpixel, nelements, plane,  &fitsstatus);                
		if(fitsstatus)
		{
			cout << "fitserror: " << errortext << endl;
			throw "fitsmerge::writePlane could not write pixels";
		}
	}
	else       // write pixels with substituting null value
	{
		fits_write_pixnull(fptr, TFLOAT, fpixel, nelements, plane,  nulval, &fitsstatus);
		if(fitsstatus)
		{
			cout << "fitserror: " << errortext << endl;
			throw "fitsmerge::writePlane could not write pixels";
		}
	}
}


/*!
 \brief For debugging purposes output an image plane
 
 \param *plane - buffer containing image plane
 \param x - x dimension of plane
 \param y - y dimension of plane
 */
void printPlane(float *plane, unsigned int x, unsigned int y)
{
	for(unsigned int i=0; i < x*y; i++)		// loop over x dimension (rows)
	{
		cout << plane[i] << "\t";
		if((i % x)==0)						// every row print an endline
			cout << endl;
	}
}


/*!
 \brief Functions that tries to check frequency header info and  
 correct it if necessary
 
 \param ftpr - FITS file ptr of image to check
 */
void correctFreqHeader(vector<string> &list, vector<double> &freqs)
{
	for(unsigned int i=0; i < list.size(); i++)
	{
		
	}
}
