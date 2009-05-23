/*! Implementation of rmCube class methods

    Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
    Date:		15-05-2009
    Last change:	22-05-2009
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
  if(buffer!=NULL)
  {

  }

  // free memory of buffer
}


/*!
  \brief rmCube constructor, creates a RM cube with given dimensions

  \param x --
  \param y --
  \param faradaySize --
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
  
}


/*!
  \brief Constructor with setting individual Faraday depth vector

  \param x --
  \param y --
  \param faradayDepths --
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


