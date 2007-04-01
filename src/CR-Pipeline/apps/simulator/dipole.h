#ifndef DIPOLE_HH
#define DIPOLE_HH

class Dipole {

public: 
  Dipole(double x=0, double y=0, int id=-1) : _x(x), _y(y), _id(id) { }

  double toa() const { return _toa; }
  int ID() const { return _id ; }
  double xcoord() const { return _x ; } 
  double ycoord() const { return _y ; }
  double zcoord() const { return 0. ; }

  void setTOA(double toa) { _toa = toa ; }
  void setReadoutID(int id) { _id = id ; }
  void setCoords(double x, double y) {
    _x = x;
    _y = y;
  }

private:  
  double _x, _y ;
  double _toa ;
  int _id ;
  
} ;



#endif
