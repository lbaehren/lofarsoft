/*! Implementation of rmCube class methods

    Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
    Date:		15-05-2009
    Last change:	27-05-2009
*/


#include <iostream>				// C++/STL iostream
#include <math.h>				// mathematics library
#include <string.h>

//#include <casa/Arrays.h>			// CASA library functions
//#include <casa/Arrays/Array.h>
//#include <casa/Utilities/DataType.h>
//#include <tables/Tables/TiledFileAccess.h>
//#include <lattices/Lattices/TiledShape.h>

#include "rmCube.h"				// rmCube class declarations

using namespace std;


//===============================================================================
//
//  Constructions / Destruction
//
//===============================================================================

/*!
  \brief Default destructor
*/
rmCube::~rmCube()
{
  delete[] this->buffer;	// delete memory of buffer which was created with new!!! 
  
  // TODO: also check for plane buffer etc.
}


/*!
  \brief Empty constructor
*/
rmCube::rmCube()
{
  // Initialize all buffers with NULL
  buffer=NULL;

  //   cout << "empty constructor" << endl;
}


/*!
  \brief rmCube constructor, creates a RM cube with given dimensions

  \param x - Horizontal dimension in pixels
  \param y - Vertical dimension in pixels
  \param faradaySize - total extension in Faraday depth
  \param stepsize - Step size between Faraday depths
*/
rmCube::rmCube(int x, int y, int faradaySize, double stepsize)
{
  int steps=0;	// number of steps=faradaySize/stepsize
  int i=0;	// loop variable

  this->xSize=x;
  this->ySize=y;
  this->faradaySize=faradaySize;

  this->buffer=NULL;	// set buffer to NULL (no buffer associated, yet)

  // Use stepsize to create a vector of equally spaced Faraday depths
  if(fmod(faradaySize, stepsize))
  {
    throw "rmCube: faradaySize is not an integral multiple of stepsize";
  }
  else
  {
    steps=faradaySize/stepsize;
    this->faradayDepths.resize(steps);	// set faradayDepths vector's size to size of steps
  
    for(i=0; i<steps; i++)
    {
      faradayDepths[i]=i*stepsize;
    }
  } 
  
  // Set remaining attributes to defaults
  this->currentX=0;
  this->currentY=0;
  this->currentFaradayDepth=0;
  this->ra=0;
  this->dec=0;
  this->ra_low=0;
  this->ra_high=0;
  this->dec_low=0;
  this->dec_high=0;

//   cout << "rmCube::rmCube(int x, int y, int faradaySize, double stepsize) constructor" << endl;
}


/*!
  \brief Constructor with setting individual Faraday depth vector

  \param x - Horizontal dimension in pixels
  \param y - Vertical dimension in pixels
  \param faradayDepths - total extension in Faraday depth
*/
rmCube::rmCube(int x, int y, vector<double> faradayDepths)
{
  // Check parameters for consistency
  if(x<=0)
	 throw "rmCube::rmCube x dimension is <=0";
  if(y<=0)
	 throw "rmCube::rmCube y dimension is <=0";
  if(faradayDepths.size()==0)
	 throw "rmCube::rmCube faradayDepths has size 0";

  this->xSize=x;
  this->ySize=y;
  this->faradaySize=faradayDepths.size();

  this->buffer=NULL;	// set buffer to NULL (no buffer associated, yet)
  
  // Set remaining attributes to defaults
  this->currentX=0;
  this->currentY=0;
  this->currentFaradayDepth=0;
  this->ra=0;
  this->dec=0;
  this->ra_low=0;
  this->ra_high=0;
  this->dec_low=0;
  this->dec_high=0;

  for(unsigned int i=0; i<faradayDepths.size(); i++)		// loop over faradayDepths vector given
  {
    faradayDepths[i]=faradayDepths[i];			// write Faraday depth into list to probe for
  }
  
//   cout << "rmCube::rmCube(int x, int y, vector<double> faradayDepths) constructor" << endl;  
}


//===============================================================================
//
//  Methods
//
//===============================================================================
//
// Reading info from the object
//
//===============================================================================

int rmCube::getXSize()
{
  return xSize;
}


