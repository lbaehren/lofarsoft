/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

#ifndef TBBTOOLS_H
#define TBBTOOLS_H

#include <crtools.h>

// Standard library header files
#include <iostream>
#include <string>

#include <casa/Arrays/Vector.h>
#include <casa/Exceptions/Error.h>
#include <casa/Arrays/ArrayMath.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class tbbTools
    
    \ingroup Analysis
    
    \brief Collection of routines to test the TBBs
    
    \author Andreas Horneffer

    \date 2008/01/25

    \test ttbbTools.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class tbbTools {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    tbbTools ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~tbbTools ();
    
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, tbbTools.
    */
    std::string className () const {
      return "tbbTools";
    }


    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Perform the simplified (mean-based) version of the FPGA trigger
      
      \param inary[in]   - input data that is to be searched for pulses
      \param level[in]   - trigger parameter, threshold level
      \param start[in]   - trigger parameter, start level
      \param stop[in]    - trigger parameter, stop level
      \param window[in]  - trigger parameter, window size index
      \param index[out]  - indices of the first sample of the found pulses
      \param sum[out]    - sum of the samples in the pulse
      \param width[out]  - width of the pulse
      \param peak[out]   - peak value inside the pulse
      \param reset[in]   - reset internal values? (Set to False if inary follows 
                           seamlessly the previous block)
      
      \return ok -- True if successfull
    */
    Bool meanFPGAtrigger(Vector<Double> inary, int level, int start, int stop, int window,
			 int afterwindow,
			 Vector<Int> &index, Vector<Int> &sum, Vector<Int> &width, 
			 Vector<Int> &peak, Vector<Int> &meanval, Vector<Int> &afterval,
			 Bool reset=True);
    
    Bool meanFPGAtrigger(Vector<Double> inary, int level, int start, int stop, int window,
			 Vector<Int> &index, Vector<Int> &sum, Vector<Int> &width, 
			 Vector<Int> &peak);
    
    /*!
      \brief Apply a single biquad-filter to the data
      
      \param inary[in]      - input data that is to be filtered
      \param B0B2[in]       - Input parameters B0 and B2 for the filter
      \param B1[in]         - Input parameter B1 for the filter
      \param A1[in]         - Input parameter A1 for the filter
      \param A2[in]         - Input parameter B2 for the filter
      \param resolution[in] - "integer resolution" of the parameters
      \param reset[in]      - reset internal values? (Set to False if inary follows 
                              seamlessly the previous block)
      
      \return the filtered data
    */
    Vector<Double> FPGAfilter(Vector<Double> &inary, int B0B2, int B1, int A1, int A2, 
			      Double resolution=1024., Bool reset=True);
 
    /*!
      \brief Apply a single notch-filter to the data
      
      \param inary[in]      - input data that is to be filtered
      \param F0[in]         - Center frequency of the filter
      \param BW[in]         - Bandwidth of the filter
      \param SR[in]         - Sample frequency on the input data
      \param resolution[in] - "integer resolution" of the parameters
      \param reset[in]      - reset internal values? (Set to False if inary follows 
                              seamlessly the previous block)
            
      \return the filtered data
    */
    Vector<Double> FPGAfilterNotch(Vector<Double> &inary, Double F0, Double BW, Double SR, 
				   Double resolution=1024., Bool reset=True);

    /*!
      \brief Apply a single lowpass-filter to the data
      
      \param inary[in]      - input data that is to be filtered
      \param F0[in]         - Corner frequency of the filter
      \param BW[in]         - (Band)width of the filter
      \param SR[in]         - Sample frequency on the input data
      \param resolution[in] - "integer resolution" of the parameters
      \param reset[in]      - reset internal values? (Set to False if inary follows 
                              seamlessly the previous block)
            
      \return the filtered data
    */
    Vector<Double> FPGAfilterLPF(Vector<Double> &inary, Double F0, Double BW, Double SR, 
				 Double resolution=1024., Bool reset=True);

    /*!
      \brief Apply a single highpass-filter to the data
      
      \param inary[in]      - input data that is to be filtered
      \param F0[in]         - Corner frequency of the filter
      \param BW[in]         - (Band)width of the filter
      \param SR[in]         - Sample frequency on the input data
      \param resolution[in] - "integer resolution" of the parameters
      \param reset[in]      - reset internal values? (Set to False if inary follows 
                              seamlessly the previous block)
            
      \return the filtered data
    */
    Vector<Double> FPGAfilterHPF(Vector<Double> &inary, Double F0, Double BW, Double SR, 
				 Double resolution=1024., Bool reset=True);

  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* TBBTOOLS_H */
  
