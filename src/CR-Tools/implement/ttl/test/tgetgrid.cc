#include <iostream>
#include <vector>
#include <cmath>
#include <string>

#include <ttl/coordinates.h>

using namespace ttl;

int main()
{
  const int NX=5;
  const int NY=5;

  const std::string refcode="AZEL";
  const std::string projection="STG";

  const double refLong=1.0;
  const double refLat=1.0;
  const double incLong=1.0;
  const double incLat=1.0;
  const double refX=1.0;
  const double refY=1.0;

  double status=true;

//  const std::string telescope = "LOFAR";
//  const double dateObs = 50237.29;

  std::vector<double> pixel(2*NX*NY);
  std::vector<double> opixel(2*NX*NY);
  std::vector<double> world(2*NX*NY);

  // Set grid
  int k=0;
  for (int i=0; i<NX; i++)
  {
    for (int j=0; j<NY; j++)
    {
      world[2*k] = 0.0;
      world[2*k+1] = 0.0;
      opixel[2*k] = 0.0;
      opixel[2*k+1] = 0.0;
      pixel[2*k] = static_cast<double>(i+1);
      pixel[2*k+1] = static_cast<double>(j+1);
      k++;
    }
  }

  // Calculate grid
  status=toWorld(world.begin(), world.end(),
                 pixel.begin(), pixel.end(),
                 refcode, projection,
                 refLong, refLat,
                 incLong, incLat,
                 refX, refY);

  status=toPixel(opixel.begin(), opixel.end(),
                 world.begin(), world.end(),
                 refcode, projection,
                 refLong, refLat,
                 incLong, incLat,
                 refX, refY);

  if (status)
  {
    // Display output grid
    for (int i=0; i<2*NX*NY; i++)
    {
      std::cout<<pixel[i]<<"\t"<<world[i]<<"\t"<<opixel[i]<<std::endl;
    }
  }
  else
  {
    std::cerr<<"Conversion failed."<<std::endl;
  }
}

