/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Frank Schroeder (<mail>)                                                     *
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

#ifndef ANALYSELOPESEVENT2_H
#define ANALYSELOPESEVENT2_H

// Standard library header files
#include <iostream>
#include <string>

#include "Analysis/analyseLOPESevent.h"
#include "Analysis/CompletePipeline.h"


namespace CR { // Namespace CR -- begin
  
  /*!
    \class analyseLOPESevent2
    
    \ingroup Analysis
    
    \brief Brief description for class analyseLOPESevent2
    
    \author Frank Schroeder

    \date 2008/01/30

    \test tanalyseLOPESevent2.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>

    based on analyseLOPESevent by Andreas Horneffer
    this class is intended as a test place for upsampling
    and more flexible plots
    
    <h3>Example(s)</h3>
    
  */  
class analyseLOPESevent2 : public analyseLOPESevent{
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    analyseLOPESevent2 ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another analyseLOPESevent2 object from which to create this new
      one.
    */
    analyseLOPESevent2 (analyseLOPESevent2 const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~analyseLOPESevent2 ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another analyseLOPESevent2 object from which to make a copy.
    */
    analyseLOPESevent2& operator= (analyseLOPESevent2 const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, analyseLOPESevent2.
    */
    std::string className () const {
      return "analyseLOPESevent2";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    
    /*!
      \brief Initialize the pipeline
      
      \param ObsRecord - observatories record that matches observatory names to the 
                          path to the corresponding CalTable
			  
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool initPipeline(Record ObsRecord);
    

    /*!
      \brief Process one event

      \param evname - path to the eventfile to be processed
      \param Az - value for the azimuth direction [in deg] (e.g. from KASCADE)
      \param El - value for the elevation [in deg] (e.g. from KASCADE)
      \param distance - value for the distance parameter [in m]
      \param XC - x-position of the shower center [in m]
      \param YC - y-position of the shower center [in m]
      \param RotatePos - rotate the XC/YC position (set to False if XC/YC already
             in LOPES coordinates)
      \param PlotPrefix - prefix (including path) for the plots to be generated
      \param generatePlots - generate the postscript plots
      \param FlaggedAntIDs - list of antenna IDs that are to be flagged.
      \param verbose - produce verbose output on the commandline.
      \param simplexFit - fit the direction with a simple simplex fit
      \param ExtraDelay - additional delay to shift the data in time.
      \param doTVcal - perform the phase calibration on the TV transmitter
             (1: yes, 0: no, -1: use default)

      \return Record with the results.
    */
    Record ProcessEvent(String evname,
			Double Az,
			Double El,
			Double distance,
			Double XC, 
			Double YC,
			Bool RotatePos,
			string PlotPrefix=String(),
			Bool generatePlots=False,
			Vector<Int> FlaggedAntIDs=Vector<Int>(),
			Bool verbose=False,
			Bool simplexFit=False,
			Double ExtraDelay=0.,
			int doTVcal=-1);

  protected:

    //! the new pipeline object (like CRinvFFT + upsampling)
    CompletePipeline *pipeline_p;
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (analyseLOPESevent2 const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* ANALYSELOPESEVENT2_H */
  
