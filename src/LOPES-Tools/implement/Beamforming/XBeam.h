
#ifndef XBEAM_H
#define XBEAM_H

/* $Id: XBeam.h,v 1.4 2006/04/13 13:57:32 bahren Exp $ */

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/BasicSL/Complex.h>

using casa::IPosition;
using casa::Matrix;
using casa::String;
using casa::Vector;

namespace LOPES {  // namespace LOPES -- begin
  
  /*!
    \class XBeam
    
    \ingroup Beamforming
    
    \brief A collection of routines for the computation of the X-Beam.
    
    \author Lars B&auml;hren
    
    \date 2005/05
    
    \test tXBeam.cc
  */
  class XBeam {
    
  public:
    
    enum method {
      method1,
      method2,
      method3,
      method4
    };
    
  private:
    
    //! Method applied for computation
    XBeam::method method_p;
    
    //! Clipping of the X-Beam amplification factor
    double sigma_p;
    
    //! Number of blocks, over which an average is computed.
    int blockaverage_p;
    
  public:
    
    // --- Construction ----------------------------------------------------------
    
    /*!
      \brief Default constructor
    */
    XBeam ();
    
    /*!
      \brief Argumented constructor
      
      \param which        -- Method applied for computation.
      \param sigma        -- Clipping of the X-Beam amplification factor.
      \param blockaverage -- Number of blocks, over which an average is computed.
    */
    XBeam (XBeam::method which,
	   const double& sigma,
	   const int& blockaverage);
    
    // --- Destruction -----------------------------------------------------------
    
    ~XBeam ();
    
    // --- Computation parameters ------------------------------------------------
    
    /*!
      \brief What is the method applied form computation of the X-Beam?
      
      \return method -- The method number.
    */
    XBeam::method methodType ();
    
    /*!
      \brief What is the method applied form computation of the X-Beam?
      
      \return name -- The methods name.
    */
    String methodName ();
    
    /*!
      \brief What is the method name corresponding to a certain method number?
      
      \param method -- The method number.
      
      \return name -- The methods name.
    */
    String methodName (XBeam::method method);
    
    // --- Computation -----------------------------------------------------------
    
    /*!
      \brief Calculate the x-beam from an array time-domain data
      
      \param antennaData -- 2-dim array with the time-domain data from all
      antennas, [nofAntennas,nofSamples].
      
      \return xbeam -- 
    */
    Vector<double> xbeam (const Matrix<double>& antennaData);
    
  };
  
}  // namespace LOPES -- end

#endif
  
