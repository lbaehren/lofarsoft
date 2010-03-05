
#ifndef RM_CUBE_H
#define RM_CUBE_H

#include <vector>
#include "rm.h"
#include "rmIO.h"

namespace RM {
  
  /*!
    \class rmCube
    \ingroup RM
    
    \brief Provides methods for handling a complete Faraday Rotation Measure cube
    
    \author Sven Duscha
    
    \date 15/05/2009
    
    \test trmCube.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>rm
    </ul>
    
    <h3>Synopsis</h3>
    
    The rmCube class provides methods for handling Faraday cube objects. It stores
    the cube attributes (dimensions and Faraday depths), and a pointer to a buffer
    used for computation of RM values. Depending on the settings of bufferDimensions
    this buffer can used as line-of-sight, tile or (sub-)Cube buffer to store
    computed Faraday depths.
    
    <h3>Example(s)</h3>
    
  */ 
  class rmCube : public rm , public rmIO
  {
  private:
    
    //! Horizontal size in pixels
    int xSize;
    //! Vertical size in pixels
    int ySize;
    //! Total Farday range covered
    int faradaySize;
    
    int currentX;	              //!> current X position in cube
    int currentY;	              //!> current Y position in cube
    int currentFaradayDepth;    //!> current Faraday Depth
    
    // Image plane dimensions
    double ra;			//!> total RA of field
    double dec;		//!> total DEC of field
    double ra_low;	//!> lower limit (edge) of RA of field
    double ra_high;	//!> upper limit (edge) of RA of field
    double dec_low;	//!> lower limit (edge) of DEC of field
    double dec_high;	//!> upper limit (edge) of DEC of field
    
    // Faraday depth dimension
    double faraday_low;			//!> lower limit of Faraday Depth computed
    double faraday_high;		//!> upper limiit of Faraday Depth computed
    std::vector<double> faradayDepths;	//!> complete list of Faraday depths in RM cube
    
    std::string rmAlgorithm;					//!> algorithm used to compute RM
    std::string errorEstimationAlgorithm;	//!> algorithm used for error estimation
    
    double *buffer; 									//!> pointer to buffer for computed Faraday depths
    std::vector<int> bufferDimensions;			//!> dimensions of buffer (line, tile, plane,...)
  
    // Keep variables that do not need to be computed for every single RM
    std::vector<double> lambdaSqs;				//!> lambda squareds of channels
    std::vector<double> deltaLambdaSqs;		//!> delta lambda squareds that belong to RM cube input image
    std::vector<double> weights;					//!> weighting factors for channels
    std::string weightingAlgorithm;				//!> algorithm used to compute weights
    //! Rotation Measure Spread Function
    std::vector<std::complex<double> > rmsf;
  
  public:

    // === Construction =========================================================

    //! Empty constructor without initial values
    rmCube();
    //! Argumented constructor with dimensions
    rmCube (int x,
	    int y,
	    int faradaySize);
    //! Construct a rmCube from given faradaySize and stepsize
    rmCube (int x,
	    int y,
	    int faradaySize,
	    double stepsize);
    //! Constructor giving individual faradayDepths vector
    rmCube (int x,
	    int y,
	    std::vector<double>);

    // === Destruction ==========================================================
    
    //! Destructor
    ~rmCube();

    // === Parameter access =====================================================

    //! Get XSize of Faraday cube
    int getXSize();
    //! Get YSize of Faraday cube
    int getYSize();
    //! Get FaradaySize of Faraday cube
    int getFaradaySize();
    //! Get current X position in Faraday cube
    int getCurrentX();
    //! Get current Y position in Faraday cube
    int getCurrentY();
    
    int setCurrentX(int currentX);	//! set current X position in Faraday cube
    int setCurrentY(int currentY);	//! set current Y position in Faraday cube   
    
    double getRA();				//! get total RA of field
    double getDec();				//! get total Dec of field
    double getRaLow();			//! get the lower limit of RA in the field
    double getDecLow();			//! get the lower limit of Dec in the field
    double getRaHigh();			//! get the upper limit of RA in the field
    double getDecHigh();		//! get the upper limit of Dec in the field
    
    void setRA(double RA);			//! set the total RA of the field
    void setDec(double Dec);		//! set the total Dec of the field
    void setRaLow(double ra);		//! set the lower limit of RA of the field
    void setDecLow();				//! set the lower limit of Dec of the field
    void setRaHigh();				//! set the upper limit of RA of the field
    void setDecHigh();				//! set the upper limit of Dec of the field

    double getFaradayLow();			//! get the lower limit of Faraday depth of the Faraday cube
    double getFaradayHigh();			//! set the higher limit of Faraday depth of the Faraday cube
    std::vector<double> getFaradayDepths();		//! get a vector of Faraday Depths in the Faraday cube
    void setFaradayDepths(std::vector<double> &);	//! set Faraday depths of Cube
    void setFaradayDepths(double low, double high, double stepsize); //!> set Faraday depths of Cube
    void setFaradayLow(double faradayLow);	//! set the lower limit of Faraday depth of the Faraday cube
    void setFaradayHigh(double faradayHigh);	//! set the higher limit of Faraday depth of the Faraday cube
    
    void createBuffer(long long size);						//! create buffer for computed Faraday depths
    void deleteBuffer();										//! delete associated buffer
    void createBufferPlane();									//! create buffer for one Faraday plane
    void createBufferCube();									//! create buffer for the whole cube

    std::vector<int> getBufferDimensions();				//! get dimensions of buffer
    void setBufferDimensions(std::vector<int> &dimensions);	//! set dimensions of buffer (i.e. plane, cube)
 
    std::string getWeightingAlgorithm();					//! get weihting Algorithm
    void setWeightingAlgorithm(std::string &);			//! set weighting Algorithm
 
    std::string getRMAlgorithm();							//! get algorithm used to compute cube
    void setRMAlgorithm(const std::string &);			//! set algorithm used to compute cube
 
    std::vector<double> getLambdaSqs();					//! get lambda squareds
    void setLambdaSqs(vector<double> &);					//! set lambda squareds
 
    std::vector<double> getDeltaLambdaSqs();				//! get delta lambda squareds
    void setDeltaLambdaSqs(std::vector<double> &);		//! set delta lambda squareds
 
    std::vector<double> getWeights();						//! get weights of cube frequencies
    void setWeights(vector<double> &);						//! set weights of cube frequencies
 
    std::vector<std::complex<double> > getRMSF();		//! get RMSF
    void computeRMSF(const std::vector<double> &, const std::vector<double> &, bool);	//! compute RMSF with inherited method from class rm

    // High-level RM compute functions
    void computePlane(double faradayDepth);				//! compute one Faraday plane for faradayDepth

    //! Compute the whole Cube with paramaters from attributes
    void computeCube();

  }; // END -- class rmCube

} // END -- namespace RM

#endif
