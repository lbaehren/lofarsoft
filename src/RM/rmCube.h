/*!
  \class rmCube
  
  \ingroup RM
  
  \brief Provides methods for handling a complete Faraday Rotation Measure cube
  
  \author Sven Duscha
  
  \date 15.05.09.
  
  \test trmCube.cc
  
  <h3>Prerequisite</h3>
  
  <ul type="square">
  <li>rm
  </ul>
    
  <h3>Synopsis</h3>

  The rmCube class provides methods for handling Faraday cube objects. It stores the cube attributes (dimensions and Faraday depths), and a pointer to a buffer used for computation
  of RM values. Depending on the settings of bufferDimensions this buffer can used as line-of-sight, tile or (sub-)Cube buffer to store computed Faraday depths.
    
  <h3>Example(s)</h3>
  
*/ 

#ifndef RM_CUBE_H
#define RM_CUBE_H

#include "rm.h"
#include <vector>

class rmCube
{
  private:
    int xSize;		// horizontal size in pixels
    int ySize;		// vertical size in pixels
    int faradaySize;	// total Farday range covered

    int currentX;	// current X position in cube
    int currentY;	// current Y position in cube

    // Image plane dimensions
    double ra;		// total RA of field
    double dec;		// total DEC of field
    double ra_low;	// lower limit (edge) of RA of field
    double ra_high;	// upper limit (edge) of RA of field
    double dec_low;	// lower limit (edge) of DEC of field
    double dec_high;	// upper limit (edge) of DEC of field
    
    // Faraday depth dimension
    double faraday_low;
    double faraday_high;
    std::vector<double> faradayDepths;	// complete list of Faraday depths in RM cube
    
    std::string algorithm;			// algorithm used to compute RM
    std::string errorEstimationAlgorithm;	// algorithm used for error estimation

    double *buffer; 			// pointer to buffer
    vector<int> bufferDimensions;	// dimensions of buffer (line, tile, plane,...)
  
    // Keep variables that do not need to be computed for every single RM
    double weightfactor;		// weighting factor
  
  
  public:
    rmCube();					//!> constructor without initial values
    rmCube(int x, int y, int faradaySize);	//!> default constructor with dimensions
    //! construct a rmCube from given faradaySize and stepsize
    rmCube(int x, int y, int faradaySize, double stepsize);
    rmCube(int, int, vector<double>); 		//!> constructor giving individual faradayDepths vector

    ~rmCube();		// destructor

    // Methods to access attributes
    int getXSize();
    int getYSize();
    int getFaradaySize();
    int getCurrentX();
    int getCurrentY();
    
    void setCurrentX(int currentX);
    void setCurrentY(int currentY);
    
    
    double getRA();
    double getDec();
    double getRaLow();
    double getDecLow();
    double getRaHigh();
    double getDecHigh();
    
    void setRA(double RA);
    void setDec(double Dec);
    void setRaLow(double ra);
    void setDecLow();
    void setRaHigh();
    void setDecHigh();

    double getFaradayLow();
    double getFaradayHigh();
    vector<double> getFaradayDepths();
    void setFaradayLow(double faradayLow);
    void setFaradayHigh(double faradayHigh);
    
    int createBuffer(long long size);

    vector<int> getBufferDimensions();
    void setBufferDimensions(vector<int> dimensions);
};

#endif
