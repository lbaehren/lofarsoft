/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#ifndef UPSAMPLEDDR_H
#define UPSAMPLEDDR_H

// Standard library header files
#include <iostream>
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>
#include <scimath/Mathematics/InterpolateArray1D.h>

#include <casa/namespace.h>

//CR-tools header files
#include <IO/DataReader.h>
#include <Analysis/SecondStagePipeline.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class UpSampledDR
    
    \ingroup CR
    \ingroup IO
    
    \brief DataReader class that upsamples the the output of another DataReader
    
    \author Andreas Horneffer

    \date 2008/04/23

    \test tUpSampledDR.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li> DataReader
    </ul>
    
    <h3>Synopsis</h3>

    This class generates data by generating an empty set of frequency domain data,
    filling (parts of) it by interpolating the frequency domain data from the
    input, and performing a backwards FFT.

    <b>Note:</b> This class only fills in data in the first Nyquist band of the
    new sample frequency. If there is no overlap between this band and the
    original frequency band, e.g., if the original data was sampled in a higher
    Nyquist zone, then you will get empty data.

    To include all the available data set the new sample frequency to at least: 
    \f[\nu_{\rm new} \ge \nu_{\rm old}\cdot{\rm [NyquistZone]} \f]

    <b>Note:</b> The exception is if the newSampleFrequency is identical to the
    sample frequency of the original data. In this case no interpolation is done
    and the original frequency domain data is copied as-is. This is e.g. useful
    if you want to use this class to select a smaller time interval after doing,
    e.g., the RFI filtering on the full time interval.
    
    <h3>Example(s)</h3>
    
  */  
  class UpSampledDR : public DataReader {

  private:

    /*!
      \brief Pointer to the data Raeder from which the data is read
    */
    DataReader *inpDR_p;

    /*!
      \brief Use the calfft() (instead of the fft()) of the DataReader, do not Upsampling 
      (leave the data as_is).
    */
    Bool UseCalFFT_p;
    Bool noUpsample;

    /*!
      \brief Pointer to a processing pipeline for additional processing of the input data.
    */
    SecondStagePipeline *inpPipeline_p;

  public:
    
    // === Construction =========================================================
    
    //! Default constructor
    UpSampledDR ();
        
    // === Destruction ==========================================================

    //! Destructor
    ~UpSampledDR ();
        
    // === Parameter access =====================================================
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, UpSampledDR.
    */
    std::string className () const {
      return "UpSampledDR";
    }

    //! Provide a summary of the internal status
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
      \brief Setup this DataReader

      \param inputDR -- pointer to the DataReader object to read from 
      \param newSampleFrequency -- the new sample frequency (in Hz), should be
             identical to or larger than the one in <tt>inputDR<\tt> (see note
	     above)
      \param UseCalFFT  -- Get the data from the calfft() instead of the fft() of
             the input DR
      \param inpPipeline -- pointer to an optional processing pipeline 
                            <b> The scructure of this may change! </b>
      
      \return ok -- True if successfull
    */
    Bool setup (DataReader *inputDR,
		double newSampleFrequency, 
		Bool UseCalFFT=True,
		SecondStagePipeline *inpPipeline=NULL);

    //! Return the upsampled data in the original units.
    Matrix<Double> fx();
    //! Return the upsampled data in the original units.
    void fx (Matrix<Double> &data);
    
  protected:
    
    //! Set up the data streams - nothing to do here.
    bool setStreams() { return true;};

    /*!
      \brief Fill the header information from the into a header record.
      
      \return status -- Status of the operation; returns <i>true</i> if everything
      went fine.
    */
    bool setHeaderRecord();
    
  private:

    //! Unconditional deletion 
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* UPSAMPLEDDR_H */
  
