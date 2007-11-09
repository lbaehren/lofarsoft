/*-------------------------------------------------------------------------*
 | $Id: analyseLOPESevent.h,v 1.8 2007/08/03 12:27:24 horneff Exp $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#ifndef ANALYSELOPESEVENT_H
#define ANALYSELOPESEVENT_H

// Standard library header files
#include <string>

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Record.h>
#include <casa/Exceptions/Error.h>
#include <scimath/Mathematics/FFTServer.h>
#include <scimath/Mathematics/InterpolateArray1D.h>

#include <Analysis/CRinvFFT.h>
#include <Analysis/CRflaggingPlugin.h>
#include <Analysis/fitCR2gauss.h>
#include <Data/LopesEventIn.h>
#include <Display/SimplePlot.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class analyseLOPESevent
    
    \ingroup Analysis
    
    \brief Brief description for class analyseLOPESevent
    
    \author Andreas Horneffer

    \date 2007/06/19

    \test tanalyseLOPESevent.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class analyseLOPESevent {
    
    //! the pipeline object
    CRinvFFT *pipeline_p;

    //! the DataReader object to read in LOPES events
    LopesEventIn *lev_p;

    //! the used fitobject
    fitCR2gauss fitObject;

    //! the flagging object
    CRflaggingPlugin flagger;

    // \brief div. parameters
    Double remoteStart;
    Double remoteStop;
    Double fitstart;
    Double fitstop;
    Double plotstart;
    Double plotstop;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    analyseLOPESevent ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~analyseLOPESevent ();
        
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the internal pipeline object
      
      \return pointer to the pipeline object
    */
    inline CRinvFFT * GetPipeline() {
      return pipeline_p;
    };
    
    /*!
      \brief Get the internal DataReader object
      
      \return pointer to the DataReader object
    */
    inline LopesEventIn * GetDataReader() {
      return lev_p;
    };
    
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, analyseLOPESevent.
    */
    std::string className () const {
      return "analyseLOPESevent";
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
    

    /*
      \brief Process one event

      \param evname - path to the eventfile to be processed
      \param Az - value for the azimuth direction [in deg] (e.g. from KASCADE)
      \param El - value for the elevation [in deg] (e.g. from KASCADE)
      \param distance - value for the distance parameter [in m]
      \param XC - x-position of the shower center [in m]
      \param YC - y-position of the shower center [in m]
      \param RotatePos - rotate the XC/YC position (set to False if XC/YC already in LOPES coordinates)
      \param PlotPrefix - prefix (including path) for the plots to be generated
      \param generatePlots - generate the postscript plots
      \param FlaggedAntIDs - list of antenna IDs that are to be flagged.
      \param verbose - produce verbose output on the commandline.
      \param simplexFit - fir the direction with a simple simplex fit
      \param ExtraDelay - additional delay to shift the data in time.
      \param doTVcal - perform the phase calibration on the TV transmitter (1: yes, 0: no, -1: use default)

      \return Record with the results.
    */
    Record ProcessEvent(String evname,
			Double Az,
			Double El,
			Double distance,
			Double XC, 
			Double YC,
			Bool RotatePos,
			String PlotPrefix=String(),
			Bool generatePlots=False,
			Vector<Int> FlaggedAntIDs=Vector<Int>(),
			Bool verbose=False,
			Bool simplexFit=False,
			Double ExtraDelay=0.,
			int doTVcal=-1);

    /*
      \brief Fit the position with a simplex fit
      
      \param Az - (initial) value for the azimuth direction [in deg]; modified in place
      \param El - (initial) value for the elevation [in deg]; modified in place
      \param distance - (initial) value for the distance parameter [in m]; modified in place
      \param center - (initial) value for the position of the pulse (center of the gaussian) [in s]; modified in place
      \param AntennaSelection - Vector of bool to select only part of the antennas.

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool SimplexFit (Double &Az,
		     Double &El,
		     Double &distance,
		     Double &center,
		     Vector<Bool> AntennaSelection);

    Bool SimplexFit2 (Double &Az,
		      Double &El,
		      Double &distance,
		      Double &center,
		      Vector<Bool> AntennaSelection);

    /*
      \brief Evaluate a smallish grid around to find a good starting point for the simplenx fit
      
      \param Az - (initial) value for the azimuth direction [in deg]; modified in place
      \param El - (initial) value for the elevation [in deg]; modified in place
      \param distance - value for the distance parameter [in m]
      \param AntennaSelection - Vector of bool to select only part of the antennas.
      \param center* - position into which the center can be returned

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool evaluateGrid(Double &Az,
		      Double &El,
		      Double &distance,
		      Vector<Bool> AntennaSelection, 
		      Double *centerp=NULL);

    /*
      \brief Convert positions from lab (Earth-bound) coordinates to air shower coordinates
      
      \param pos - Matrix with the positions to convert
      \param Az - Azimuth angle in degrees
      \param El - Elevation angle in degrees
      
      \return the converted positions
    */

    Matrix<Double> toShower(Matrix<Double> & pos,
			    Double Az,
			    Double El);

  private:
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief clear the object
    */
    void clear(void);

    /*!
      \brief needed for FitPosition
    */
    Double getHeight (Double az,
		      Double el,
		      Double dist,
		      Vector<Bool> AntennaSlection,
		      Double *centerp=NULL);

    /*!
      \brief Gets the height of the X-Beam at this position...
    */
    Double getHeight2 (Double az,
		       Double el,
		       Double dist,
		       Double Center,
		       Vector<Bool> AntennaSlection);
    
  };
  
} // Namespace CR -- end

#endif /* ANALYSELOPESEVENT_H */
  
