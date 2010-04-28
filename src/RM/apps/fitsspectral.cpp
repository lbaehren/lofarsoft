// Program that reads in a fits cube and takes out a spectral line
//
// File:		fitsspectral.cpo
// Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
// Date:		23-10-2009
// Last change:	26-10-2009


#include <iostream>
#include <fstream>                      // file stream functions
#include <vector>                       // vector used to keep list of input images
#include "fitsio.h"
#include <rmFITS.h>
#include <rmIO.h>

#define debug_


using namespace std;

bool checkCube(string filename, long *naxes);


int main (int argc, char * const argv[]) 
{
  // Input and output
  string fitsfilename;						// filename of FITS file to extract spectrum from
  string spectralfilename="spectral.dat";	// default output name for spectral line file
  string coordfilename;						// text file containing
  unsigned long xpos=0, ypos=0;				// x and y coordinate to read spectral line in cube
  
  // Internal variables
  long *naxes=NULL;                     // ... and dimensions of FITS cube
  long inc[3]={0,0,1};	                // increment within FITS cube
  vector<double> spectralLine(200);     // vector to store one spectral line
  vector<double> coordinate(200);       // vector containing coordinate (frequency, lambdaSq, or Faraday depth)
  
  if(argc<2)
    {
      cout << endl << "Usage: " << argv[0]  << " -f <cube.fits> -o <spectral.dat> -x <xpos> -y <ypos>" << endl;
      exit(0);
    }
  else
    {
      int c=0;	// character found as command line parameter
      opterr = 0;
      
      // Parse command line parameters
      while ((c = getopt (argc, argv, "x:y:f:o:l:")) != -1)
	{
	  switch (c)
	    {
	    case 'x':		  // x coordinate to extract spectral line
	      xpos = atoi(optarg);
	      break;
	    case 'y':		  // y coordinate to extract spectral line
	      ypos = atoi(optarg);
	      break;
	    case 'f':			// read from FITS file with filename
	      fitsfilename = optarg;  
	      break;
	    case 'o':			// maximum number of iterations
	      spectralfilename = optarg;
	      break;
	    case 'l':			// optionaly provide text file with frequencies / Faraday depths
	      coordfilename = optarg;
	      break;
	    case '?':
	      if (optopt=='s' || optopt=='n'  || optopt=='c' )
		fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	      else if (isprint (optopt))
		fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	      else
		fprintf (stderr,
			 "Unknown option character `\\x%x'.\n",
			 optopt);
	      return 1;
	    default:
	      abort ();
	    }
	}
      
      
    }
  
  
  
  //*****************************************************
  try 
    {		
      checkCube(fitsfilename, naxes);				// check if FITS file is a 3D-cube
      
      //fits_open_image(&infptr, filename.c_str(), READONLY, &fitsstatus);
      RM::rmFITS image(fitsfilename, READONLY);
      RM::rmIO io;				// create rmio object for spectral ASCII output
      
      image.moveAbsoluteHDU(1);
      image.getImgDim();
      image.getImgSize();
      
      cout << image.getX() << "," << image.getY() << "," << image.getZ() << endl;
      spectralLine.resize(image.getZ());	
      image.readLine(spectralLine, xpos, ypos, inc, 0);
      
      if(coordfilename!="")		// if a frequency / Faraday depths file was given...
	{
	  io.readVectorFromFile(coordinate, coordfilename);
	  
	  if(coordinate.size()!=spectralLine.size())
	    throw "fitsspectral spectral line and coordinate differ in size";
	  else
	    io.write2VectorsToFile(coordinate, spectralLine, spectralfilename);
	}
      else
	{
	  io.writeRMtoFile(spectralLine, spectralfilename);	
	}
      
      image.close();
      
    }
  catch (const string e)		// catch exceptions
    {
      cout << e << endl;
    }
  
  return 0;
}


// Check if FITS file is a 3D-Cube
bool checkCube(string filename, long *naxes)
{
  fitsfile *infptr=NULL;
  int fitsstatus=0;
  int naxis=0;
  
  fits_open_image(&infptr, filename.c_str(), READONLY, &fitsstatus);
  fits_get_img_dim(infptr, &naxis, &fitsstatus);
  
  if(naxis!=3)
    {
      return false;
    }
  else 
    {
      naxes=(long int*)calloc(naxis, sizeof(long int));
      return true;
    }
  
  fits_close_file(infptr, &fitsstatus);
}
