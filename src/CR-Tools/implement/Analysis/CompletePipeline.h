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

#ifndef COMPLETEPIPELINE_H
#define COMPLETEPIPELINE_H

#include <crtools.h>

// Standard library header files
#include <iostream>
#include <string>
#include <cmath>
#include <map>

#ifdef HAVE_STARTOOLS

// for random generator
#include <TRandom3.h>
// This class is derived form CRinvFFT an adds upsampling
#include <Analysis/CRinvFFT.h>
// For Plots:
#include <Utilities/SimplePlot.h>
// For Upsampling:
#include <startools/reconstruction.hh>

#include <Analysis/PulseProperties.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class CompletePipeline
    
    \ingroup Analysis
    
    \brief Brief description for class CompletePipeline
    
    \author Frank Schr&ouml;der

    \date 2008/01/30

    \test tCompletePipeline.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>

    This pipeline shall contain upsampling capabilities but is not working yet.
    
    <h3>Example(s)</h3>
    
  */  

  class CompletePipeline : public CRinvFFT {

  protected:
    //! Start time of the interval diplayed in the plot
    double plotStart_p;
    //! Stop time of the interval diplayed in the plot
    double plotStop_p;
    //! Time window for peak search of radio pulse
    double ccWindowWidth_p;
    //! Start frequency for the plot of the spectrum
    Double spectrumStart_p;
    //! Stop frequency for the plot of the spectrum
    Double spectrumStop_p;
    //! File names of created plots 
    vector <string> plotlist;
    //! Last upsampling exponent (-1 if no upsampling was done so far)
    int lastUpsamplingExponent;
    //! Last time upsampling exponent (-1 if no upsampling was done so far)
    int lastTimeUpsamplingExponent;
    //! Contains the upsampled fieldstrength
    Matrix<Double> upFieldStrength;
    //! Selection of upsampled antennas
    vector<bool> upsampledAntennas;                 // warning: don't use CasaCore-Vector as it makes only a flat copy using '='
    //! Contains inerpolated time axis for upsampled fieldstrength
    Vector<Double> upTimeValues;

    //! Polarization considered in analysis (should be set by analyseLOPESevent2::RunPipeline)
    string Polarization;

    //! sets pipeline to calibration mode for processing time calibration measurements
    bool calibrationMode;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    CompletePipeline ();
    
    // ------------------------------------------------------------- Intialize
    
    /*!
      \brief Reset variables
    */
    void init (void);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~CompletePipeline ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another CompletePipeline object from which to make a copy.
    */
    CompletePipeline& operator= (CompletePipeline const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, CompletePipeline.
    */
    std::string className () const {
      return "CompletePipeline";
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
      \brief Get the start time of the interval diplayed in the plot

      \return plotStart -- Start time of the interval diplayed in the plot
    */
    inline double getPlotStart () {
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
    inline double getPlotStop () {
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
      \brief Get the width of the CC-beam window

      \return ccWindowWidth_p -- width of the CC-beam window
    */
    inline double getCCWindowWidth () {
      return ccWindowWidth_p;
    }

    /*!
      \brief Set the width of the CC-beam window

      \param ccWindowWidth -- width of the CC-beam window
    */
    inline void setCCWindowWidth (double const &ccWindowWidth) {
      ccWindowWidth_p = ccWindowWidth;
    }



    /*!
      \brief Get the start frequency for the plot of the spectrum

      \return spectrumStart -- Start frequency for the plot of the spectrum
    */
    inline double getSpectrumStart () {
      return spectrumStart_p;
    }

    /*!
      \brief Set the start frequency for the plot of the spectrum

      \param spectrumStart -- Start frequency for the plot of the spectrum
    */
    inline void setSpectrumStart (double const &spectrumStart) {
      spectrumStart_p = spectrumStart;
    }

    /*!
      \brief Get the stop frequency for the plot of the spectrum

      \return spectrumStop -- Stop frequency for the plot of the spectrum
    */
    inline double getSpectrumStop () {
      return spectrumStop_p;
    }

    /*!
      \brief Set the stop frequency for the plot of the spectrum

      \param spectrumStop -- Stop frequency for the plot of the spectrum
    */
    inline void setSpectrumStop (double const &spectrumStop) {
      spectrumStop_p = spectrumStop;
    }

    /*!
      \brief Set the interval for the plot of the spectrum

      \param spectrumStart -- Start frequency for the plot of the spectrum
      \param spectrumStop  -- Stop frequency for the plot of the spectrum
    */
    inline void setSpectrumInterval (double const &spectrumStart,
                                       double const &spectrumStop) {
      spectrumStart_p = spectrumStart;
      spectrumStop_p  = spectrumStop;
    }


    /*!
      \brief Get the list of the file names of all created plots

      \return plotlist -- list of created plots
    */
    inline vector<string> getPlotList () {
      return plotlist;
    }


    /*!
      \brief Set the polarization considered in the analysis

      \param polarization -- polarization considered in the analysis (ANY, EW or NS)
    */
    inline void setPolarization (const string &polarization) {
      Polarization = polarization;
    }

    /*!
      \brief Get the polarization considered in the analysis

      \return Polarization -- polarization considered in the analysis
    */
    inline string getPolarization () {
      return Polarization;
    }

 
    /*!
      \brief Set the calibration mode

      \param calibration -- switching pipeline to time calibration mode
    */
    inline void setCalibrationMode (const bool &calibration) {
      calibrationMode = calibration;
    }

    /*!
      \brief Get the polarization considered in the analysis

      \return calibrationMode -- time calibration mode (on or off)
    */
    inline bool getCalibrationMode () {
      return calibrationMode;
    }

    /*!
      \brief Caclulates an envelope (uses the LOPES-Star library)

      \param trace            -- any trace to calculate the envelope

      \return envelope        -- envelope of the given trace (has the same lenght)
   */
    Vector<double> envelope(const Vector<double> &trace) const;


    /*!
      \brief Deselects all antennas which do not have the analysed polarization

      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- antennaSelection (passed by reference and changed by this method)
   */
    void deselectectPolarization(DataReader *dr,
                                 Vector<Bool> &antennaSelection);


    /*!
      \brief Gets the upsampled fieldstrength of selected antennas (data are stored for reuse)

      \param DataReader       -- DataReader (LopesEventIn)
      \param upsampling_exp   -- a value > 0 means that data are upsampled by a factor of 2^upsampling_exp
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.
      \param antennaSelection -- Selection of antennas

      \return UpsampledFieldstrength -- Contains the upsampled time series of the selected antennas in colums of matrix
                                        (number of columns = number of elements in the antennaSelection, but columns
                                         corresponding to not selected antennas will not contain meaningless values.)
    */
    Matrix<Double> getUpsampledFieldstrength (DataReader *dr,
                                              const int& upsampling_exp,
                                               Vector<Bool> antennaSelection = Vector<Bool>());

    /*!
      \brief Gets the upsampled raw data FX of selected antennas (data are not stored for reuse)

      \param DataReader         -- DataReader (LopesEventIn)
      \param upsampling_exp     -- a value > 0 means that data are upsampled by a factor of 2^upsampling_exp
                                   use e.g. uspampling_exp = 1 to have twice as many points plotted.
      \param antennaSelection   -- Selection of antennas
      \param offsetSubstraction -- substracts the ADC offset of the trace
      \param voltage            -- returns voltage instead of raw counts

      \return UpsampledFX       -- Contains the upsampled time series of the selected antennas in colums of matrix
                                   (number of columns = number of elements in the antennaSelection, but columns
                                   corresponding to not selected antennas will not contain meaningless values.)
    */
    Matrix<Double> getUpsampledFX (DataReader *dr,
                                   const int& upsampling_exp,
                                   Vector<Bool> antennaSelection = Vector<Bool>(),
                                   const bool& offsetSubstraction = false,
                                   const bool& voltage = false);

    /*!
      \brief Gets the time axis for upsampled data (data are stored for reuse)

      \param DataReader       -- DataReader (LopesEventIn)
      \param upsampling_exp   -- a value > 0 means that there are 2^upsampling_exp data points
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.


      \return UpsampledTime Axis -- The extended time axis containing new time values between the old ones.
                                    The first time value is the same as the first value of the original time axis.
                                    This means, that there are 2^upsampling_exp -1 data points attached after the
                                    last time value of the original axis.
    */
    Vector<Double> getUpsampledTimeAxis (DataReader *dr,
                                         const int& upsampling_exp);


    /*!
      \brief Gets a Slice with the plot interval for the given xaxis

      \param xaxis            -- a time axis (may be upsampled or not)

      \return plotRange       -- A Slice containing the indices of the xaxis interval corresponding to the
                                 class variables plotStart_p and plotStop_p
    */
    Slice calculatePlotRange (const Vector<Double>& xaxis) const;


    /*!
      \brief Gets a Slice with the spectrum interval for the given xaxis

      \param xaxis            -- a time axis (may be upsampled or not)

      \return plotRange       -- A Slice containing the indices of the xaxis interval corresponding to the
                                 class variables spectrumStart_p and spectrumStop_p
    */
    Slice calculateSpectrumRange (const Vector<Double>& xaxis) const;


    /*!
      \brief Calculates noise of a trace

      \param trace            -- trace or part of trace to calculate the noise from
      \param method           -- method of noise calculation:
                                 0: Steffen's method: noise = mean of absolute values of trace

      \return noise           -- noise (different methods will be implemented)
    */
    double calculateNoise (const Vector<Double>& trace, int method = 0) const;



    /*!
      \brief Gets a Slice with the time interval before the cc-beam
             for the given xaxis

      \param xaxis            -- a time axis (may be upsampled or not)
      \param ccBeamcenter      -- position in time of the CC-beam center

      \return noiseRange      -- a Slice containing the indices of the xaxis interval corresponding to 
                                 CC-beam-center - 15 * cc-window until CC-beam-center - 5 * cc-window
    */
    Slice calculateNoiseRange (const Vector<Double>& xaxis, const double& ccBeamcenter) const;


    /*!
      \brief Gets a Slice with the time interval around the CC-beam center,
             the range is set via key ccWindowWidth

      \param xaxis             -- a time axis (may be upsampled or not)
      \param ccBeamcenter      -- position in time of the CC-beam center

      \return calculateCCRange -- A Slice containing the indices of the xaxis interval corresponding to the
                                  to a window around the CC beam center
    */
    Slice calculateCCRange (const Vector<Double>& xaxis, 
                            const double& ccBeamcenter) const;


    /*!
      \brief Plots the CC-beam

      \param filename         -- Name of the .ps file (without ".ps") the plot will be written to
      \param DataReader       -- DataReader (LopesEventIn)
      \param fittedCCbeam     -- If supplied, the result of the fit will be plotted
      \param antennaSelection -- Selection of antennas considered for the plot
      \param filterStrength   -- Numbers of blocks used in the block-averaging filter, 0 = no smoothing
      \param remoteStart      -- Start of the remote range (in samples)
      \param remoteStop       -- End of the remote range (in samples):
                                 The remote range is used to calculate the mean of the CC-beam and P-beam.
    */

    void plotCCbeam (const string& filename, 
                     DataReader *dr,
                     Vector<Double> fittedCCbeam = Vector<Double>(),
                     Vector<Bool> antennaSelection = Vector<Bool>(),
                     const int& filterStrength = 0,
                     const unsigned int& remoteStart = 0,
                     const unsigned int& remoteStop = 0);

    /*!
      \brief Plots the X-beam

      \param filename         -- Name of the .ps file (without ".ps") the plot will be written to
      \param DataReader       -- DataReader (LopesEventIn)
      \param fittedXbeam      -- If supplied, the result of the fit will be plotted
      \param antennaSelection -- Selection of antennas considered for the plot
      \param filterStrength   -- Numbers of blocks used in the block-averaging filter, 0 = no smoothing
      \param remoteStart      -- Start of the remote range (in samples)
      \param remoteStop       -- End of the remote range (in samples):
                                 The remote range is used to calculate the mean of the X-beam and P-beam.
    */

    void plotXbeam (const string& filename, 
                    DataReader *dr,
                    Vector<Double> fittedXbeam = Vector<Double>(),
                    Vector<Bool> antennaSelection = Vector<Bool>(),
                    const int& filterStrength = 0,
                    const unsigned int& remoteStart = 0,
                    const unsigned int& remoteStop = 0);

    /*!
      \brief Plots the fieldstrength of all antennas after beam forming

      \param filename         -- Name of the .ps file (without ".ps") the plot will be written to
      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- Selection of antennas considered for the plot
      \param seperated        -- true = an individual plot for each antenna will be created
                                 (the antenna number serves as file name appendix).
      \param upsampling_exp   -- a value > 0 means that data are upsampled by a factor of 2^upsampling_exp
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.
      \param rawData          -- plot the raw ADC data instead of the calibrated fieldstrength
      \param plotEnvelope     -- plots the envelope (only if seperated is also true)
    */

    void plotAllAntennas (const string& filename,
                          DataReader *dr,
                          Vector<Bool> antennaSelection = Vector<Bool>(),
                          const bool& seperated = false,
                          const int& upsampling_exp = 0,
                          const bool& rawData = false,
                          const bool& plotEnvelope = false);

    /*!
      \brief Plots the spectrum of all antennas and a median spectrum

      \param filename         -- Name of the .ps file (without ".ps") the plot will be written to
      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- Selection of antennas considered for the plot
      \param seperated        -- true = an individual plot for each antenna will be created
                                 (the antenna number serves as file name appendix).

      Be careful: If and upsampling frequency is used, the resolution of the spectrum is reduced,
                  as the upsampling data reader uses only a part of the trace.
    */

    void plotSpectra (const string& filename,
                      DataReader *dr,
                      Vector<Bool> antennaSelection = Vector<Bool>(),
                      const bool& seperated = false);

    /*!
      \brief Prints the height of the maximum of the envelope and the corresponding time in the plot range

      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- Selection of antennas
      \param upsampling_exp   -- a value > 0 means that data are upsampled by a factor of 2^upsampling_exp
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.
      \param rawData          -- uses the raw ADC data instead of the calibrated fieldstrength
      \param cc_center        -- center (time) of CC-beam fit (tries to calculate the noise before the pulse,
                                 if this information is given)
      \param noiseMethod      -- method for calculation of noise                           

      \return pulses          -- a map with the calculated pulse parameters (keys = antennaIDs)
    */

    map <int,PulseProperties> calculateMaxima (DataReader *dr,
                                               Vector<Bool> antennaSelection = Vector<Bool>(),
                                               const int& upsampling_exp = 0,
                                               const bool& rawData = false,
                                               const double& cc_center = 1e99,
                                               int noiseMethod = 0);
    /*!
      \brief same as calculateMaxima, but different output form and does not work with the envelope of the trace

      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- Selection of antennas
      \param upsampling_exp   -- a value > 0 means that data are upsampled by a factor of 2^upsampling_exp
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.
      \param cc_center        -- center of CC-beam fit, if it is set, the pulse parameters will be calculated
                                 not in the plot range but in a region arround the cc-beam
    */
    void listCalcMaxima (DataReader *dr,
                         Vector<Bool> antennaSelection = Vector<Bool>(),
                         const int& upsampling_exp = 0,
                         const double& cc_center = 0);


    //! Get the unshifted time series for all antennas
    Matrix<Double> GetUnshiftedTimeSeries(DataReader *dr, 
                                          Vector<Bool> antennaSelection = Vector<Bool>(),
                                          String Polarization="ANY",
                                          bool substractPedastal = true);

    /*!
      \brief Adds a random delay to the data of each antenna (for timing uncertainty studies)

      \param dr                -- Pointer to the (initialized) DataReader
      \param randomDelay       -- Sigma of the gaussian random distribution
      \param randomSeed        -- Seed of the root random number generator (TRandom3)
    */
   void addRandomDelay(DataReader *dr,
                       double randomDelay = 0.,
                       unsigned int randomSeed = 1);

  private:
    
   //! Unconditional copying
   void copy (CompletePipeline const &other);
   
   //! Unconditional deletion 
   void destroy(void);
   
  };
  
} // Namespace CR -- end

#endif  /* HAVE_STARTOOLS */

#endif /* COMPLETEPIPELINE_H */
  
