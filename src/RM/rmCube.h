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
    int xSize;		//!> horizontal size in pixels
    int ySize;		//!> vertical size in pixels
    int faradaySize;	//!> total Farday range covered

    int currentX;	//!> current X position in cube
    int currentY;	//!> current Y position in cube
    int currentFaradayDepth;	//!> current Faraday Depth

    // Image plane dimensions
    double ra;		//!> total RA of field
    double dec;		//!> total DEC of field
    double ra_low;	//!> lower limit (edge) of RA of field
    double ra_high;	//!> upper limit (edge) of RA of field
    double dec_low;	//!> lower limit (edge) of DEC of field
    double dec_high;	//!> upper limit (edge) of DEC of field
    
    // Faraday depth dimension
    double faraday_low;			//!> lower limit of Faraday Depth computed
    double faraday_high;		//!> upper limiit of Faraday Depth computed
    std::vector<double> faradayDepths;	//!> complete list of Faraday depths in RM cube
    
    std::string algorithm;			//!> algorithm used to compute RM
    std::string errorEstimationAlgorithm;	//!> algorithm used for error estimation

    double *buffer; 			//!> pointer to buffer for computed Faraday depths
    vector<int> bufferDimensions;	//!> dimensions of buffer (line, tile, plane,...)
  
    // Keep variables that do not need to be computed for every single RM
    vector<double> lambda_sqs;		//!> lambda squareds of channels
    vector<double> weights;		//!> weighting factors for channels
    std::string weightingAlgorithm;	//!> algorithm used to compute weights
    vector<double> rmsf;		//!> Rotation Measure Spread Function
  
  public:
    rmCube();					//!> constructor without initial values
    rmCube(int x, int y, int faradaySize);	//!> default constructor with dimensions
    //! construct a rmCube from given faradaySize and stepsize
    rmCube(int x, int y, int faradaySize, double stepsize);
    rmCube(int, int, vector<double>); 		//!> constructor giving individual faradayDepths vector

    ~rmCube();					//!> destructor

    // Methods to access attributes
    int getXSize();			//!> get XSize of Faraday cube
    int getYSize();			//!> get YSize of Faraday cube
    int getFaradaySize();		//!> get FaradaySize of Faraday cube
    int getCurrentX();			//!> get current X position in Faraday cube
    int getCurrentY();			//!> get current Y position in Faraday cube
    
    int setCurrentX(int currentX);	//!> set current X position in Faraday cube
    int setCurrentY(int currentY);	//!> set current Y position in Faraday cube   
    
    double getRA();			//!> get total RA of field
    double getDec();			//!> get total Dec of field
    double getRaLow();			//!> get the lower limit of RA in the field
    double getDecLow();			//!> get the lower limit of Dec in the field
    double getRaHigh();			//!> get the upper limit of RA in the field
    double getDecHigh();		//!> get the upper limit of Dec in the field
    
    void setRA(double RA);		//!> set the total RA of the field
    void setDec(double Dec);		//!> set the total Dec of the field
    void setRaLow(double ra);		//!> set the lower limit of RA of the field
    void setDecLow();			//!> set the lower limit of Dec of the field
    void setRaHigh();			//!> set the upper limit of RA of the field
    void setDecHigh();			//!> set the upper limit of Dec of the field

    double getFaradayLow();			//!> get the lower limit of Faraday depth of the Faraday cube
    double getFaradayHigh();			//!> set the higher limit of Faraday depth of the Faraday cube
    vector<double> getFaradayDepths();		//!> get a vector of Faraday Depths in the Faraday cube
    void setFaradayLow(double faradayLow);	//!> set the lower limit of Faraday depth of the Faraday cube
    void setFaradayHigh(double faradayHigh);	//!> set the higher limit of Faraday depth of the Faraday cube
    
    int createBuffer(long long size);		//!> create buffer for computed Faraday depths
    int deleteBuffer();				//!> delete associated buffer
    int createBufferPlane();			//!> create buffer for one Faraday plane
    int createBufferCube();			//!> create buffer for the whole cube

    vector<int> getBufferDimensions();			//!> get dimensions of buffer
    void setBufferDimensions(vector<int> &dimensions);	//!> set dimensions of buffer (i.e. plane, cube)
 
    std::string getWeightingAlgorithm();	//!> get weihting Algorithm
    void setWeightingAlgorithm(std::string &);	//!> set weighting Algorithm
 
    vector<double> getLambdaSqs();		//!> get lambda squareds
    void setLambdaSqs(vector<double> &);	//!> set lambda squareds
 
    vector<double> getRMSF();			//!> get RMSF
 
    // High-level functions
    void computeRMSF();
};

#endif
