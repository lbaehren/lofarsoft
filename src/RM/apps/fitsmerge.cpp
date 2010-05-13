/***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sven Duscha (sduscha@mpa-garching.mpg.de)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/*
  File:              fitsmerge.cpp
  Author:            Sven Duscha (sduscha@mpa-garching.mpg.de)
  Date:              01-09-2009
  Last change:			28-04-2010
*/
/*!
  \file fitsmerge.cpp
  \ingroup RM

  \brief Small helper tool that merges a list of 2-D FITS files

  \author Sven Duscha
  \date 01-09-2009 (Last change: 10-09-2009)
  
  Small helper tool that merges a list of 2-D FITS files provided in a  
  list into one 3-D (spectral / faraday) FITS cube
*/

#include <iostream>
#include <fstream>                      // file stream functions
#include <vector>                       // vector used to keep list of input images
#include <stdio.h>
#include "fitsio.h"
#include "../implement/rmLib/rmFITS.h"	 // rmFITS class

using namespace std;

// ==============================================================================
//
//  Function prototypes
//
// ==============================================================================

void readList(const string &listfilename, vector<string> &list);
bool checkFiles(const vector<string> &list);
void merge2Dto3D(const vector<string> &list, const string  
&outfilename);

/*
void preemptivelyDeleteFITS(const string &filename);
void correctFreqHeader(vector<string> &list, vector<double> &freqs);
void readPlane(fitsfile *fptr, float *plane, const unsigned long z, void *nulval, int &fitsstatus);

 //! Read list of input FITS images from file
void readList (const string &listfilename,
	       vector<string> &list);
//! Check images for consistency
bool checkFiles (const vector<string> &list);
//! Merge a list of 2D images into a 3-D cube
void merge2Dto3D(const vector<string> &list,
		 const string &outfilename);
//! Preemptively delete an existing FITS file
void preemptivelyDeleteFITS (const string &filename);
void correctFreqHeader (vector<string> &list,
			vector<double> &freqs);
//! Read an image plane from a FITS file at z position
void readPlane (fitsfile *fptr,
		float *plane,
		const unsigned long z,
		void *nulval,
		int &fitsstatus);
//! Write an image plane to a FITS file at z position
void writePlane (fitsfile *fptr,
		 float *plane,
		 const long x,
		 const long y,
		 const long z,
		 void *nulval,
		 int &fitsstatus);
//! Write an entire FITS cube (can be higher-dimensional) into a FITS file
void writeCube (fitsfile *fptr,
		float *cube,
		void* nulval);
//! For debugging purposes output an image plane
void printPlane (float *plane,
		 unsigned int x,
		 unsigned int y);

// ==============================================================================
//
//  Function implementations
//
// ==============================================================================
*/

//_______________________________________________________________________________
//                                                                           main
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

//_______________________________________________________________________________
//                                                                       readList

/*!
  \brief Read a text file and put filenames into a vector

  \param filename - text file containing input list with 2-D FITS files
  \param list - vector of strings to contain 2-D FITS filenames
  */
void readList(const string &listfilename, vector<string> &list)
{
      string filenameitem;    // local variable for filename

      ifstream infile(listfilename.c_str(), ifstream::in);    // input filestream
        
		filenameitem.resize(100);  // resize filenameitem string (needed on OS X)
      list.clear();   				// empty vector
	
      if(infile.fail())
		  throw "fitsmerge::readList failed to open file";
        
		while(infile.good())                       // as long as we can read from the text file
		{
         infile >> filenameitem;                 // read one line into filename    
		   list.push_back(filenameitem);           // push back into vector
		}
		list.pop_back();		// delete last duplicate, why do we need this?
	
		infile.close();
}


