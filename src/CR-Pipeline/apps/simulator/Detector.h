#ifndef DETECTOR_HH
#define DETECTOR_HH

//standard c includes
#include<map>
#include<cmath>
#include<vector>

//project header files includes
#include "dipole.h"
#include "Shower.h"
#include "mathclasses.h"

//ROOT includes
#include <TH1F.h>
#include <TFile.h>


using namespace std;


/*!
  \class Detector

  \author Yan Grange
*/
class Detector {
  
public:

  Detector(int size) : _size(size), flags(0) {
    
    _array = new Dipole[size] ;
    
  }
  
  ~Detector() {
    delete[] _array ;
  }
  
  Dipole* dipole(int i) { return &_array[i] ; }
  
  map<double,int> bases(void ) { 
    for(int i=0;i<_size;i++) {
      for(int j=0;j<_size;j++) {
	double baseline=0;
	baseline=pow(((_array[i]).xcoord()-(_array[j]).xcoord()),2) + pow(((_array[i]).ycoord()-(_array[j]).ycoord()),2) ;
	baseline=sqrt(baseline);
	if(baseline!=0) baselines[baseline]++ ;
      }
    }
    return baselines ;
  }
  
  int getSize() {return _size - flags ;}
  void AddFlag () {flags++ ;}
  
  double* xes() {
    double* xvals = new double[_size - flags] ;
    int j=0;
    for(int i=0;i<_size;i++) {
      if(_array[i].ID() != -1){
	xvals[j] = _array[i].xcoord();
	j++ ;
      }
     }
    return xvals;
  }  
  double* ys() {
    double* yvals = new double[_size - flags] ;
    int j=0;
    for(int i=0;i<_size;i++) {
      if(_array[i].ID() != -1){
	yvals[j] = _array[i].ycoord();
	j++ ;
      }
    }
    return yvals;
  }

  void interact(Shower* show) {
    for(int i=0;i<_size;i++) {
      _array[i].setTOA(show->get_toa(_array[i].xcoord(), _array[i].ycoord()));
    }
  }

  double baseline (int i, int j) {
    double bl;
    bl=sqrt(pow(((_array[i]).xcoord()-(_array[j]).xcoord()),2) + pow(((_array[i]).ycoord()-(_array[j]).ycoord()),2)) ;
    return bl;
  }
  
  void toa_th1(const char* name) {
    
    char file[20] ;
    
    sprintf( file, "%s.root",name);
    cout<<"writing out to file "<<file<<endl;
    
    TFile f( file, "recreate" ) ;
    TH1F* plot = new TH1F("   ","Times of arrival for the different dipoles",40,6e-7,1e-8 );
    
    for(int i=0;i<_size;i++) {
      if(_array[i].ID() != -1){
	plot->Fill(_array[i].toa());
      }
    }
    plot->Write() ;
    f.Close();
  }

  void testrun(double x, double y, double z, double tnul) {
    double distance, toa ;
    double dx, dy, dz ;
    for(int i=0;i<_size;i++) {
      if(_array[i].ID()!=-1) {
	dx = pow((_array[i].xcoord() - x),2);      
	dy = pow((_array[i].ycoord() - y),2);    
	dz = pow((_array[i].zcoord() - z),2);
	distance = sqrt(dx + dy + dz) ;
	toa = distance / c() ;

	_array[i].setTOA(toa)  ;    
      }
    }   
  }