int rmCube::getYSize()
{
  return ySize;
}


int rmCube::getFaradaySize()
{
  return faradaySize;
}


int rmCube::getCurrentX()
{
  return currentX;
}


int rmCube::getCurrentY()
{
  return currentY;
}


//===============================================================================
//
// Writing info to the object
//
//===============================================================================

int rmCube::setCurrentX(int currentX)
{
  if(currentX >= this->xSize)
  {
    throw "rmCube::setCurrentX, value out of range";
    return -1;
  }
  else
  {
    this->currentX=currentX;
  }
  
  return 0;
}


int rmCube::setCurrentY(int currentY)
{

  if(currentY >= this->ySize)
  {
    throw "rmCube::setCurrentY, value out of range";
    return -1;
  }
  else
  {
    this->currentY=currentY;
  }

  return 0;
}


double rmCube::getRA()
{
  return this->ra;
}


double rmCube::getDec()
{
  return this->dec;
}


void rmCube::setRA(double ra)
{
  if(ra>=0)
    this->ra=ra;
  else
    throw "rmCube::setRA: out of range";
}


void rmCube::setDec(double dec)
{
  if(dec>=0)
    this->dec=dec;
  else
    throw "rmCube::setDec: out of range";
}


double rmCube::getRaLow()
{
  return this->ra_low;
}


double rmCube::getDecLow()
{
  return this->dec_low;
}


double rmCube::getRaHigh()
{
  return this->ra_high;
}


double rmCube::getDecHigh()
{
  return this->dec_high;
}


double rmCube::getFaradayLow()
{
  return this->faraday_low;
}


double rmCube::getFaradayHigh()
{
  return this->faraday_high;
}


vector<double> rmCube::getFaradayDepths()
{
  return this->faradayDepths;
}


void rmCube::setFaradayDepths(vector<double> &depths)
{
  this->faradayDepths=depths;
}


void rmCube::setFaradayDepths(double low, double high, double stepsize)
{   
  this->faraday_low=low;
  this->faraday_high=high;
  this->faradaySize=abs(high-low)*stepsize;
}


void rmCube::setFaradayLow(double faradayLow)
{
  this->faraday_low=faradayLow;
}
    

void rmCube::setFaradayHigh(double faradayHigh)
{
  this->faraday_high=faradayHigh;
}
    
    
void rmCube::createBuffer(long long size)
{    
  if(buffer==NULL)	// check if we have already a buffer
  {
 //  this->buffer=(double *) calloc(size, sizeof(double)); // allocate memory of size Bytes
    this->buffer=new double[size];		// allocate memory for No. size of type double

    if(this->buffer==NULL)
    {
      throw "rmCube::createBuffer memory allocation failed";
    }
  }
  else
  {
    throw "rmCube::createBuffer buffer exists";
  }
}


void rmCube::deleteBuffer()
{
  if(this->buffer!=NULL)
  {
//    free(this->buffer); 
    delete buffer;
  }
  else
  {
    throw "rmCube::deleteBuffer";
  }
}


void rmCube::createBufferPlane()
{
  if(buffer==NULL)	// check if we have already a buffer
  {
//    this->buffer=(double *) calloc(this->xSize*this->ySize, sizeof(double)); // allocate memory for one Faraday plane
    this->buffer= new double[xSize*ySize];

    if(this->buffer==NULL)
    {
      throw "rmCube::createBufferPlane memory allocation failed";
    } 
  }
  else
  {
    throw "rmCube::createBufferPlane buffer already exists";
  }
}


void rmCube::createBufferCube()
{
  vector<int> dimensions(3);

  if(this->buffer==NULL)	// check if we have already a buffer
  {
   if(this->xSize > 0 && this->ySize > 0 && this->faradaySize > 0)
   {
//       this->buffer=(double *) calloc(xSize*ySize*faradaySize, sizeof(double)); // allocate memory of size Bytes
      this->buffer=new double[xSize*ySize*faradaySize];
   }

   if(this->buffer==NULL)
   {
    throw "rmCube::createBufferCube memory allocation failed";
   }
   
   // Set parameters
   if(this->xSize > 0)
    dimensions[0]=this->xSize;
   if(this->ySize > 0)
    dimensions[1]=this->ySize;
   if(this->faradaySize > 0)
    dimensions[2]=this->faradaySize;
   
   this->setBufferDimensions( dimensions );
  }
  else
  {
    throw "rmCube::createBufferCube buffer already exists";
  }
}