//_______________________________________________________________________________
//                                                                    merge2Dto3D
/*!
  \param list - vector containing a list of 2-D FITS files
  \param outfilename - filename of 3-D FITS file to be created
 // \param freqlist - text file the frequencies of the input files is written to
*/
void merge2Dto3D(const vector<string> &list, const string  
&outfilename)
{
	RM::rmFITS inFITS;						// rmFITS input image
	RM::rmFITS outFITS;						// rmFITS output image

  // These are now all handled in rmFITS objects:
  //     fitsfile *infptr=NULL, *outfptr=NULL;                           // fileptr for current input  and (one) output file
  //      ofstream outfile(const_cast<const char*>(freqlist.c_str()),  ofstream::out); // output filestream to create frequency list file
  //      int fitsstatus=0;                                                                       // status returned from cfitsio functions
  //      int naxis=0;													// number of axes for output image
  //      long *naxes;													// array holding dimensions for each axis
	unsigned long z=0;
	long nelements=0;												// number of elements in a plane
	
	// Check input parameters
   if(list.size()==0)
		throw "fitsmerge::merge2Dto3D file list has length 0";
	if(outfilename=="")
		throw "fitsmerge::merge2Dto3D no filename given";
        
   //-----------------------------------------------
	checkFiles(list);		

	//fits_open_image(&infptr, list[0].c_str(), READONLY, &fitsstatus);
	//fits_get_img_dim(infptr, &naxis, &fitsstatus);
	inFITS.open(list[0], READONLY);
	int naxis=inFITS.getImgDim();
	
	cout << "naxis = " << naxis << endl;	
	
	// Prepare creation of 3D FITS image
//naxis=3;
//	naxes=new long[naxis];
// naxes[2]=list.size();                   // Length of list = z-axis of output  FITS
	
	//	fits_get_img_size(infptr, naxis, naxes, &fitsstatus);					// check  img  dimensions of input FITS
	inFITS.updateImageDimensions();			 // get image size of input images
	z=list.size();
	nelements=inFITS.getX()*inFITS.getY()*z;
	
	/*
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

	outFITS.createImg(FLOAT_IMG, naxis, naxes);
	nelements=naxes[0]*naxes[1];		// number of elements in one image plane
	*/
	
	
	//double *cube=new double[inFITS.X()*inFITS.Y()*z];		// create buffer to read cube into
	double *plane=new double[inFITS.getX()*inFITS.getY()];
	
//	long fpixel[3]={1,1,1};
	

	inFITS.setNulval(0.0);											// set nulval
   // Loop over list and copy 2-D FITS files into output file
   for(unsigned long int i=0; i<list.size(); i++)
   {
//		fits_open_image(&infptr, list[i].c_str(), READONLY , &fitsstatus);			// update infptr
		inFITS.open(list[i], READONLY);		// open current file in list
		
//		fits_file_name(infptr, errortext, &fitsstatus);
//		cout << "i = " << i << "\t" << list[i].c_str() << "\t" << errortext << endl;
//		fits_read_pix(infptr, TFLOAT, fpixel, nelements, &nulval, cube+(i*nelements), NULL, &fitsstatus);			
			
//			readPlane(infptr, cube+(i*nelements), 1, &nulval, fitsstatus);				// read plane from input file: FITS start with index 1!							
			
//			writePlane(outfptr, plane, naxes[0], naxes[1], i+1, &nulval, fitsstatus);	// write to i-th plane in output file

		inFITS.readPlane(plane, 1);
		outFITS.writePlane(plane, 1);
	//	fits_close_file(infptr, &fitsstatus);
		inFITS.close();
	}
			
	//writeCube(outfptr, cube, &nulval);	
   // Close input and output files
   //fits_close_file(outfptr, &fitsstatus);
	//	delete[] naxes;
		delete[] plane;
	//	delete[] cube;

	//outFITS.writeCube(cube);
	outFITS.close();
}

//_______________________________________________________________________________
//                                                                     checkFiles

