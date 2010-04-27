/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                  *
 *   maaijke mevius (<mail>)                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef RFIMITIGATION_H
#define RFIMITIGATION_H

// Standard library header files
#include <iostream>
#include <string>
#include <assert.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Mathematics/InterpolateArray1D.h>


using namespace casa;
namespace CR { // Namespace CR -- begin
  
  /*!
    \class RFIMitigation
    
    \ingroup Calibration
    
    \brief Brief description for class RFIMitigation
    
    Class to remove RFI from a frequency spectrum. Based on the old code developed by Kalpana Singh. 

    \author maaijke mevius

    \date 2010/04/23

    \test tRFIMitigation.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class RFIMitigation {
    
  public:
    /*!
      Interpolation can be done using the following methods:
      Linear (default unless there is only one data point)
      Cubic Polynomial
      Natural Cubic Spline
      (see casacore InterpolateArray1D)
    */

    enum InterpolationMethod {
      linear,
      cubic,
      spline
    };
   
    // === Construction =========================================================
    
    //! Default constructor
    RFIMitigation ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another RFIMitigation object from which to create this new
             one.
    */
    RFIMitigation (RFIMitigation const &other);
    
    // === Destruction ==========================================================

    //! Destructor
    ~RFIMitigation ();
    
    // === Operators ============================================================
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another RFIMitigation object from which to make a copy.
    */
    RFIMitigation& operator= (RFIMitigation const &other); 
    
    // === Parameter access =====================================================
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, RFIMitigation.
    */
    inline std::string className () const {
      return "RFIMitigation";
    }

    //! Provide a summary of the object's internal parameters and status
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
     void summary (std::ostream &os);    

    // === Methods ==============================================================
    
    
    /*!
      
    \brief downsampling of the input spectrum
    \param spectrumVec	-- double vector of data of the whole frequency range
    \param nr_samples   -- number of samples the input vector should be divided  in
    \param amplVec      -- output vector of average amplitude per sample, should have size of nr_samples! 
    \param rmsVec       -- output vector of rms per sample, should have size of nr_samples! 
    
    \ returns the array of absolute values of gain
    */
    
     static void doDownsampling(const Vector<Double> &spectrumVec,
			const uint nr_samples,
			Vector<Double> &amplVec,
			Vector<Double> &rmsVec
			);
    

    /*!
   
    \brief baseline fitting of the sampled amplitudes
    \param amplVec      -- output vector of average amplitude per sample, should have size of nr_samples! 
    \param rmsVec       -- output vector of rms per sample, should have size of nr_samples!     
    \param rmsThresholdValue -- Threshold value: if RMS_i < RMSThreshold then bin i is taken into account for the fit  
    \param fitVec       -- output vector of fitted values, the vector is assumed to be evenly distributed over the nr_samples, i.e. nr points per sample = fitVec.size()/nr_samples
    \ returns the array of interpolated amplitudes
    */
    
     static void doBaselineFitting(const Vector<Double> &amplVec,
				   const Vector<Double> &rmsVec,
				   const Double rmsThresholdValue,
				   Vector<Double> &fitVec,
				   int method = 0
				   );
    
   /*!
   
    \brief baseline fitting of the sampled amplitudes.
    \param spectrumVec	-- double vector of data of the whole frequency range
    \param nr_samples   -- number of samples the input vector should be divided  in
    \param rmsThresholdValue -- Threshold value: if RMS_i < RMSThreshold then bin i is taken into account for the fit   
    \param fitVec       -- output vector of fitted values, the vector is assumed to be evenly distributed over the nr_samples, i.e. nr points per sample = fitVec.size()/nr_samples
    \ Does the downsampling and baseline fitting in one method.
    \ returns the array of absolute values of gain
    */
     static void doBaselineFitting(const Vector<Double> &spectrumVec,
				   const uint nr_samples,
				   const Double rmsThresholdValue,
				   Vector<Double> &fitVec,
		 		   int method = 0
				   );


    /*!
   
    \brief rfi flagging of the initial spectrum, given the baseline of the spectrum.
    \param spectrumVec	-- double vector of data of the whole frequency range
    \param fitVec       -- input vector of fitted baseline. Note: fitVec should have the same size as spectrumVec!
    \param flagThresholdValue -- Treshold for flagging  
    \param flagVec       -- output vector of flags. Note: flagVec should have the same size as SpectrumVec and fitVec!!
    \ if  Spectrum_i/BaselineFit_i > FlagThreshold flag this data point (0=unflagged,1=flagged)
    \ returns the vector of flags
    */

     static void doRFIFlagging(const Vector<Double> &spectrumVec,
		       const Vector<Double> &fitVec,
		       const Double flagThresholdValue,
		       Vector<Int> &flagVec
		       );
    