vector<int> rmCube::getBufferDimensions()
{
  return this->bufferDimensions;
}


void rmCube::setBufferDimensions(vector<int> &dimensions)
{
  this->bufferDimensions=dimensions;
}


vector<double> rmCube::getLambdaSqs()
{
  return this->lambdaSqs;
}


void rmCube::setLambdaSqs(vector<double> &lambdaSqs)
{
  if(lambdaSqs.size())			// only if valid vector
    this->lambdaSqs=lambdaSqs;
}


vector<double> rmCube::getDeltaLambdaSqs()
{
  return this->deltaLambdaSqs;
}


void rmCube::setDeltaLambdaSqs(vector<double> &deltaLambdaSqs)
{
  if(deltaLambdaSqs.size()!=0)
    this->deltaLambdaSqs=deltaLambdaSqs;
  else
    throw "rmCube::setDeltaLambdaSqs size 0";
}


std::string rmCube::getWeightingAlgorithm()
{
  return this->weightingAlgorithm;
}


void rmCube::setWeightingAlgorithm(std::string &weightingAlgorithm)
{
  this->weightingAlgorithm=weightingAlgorithm;
}


vector<double> rmCube::getWeights()
{
  return this->weights;
}


void rmCube::setWeights(vector<double> &weights)
{
  if(weights.size() != 0)
    this->weights=weights;
  else
    throw "rmCube::setWeights size=0";
}


std::string rmCube::getRMAlgorithm()
{
  return this->rmAlgorithm;
}


void rmCube::setRMAlgorithm(const std::string &algorithm)
{
  this->rmAlgorithm=algorithm;
}


vector<complex<double> > rmCube::getRMSF()
{
  return this->rmsf;
}


void rmCube::computeRMSF(const vector<double> &lambdaSqs, const vector<double> &deltaLambdaSqs, bool freq=true)
{
  if(faradayDepths.size()!=0)
  {
    // use rmCube attributes (must be set) class rm method to compute RMSF
    this->rmsf=rm::RMSF(faradayDepths, lambdaSqs, weights, deltaLambdaSqs, freq);
  }
}


//****************************************************
//
// High-level RM computing functions
//
//****************************************************


/*!
	\brief Compute one Faraday plane with algorithm given in class attribute
*/
void rmCube::computePlane(double faradayDepth)
{
	if(rmAlgorithm=="")		// if algorithm was not set...
		throw "rmCube::computePlane algorithm is not set";
   else if(rmAlgorithm!="rmsynthesis" || rmAlgorithm!="wienerfilter" || rmAlgorithm!="wavelet")
		throw "rmCube::computePlane unknown algorithm in attribute";
  else if(faradayDepths.size()==0)
	   throw "rmCube::computePlane faradayDepths attribute is not set";

  // loop over x and y to compute one plane

  // Problem need access to whole image Data, this is not abstracted yet
}


/*!
	\brief Compute the whole cube with algorithm given in class attribute
*/
void rmCube::computeCube()
{
	if(rmAlgorithm=="")		// if algorithm was not set...
		throw "rmCube::computeCube algorithm is not set";
   else if(rmAlgorithm!="rmsynthesis" || rmAlgorithm!="wienerfilter" || rmAlgorithm!="wavelet")
		throw "rmCube::computeCube unknown algorithm in attribute";
   else if(faradayDepths.size()==0)
		throw "rmCube::computeCube faradayDepths attribute is not set";



	// loop over x and y for each plane in faradayDepths

  // Problem need access to whole image Data, this is not abstracted yet
}



/*!
  \brief Compute RM with parameters from rmCube attributes
  
  \return RM along line of sight at Faraday Depths as defined in rmCube attribute
*/
/*
vector<complex<double> > rmCube::inverseFourier()
{
  return rm::RMSF(faradayDepths, lambdaSqs, weights, deltaLambdaSqs, false);
}
*/