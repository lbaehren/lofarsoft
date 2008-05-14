/*-------------------------------------------------------------------------*
 | $Id:: analyseLOPESevent.h,v 1.8 2007/08/03 12:27:24 horneff Exp       $ |
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

using casa::Bool;
using casa::Double;
using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class analyseLOPESevent
    
    \ingroup Analysis
    
    \brief Class to merge required functionality for processing LOPES events
    
    \author Andreas Horneffer

    \date 2007/06/19

    \test tanalyseLOPESevent.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>

    <ol>
      <li>Set custom values for the various intervals:
      \code
      analyseLOPESevent event;
      
      // set the interval considered in the analysis
      event.setRemoteInterval(1./3.,
                              2./3.);
      // set the interval considered in the fitting
      event.setFitInterval(-2e-6,
                           -1.7e-6);
      // set the interval displayed in the generated plot
      event.setPlotInterval(-2.05e-6,
                            -1.55e-6);
      \endcode
    </ol>
    
  */  
  class analyseLOPESevent {
    
  protected: //variables are protected to make them available in child class
    //! the pipeline object
    CRinvFFT *pipeline_p;
    
    //! the DataReader object to read in LOPES events
    LopesEventIn *lev_p;
    
    //! the used fitobject
    fitCR2gauss fitObject;
    
    //! the flagging object
    CRflaggingPlugin flagger;
    
    //! Start of the interval considered in the analysis, fraction of data length
    Double remoteStart_p;
    //! Stop of the interval considered in the analysis, fraction of data length
    Double remoteStop_p;
    //! 
    Double fitStart_p;
    //! 
    Double fitStop_p;
    //! Start time of the interval diplayed in the plot
    Double plotStart_p;
    //! Stop time of the interval diplayed in the plot
    Double plotStop_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    analyseLOPESevent ();
    
    /*!
      \brief Argumented constructor

      \param remoteStart - Start of the interval considered in the analysis,
             fraction of data length
      \param remoteStop - Stop of the interval considered in the analysis,
             fraction of data length
      \param fitStart  -- The start of the interval to be considered in the fit
      \param fitStop   -- The stop of the interval to be considered in the fit
      \param plotStart -- Start time of the interval diplayed in the plot
      \param plotStop  -- Stop time of the interval diplayed in the plot
    */
    analyseLOPESevent (double const &remoteStart,
		       double const &remoteStop,
		       double const &fitStart,
		       double const &fitStop,
		       double const &plotStart,
		       double const &plotStop);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    virtual ~analyseLOPESevent ();
        
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the Start of the interval considered in the analysis

      \return remoteStart - Start of the interval considered in the analysis,
              fraction of data length
    */
    inline double remoteStart () {
      return remoteStart_p;
    }
    
    /*!
      \brief Set the Start of the interval considered in the analysis

      \param remoteStart - Start of the interval considered in the analysis,
             fraction of data length
    */
    inline void setRemoteStart (double const &remoteStart) {
      remoteStart_p = remoteStart;
    }
    
    /*!
      \brief Get the Stop of the interval considered in the analysis

      \return remoteStop - Stop of the interval considered in the analysis,
              fraction of data length
    */
    inline double remoteStop () {
      return remoteStop_p;
    }
    
    /*!
      \brief Set the Stop of the interval considered in the analysis

      \param remoteStop - Stop of the interval considered in the analysis,
             fraction of data length
    */
    inline void setRemoteStop (double const &remoteStop) {
      remoteStop_p = remoteStop;
    }

    /*!
      \brief Set the interval considered in the analysis

      \param remoteStart - Start of the interval considered in the analysis,
             fraction of data length
      \param remoteStop - Stop of the interval considered in the analysis,
             fraction of data length
    */
    inline void setRemoteInterval (double const &remoteStart,
				   double const &remoteStop) {
      remoteStart_p = remoteStart;
      remoteStop_p  = remoteStop;
    }

    /*!
      \brief Get the start of the interval to be considered in the fit

      \return fitStart - The start of the interval to be considered in the fit
     */
    inline double fitStart () {
      return fitStart_p;
    }
    
    /*!
      \brief Set the start of the interval to be considered in the fit

      \param fitStart - The start of the interval to be considered in the fit
     */
    inline void setFitStart (double const &fitStart) {
      fitStart_p = fitStart;
    }
    
    /*!
      \brief Get the stop of the interval to be considered in the fit

      \return fitStop - The stop of the interval to be considered in the fit
     */
    inline double fitStop () {
      return fitStop_p;
    }
    
    /*!
      \brief Set the stop of the interval to be considered in the fit
      
      \param fitStop - The stop of the interval to be considered in the fit
    */
    inline void setFitStop (double const &fitStop) {
      fitStop_p = fitStop;
    }
    
    /*!
      \brief Set the interval to be considered in the fit
      
      \param fitStart - The start of the interval to be considered in the fit
      \param fitStop  - The stop of the interval to be considered in the fit
    */
    inline void setFitInterval (double const &fitStart,
				double const &fitStop) {
      fitStart_p = fitStart;
      fitStop_p  = fitStop;
    }

    /*!
      \brief Get the start time of the interval diplayed in the plot

      \return plotStart -- Start time of the interval diplayed in the plot
    */
    inline double plotStart () {
      return plotStart_p;
    }
    
    /*!
      \brief Set the start time of the interval diplayed in the plot

      \param plotStart -- Start time of the interval diplayed in the plot
    */
    inline void setPlotStart (double const &plotStart) {
      plotStart_p = plotStart;
    }
    
    /*!
      \brief Get the stop time of the interval diplayed in the plot

      \return plotStop -- Stop time of the interval diplayed in the plot
    */
    inline double plotStop () {
      return plotStop_p;
    }
    
    /*!
      \brief Set the stop time of the interval diplayed in the plot

      \param plotStop -- Stop time of the interval diplayed in the plot
    */
    inline void setPlotStop (double const &plotStop) {
      plotStop_p = plotStop;
    }

    /*!
      \brief Set the interval diplayed in the plot

      \param plotStart -- Start time of the interval diplayed in the plot
      \param plotStop  -- Stop time of the interval diplayed in the plot
    */
    inline void setPlotInterval (double const &plotStart,
				 double const &plotStop) {
      plotStart_p = plotStart;
      plotStop_p  = plotStop;
    }
    
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
    virtual inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    virtual void summary (std::ostream &os);    

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
			String PlotPrefix=String(),
			Bool generatePlots=False,
			Vector<Int> FlaggedAntIDs=Vector<Int>(),
			Bool verbose=False,
			Bool simplexFit=False,
			Double ExtraDelay=0.,
			int doTVcal=-1);

    /*!
      \brief Fit the position with a simplex fit
      
      \param Az - (initial) value for the azimuth direction [in deg]; modified in
             place
      \param El - (initial) value for the elevation [in deg]; modified in place
      \param distance - (initial) value for the distance parameter [in m]; modified
             in place
      \param center - (initial) value for the position of the pulse (center of the
             gaussian) [in s]; modified in place
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

    /*!
      \brief Evaluate a smallish grid around to find a good starting point for the simplenx fit
      
      \param Az - (initial) value for the azimuth direction [in deg]; modified in
             place
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

    /*!
      \brief Convert positions from lab (Earth-bound) coordinates to air shower coordinates
      
      \param pos - Matrix with the positions to convert
      \param Az - Azimuth angle in degrees
      \param El - Elevation angle in degrees
      
      \return the converted positions
    */

    Matrix<Double> toShower(Matrix<Double> & pos,
			    Double Az,
			    Double El);

  protected: //this methods are protected to make them available in child class
    
    /*!
      \brief clear the object
    */
    void clear(void);

    /*!
      \brief needed for FitPosition

      \param az -- Azimuth component of the direction
      \param el -- Elevation component of the direction
      \param dist -- Value of the distance parameter
      \param AntennaSelection --
      \param centerp -- 
    */
    Double getHeight (Double az,
		      Double el,
		      Double dist,
		      Vector<Bool> AntennaSelection,
		      Double *centerp=NULL);

    /*!
      \brief Gets the height of the X-Beam at this position...
    */
    Double getHeight2 (Double az,
		       Double el,
		       Double dist,
		       Double Center,
		       Vector<Bool> AntennaSelection);
    
  };
  
} // Namespace CR -- end

#endif /* ANALYSELOPESEVENT_H */
  
