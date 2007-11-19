/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

/* $Id: TIM40Header.h,v 1.1 2006/08/01 15:00:31 bahren Exp $*/

#ifndef _TIM40HEADER_H_
#define _TIM40HEADER_H_

#include <string>

#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Quanta/MVFrequency.h>

using std::string;

#include <casa/namespace.h>

namespace CR {  //  Namespace CR -- begin
  
  /*!
    \class TIM40Header
    
    \ingroup CR_Data
    
    \brief Brief description for class TIM40Header
    
    \author Lars B&auml;hren
    
    \date 2006/01/11
    
    \test tTIM40Header.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[CASA] <a href="http://aips2.nrao.edu/docs/casa/implement/Quanta.html">Quantity</a> -- Quantities (i.e. dimensioned values) 
      <li>[CASA] <a href="http://aips2.nrao.edu/docs/casa/implement/Quanta/MVFrequency.html">MVFrequency</a> -- Internal value for MFrequency
      <li>[CASA] <a href="http://aips2.nrao.edu/docs/casa/implement/Quanta/MVEpoch.html">MVEpoch</a> -- A class for high precision time
    </ul>
    
    <h3>Synopsis</h3>
    
    \code
    header:=[Project=LOPES: Event measurements,
             DataType=TIM40,
	     Filename=Event1067339149-10101,
	     ID=1,
	     Measurement=1,
	     FrequencyUnit=1e6,
	     SamplerateUnit=1e6,
	     Samplerate=80,
	     NyquistZone=2,
	     Location=Karlsruhe at KASCADE,
	     AntPos=[13.573,-50.195,124.729],
	     Weather=Undefined,
	     Timezone=00,
	     Time=11:05:49.000,
	     Date=2003/10/28/11:05:49.000,
	     ADCMaxChann=2048,
	     ADCMinChann=-2048,
	     MaxVolt=1,
	     LocalCalFile=empty.tab,
	     AntennaCalFile=lopes10-neueich.tab,
	     CalFileFreqUnit=1e6,
	     Files=Event1067339149-10101];
    
    header.Bandwidth:=(as_float(Samplerate)/2 *as_float(SamplerateUnit)/as_float(FrequencyUnit));
    \endcode
    
    <h3>Example(s)</h3>
    
  */
  
  class TIM40Header {
    
    string Project_p;
    string DataType_p;
    string Filename_p;
    uInt ID_p;
    uInt Measurement_p;
    string FrequencyUnit_p;
    MVFrequency Samplerate_p;
    uInt NyquistZone_p;
    string Location_p;
    Vector<Float> AntPos_p;
    string Weather_p;
    Int Timezone_p;
    string Time_p;
    string Date_p;
    Int ADCMaxChann_p;
    Int ADCMinChann_p;
    Float MaxVolt_p;
    string LocalCalFile_p;
    string AntennaCalFile_p;
    string CalFileFreqUnit_p;
    string Files_p;
    MVFrequency Bandwidth_p;
    
  public:
    
    /*!
      \brief Data type
    */
    enum DataType {
      //! Short data files, once loaded into memory
      TIM40,
      //! Long data files, where only the currently required part is loaded into memory
      TIM40f
    };
    
    // --------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    TIM40Header ();
    
