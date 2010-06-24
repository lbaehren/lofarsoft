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
  const std::string telescope = "LOFAR";

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
      telescope);

  // Display output grid
  for (int i=0; i<NX*NY; i++)
  {
    std::cout<<ox[i]<<"\t"<<oy[i]<<std::endl;
  }
}

