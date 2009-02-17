/***************************************************************************
 *   Copyright (C) 2007                                                  *
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

/* $Id$*/

#ifndef SECONDSTAGEPIPELINE_H
#define SECONDSTAGEPIPELINE_H


// Standard library header files
#include <string>

// CR-Tools header files
#include <crtools.h>
#include <Analysis/FirstStagePipeline.h>
#include <Calibration/TVCalibrationPlugin.h>
#include <Calibration/RFIMitigationPlugin.h>

// AIPS++/CASA header files
#include <casa/Exceptions/Error.h>
#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class SecondStagePipeline
    
    \ingroup Analysis
    
    \brief Second stage of our pipeline. Feed the data through a number of plugins. 
    
    \author Andreas Horneffer

    \date 2007/02/22

    \test tSecondStagePipeline.cc

    This takes the data out of the first stage, feeds it through a number of plugins 
    (currently only PhaseCalibration and RFIMitigation) and then makes it available to 
    prospective users. 

    Handles only one block of data and assumes that there is no other block.

    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class SecondStagePipeline : public FirstStagePipeline {

  protected:

    /*!
      \brief Local phase calibration object
    */
    //PhaseCalibrationPlugin pCal_p;
    TVCalibrationPlugin pCal_p;

    /*!
      \brief Local RFI mitigation object
    */
    RFIMitigationPlugin rfiM_p;

    /*!
      \brief Do the phase calibration (default=True)
    */
    Bool DoPhaseCal_p;

    /*!
      \brief Do the RFI mitigation (default=True)
    */
    Bool DoRFImitigation_p;

    /*!
      \brief Flags antennas marked as not active in the CalTables (default=True)
    */
    Bool DoFlagNotActiveAnts_p;

    /*!
      \brief Store the data once it is calculated.
    */
    Matrix<DComplex> CachedData_p;

    /*!
      \brief For which event (observation time) was the data cached
    */
    uInt cachedDate_p;   

    /*!
      \brief Which antennas were good and which were not.
    */
    Vector<Bool> AntennaMask_p;

    /*!
      \brief Which antennas were good and which were not.
    */
    Vector<Double> PhaseCalDelays_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    SecondStagePipeline ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~SecondStagePipeline ();
    
    // ---------------------------------------------------------------- Operators
    
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, SecondStagePipeline.
    */
    std::string className () const {
      return "SecondStagePipeline";
    }

    /*!
      \brief Enable/disable doing the phase calibration

      \param DoPhaseCal - do the phase calibration?
    */
    inline void doPhaseCal(Bool const &DoPhaseCal=True) {
      DoPhaseCal_p = DoPhaseCal;
      SecondStageCacheValid_p = False;
      if (verbose && !DoPhaseCal){
	cout << "SecondStagePipeline:: switched off phase calibration." << endl;
      };
    }

    /*!
      \brief Enable/disable doing the RFI mitigation

      \param DoRFImitigation - do the RFI mitigation?
    */
    inline void doRFImitigation(Bool const &DoRFImitigation=True) {
      DoRFImitigation_p = DoRFImitigation;
      SecondStageCacheValid_p = False;
      if (verbose && !DoRFImitigation){
	cout << "SecondStagePipeline:: switched off RFI mitigation." << endl;
      };
    }

    /*!
      \brief Enable/disable flagging antennas marked as not active in the CalTables

      \param DoFlagNotActiveAnts - do the flagging?
    */
    inline void doFlagNotActiveAnts(Bool const &DoFlagNotActiveAnts=True) {
      DoFlagNotActiveAnts_p = DoFlagNotActiveAnts;
      SecondStageCacheValid_p = False;
      if (verbose && !DoFlagNotActiveAnts){
	cout << "SecondStagePipeline:: switched off DoFlagNotActiveAnts." << endl;
      };
    }

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get the processed Frequency domain data. Does some caching.
      
      \return The processed data.
    */
    Matrix<DComplex> GetData(DataReader *dr);


    /*!
      \brief Get the mask of flagged antennas
      
      \return The antenna mask;
    */
    Vector<Bool> GetAntennaMask(DataReader *dr);

    /*!
      \brief Get the delays from the phase calibration
      
      \return Vector with the delays [in seconds];
    */
    Vector<Double> GetPhaseCalDelays(DataReader *dr);
    
    
  private:
    
    /*!
      \brief Initialize the phase calibration plugin

      \return is-initialized -- Returns <tt>True</tt> if the calibration plugin can be used afterwards
    */
    Bool InitPhaseCal(DataReader *dr);

    //    /*!
    //      \brief Initialize the RFI mitigation plugin
    //    */
    //    Bool InitRFIMitigation(DataReader *dr);

    /*!
      \brief Flags antennas which are marked as not-active in the CalTables.

      \return status -- Returns <tt>True</tt> if no error occured.
    */
    Bool FlagNotActiveAntennas(DataReader *dr);

    /*!
      \brief Do the calculations. Gets the data from the DataReader object, feeds
      it through the pipelines and puts the results into the local storage.
    */
    Bool updateCache(DataReader *dr);
     
    /*!
      \brief Initialize the data
    */
    void init();

    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* SECONDSTAGEPIPELINE_H */
  