    /*!
      \brief Argumented constructor
      
      \param project         -- The name of the project
      \param dataType        -- The type of the data
      \param filename        -- The name of the event file
      \param id              -- ID of the data type
      \param measurement     -- Identifier for the type of measurement
      \param frequencyUnit   -- The units in which the frequencies values are given
      \param samplerateUnit  -- The units in which the samplerate values are given
      \param samplerate      -- 
      \param nyquistZone     -- 
      \param location        -- The location at which the data were recorded
      \param antPos          -- 
      \param weather         -- Description of the weather situation
      \param timezone        -- Time zone in which the location of the experiment is
      situated.
      \param time            -- 
      \param date            -- 
      \param ADCMaxChann     -- 
      \param ADCMinChann     -- 
      \param maxVolt         -- 
      \param localCalFile    -- 
      \param antennaCalFile  -- 
      \param calFileFreqUnit -- 
      \param files           -- 
      
      The default constructor will set the typical parameters as for a LOPES
      observation:
      \code
      [TIM40Header]
      - Project         = UNDEFINED
      - DataType        = TIM40
      - Filename        = UNDEFINED
      - ID              = 1
      - Measurement     = 1
      - FrequencyUnit   = MHz
      - Samplerate      = 80 MHz
      - Bandwidth       = 40 MHz
      - NyquistZone     = 2
      - Location        = UNDEFINED
      - AntPos          = [0, 0, 0]
      - Weather         = UNDEFINED
      - Timezone        = 0
      - Time            = UNDEFINED
      - Date            = UNDEFINED
      - ADCMaxChann     = 2048
      - ADCMinChann     = -2048
      - MaxVolt         = 1
      - LocalCalFile    = UNDEFINED
      - AntennaCalFile  = UNDEFINED
      - CalFileFreqUnit = MHz
      - Files           = UNDEFINED
      \endcode
    */
    TIM40Header (const string& project,
		 const string& dataType,
		 const string& filename,
		 const uInt& id,
		 const uInt& measurement,
		 const string& frequencyUnit,
		 const double& samplerateValue,
		 const string& samplerateUnit,
		 const uInt& nyquistZone,
		 const string& location,
		 const Vector<Float>& antPos,
		 const string& weather,
		 const Int& timezone,
		 const string& time,
		 const string& date,
		 const Int& ADCMaxChann,
		 const Int& ADCMinChann,
		 const Float& maxVolt,
		 const string& localCalFile,
		 const string& antennaCalFile,
		 const string& calFileFreqUnit,
		 const string& files);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another TIM40Header object from which to create this new
      one.
    */
    TIM40Header (TIM40Header const& other);
    
    // ---------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~TIM40Header ();
    
    // ------------------------------------------------------------------ Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another TIM40Header object from which to make a copy.
    */
    TIM40Header &operator= (TIM40Header const &other); 
    
    // ----------------------------------------------------------------- Parameters
    
    // --- Access to individual parameters ---
    
    /*!
      \brief Get the name of the project
      
      \return project -- The name of the project
    */
    string project () {
      return Project_p;
    }
    
    /*!
      \brief Set the name of the project
      
      \param project -- The name of the project
    */
    void setProject (const string& project) {
      Project_p = project;
    }
    
    /*!
      \brief Get the type of the data
      
      \return dataType -- The type of the data, e.g. <i>TIM40</i>, <i>TIM40f</i>
    */
    string dataType () {
      return DataType_p;
    }
    
    /*!
      \brief Set the type of the data
      
      \param dataType -- The type of the data
    */
    void setDataType (const string& dataType) {
      DataType_p = dataType;
    }
    
    /*!
      \brief Get the name of the event file
      
      \return filename -- The name of the event file, e.g. <i>Event1067339149-10101</i>
    */
    string filename () {
      return Filename_p;
    }
    
    /*!
      \brief Set the name of the event file
      
      \param filename -- The name of the event file
    */
    void setFilename (const string& filename) {
      Filename_p = filename;
    }
    
    /*!
      \brief Get the ID of the data type 
      
      \return id -- ID of the data type, indicating wether it is from an experiment,
      a simulation, etc.
    */
    uInt id () {
      return ID_p;
    }
    
    /*!
      \brief Set the ID of the data type
      
      \param id -- ID of the data type
    */
    void setId (const uInt& id) {
      ID_p = id;
    }
    
    /*!
      \brief Get the identifier for the type of measurement
      
      \return measurement -- Identifier for the type of measurement
    */
    uInt measurement () {
      return Measurement_p;
    }
    
    /*!
      \brief Set the identifier for the type of measurement
      
      \param measurement -- Identifier for the type of measurement
    */
    void setMeasurement (const uInt& measurement) {
      Measurement_p = measurement;
    }
    
    /*!
      \brief Get the units in which the frequencies values are given
      
      \return frequencyUnit -- The units in which the frequencies values are given
    */
    string frequencyUnit () {
      return FrequencyUnit_p;
    }
    
    /*!
      \brief Set the units in which the frequencies values are given
      
      \param frequencyUnit -- The units in which the frequencies values are given
    */
    void setFrequencyUnit (const string& frequencyUnit) {
      FrequencyUnit_p = frequencyUnit;
    }
    
    /*!
      \brief Get the samplerate as MVFrequency
      
      \return samplerate -- The samplerate
    */
    MVFrequency samplerate () {
      return Samplerate_p;
    }
    