  vector<double> skypos() {
    vector<double> position;
    int totnum = 0 ;
    double xtotal, ytotal, ztotal;
    // a1 loop
    for(int a1=0;a1<_size;a1++){
      if(_array[a1].ID() != -1){
	// a2 loop
	for(int a2=0;a2<_size;a2++){
	  if(_array[a2].ID() != -1) {
	    // a3 loop
	    for(int a3=0;a3<_size;a3++){
	      if(_array[a3].ID() != -1) {
		//a4 loop
		for(int a4=0;a4<_size;a4++){
		  if(_array[a4].ID() != -1){
		    double aa, bb, cc, dd, ee, ff, gg, hh, ii, t3, t4, x1, x2, y1, y2, z1, z2;
		    double x0, y0, z0, z00, z01;
		    // double xtot, ytot, ztot ;

		    t3 = 0.5 * ( pow(c(),2) * t(a1,a2)*t(a1,a3)*t(a3,a2) + t(a1,a2) * r(a3,a1) - t(a1,a3) * r(a2,a1) ) ;
		    t4 = 0.5 * ( pow(c(),2) * t(a1,a2)*t(a1,a4)*t(a4,a2) + t(a1,a2) * r(a4,a1) - t(a1,a4) * r(a2,a1) ) ;
		    
		    x1 = t(a1,a2) * x(a3,a1) - t(a1,a3) * x(a2,a1) ;
		    x2 = t(a1,a2) * x(a4,a1) - t(a1,a4) * x(a2,a1) ;
		    y1 = t(a1,a2) * y(a3,a1) - t(a1,a3) * y(a2,a1) ;
		    y2 = t(a1,a2) * y(a4,a1) - t(a1,a4) * y(a2,a1) ;
		    z1 = t(a1,a2) * y(a3,a1) - t(a1,a3) * z(a2,a1) ;
		    z2 = t(a1,a2) * z(a4,a1) - t(a1,a4) * z(a2,a1) ;
		   
		   dd = ((y1 * z2) - (y2 * z1))/((x1 * y2) - (x2 * y1));
		   ee = ((y2 * t3) - (y1 * t4))/((x1 * y2) - (x2 * y1));
		   ff = ((x2 * z1) - (x1 * y2))/((x1 * y2) - (x2 * y1));
		   gg = ((x1 * t4) - (x2 * t3))/((x1 * y2) - (x2 * y1));
		   
		   hh  = ( 1/(c() * t(a1,a2))) * (x(a2,a1) * dd + y(a2,a1) * ff + z(a2,a1)) ;
		   ii = 0.5*c()*t(a1,a2) + 1/(2*c()*t(a1,a2))*(2*(x(a2,a1)*ee + y(a2,a1)*gg)-r(a2,a1));

		   aa = 2 * ( dd * (ee - x(a1)) + ff * (gg - y(a1))- z(a1) - hh * ii) ;
		   bb = pow(dd,2) + pow(ff,2) + 1 - pow(hh,2) ;
		   cc = pow((ee - x(a1)),2) + pow((gg - y(a1)),2) + pow(z(a1),2) - pow(ii,2) ;

		   z00 = -aa/(2*bb) + sqrt(pow((aa/(2*bb)),2) - cc/bb);
		   z01 = -aa/(2*bb) - sqrt(pow((aa/(2*bb)),2) - cc/bb);
		   if(finite(z00)||finite(z01)) {
		     
		     if(z00 > 0 && z01 < 0) z0 = z00 ;
		     if(z00 < 0 && z01 > 0) z0 = z01 ;
		     if(z00 > 0 && z01 > 0) z0 = (z00 + z01) / 2 ;
		     x0 = dd * z00 + ee;
		     y0 = ff * z00 + gg;
		     totnum++;
		     if(z00 < 0 && z01 < 0) {
		       totnum--;
		       x0 = 0;
		       y0 = 0;
		       z0 = 0;
		     }
		     xtotal+=x0;
		     ytotal+=y0;
		     ztotal+=z0;
		     
		   }
		  }
		}
		// end a4 loop		
	      }
	    }
	    // end a3 loop
   	  }
	}
	// end a2 loop
     }
    }
    // end a1 loop
    cout<<"x = "<<xtotal / totnum<<"\t y = "<<ytotal / totnum<<"\t z = "<<ztotal / totnum<<endl;
   
    return position ;
  }
 
  
  
private:


  map<double,int> baselines ;
  int _size;
  Dipole* _array ;
  int flags ;
 
  double t(int i, int j) {
    double te;
    te = _array[i].toa() - _array[j].toa() ;
    return te;
  }
  double t(int i) {
    return _array[i].toa() ;
  }

  double x(int i, int j){
    double ix;
    ix = _array[i].xcoord() - _array[j].xcoord() ;
    return ix;
  }
  double x (int i) {
    return _array[i].xcoord();
  }

  double y(int i, int j) {
    double iy;
    iy = _array[i].ycoord() - _array[j].ycoord() ;
    return iy;
  }

  double y (int i) {
    return _array[i].ycoord();
  }

  double z(int i, int j) {
    double ez;
    ez = _array[i].zcoord() - _array[j].zcoord() ;
    return ez;
  }

  double z (int i) {
    return _array[i].zcoord();
  }

  double r(int i, int j) {
    double er;
    er = diffsq(_array[i].xcoord(), 
		_array[i].ycoord(),
		_array[i].zcoord(),
		_array[j].xcoord(),
		_array[j].ycoord(),
		_array[j].zcoord() );
    return er;
  }
  

};


#endif
