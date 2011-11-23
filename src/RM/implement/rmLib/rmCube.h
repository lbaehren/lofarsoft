
#ifndef RM_CUBE_H
#define RM_CUBE_H

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <complex>
#include <algorithm>
#include <vector>
#include <fstream>
#include <algorithm>			// for minimum element in vector
#include <limits>			// limit for S infinite
#include <iomanip>
#include <iostream>
#include <sstream>
#include "rm.h"
#include "rmIO.h"
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Containers/RecordDesc.h>
#include <casa/Containers/RecordInterface.h>
#include <lattices/Lattices/TiledShape.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableInfo.h>
#include <images/Images/PagedImage.h>
#include <lattices/Lattices/LatticeRegion.h>
#include <coordinates/Coordinates/Coordinate.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/TabularCoordinate.h>
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
    uint xSize;
    //! Vertical size in pixels
    uint ySize;
    //! Total Farday range covered
    uint faradaySize;
    uint freqSize;
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
    int axis ;
    int pix ;          //! type of the images
    // Faraday depth dimension
    double faraday_low;			//!> lower limit of Faraday Depth computed
    double faraday_high;		//!> upper limiit of Faraday Depth computed
    std::vector<double> faradayDepths;	//!> complete list of Faraday depths in RM cube
    
    std::string rmAlgorithm;					//!> algorithm used to compute RM
    std::string errorEstimationAlgorithm;	//!> algorithm used for error estimation
    
    double *buffer; 									//!> pointer to buffer for computed Faraday depths
    std::vector<int> bufferDimensions;			//!> dimensions of buffer (line, tile, plane,...)
    casa::CoordinateSystem cs ;
    casa::LoggerHolder log;
    casa::IPosition shape;
						//! where the frequencies (faraday depths) are written one after the other
    // Keep variables that do not need to be computed for every single RM
    std::vector<double> lambdaSqs;				//!> lambda squareds of channels
    std::vector<double> deltaLambdaSqs;		//!> delta lambda squareds that belong to RM cube input image
    std::vector<double> freqs ;
    std::vector<double> freqsInter ;
    std::vector<double> weights;					//!> weighting factors for channels
    std::string weightingAlgorithm;				//!> algorithm used to compute weights
    //! Rotation Measure Spread Function
    std::vector<std::complex<double> > rmsf;
    casa::TableRecord recSrc ; 
  
  public:

    // === Construction =========================================================
    casa::Cube<complex<double> > vals ;			//! complex values of the cube stored inside a one dimensional complex field
    //! Empty constructor without initial values
    rmCube();
    //! Argumented constructor with dimensions
    rmCube (int faradaySize,int x,int y, int axis,int pix);
    //! Construct a rmCube from given faradaySize and stepsize
    rmCube (int faradaySize,int x,int y,double stepsize);
    //! Constructor giving individual faradayDepths vector
    rmCube (int x,int y,std::vector<double>);
    //! Constructor for a rmCube generated from two sets of fitsimages (q and u polarisation)
    rmCube(string qpath, string upath,int axis, double f_min, double f_max) ;
    //! Constructo for a rmCube generated from an casa image directory
    rmCube(string imagePath, string imageName, string query, casa::Vector<int> &inds ) ;
    //! procedure to allocate memory for the current rmCube object, to be called from constructors 
    void allocMemory(bool faraday) ;
    void readDir(string path, vector<string> &dats) ;
    void removeNoFits(vector<string> &names) ;
    void getFitsSize(string &path, vector<string> &names, uint &x, uint &y, int &pix) ;
    void writeRM_Cube(string &qpath, string &upath, string &fileNames);
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
    //! get the number of frequency channels of the current cube
    uint getfreqSize() ;
    /*! procedure copies the metadata of the current imagecube, stored inside the 
        TableRecord object (used from casacore library) to the given image object */ 
    void copyMetaData(casa::PagedImage<casa::Float> &tarImage) ;
    vec getFreqsCenters() ;
    vec getFreqsLengths() ;
    //! Get current Y position in Faraday cube
    int getCurrentY();
    int getPix() {
      return pix ;
    }
    casa::CoordinateSystem get_cs() { return cs;}
    casa::LoggerHolder get_log() {return log ;}
    casa::IPosition get_shape() {return shape; }
    int setCurrentX(uint currentX);	//! set current X position in Faraday cube
    int setCurrentY(uint currentY);	//! set current Y position in Faraday cube   
    vector<double> lambdaToFarraday(vector<double> &lambda ) ;
    vector<double> lambdaToFarradayBrentjens(vector<double> &lambda ) ;
    vector<double> lambdaToFarradayBrentjensSym(vector<double> &lambda ) ;
    vector<double> freqsToFarraday(vector<double> freqs, double cq ) ;
    vector<complex<double> > performRMSynthesis(vector<complex<double> > &QU, vector<double> &lambdas, vector<double> &faradays, vector<uint> &gaps, double nu_0, double alpha, double epsilon);
    complex<double> integrateLamdaSq(vector<complex<double> > &QU,vector<double> &lambdas, double faraday, vector<uint> gaps, double nu_0, double alpha, double epsilon) ;
    vector<double> freqToLambdaSq(const vector<double> &frequency) ;
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
    void setFirstCoord(uint index, double val)  ; //! procedure set the correct type of first coordinate at the index to value val

    void createBuffer(long long size);						//! create buffer for computed Faraday depths
    void deleteBuffer();										//! delete associated buffer
    void createBufferPlane();									//! create buffer for one Faraday plane
    void createBufferCube();									//! create buffer for the whole cube
    complex<double> getVal(uint f, uint x, uint y) ;	/*! method returns the complex value of polar emission at position 
							   frequence of faradaydepth f, and impage position x,y */
    void setVal(uint f, uint x, uint y, complex<double> val) ; /*! set the value of the polarized (complex) emission  at 
								the field position f,x,y to the value val */

    std::vector<int> getBufferDimensions();				//! get dimensions of buffer
    void setBufferDimensions(std::vector<int> &dimensions);	//! set dimensions of buffer (i.e. plane, cube)
 
    std::string getWeightingAlgorithm();					//! get weihting Algorithm
    void setWeightingAlgorithm(std::string &);			//! set weighting Algorithm
 
    std::string getRMAlgorithm();							//! get algorithm used to compute cube
    void setRMAlgorithm(const std::string &);			//! set algorithm used to compute cube
 
    std::vector<double> getLambdaSqs();					//! get lambda squareds
    void setLambdaSqs(vector<double> &);					//! set lambda squareds

    std::vector<double> getFreqs();					//! get lambda squareds
    void setFreqs(vector<double> &);					//! set lambda squareds
 
    std::vector<double> getDeltaLambdaSqs();				//! get delta lambda squareds
    void setDeltaLambdaSqs(std::vector<double> &);		//! set delta lambda squareds
 
    std::vector<double> getWeights();						//! get weights of cube frequencies
    void setWeights(vector<double> &);						//! set weights of cube frequencies
 
    std::vector<std::complex<double> > getRMSF();		//! get RMSF
    void computeRMSF(const std::vector<double> &, const std::vector<double> &, bool);	//! compute RMSF with inherited method from class rm
    void getLineOfSight(uint x, uint y,std::vector<std::complex<double> > &line) ;
    void setLineOfSight(uint x, uint y,std::vector<std::complex<double> > &line) ;
    void getLineOfSight(uint x, uint y,cvec &line) ;
    // High-level RM compute functions
    void computePlane(double faradayDepth);				//! compute one Faraday plane for faradayDepth

    //! Compute the whole Cube with paramaters from attributes
    void computeCube();
    void fillRM_Cube(string &qpath, string &upath, vector<string> &qimag, vector<string> &uimag, uint f, uint x, uint y, double fmin, double fmax) ;
  }; // END -- class rmCube

} // END -- namespace RM

#endif
