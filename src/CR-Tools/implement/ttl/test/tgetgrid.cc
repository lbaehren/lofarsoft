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
  const double refLong=1.0;
  const double refLat=1.0;
  const double incLong=1.0;
  const double incLat=1.0;
  const double refX=1.0;
  const double refY=1.0;
  const std::string telescope = "LOFAR";
  const double dateObs = 50237.29;

  std::vector<double> ox(NX*NY);
  std::vector<double> oy(NX*NY);
  std::vector<double> ix(NX*NY);
  std::vector<double> iy(NX*NY);

  // Set grid
  int k=0;
  for (int i=0; i<NX; i++)
  {
    for (int j=0; j<NY; j++)
    {
      ox[k] = 0.0;
      oy[k] = 0.0;
      ix[k] = static_cast<double>(i+1);
      iy[k] = static_cast<double>(j+1);

      k++;
    }
  }

  // Calculate grid
  getGrid(ox.begin(), ox.end(),
      oy.begin(), oy.end(),
      ix.begin(), ix.end(),
      iy.begin(), iy.end(),
      telescope,
      dateObs,
      refLong, refLat,
      incLong, incLat,
      refX, refY);

  // Display output grid
  for (int i=0; i<NX*NY; i++)
  {
    std::cout<<ox[i]<<"\t"<<oy[i]<<std::endl;
  }
}

