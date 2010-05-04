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

#ifndef RFIMITIGATIONPLUGIN_H
#define RFIMITIGATIONPLUGIN_H

// Standard library header files
#include <iostream>
#include <string>
#include <Calibration/PluginBase.h>
#include <Calibration/RFIMitigation.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class RFIMitigationPlugin
    
    \ingroup Calibration
    
    \brief Plugin for RFIMitigation class
    
    \author maaijke mevius

    \date 2010/04/29

    \test tRFIMitigationPlugin.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class RFIMitigationPlugin  : public PluginBase<DComplex> {
    
  public:
    
    // === Construction =========================================================
    
    //! Default constructor
    RFIMitigationPlugin ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another RFIMitigationPlugin object from which to create this new
             one.
    */
    RFIMitigationPlugin (RFIMitigationPlugin const &other);
    
    // === Destruction ==========================================================

    //! Destructor
    ~RFIMitigationPlugin ();
    
    // === Operators ============================================================
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another RFIMitigationPlugin object from which to make a copy.
    */
    RFIMitigationPlugin& operator= (RFIMitigationPlugin const &other); 
    
    // === Parameter access =====================================================
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, RFIMitigationPlugin.
    */
    inline std::string className () const {
      return "RFIMitigationPlugin";
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
 
    Bool setNOfSegments( Int NewNOfSegments ) ;
    Bool setRmsThreshold( Double NewRmsThreshold );
    Bool setFlagThreshold( Double NewFlagThreshold );
    Bool setMethod( Int NewMethod ) ;
     
  /*!
    \brief Apply the filter to a block of data

    \param data           -- Input data to the filter
    \param computeWeights -- Recompute the weights based on the input data

    \retval data -- The data after application of the filter.
   */
    Bool apply (Matrix<DComplex>& data,
		const Bool& computeWeights=True);

  /*!
    \brief Apply the filter to a block of data

    \param inputData      -- Input data to the filter
    \param computeWeights -- Recompute the weights based on the input data

    \retval outputData -- The data after application of the filter.
  */
    Bool apply (Matrix<DComplex> &outputData,
		const Matrix<DComplex>& inputData,
		const Bool& computeWeights=True);
    


  private:
    
    //! Unconditional copying
    void copy (RFIMitigationPlugin const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class RFIMitigationPlugin -- end
  
} // Namespace CR -- end

#endif /* RFIMITIGATIONPLUGIN_H */
  
