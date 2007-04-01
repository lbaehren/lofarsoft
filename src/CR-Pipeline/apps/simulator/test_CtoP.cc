#include <iostream>
#include <cmath>


using namespace std;

int main (void) {
  
  for(double x =-1; x<2;x++) {
    for(double y =-1; y<2;y++) {
      if(x!=0 && y!=0) cout<<x<<","<<y<<endl;
      double r, theta ;
     
      r = sqrt(pow(x,2)+pow(y,2));

      if(y<0) {
      	theta = atan(y/x) - acos(-1);
      }
      else if (x==0 && y==0) {
	  theta = 0;
	}
      else {
       	theta = atan(y/x) ;
      } 
     
      double x2, y2;
      x2=r*sin(theta);
      y2=r*cos(theta);
      if(x!=0 && y!=0) cout<<x2<<","<<y2<<endl;
      if(x!=0 && y!=0) cout<<theta*180 / acos(-1)<<endl;
      cout<<endl;
      }
    }
    

  return 0;
}