    /*!
      \brief Get the samplerate as quantity
      
      \param unit -- The unit in which to return the samplerate, e.g. <i>MHz</i>
      
      \return samplerate -- The samplerate as quantity
    */
    Quantity samplerate (const string& unit);
    
    /*!
      \brief Set the samplerate
      
      \param samplerate -- The samplerate, provided as MVFrequency.
    */
    void setSamplerate (const MVFrequency& samplerate);
    
    /*!
      \brief Set the samplerate
      
      \param samplerate -- The samplerate, provided as Quantity.
    */
    void setSamplerate (const Quantity& samplerate);
    
    /*!
      \brief Set the samplerate
      
      \param value -- Value of the samplerate, e.g. <i>80.0</i>
      \param unit  -- Physical units of the samplerate, e.g. <i>MHz</i>
    */
    void setSamplerate (double const &value,
			string const &unit);
    
    /*!
      \brief Get the Nyquist zone in which the data are sampled
      
      \return nyquistZone -- The Nyquist zone in which the data are sampled
    */
    uInt nyquistZone () {
      return NyquistZone_p;
    }
    
    /*!
      \brief Set the Nyquist zone in which the data are sampled
      
      \param nyquistZone -- The Nyquist zone in which the data are sampled
    */
    void setNyquistZone (const uInt& nyquistZone) {
      NyquistZone_p = nyquistZone;
    }
    
    /*!
      \brief Get the location at which the data were recorded
      
      \return location -- The location at which the data were recorded
    */
    string location () {
      return Location_p;
    }
    
    /*!
      \brief Set the location at which the data were recorded
      
      \param location -- The location at which the data were recorded
    */
    void setLocation (const string& location) {
      Location_p = location;
    }
    
    /*!
      \brief Get position of the antenna from which these data originate
      
      \return antPos -- The position of the antenna from which these data originate
    */
    Vector<Float> antPos () {
      return AntPos_p;
    }
    
    /*!
      \brief Set position of the antenna from which these data originate
      
      \param antPos -- The position of the antenna from which these data originate
    */
    void setAntPos (const Vector<Float>& antPos) {
      AntPos_p.resize(antPos.shape());
      AntPos_p = antPos;
    }
    
    /*!
      \brief Get description of the weather situation
      
      \return weather -- Description of the weather situation
    */
    string weather () {
      return Weather_p;
    }
    
    /*!
      \brief Set description of the weather situation
      
      \param weather -- Description of the weather situation
    */
    void setWeather (const string& weather) {
      Weather_p = weather;
    }
    
    /*!
      \brief In which time zone is the location of the experiment situated?
      
      \return timezone -- The time zone in which the location of the experiment is
      situated; this can be used for computation of seasonal
      offsets.
    */
    Int timezone () {
      return Timezone_p;
    }
    
    /*!
      \brief In which time zone is the location of the experiment situated?
      
      \param timezone -- The time zone in which the location of the experiment is
      situated; this can be used for computation of seasonal
      offsets.
    */
    void setTimezone (const Int& timezone) {
      Timezone_p = timezone;
    }
    
    /*!
      \brief Get the time at which the data were recorded/written
      
      \return time -- The time at which the data were recorded/written, e.g.
      <i>11:05:49.000</i>
    */
    string time () {
      return Time_p;
    }
    
    /*!
      \brief Set the time at which the data were recorded/written
      
      \param time -- The time at which the data were recorded/written
    */
    void setTime (const string& time) {
      Time_p = time;
    }
    
    /*!
      \brief Get the date/epoch at which the data were recorded/written
      
      \return date -- The date/epoch at which the data were recorded/written,
      e.g <i>2003/10/28/11:05:49.000</i>
    */
    string date () {
      return Date_p;
    }
    
    /*!
      \brief Get the date/epoch at which the data were recorded/written
      
      \param date -- The date/epoch at which the data were recorded/written
    */
    void setDate (const string& date) {
      Date_p = date;
    }
    
    /*!
      \brief Set the maximum channel of the ADC converter
      
      \param ADCMaxChann -- Maximum channel of the ADC converter
    */
    void setADCMaxChann (const Int& ADCMaxChann) {
      ADCMaxChann_p = ADCMaxChann;
    }
    
    /*!
      \brief Set the minimum channel of the ADC converter
      
      \param ADCMinChann -- Minimum channel of the ADC converter
    */
    void setADCMinChann (const Int& ADCMinChann) {
      ADCMinChann_p = ADCMinChann;
    }
    
