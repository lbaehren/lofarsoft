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

#ifndef FIRSTSTAGEPIPELINE_H
#define FIRSTSTAGEPIPELINE_H

// Standard library header files
#include <string>

#include <crtools.h>
#include <IO/DataReader.h>
#include <Calibration/CalTableReader.h> 
#include <Calibration/CalTableInterpolater.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class FirstStagePipeline
    
    \ingroup Analysis
    
    \brief First stage of a pipeline: Opening file and putting the calibration values 
           into the DataReader
    
    \author Andreas Horneffer

    \date 2007/01/11

    \test tFirstStagePipeline.cc
    
    <h3>Prerequisite</h3>    
    <ul>
      <li> Open an event file via the DataReader
      <li> Get the calibration values from the CalTables
      <li> Feed the calibration values into the DataReader
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>

    Please have a look at "tFirstStagePipeline.cc" 
    I (tried to) added quit a bit of comments into the code...
    
  */  
  class FirstStagePipeline {
  protected:
    
    /*!
      \brief Generate verbose output;
    */
    Bool verbose;

    /*!
      \brief Do the gain calibration (default=True)
    */
    Bool DoGainCal_p;

    /*!
      \brief Correct for dispersion (default=True)
    */
    Bool DoDispersionCal_p;

    /*!
      \brief Correct for the delays (default=True)
    */
    Bool DoDelayCal_p;

    /*!
      \brief Have the CalTableInterpolator objects been initialized?
    */
    Bool InterpInit;

    //! Is the data cache from the SecondStagePipeline up to date?
    Bool SecondStageCacheValid_p;

    //! Has the antenna gain interpolater from CRinvFFT been initialized?
    Bool AntGainInterpInit_p;
    
    /*!
      \brief The CalTableInterpolator objects for the different fields
    */
    CalTableInterpolater<Double> *InterElGainCal_p;
    CalTableInterpolater<DComplex> *InterPhaseCal_p;
    
    
    /*!
      \brief blocksize and samplerate, needed to check if freqency axis has changed
    */
    uInt blocksize;
    Double samplerate;

    /*!
      \brief Record with the pathes to the CalTables
    */
    Record ObsToCTPath;

    /*!
      \brief The CalTableReader
    */
    CalTableReader *CTRead;

    /*!
      \brief Lowest frequency set for analysis [in Hz]
    */
    Double setStartFreq_p;
    /*!
      \brief Highest frequency set for analysis [in Hz]
    */
    Double setStopFreq_p;

    /*!
      \brief Lowest frequency actually used for analysis [in Hz]
    */
    Double startFreq_p;
    /*!
      \brief Highest frequency actually used for analysis [in Hz]
    */
    Double stopFreq_p;

    // ---------------------------------------------------------- Private Methods

    /*!
      \brief Initialize the CalTableInterpolaters

      \param *dr - Pointer to the DataReader that holds the data
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool Init_CalTableInter(DataReader *dr);
    
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    FirstStagePipeline ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~FirstStagePipeline ();
    
    // ---------------------------------------------------------------- Operators
    
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, FirstStagePipeline.
    */
    std::string className () const {
      return "FirstStagePipeline";
    }
    

    /*!
      \brief Enable/disable doing the gain calibration (electrical fieldstrength)

      \param DoGainCal - do the calibartion of the fieldstrength?
    */
    inline void doGainCal(Bool const &DoGainCal=True) {
      DoGainCal_p = DoGainCal;
      SecondStageCacheValid_p = False;
      if (verbose && !DoGainCal){
	cout << "FirstStagePipeline:: switched off gain calibration." << endl;
      };
    }


    /*!
      \brief Enable/disable doing the disperion calibration (aplying frequency dependent phase corrections)

      \param DoDispesionCal - correct for the dispersion of the electronics?
                              (the values are stored in the PhaseCal calibration table)
    */
    inline void doDispersionCal(Bool const &DoDispersionCal=True) {
      DoDispersionCal_p = DoDispersionCal;
      SecondStageCacheValid_p = False;
      if (verbose && !DoDispersionCal){
	cout << "FirstStagePipeline:: switched off correction for dispersion (frequency dependent phase behaviour)." << endl;
      };
    }


    /*!
      \brief Enable/disable applying delays of the measurement chain

      \param DoDelayCal - correct for the delays of the LOPES setup?
    */
    inline void doDelayCal(Bool const &DoDelayCal=True) {
      DoDelayCal_p = DoDelayCal;
      SecondStageCacheValid_p = False;
      if (verbose && !DoDelayCal){
	cout << "FirstStagePipeline:: switched off delay correction." << endl;
      };
    }


    /*!
      \brief Get the start frequency used for the analysis (lower frequencies will be supressed)

      \return startFreq -- Start frequency used in the analysis [in Hz]
    */
    inline double getStartFreq () {
      return startFreq_p;
    }

    /*!
      \brief Set the start frequency used for the analysis (lower frequencies will be supressed)

      \param startFreq -- Start frequency for the analysis [in Hz]
    */
    inline void setStartFreq (double const &startFreq) {
      setStartFreq_p = startFreq;
      startFreq_p = startFreq;
      SecondStageCacheValid_p = False;
    }

    /*!
      \brief Get the stop frequency used for the analysis (higher frequencies will be supressed)

      \return stopFreq  -- Stop frequency used in the analysis [in Hz]
    */
    inline double getStopFreq () {
      return stopFreq_p;
    }

    /*!
      \brief Set the stop frequency used for the analysis (higher frequencies will be supressed)

      \param stopFreq  -- Stop frequency for the analysis [in Hz]
    */
    inline void setStopFreq (double const &stopFreq) {
      setStopFreq_p = stopFreq;
      stopFreq_p = stopFreq;
      SecondStageCacheValid_p = False;
    }

    /*!
      \brief Set the interval for the frequencies used in the analysis (start and stop)

      \param startFreq -- Start frequency for the analysis [in Hz]
      \param stopFreq  -- Stop frequency for the analysis [in Hz]
    */
    inline void setFreqInterval (double const &startFreq,
                                   double const &stopFreq) {
      setStartFreq_p = startFreq;
      setStopFreq_p = stopFreq;
      startFreq_p = startFreq;
      stopFreq_p = stopFreq;
      SecondStageCacheValid_p = False;
    }


    // ------------------------------------------------------------------ Methods
    /*!
      \brief Initialize a new event, and fill the DataReader with the needed calibration 
      values. According to the header data (especially "Observatory", "Date", and 
      "AntennaIDs") the calibration values are retrieved and put into the data reader object.
      
      \param *dr      - Pointer to an already existing DataReader object, that already has
                       (header-)data loaded. 
      \param setCal   - Set the FFT2calFFT array?
                        
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */

    Bool InitEvent(DataReader *dr, Bool setCal=True);
    
    /*!
      \brief Retrieve the CalTableReader object.

      The pipeline object creates it's own CalTableReader. This can also be used by
      other routines.
      The returned object is valid as at least as long as the pipeline object exists 
      and no new event was opened. If a new event was opened, the CalTableReader may 
      (but does not have to) change.
            
      \return CalTableReader -- Pointer to the CalTableReader object. 
    */

    CalTableReader *GetCalTableReader(){
      return CTRead;
    };
    
    /*!
      \brief Set the record that matches observatory names to the path to the corresponding CalTable

      \param InputRecord - record with the data

      \return Returns True
    */

    Bool SetObsRecord(Record InputRecord) {
      ObsToCTPath.assign(InputRecord);
      return True;
    };

    /*!
      \brief Set the verbosity
    */
    inline void setVerbosity(Bool newVerbosity) { verbose = newVerbosity; };
    
  private:
    
    /*!
      \brief Set the FFT2calFFT array with 

      \param *dr      - Pointer to an already existing DataReader object, that already has
                       (header-)data loaded. 
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool setCalibration(DataReader *dr);
    
    /*!
      \brief Standard initialization
    */
    void init(void);

   /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* FIRSTSTAGEPIPELINE_H */
  
