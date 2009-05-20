/*! Implementation of rmCube class methods

    Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
    Date:		15-05-2009
    Last change:	19-05-2009
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


void rmCube::setCurrentX(int currentX)
{
  this->currentX=currentX;
}


void rmCube::setCurrentY(int currentY)
{
  this->currentY=currentY;
}

