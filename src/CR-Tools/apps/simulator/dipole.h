#ifndef DIPOLE_HH
#define DIPOLE_HH

/*!
  \class Dipole
*/
class Dipole {
  
 public: 

  /*!
    \brief Constructor
  */
  Dipole (double x=0,
	  double y=0,
	  int id=-1)
    : _x(x), _y(y), _id(id) {;}
    
    /*!
      \brief Get the time of arrival
    */
    inline double toa() const { return _toa; }

    inline int ID() const { return _id ; }

    inline double xcoord() const { return _x ; } 

    inline double ycoord() const { return _y ; }

    inline double zcoord() const { return 0. ; }

    /*!
      \brief Set the time of arrival

      \param toa -- The time of arrival.
    */
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
