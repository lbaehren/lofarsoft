#include <iostream>
#include <vector>
#include <cmath>

#include <ttl/math.h>
#include <ttl/beamforming.h>

using namespace ttl;

int main()
{
  std::vector<double> v1(9);
  std::vector<double> v2(9);
  std::vector<double> v3(9);

  for (int i=0; i<9; i++)
  {
    v1[i]=static_cast<double>(i);
    v2[i]=static_cast<double>(i);
    v3[i]=static_cast<double>(i);
  }

  geometricDelays(v1.begin(),v1.end(),v2.begin(),v2.end(),v3.begin(),v3.end(),true);

  for (int i=0; i<9; i++)
  {
    std::cout<<v1[i]<<std::endl;
  }
}

