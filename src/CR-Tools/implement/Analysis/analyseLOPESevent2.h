/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Frank Schroeder (<mail>)                                              *
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

#include <crtools.h>

// Standard library header files
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#ifdef HAVE_STARTOOLS

#include <Analysis/analyseLOPESevent.h>
#include <Analysis/CompletePipeline.h>

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
class analyseLOPESevent2 : public analyseLOPESevent {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    analyseLOPESevent2 ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another analyseLOPESevent2 object from which to create
             this new one.
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
      \brief Get the upsampling exponent

      \return upsamplingExponent -- The upsampling factor is 2^upsamplingExponent
    */
    inline unsigned int getUpsamplingExponent () {
      return upsamplingExponent;
    }
    
    
    /*!
      \brief Set the upsampling exponent for LOPES-Star-Upsampling

      \param upExp -- The upsampling factor is 2^upExp (0 = no upsampling)
    */
    inline void setUpsamplingExponent (unsigned int const &upExp) {
      upsamplingExponent = upExp;
    }

    /*!
      \brief Get time window of the interval to search for radio pulse

      \return ccWindowWidth_p -- time window for search of radio pulse
    */
    inline double getCCWindowWidth () {
       return ccWindowWidth_p;
    }

    /*!
      \brief Set time window of the interval to search for radio pulse

      \return ccWindowWidth_p -- time window for search of radio pulse
    */
    inline void setCCWindowWidth (double const &ccWindowWidth) {
       ccWindowWidth_p = ccWindowWidth;
    }



    /*!
      \brief Initialize the pipeline
      
      \param ObsRecord - observatories record that matches observatory names to the 
                          path to the corresponding CalTable
			  
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool initPipeline(Record ObsRecord);
    

    /*!
      \brief Process one event using the CompletePipeline class

      \param evname          - path to the eventfile to be processed
      \param Az              - value for the azimuth direction [in deg] (e.g. from KASCADE)
      \param El              - value for the elevation [in deg] (e.g. from KASCADE)
      \param distance        - value for the distance parameter [in m]
      \param XC              - x-position of the shower center [in m]
      \param YC              - y-position of the shower center [in m]
      \param RotatePos       - rotate the XC/YC position (set to False if XC/YC already
                               in LOPES coordinates)
      \param PlotPrefix      - prefix (including path) for the plots to be generated
      \param generatePlots   - generate the postscript plots
      \param FlaggedAntIDs   - list of antenna IDs that are to be flagged.
      \param verbose         - produce verbose output on the commandline.
      \param simplexFit      - fit the direction with a simple simplex fit
      \param ExtraDelay      - additional delay to shift the data in time.
      \param doTVcal         - perform the phase calibration on the TV transmitter
                               (1: yes, 0: no, -1: use default)
      \param doGainCal       - apply calibration of electrical fieldstrength
      \param doDispersionCal - correct for the dispersion (frequency dependend PhaseCal values)
      \param doDelayCal      - correct for general delay of the antennas
      \param UpSamplingRate  - Samplerate for upsampling. If smaller than the original
                               samplerate (80MHz for LOPES) then no upsampling is done.
			            (Check the docs of <tt>UpSampledDR<\tt> for more info.)
      \param Polarization    - Polarization type to select only part of the antennas 
                               ("ANY": ignore antenna polarization)
      \param SinglePlots     - makes a plot for each antenna
      \param PlotRawData     - Plots the raw data FX
      \param CalculateMaxima - Finds the maximum and the minimum of the trace in the plot range
      \param listCalcMaxima  - output version of CalculateMax
      \param printShowerCoordinates -	 print the x,y and total distance between antenna and shower core in shower coordinates

      \return Record with the results.
    */
    Record RunPipeline (const string& evname,
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
			int doTVcal=-1,
			bool doGainCal=true,
			bool doDispersionCal=true,
			bool doDelayCal=true,
			Double UpSamplingRate=0.,
			String Polarization="ANY",
			bool SinglePlots=false,
			bool PlotRawData=false,
			bool CalculateMaxima=false,
			bool listCalcMaxima=false,
			bool printShowerCoordinates=false);

    /*!
      \brief Prints the distances of the antennas to the core (in shower coordinates)
      
      \param distances  - distances from antennas to shower core (should be in shower coordinates)
      \param AntPos     - antenna positions (in shower coordinates)
      \param Az         - value for the azimuth direction [in deg]
      \param El         - value for the elevation [in deg]
      \param XC         - x-position of the shower center [in m]
      \param YC         - y-position of the shower center [in m]
      \param date - date of the event in seconds
    */
    void printAntennaDistances (const Vector <double>& distances,
                                const Matrix<double>& AntPos,
                                const double& Az,
                                const double& El,
                                const double& Xc,
                                const double& Yc,
                                const unsigned int date) const;

    /*!
      \brief Creates a summary postscript of all created plots (uses LaTeX)
      
      \param filename - name of the file for the summary plot (without final ".ps").
      \param columns - number of columns in the summary (if columns=0, nothing is done)
    */
    void summaryPlot(string filename,
                     unsigned int columns = 3);

  protected:

    //! the new pipeline objects, one for the input, one for the upsampling (like CRinvFFT + LopesSTAR-upsampling)
    CompletePipeline pipeline, upsamplePipe;
    //! switcher to the used pipeline (pipeline or upsamplePipe)
    CompletePipeline *CompleteBeamPipe_p;
    
  private:
   
    //! upsampling will be done by a factor of 2^upsamplingExponent
    unsigned int upsamplingExponent;

    //! time window of the interval to search for radio pulse
    Double ccWindowWidth_p;
  };
  
} // Namespace CR -- end

#endif /* HAVE_STARTOOLS */

#endif /* ANALYSELOPESEVENT2_H */
  
