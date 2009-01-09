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
#include <iostream>

// Header files for CR-Tools
#include <crtools.h>
#include <Analysis/CRinvFFT.h>
#include <Analysis/CRflaggingPlugin.h>
#include <Analysis/fitCR2gauss.h>
#include <Data/LopesEventIn.h>
#include <Data/UpSampledDR.h>
#include <Display/SimplePlot.h>

// CASA/casacore header files
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/RecordField.h>
#include <casa/Containers/List.h>
#include <casa/Containers/ListIO.h>
#include <casa/Containers/OrderedMap.h>
#include <casa/Exceptions/Error.h>
#include <scimath/Mathematics/FFTServer.h>
#include <scimath/Mathematics/InterpolateArray1D.h>

using casa::Bool;
using casa::Double;
using casa::Matrix;
using casa::String;
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
      
      // set the interval used in the calculation of the background
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
    //! the pipeline objects, one for the input, one for the upsampling, and the pointer use for the beamforming
    CRinvFFT *pipeline_p, *upsamplePipe_p, *beamPipe_p;
    
    //! the DataReader object to read in LOPES events
    LopesEventIn *lev_p;
    
    //! the DataReader object for the upsampling
    UpSampledDR *upsampler_p;

    //! the pointer to the DataReader used for the beamforming
    DataReader *beamformDR_p;

    //! the used fitobject
    fitCR2gauss fitObject;
    
    //! the flagging object
    CRflaggingPlugin flagger;
    
    //! Start of the interval used in the calculation of the background, fraction of data length
    Double remoteStart_p;
    //! Stop of the interval used in the calculation of the background, fraction of data length
    Double remoteStop_p;
    Vector<uInt> remoteRange_p;
    //! 
    Double fitStart_p;
    //! 
    Double fitStop_p;
    //! Start time of the interval diplayed in the plot
    Double plotStart_p;
    //! Stop time of the interval diplayed in the plot
    Double plotStop_p;
    //! Start frequency for the analysis
    Double freqStart_p;
    //! Stop frequency for the analysis
    Double freqStop_p;
    //! Strength of the mean-filter used for the smoothing
    int filterStrength_p;
    //! Which polarization to use
    String Polarization_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    analyseLOPESevent ();
    
    /*!
      \brief Argumented constructor

      \param remoteStart -- Start of the interval used in the calculation of the
             background, fraction of data length
      \param remoteStop -- Stop of the interval used in the calculation of the
             background, fraction of data length
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

    //@{
    /*!
      \brief Get the Start of the interval used in the calculation of the background

      \return remoteStart - Start of the interval used in the calculation of the
              background, fraction of data length
    */
    inline double remoteStart () {
      return remoteStart_p;
    }
    
    /*!
      \brief Set the Start of the interval used in the calculation of the background

      \param remoteStart - Start of the interval used in the calculation of the
             background, fraction of data length
    */
    inline void setRemoteStart (double const &remoteStart) {
      remoteStart_p = remoteStart;
    }
    
    /*!
      \brief Get the Stop of the interval used in the calculation of the background

      \return remoteStop - Stop of the interval used in the calculation of the
              background, fraction of data length
    */
    inline double remoteStop () {
      return remoteStop_p;
    }
    
    /*!
      \brief Set the Stop of the interval used in the calculation of the background

      \param remoteStop - Stop of the interval used in the calculation of the
             background, fraction of data length
    */
    inline void setRemoteStop (double const &remoteStop) {
      remoteStop_p = remoteStop;
    }

    /*!
      \brief Set the interval used in the calculation of the background

      \param remoteStart - Start of the interval used in the calculation of the background,
             fraction of data length
      \param remoteStop - Stop of the interval used in the calculation of the background,
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
      \brief Get the start frequency for the analysis

      \return freqStart -- Start frequency for the analysis
    */
    inline double getFreqStart () {
      return freqStart_p;
    }

    /*!
      \brief Set the start frequency for the analysis

      \param freqStart -- Start frequency for the analysis
    */
    inline void setFreqStart (double const &freqStart) {
      freqStart_p = freqStart;
    }

    /*!
      \brief Get the stop frequency for the analysis

      \return freqStop -- Stop frequency for the analysis
    */
    inline double getFreqStop () {
      return freqStop_p;
    }

    /*!
      \brief Set the stop frequency for the analysis

      \param freqStop -- Stop frequency for the analysis
    */
    inline void setFreqStop (double const &freqStop) {
      freqStop_p = freqStop;
    }

    /*!
      \brief Set the interval for the analysis

      \param freqStart -- Start frequency for the analysis
      \param freqStop  -- Stop frequency for the analysis
    */
    inline void setFreqInterval (double const &freqStart,
                                       double const &freqStop) {
      freqStart_p = freqStart;
      freqStop_p  = freqStop;
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
    //@}

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

      \param evname         - path to the eventfile to be processed
      \param Az             - value for the azimuth direction [in deg] (e.g. from KASCADE)
      \param El             - value for the elevation [in deg] (e.g. from KASCADE)
      \param distance       - value for the distance parameter [in m]
      \param XC             - x-position of the shower center [in m]
      \param YC             - y-position of the shower center [in m]
      \param RotatePos      - rotate the XC/YC position (set to False if XC/YC already
                              in LOPES coordinates)
      \param PlotPrefix     - prefix (including path) for the plots to be generated
      \param generatePlots  - generate the postscript plots
      \param FlaggedAntIDs  - list of antenna IDs that are to be flagged.
      \param verbose        - produce verbose output on the commandline.
      \param simplexFit     - fit the direction with a simple simplex fit
      \param ExtraDelay     - additional delay to shift the data in time.
      \param doTVcal        - perform the phase calibration on the TV transmitter
                              (1: yes, 0: no, -1: use default)
      \param UpSamplingRate - Samplerate for upsampling. If smaller than the original
                              samplerate (80MHz for LOPES) then no upsampling is done.
			      (Check the docs of <tt>UpSampledDR<\tt> for more info.)
      \param Polarization   - Polarization type to select only part of the antennas 
                              ("ANY": ignore antenna polarization)

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
			int doTVcal=-1,
			Double UpSamplingRate=0.,
			String Polarization="ANY");


    /*!
      \brief Setup one event, perform calibration and filtering, sets beamformDR_p and beamPipe_p

      \param evname           - path to the eventfile to be processed
      \param doTVcal          - perform the phase calibration on the TV transmitter
                                (1: yes, 0: no, -1: use default)
      \param FlaggedAntIDs    - list of antenna IDs that are to be flagged.
      \param AntennaSelection - Mask (boolean array) which Antenna(-channels) are selected (modified in place)
      \param UpSamplingRate   - Samplerate for upsampling. If smaller than the original
                                samplerate (80MHz for LOPES) then no upsampling is done.
                                (Check the docs of <tt>UpSampledDR<\tt> for more info.)
      \param ExtraDelay       - additional delay to shift the data in time.
      \param verbose          - produce verbose output on the commandline.
      \param doGainCal        - does the calibration of the electric fieldstrength
      \param doDispersionCal  - corrects the dispersion (frequency dependend delay)
      \param doDelayCal       - corrects the delay of each antenna
      \param doRFImitigation  - suppresses narrow band noise

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool SetupEvent(String evname, 
		    int doTVcal,
		    Vector<Int> FlaggedAntIDs, 
		    Vector<Bool> &AntennaSelection,
		    Double UpSamplingRate,
		    Double ExtraDelay,
		    Bool verbose,
		    Bool doGainCal=True,
		    Bool doDispersionCal=True,
		    Bool doDelayCal=True,
		    Bool doRFImitigation=True);
    
    /*!
      \brief Perform the direction fitting

      \param Az             - value for the azimuth direction [in deg] (modified in place)
      \param El             - value for the elevation [in deg] (modified in place)
      \param distance       - value for the distance parameter [in m] (modified in place)
      \param center         - position in time of the peak [in s] (modified in place)
      \param XC             - x-position of the shower center [in m]
      \param YC             - y-position of the shower center [in m]
      \param RotatePos      - rotate the XC/YC position (set to False if XC/YC already
                              in LOPES coordinates)
      \param AntennaSelection - Mask (boolean array) which Antenna(-channels) are selected 
      \param Polarization   - Polarization type to select only part of the antennas 
                              ("": use stored setting;"ANY": ignore antenna polarization)
      \param simplexFit     - fit the direction with a simple simplex fit
      \param verbose        - produce verbose output on the commandline.

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool doPositionFitting(Double &Az, Double &El, Double &distance, 
			   Double &center,
			   Double &XC, Double &YC, Bool RotatePos,
			   Vector<Bool> AntennaSelection,
			   String Polarization="",
			   Bool simplexFit=True,
			   Bool verbose=False);
    
   /*!
      \brief Perform the direction fitting

      \param Az               - value for the azimuth direction [in deg] (modified in place)
      \param El               - value for the elevation [in deg] (modified in place)
      \param distance         - value for the distance parameter [in m] (modified in place)
      \param center           - position in time of the peak [in s] (modified in place)
      \param AntennaSelection - Mask (boolean array) which Antenna(-channels) are selected 
      \param evname           - path to the eventfile to be processed
      \param erg              - Record with the final results.
      \param fiterg           - Record with the results from the fit.
      \param Polarization     - Polarization type to select only part of the antennas 
                                ("": use stored setting;"ANY": ignore antenna polarization)
      \param verbose          - produce verbose output on the commandline.
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
   */
    Bool GaussFitData(Double &Az, Double &El, Double &distance, Double &center, 
		      Vector<Bool> AntennaSelection, String evname, 
		      Record &erg, Record &fiterg, 
		      String Polarization="", Bool verbose=False);

   /*!
      \brief Generate the standard-plots

     \param PlotPrefix       - prefix (including path) for the plots to be generated
     \param ccgauss          - trace (time series data) with the gaussian fir to the cc-beam
     \param xgauss           - trace (time series data) with the gaussian fir to the x-beam
     \param AntennaSelection - Mask (boolean array) which Antenna(-channels) are selected 
     \param Polarization     - Polarization type to select only part of the antennas 
                               ("": use stored setting;"ANY": ignore antenna polarization)


      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
   */
    Bool doGeneratePlots(String PlotPrefix, Vector<Double> ccgauss, Vector<Double> xgauss, 
			 Vector<Bool> AntennaSelection,	String Polarization="");
      


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

    Matrix<Double> toShower(const Matrix<Double> & pos,
			    Double Az,
			    Double El);

  protected: //this methods are protected to make them available in child class
    
    /*!
      \brief initialize the object
    */
    void init(void);

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
    
  };
  
} // Namespace CR -- end

#endif /* ANALYSELOPESEVENT_H */
  