    /*!
      \brief Get the voltage value corresponding to the maximum ADC channel
      
      \return maxVolt -- The voltage value corresponding to the maximum ADC channel
    */
    Float maxVolt () {
      return MaxVolt_p;
    }
    
    /*!
      \brief Set the voltage value corresponding to the maximum ADC channel
      
      \param maxVolt -- The voltage value corresponding to the maximum ADC channel
    */
    void setMaxVolt (const Float& maxVolt) {
      MaxVolt_p = maxVolt;
    }
    
    /*!
      \brief Local file storing optional calibration data
      
      \return localCalFile -- Local file storing optional calibration data
    */
    string localCalFile () {
      return LocalCalFile_p;
    }
    
    /*!
      \brief Local file storing optional calibration data
      
      \param localCalFile -- Local file storing optional calibration data
    */
    void setLocalCalFile (const string& localCalFile) {
      LocalCalFile_p = localCalFile;
    }
    
    /*!
      \brief Local file storing antenna calibration data
      
      \return antennaCalFile -- Local file storing antenna calibration data
    */
    string antennaCalFile () {
      return AntennaCalFile_p;
    }
    
    /*!
      \brief Local file storing antenna calibration data
      
      \param antennaCalFile -- Local file storing antenna calibration data
    */
    void setAntennaCalFile (const string& antennaCalFile) {
      AntennaCalFile_p = antennaCalFile;
    }
    
    /*!
      \brief Units in which the values in the antenna calibration file are given
      
      \return calFileFreqUnit -- Units in which the values in the antenna
      calibration file are given
    */
    string calFileFreqUnit () {
      return CalFileFreqUnit_p;
    }
    
    /*!
      \brief Units in which the values in the antenna calibration file are given
      
      \param calFileFreqUnit -- Units in which the values in the antenna
      calibration file are given
    */
    void setCalFileFreqUnit (const string& calFileFreqUnit) {
      CalFileFreqUnit_p = calFileFreqUnit;
    }
    
    /*!
      \brief Files associated with the data
      
      \return files -- Files associated with the data
    */
    string files () {
      return Files_p;
    }
    
    /*!
      \brief Files associated with the data
      
      \param files -- Files associated with the data
    */
    void setFiles (const string& files) {
      Files_p = files;
    }

    /*!
      \brief Frequency bandwidth
      
      \return bandwidth -- Frequency bandwidth
    */
    MVFrequency bandwidth () {
      return Bandwidth_p;
    }
    
    /*!
      \brief Get the bandwidth as quantity
      
      \param unit -- The unit in which to return the bandwidth, e.g. <i>MHz</i>
    */
    Quantity bandwidth (const string& unit);
    
    // --- Access to sets of parameters ---
    
    /*!
      \brief Set the sampling parameters
      
      \param samplerateUnit -- The units in which the sample rate is given, e.g.
      <i>MHz</i>
      \param samplerate     -- The sample rate, in units of the <i>samplerateUnit</i>
      \param nyquistZone    -- The Nyquist zone in which the data are sampled
    */
    void setSampleInfo (const double& samplerateValue,
			const string& samplerateUnit,
			const uInt& nyquistZone);
    
    /*!
      \brief Get the range of the ADC channels
      
      \return ADCRange -- Range of the ADC channels.
    */
    Vector<Int> ADCRange ();
    
    /*!
      \brief Set the range of the ADC channels.
      
      \param range -- Range of the ADC channels. This function also accepts a
      vector of more than two values; in this case the minimum and
      maximum value are stored.
    */
    void setADCRange (const Vector<Int>& range);
    
    /*!
      \brief Multiplikation factor to convert ADC values to voltages
    */
    Float ADC2Voltage ();
    
    // -------------------------------------------------------------------- Methods
    
    /*!
      \brief Show the current settings
      
      Feedback is written to standard output.
    */
    void show ();
    
    /*!
      \brief Show the current settings
      
      \param stream -- Stream to which the data are written (standard output, file,
      etc.)
    */
    void show (std::ostream& stream);

  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (TIM40Header const& other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
    /*!
      \brief Initialize internal paramter using default values
    */
    void init ();
    
    /*!
      \brief Compute the bandwidth from sampling frequency information
    */
    void setBandwidth ();
    
  };

}  // Namespace CR -- end
  
#endif /* _TIM40HEADER_H_ */
