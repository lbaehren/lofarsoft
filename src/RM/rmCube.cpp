/*! Implementation of rmCube class methods

    Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
    Date:		15-05-2009
    Last change:	15-05-2009
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
  \brief Default rmCube constructor, creates a RM cube with given dimensions

  \param x --
  \param y --
  \param faradaySize
*/
rmCube::rmCube(int x, int y, int faradaySize)
{


}


/*!
  \brief Default destructor
*/
rmCube::~rmCube()
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