/*!
  \param &list - vector containing list of FITS files
*/
bool checkFiles(const vector<string> &list)
{
	fitsfile *infptr=NULL;											// fileptr for current input file
	//      ofstream outfile(const_cast<const char*>(freqlist.c_str()),  ofstream::out); // output filestream to create frequency list file
	int fitsstatus=0;                                  // status returned from cfitsio functions
	int prevx=0, prevy=0;                              // x and y dimensions of previous  image in list
	int naxis=1;													// number of axes for output image
	long naxes[3];                                     // array holding dimensions for each axis
	//	void *nulval;												// null value to be used if nan is encountered in FITS file
	char errortext[255];											// char string to contain fits error message
	int hdu=1;														// HDU containing image
	
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
		fits_movabs_hdu(infptr, hdu, NULL, &fitsstatus);
		
		if(fitsstatus)
		{
			fits_get_errstatus(fitsstatus, errortext);
			throw "fitsmerge::merge2Dto3D could not open image " + filename;
      }                
		fits_get_hdu_type(infptr, &hdutype ,&fitsstatus);                 // Check if  current HDU is an image extension
		if(fitsstatus)
			throw "fitsmerge::merge2Dto3D";
		
		fits_get_img_dim(infptr, &naxis, &fitsstatus);
		fits_get_img_size(infptr, naxis, naxes, &fitsstatus);					// check  img  dimensions of input FITS
		
		if((prevx!=naxes[0] || prevy!=naxes[1]) && i>0)                   // only compare  2nd image to 1st and so on...
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

//_______________________________________________________________________________
//                                                         preemptivelyDeleteFITS

/*!
 \param filename - name of file to check for
*/
/*
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
*/

/*
void readPlane(fitsfile *fptr, float *plane, const unsigned long z, void *nulval, int &fitsstatus)
//_______________________________________________________________________________
//                                                                      readPlane

void readPlane (fitsfile *fptr,
		float *plane,
		const unsigned long z,
		void *nulval,
		int &fitsstatus)
{
    long fpixel[3];		// read vector where reading starts
    int hdutype=0;		// HDU type which is checked for
    int nelements=0;		// number of elements to read
    int anynul=0;		// indicator if any nul value has been read
    int naxis=0;		// number axes present in image
    long naxes[3];		// dimensions of these axes
    char errortext[255];	// char array for fits messages
	
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

	fits_file_name(fptr, errortext, &fitsstatus);
	cout << "Filename = " << errortext << endl;
	
	
    if (plane!=NULL)	// only if valid pointer is given
    {
        fits_read_pix(fptr, TFLOAT, fpixel, nelements, nulval, plane, &anynul, &fitsstatus);
	}
    else
    {
        throw "fitsmerge::readPlane pointer is NULL";
    }
}

//_______________________________________________________________________________
//                                                                     writePlane

/*! 
 \param fptr - file pointer of FITS file
 \param x - X dimension of image
 \param y - Y dimension of image
 \param z - z coordinate plane to write plane to
 \param nulval - pointer to nulval which is used for NULL pixels
 \parm fitsstatus - fits status variable
*/
/*
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
			fits_get_errstatus(fitsstatus, errortext);				 
			cout << "fitserror: " << errortext << endl;
			throw "fitsmerge::writePlane could not write pixels";
		 }
     }
     else       // write pixels with substituting null value
     {
         fits_write_pixnull(fptr, TFLOAT, fpixel, nelements, plane,  nulval, &fitsstatus);
		 if(fitsstatus)
		 {
			fits_get_errstatus(fitsstatus, errortext);		 
			cout << "fitserror: " << errortext << endl;
			throw "fitsmerge::writePlane could not write pixels";
		 }
     }
}
*/

//_______________________________________________________________________________
//                                                                      writeCube

/*!
 \param fptr - file pointer of file to write into
 \param cube - pointer to float containing the cube data
 \param naxis - number of axis
 \param naxes - dimension of each axis
*/
/*
void writeCube(fitsfile *fptr, float *cube, void* nulval)
void writeCube (fitsfile *fptr,
		float *cube,
		void* nulval)
{
	unsigned long nelements=1;	// number of elements to write
	int fitsstatus=0;
	char errormessage[255];		// fits error message char array
	int naxis=0;
	long *naxes=NULL;
	long *fpixel;
	
	//----------------------------------------
	if(cube==NULL)
		throw "fitsmerge::writeCube cube pointer is NULL";
	//----------------------------------------
	
	fits_get_img_dim(fptr, &naxis, &fitsstatus);			// get image dimensions
	if(fitsstatus)
	  {
	    fits_get_errstatus(fitsstatus, errormessage);
	    cout << errormessage << endl;
	    throw "fitsmerge::writeCube fits_get_img_dim failed";		
	  }
	
	naxes=new long[naxis];									// get memory for naxes to hold axes sizes 
	fits_get_img_size(fptr, naxis, naxes, &fitsstatus);		// get axes sizes
	
	fpixel=(long*)malloc(sizeof(long)*naxis);
	for(int i=0; i < naxis; i++)
	  fpixel[i]=1;
	
	// Loop over (hyper)cube's axes
	for(int i=0; i < naxis; i++) {
	    nelements*=naxes[i];								// compute nelements in cube
	    cout << nelements << endl;
	  }
	if(nelements==0)
	  throw "fitsmerge::writeCube nelements is 0";
	
	fits_write_pixnull(fptr, TFLOAT, fpixel, nelements, cube, nulval, &fitsstatus);
	if(fitsstatus)
	  {
	    fits_get_errstatus(fitsstatus, errormessage);
	    cout << errormessage << endl;
	    throw "fitsmerge::writeCube failed";
	  }
}
*/

//_______________________________________________________________________________
//                                                                     printPlane
/*!
  \param *plane - buffer containing image plane
  \param x - x dimension of plane
  \param y - y dimension of plane
*/
/*
void printPlane (float *plane,
		 unsigned int x, 
		 unsigned int y)
{
	if(plane==NULL)
		throw "fitsmerge::printPlane plane is a NULL pointer";
	
	for(unsigned int i=0; i < x*y; i++)		// loop over x dimension (rows)
	{
		cout << plane[i] << "\t";
		if((i % x)==0)						// every row print an endline
			cout << endl;
	}
}
*/

/*!
  \brief Functions that tries to check frequency header info and  
correct it if necessary

  \param list - vector of list of filenames
  \param freqs - vector containing corresponding frequencies
*/
void correctFreqHeaders(vector<string> &list, vector<double> &freqs)
{
        for(unsigned int i=0; i < list.size(); i++)
        {

        }
}


/*!
 \brief Add necessary information to incomplete FITS headers (legacy interface)
 
 \param
*/
void correctFITSHeader(int naxis, long *naxes, int bitpix)
{





}