    /*!
   
    \brief rfi flagging of the initial spectrum.
    \param spectrumVec	-- double vector of data of the whole frequency range
    \param flagThresholdValue -- Treshold for flagging  
    \param flagVec       -- output vector of flags. Note: flagVec should have the same size as SpectrumVec and fitVec!!
    \ if  Spectrum_i/BaselineFit_i < FlagThreshold flag this data point
    \ Does the downsampling and baseline fitting and flagging in one method.
    \ returns the vector of flags
    */

     static void doRFIFlagging(const Vector<Double> &spectrumVec,
			       const uint nr_samples,
			       const Double rmsThresholdValue,
			       const Double flagThresholdValue,
			       Vector<Int> &flagVec,
			       int method = 0
			       );
      
    /*!
   
    \brief rfi mitigation of the initial spectrum,given the flags and the fitted baseline
    \param spectrumVec	-- double vector of data of the whole frequency range
    \param fitVec       -- input vector of fitted baseline. Note: fitVec should have the same size as spectrumVec!
    \param flagVec      -- input vector of flags. Note: flagVec should have the same size as SpectrumVec and fitVec!!
    \param mitigatedSpectrumVec      -- output vector of Mitigated Spectrum. Note: mitigatedSpectrumVec should have the same size as SpectrumVec, fitVec and flagVec!!
    \ replaces spectrumVec_i with fitVec_i value if flagVec_i==1
    \ returns the RFI mitigated spectrum  
    */
  
    
     static void doRFIMitigation(const Vector<Double> &spectrumVec,
			 const Vector<Double> &fitVec,
			 const Vector<Int> &flagVec,
			 Vector<Double> &mitigatedSpectrumVec
			 );

    /*!
   
    \brief rfi mitigation of the initial spectrum,given the flags and the fitted baseline
    \param spectrumVec	-- double vector of data of the whole frequency range
    \param fitVec       -- input vector of fitted baseline. Note: fitVec should have the same size as spectrumVec!
    \param flagVec      -- input vector of flags. Note: flagVec should have the same size as SpectrumVec and fitVec!!
    \param mitigatedSpectrumVec      -- output vector of Mitigated Spectrum. Note: mitigatedSpectrumVec should have the same size as SpectrumVec, fitVec and flagVec!!
    \ replaces spectrumVec_i with fitVec_i value if flagVec_i==1
    \ Does the downsampling, baseline fitting, flagging and rfi mitigation in one method.
     \ returns the RFI mitigated spectrum  
    */
  
    
     static void doRFIMitigation(const Vector<Double> &spectrumVec,
				 const uint nr_samples,
				 const Double rmsThresholdValue,
				 const Double flagThresholdValue,
				 Vector<Double> &mitigatedSpectrumVec,
				 int method = 0
				 );
     
    /*!
   
    \brief rfi mitigation of the initial spectrum,given the flags and the fitted baseline
    \param spectrumVec	-- double vector of data of the whole frequency range
    \param flagVec      -- input vector of flags. Note: flagVec should have the same size as SpectrumVec!!
    \param fitValue       -- double value to replace
    \param mitigatedSpectrumVec      -- output vector of Mitigated Spectrum. Note: mitigatedSpectrumVec should have the same size as SpectrumVec and flagVec!!
    \ replaces spectrumVec_i with fitValue value if flagVec_i==1
    \ returns the RFI mitigated spectrum  
    */
  
    
     static void doRFIMitigation(const Vector<Double> &spectrumVec,
			 const Vector<Int> &flagVec,
			 const Double fitValue,
			 Vector<Double> &mitigatedSpectrumVec
			 );

     /*!
   
    \brief rfi mitigation of the initial spectrum,given the flags and the fitted baseline
    \param spectrumVec	-- double vector of data of the whole frequency range
    \param flagVec      -- input vector of flags. Note: flagVec should have the same size as SpectrumVec!!
    \param fitValue       -- double value to replace
    \param mitigatedSpectrumVec      -- output vector of Mitigated Spectrum. Note: mitigatedSpectrumVec should have the same size as SpectrumVec and flagVec!!
    \ replaces spectrumVec_i with fitValue value if flagVec_i==1
    \ Does the downsampling, baseline fitting, flagging and rfi mitigation in one method.
     \ returns the RFI mitigated spectrum  
    */
  
    
    static void doRFIMitigation(const Vector<Double> &spectrumVec,
				const uint nr_samples,
				const Double rmsThresholdValue,
				const Double flagThresholdValue,
				const Double fitValue,
				Vector<Double> &mitigatedSpectrumVec,
				int method = 0
				);

    
  private:
 /*    uint Nsamples; */
/*     double rmsTreshold; */
/*     double flagTreshold; */
/*     uint blocksize; */

    //! Unconditional copying
    void copy (RFIMitigation const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class RFIMitigation -- end
  
} // Namespace CR -- end

#endif /* RFIMITIGATION_H */
  
