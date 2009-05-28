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
  if(buffer!=NULL)		// If there is a buffer associated...
  {
    free(this->buffer);		// free memory of buffer
  }
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
  
    for(i=0; i<=steps; i++)
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
}


/*!
  \brief Constructor with setting individual Faraday depth vector

  \param x - Horizontal dimension in pixels
  \param y - Vertical dimension in pixels
  \param faradayDepths - total extension in Faraday depth
*/
rmCube::rmCube(int x, int y, vector<double> faradayDepths)
{




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
    
    
int rmCube::createBuffer(long long size)
{    
  if(buffer==NULL)	// check if we have already a buffer
  {
   this->buffer=(double *) calloc(size, sizeof(double)); // allocate memory of size Bytes

   if(this->buffer==NULL)
   {
    throw "rmCube::createBuffer memory allocation failed";
    return -1;
    }
  }
  else
  {
    throw "rmCube::createBuffer buffer exists";
    return -1;
  }

  return 0;
}


int rmCube::deleteBuffer()
{
  if(this->buffer!=NULL)
  {
    free(this->buffer); 
  }
  else
  {
    throw "rmCube::deleteBuffer";
    return -1;
  }
  
  return 0;
}


int rmCube::createBufferPlane()
{
  if(buffer==NULL)	// check if we have already a buffer
  {
   this->buffer=(double *) calloc(this->xSize*this->ySize, sizeof(double)); // allocate memory for one Faraday plane

   if(this->buffer==NULL)
   {
    throw "rmCube::createBufferPlane memory allocation failed";
    return -1;
   } 
  }
  else
  {
    throw "rmCube::createBufferPlane buffer already exists";
    return -1;
  }

  return 0;
}


int rmCube::createBufferCube()
{
  vector<int> dimensions(3);

  if(buffer==NULL)	// check if we have already a buffer
  {
   this->buffer=(double *) calloc(this->xSize*this->ySize*this->faradaySize, sizeof(double)); // allocate memory of size Bytes

   if(this->buffer==NULL)
   {
    throw "rmCube::createBufferCube memory allocation failed";
    return -1;
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
    return -1;
  }

  return 0;
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
  return this->lambda_sqs;
}


void rmCube::setLambdaSqs(vector<double> &lambdaSqs)
{
  if(lambdaSqs.size())			// only if valid vector
    this->lambda_sqs=lambdaSqs;
}


std::string rmCube::getWeightingAlgorithm()
{
  return this->weightingAlgorithm;
}


void rmCube::setWeightingAlgorithm(std::string &weightingAlgorithm)
{
  this->weightingAlgorithm=weightingAlgorithm;
}


std::string rmCube::getRMAlgorithm()
{
  return this->algorithm;
}


void rmCube::setRMAlgorithm(const std::string &algorithm)
{
  this->rmAlgorithm=algorithm;
}


vector<double> rmCube::getRMSF()
{
  return this->rmsf;
}


void rmCube::computeRMSF()
{


}