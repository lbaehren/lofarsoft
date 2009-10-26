/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Frank Schröder                                                       *
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

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include <crtools.h>
#include <Analysis/analyseLOPESevent2.h>
#include <Analysis/PulseProperties.h>
#include <Analysis/AntennasDisplay.h>

using CR::analyseLOPESevent2;
using CR::LopesEventIn;

/*!
  \file call_pipeline.cc

  \ingroup CR_Applications

  \brief Calls the LOPES analysis pipeline in "analyseLOPESevent2"

  \author Frank Schr&ouml;der

  \date 2008/20/05

  <h3>Motivation</h3>

  The aim of this short application is to provide a tool which
  makes the LOPES-analysis-pipeline usable without changing any
  code. Any information about the LOPES-events you want to analyse
  and any configurations how to analyse the events should be provide
  be external, human readable files.

  <h3>Usage</h3>

  \verbatim
  ./call_pipeline arguments
  \endverbatim

  <ul>
    Requires one of the following arguments:
    <li> --in         eventfile list      list with LOPES event files (.event)
    <li> --k          Kascade root file   (containing also the LOPES event names)
    Optional argumens:
    <li> --config     textfile            a file with configuration settings for the pipeline
    <li> --help                           prints a help message
  </ul>

  <h3>Prerequisites</h3>

  You need at least one event list file (textfile) like the example files below:<br>
  (remark: at the moment we are testing to read eventlists from root files instead)<br>

  \verbatim
  Example event list
  some lines of comments here
  filename(event) azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]
  =========================================================================================
  /home/schroeder/data/lopesstar/first.event 354.55215 63.882182 2750 8.6060886 -368.0933
  /home/schroeder/data/lopesstar/second.event 354.55215 63.882182 2750 8.6060886 -368.0933
  /home/schroeder/data/lopesstar/third.event 354.55215 63.882182 2750 8.6060886 -368.0933
  \endverbatim
  <br>
  Warning: Make sure you provide the elevation not the zenith of the event!<br>
  <br>
  If you don't want to process the events in the default manner<br>
  you can also provide a config file (textfile), e.g. config.txt:<br>
  \verbatim
  # Example configuration file:
  # Comments start with '#'.
  # If an option is missing or misspelled, then the default value will be used.
  # Default values are:
  caltablepath            = $LOFARSOFT/data/LOPES/LOPES-CalTable
  path                    = /home/schroeder/lopes/events
  RotatePos               = true
  preferGrande            = false
  GeneratePlots           = true
  EventDisplayPlot        = false
  GenerateSpectra         = true
  SinglePlots             = true
  PlotRawData             = false
  CalculateMaxima         = false
  listCalcMaxima          = false
  printShowerCoordinates  = false
  verbose                 = true
  ignoreDistance          = true
  simplexFit              = true
  doTVcal                 = -1
  doGainCal               = true
  doDispersionCal         = true
  doDelayCal              = true
  doRFImitgation          = true
  doFlagNotActiveAnts     = true
  doAutoFlagging          = true
  polarization            = ANY
  plotStart               = -2.05e-6
  plotStop                = -1.60e-6
  spectrumStart           = 40e6
  spectrumStop            = 80e-6
  freqStart               = 40e6
  freqStop                = 80e-6
  upsamplingRate          = 320e6
  upsamplingExponent      = 1
  minBeamformingAntennas  = 4
  summaryColumns          = 3
  ccWindowWidth           = 0.045e-6
  flagged                 = 10101
  flagged                 = 10102
  rootfilename            = output.root
  rootfilemode            = recreate
  writeBadEvents          = false
  calibration             = false
  lateralDistribution     = false
  lateralOutputFile       = false
  lateralSNRcut           = 1.0
  lateralTimeCut          = 15e-9
  calculateMeanValues     = false
  lateralTimeDistribution = false
  \endverbatim

  <h3>Configuration options </h3>
  <b>Useful options:</b><br>
    <li>\b Path             Status: <i>optional</i>  - normally required<br>
                            Location of the events in the event list. Setting a path is not neccessary if
                            the events are in the current directory or if the full path is contained in the
                            event list.<br>
    <li>\b polarization     Status: <i>optional</i><br>
                            Polarizations to be analyzed. There are four possibilitys:<br>
                            \i ANY  The analysis does not take care of the polarization at all.<br>
                            \i EW   All none east-west antennas are flagged.<br>
                            \i NS   All none north-south antennas are flagged.<br>
                            \i BOTH Each event is analyzed twice: Once for EW and once for NS polarization.<br>
    <li>\b simplexFit       Status: <i>optional</i><br>
                            By default the arrival direction and radius of curvature (distance) is optimized by
                            a simplex fit, using the values in the event list as starting point. Only if you
                            want to do the beam forming exactly in the direction and with the distance
                            specified, then you should switch off the simplex fit.<br>
                            The fit has several steps:<br>
                            1. The optimal starting distance is determined (unless ignoreDistance = false)<br>
                            2. The simplex fit roughly optimizes the X-beam<br>
                            3. The simplex fit optimizes the CC-beam starting from the X-beam maximum<br>
    <li>\b ignoreDistance   Status: <i>optional</i> - important<br>
                            If set to true (default) then the distance value (curvature radius) for the beam
                            forming will be estimated and the value in the eventlist ignored. That means that
                            the value specified in the event list is completly ignored unless 'ignoreDistance'
                            is set to false.<br>
    <li>\b upsamplingRate   Status: <i>optional</i> - recommended<br>
                            Calibrated traces will be upsampled to a upsampling rate specified in MHz.<br>
                            Recommended for CC-beam analysis: at least 320 MHz,<br>
                            For lateral distribution: at least 640 MHz.<br>
    <li>\b flagged          Status: <i>optional</i> - repeatable<br>
                            Manually flaggs antennas for the analysis. This keyword can be repeated
                            several times, once for each antenna which has to be flagged.
                            It has to be followed either by the antenna ID or the antenna number, e.g.:<br>
                            flagged = 5<br>
                            flagged = 20101<br>
    <li>\b GeneratePlots    Status: <i>optional</i><br>
                            Can be set to false if no plots (e.g. CC beam) are wanted.<br>
    <li>\b SinglePlots      Status: <i>optional</i><br>
                            Produces a plot for each individual antenna.<br>
    <li>\b plotStart        Status: <i>optional</i><br>
    <li>\b plotStop         Status: <i>optional</i><br>
                            Start and stop range for the plots. Take care:<br>
                            Also some analysis (e.g. pulse searching) is done only in the plot range.<br>
    <li>\b GenerateSpectra  Status: <i>optional</i> - not well tested<br>
                            A spectrum will be plotted for every antenna.<br>
    <li>\b spectrumStart    Status: <i>optional</i><br>
    <li>\b spectrumStop     Status: <i>optional</i><br>
                            Frequency range for spectrum plots (maximal 40 - 80 MHz)<br>
    <li>\b summaryColumns   Status: <i>optional</i><br>
                            There will by a summary postscript of all created plots with the specified
                            number of columns (requires LateX), use 0 for no summary plot.<br>
    <li>\b rootfilename     Status: <i>optional</i><br>
                            If a file name is specified (e.g. "output.root"), then the results of the
                            pipeline are written into this root file.<br>
    <li>\b verbose          Status: <i>optional</i><br>
                            Set to false to reduce the text output during the analysis.<br>
  <br>
  <b>Options ocaisonally needed:</b><br>
    <li>\b CalTablePath     Status: <i>optional</i> - normally not neccessary <br>
                            The caltables are in $LOFARSOFT/data/LOPES/LOPES-CalTable; the main importance 
                            of this variable is to locate the LOPES CalTable in case you have put it down 
                            in some non-standard location - otherwise the configuration script will be able
                            to locate it and place the path into the "crtools.h" header file.<br>
    <li>\b RotatePos        Status: <i>for experts</i> - don't touch it<br>
                            If set to true (default), the given positions in the Kascade coordinate system
                            are rotated to the LOPES system.<br>
    <li>\b EventDisplayPlot Status: <i>preliminary</i> - only partially working<br>
                            An event display plot containing amplitude and time information of each
                            antenna.<br>
    <li>\b PlotRawData      Status: <i>optional</i><br>
                            Plots the raw ADC values (FX).<br>
    <li>\b CalculateMaxima  Status: <i>optional</i><br>
                            Calcutlates the time and the height of the pulse in the plot range for each
                            antenna.<br>
    <li>\b doTVcal          Status: <i>optional</i><br>
                            The TV (respectively beacon) calibration will be done by default (-1).
                            Switch it off with 0 and on with 1.<br>
    <li>\b doGainCal        Status: <i>optional</i><br>
                            The electrical gain calibration (fieldstrength)<br>
    <li>\b doDispersionCal  Status: <i>optional</i><br>
                            Correction of the dispersion of the filter box (frequency dependent delay)<br>
    <li>\b doDelayCal       Status: <i>optional</i><br>
                            Correction of the general delay<br>
    <li>\b doRFImitgation   Status: <i>optional</i><br>
                            Supression of narrow band noise (RFI)<br>
    <li>\b doFlagNotActiveAnts Status: <i>optional</i><br>
                            Flags antennas marked as "not active" in the CalTables
                            (e.g. due to known problems or tests).<br>
    <li>\b doAutoFlagging   Status: <i>optional</i><br>
                            Flags antennas due to bad signal (does not affect flagging by the TV
                            calibration).<br>
    <li>\b freqStart        Status: <i>for experts</i><br>
    <li>\b freqStop         Status: <i>for experts</i><br>
                            Frequency range to be used in the analysis. This value has no effect if the
                            limits are wide than the ones in the calibration tables (normally 43-74 MHz).
                            A hanning filter will be used to supress the frequencies outside of the band.<br>
    <li>\b upsamplingExponent Status: <i>for experts</i> - for calibration an tests<br>
                            Upsampling of the calibrated antenna fieldstrengthes and raw data.
                            Upsampling will be done to a rate of 2^upsamplingExponent * 80 MHz.<br>
    <li>\b minBeamformingAntennas Status: <i>optional</i><br>
                            Number of unflagged antennas required for beamforming. Antennas can be flagged
                            for different reasons (e.g. manually, CalTables, low signal, mismatch of
                            the different frequencies during the TV calibration). If not enough unflagged
                            antennas are left, the pipeline will exit and the event will not be written
                            to the root file with the results (unless 'wirteBadEvents' is set to 'true').<br>
    <li>\b ccWindowWidth    Status: <i>for experts</i><br>
                            Time range to search for CC-beam-peak in lateral distribution studies,
                            default is +/- 45 ns.<br>
    <li>\b wirteBadEvents   Status: <i>preliminary</i><br>
                            Events with a bad reconstruction (e.g. simplex fit crashed) will be written 
                            to the root file. Though, detection of bad reconstructions does not work
                            reliable.<br>
    <li>\b calibration      Status: <i>for experts</i><br>
                            Should be set to 'true' for the evaluation of time calibration measurements.<br>
    <li>\b PreferGrande     Status: <i>preliminary</i> - under test<br>
                            In doubt the KASCADE (instead of Grande) reconstruction is taken as input.<br>
  <br>
  <b>Other options (for completeness):</b><br>
    <li>\b lateralOutputFile Status: <i>obsolete</i><br>
                            Special output file for the lateral distribution analysis of S. Nehls.<br>
    <li>\b listCalcMaxima   Status: <i>obsolete</i><br>
                            Prints a list of the absolut maxima in a special format used for the lateral
                            distribution analysis of S. Nehls.<br>
    <li>\b printShowerCoordinatesStatus: <i>obsolete</i><br>
                            Prints the distance form the core to the antennas in shower coordinates using a
                            special output format of the lateral distribution analysis of S. Nehls.<br>
    <li>\b lateralDistribution Status: <i>preliminary</i> - use with care<br>
                            The lateral distribution of pulse amplitudes will be plotted and fitted.
                            The applied cuts and the method itself is under test and investigation. Thus,
                            don't use it, if you are not exactly sure, what you are doing. The physics results
                            could be misleading.<br>
    <li>\b lateralSNRcut    Status: <i>preliminary</i><br>
                            SNR cut for the lateral distribution.<br>
                            Caution: This feature is preliminary, not well tested, and might be removed.<br>
    <li>\b lateralTimeCut   Status: <i>preliminary</i><br>
                            Points with a time position more than a certain value away of the CC beam 
                            center will be removed from the lateral distribution.<br>
                            Caution: This feature is preliminary, not well tested, and might be removed.<br>
    <li>\b calculateMeanValues Status: <i>preliminary</i><br>
                            Output of mean values of lateral distribution results.<br>
                            Caution: This feature is preliminary, not well tested, and might be removed.<br>
    <li>\b rootfilemode     Status: <i>unfinished</i><br>
                            The only mode available at the moment is RECREATE. This means that an old root
                            file with the same name will be overwritten.<br>
    <li>\b lateralTimeDistribution Status: <i>unfinished</i> - not tested, under development<br>
                            Plots lateral distribution of pulse arrival times.<br>

  <h3>Examples</h3>

  \verbatim
  ./call_pipeline --in eventlist.txt --config config.txt
  ./call_pipeline --in eventlist.txt --config conf1.txt --config conf2.txt
  \endverbatim
  In the second example the config values will be read from two files squentially.
  In case of condractiory config options the last ones will be used.

  <h3>Upsampling</h3>

  Currently there are two upsampling methods implemented:
  <ol>
    <li>The one called by setting an 'upsampling exponent' uses the Upsampling
        routines of LOPES-Star and has an effect only to the traces of single
        antennas. It is recommend for time calibration only.
    <li>The upsampling due to setting an 'upsamplingRate' greater than 160 MHz
        effects the whole analysis chain, including the CC-Beam. This method is
        recommended for standard analysis (if no upsampling of the raw data is needed).
  </ol>
  You should prefer 2^n * 80 MHz rates (e.g. 160, 320, 640, 1280, ...).

  WARNING:
  The two upsampling methods should not be mixed!
  Otherwise wrong traces are obtained.

  <h3>Calibration</h3>

  The calibration mode can be switched on by calibration = true in the config file.
  It can be used for processing LOPES events for the delay calibration.
  In calibration mode the eventlist must contain only the event files but not additional
  information like Az, El, core and curvature.

*/

// --- Classes enhancing config file reading ---
const int          iDef  = 0;
const unsigned int uiDef = 0;
const double       dDef  = 0;
const bool         bDef  = false;
const string       sDef  = "";

enum ObjectType
{
  _AnyType,
  _BoolType,
  _DoubleType,
  _IntType,
  _UintType,
  _StringType
};

template<class T>
bool fromString(T &t, const string &s, std::ios_base& (*f)(ios_base&))
{
    istringstream iss(s);
    return !(iss>>f>>t).fail();
} // Utility function converting string to any type

// Abstract class representing "any" type
class AnyType
{
public:
    virtual bool         setValue(string value)       { return false; }
    virtual string       errorSetVal()                { return ""; }
    virtual bool         bValue()
    {
      cerr<<"Error: calling bValue() on object which is not bool"<<endl;
      return bDef; 
    }
    virtual double       dValue()
    {
      cerr<<"Error: calling dValue() on object which is not double"<<endl;
      return dDef; 
    }
    virtual int          iValue()
    {
      cerr<<"Error: calling iValue() on object which is not int"<<endl;
      return iDef; 
    }
    virtual unsigned int uiValue()
    {
      cerr<<"Error: calling uiValue() on object which is not uint"<<endl;
      return uiDef; 
    }
    virtual string       sValue()
    {
      cerr<<"Error: calling sValue() on object which is not string"<<endl;
      return sDef; 
    }
    virtual int          getType()                    { return (int)_AnyType; }
};

// Bool type
class BoolType : public AnyType
{
public:
    BoolType(bool value = bDef) { this->value = value; }

    virtual bool    bValue     () { return this->value; }
    virtual bool    setValue   (string value) 
    {
        bool result = fromString<bool>(this->value, value, std::boolalpha);
        this->value = result ? this->value : this->defaultValue;
        return result;
    }
    virtual string  errorSetVal() { return "Value must be 'bool' type."; }
    virtual int     getType() { return (int)_BoolType; }
private:
    bool defaultValue;
    bool value;
};

// Double type
class DoubleType : public AnyType
{
public:
    DoubleType(double value = dDef) { this->value = value; }

    virtual double  dValue     () { return this->value; }
    virtual bool    setValue   (string value) 
    {
        bool result = fromString<double>(this->value, value, std::scientific);
        this->value = result ? this->value : this->defaultValue;
        return result;
    }
    virtual string  errorSetVal() { return "Value must be 'double' type."; }
    virtual int     getType() { return (int)_DoubleType; }
private:
    double defaultValue;
    double value;
};

// Integer type
class IntType : public AnyType
{
public:
    IntType(int value = iDef) { this->value = value; }

    virtual int     iValue     () { return this->value; }
    virtual bool    setValue   (string value)
    {
        bool result = fromString<int>(this->value, value, std::dec);
        if(allowedValues.size() > 0) {
            vector<int>::iterator iter = find(allowedValues.begin(), allowedValues.end(), this->value);
            result = result && (iter != allowedValues.end());
        }
        this->value = result ? this->value : this->defaultValue;
        return result;
    }
    virtual string  errorSetVal() 
    {
        stringstream errorMsg;
        errorMsg << "Value must be 'int' type.";
        if(allowedValues.size() > 0) {
            errorMsg << endl << "Allowed values : ";
            for(unsigned int i=0; i<allowedValues.size(); i++)
                errorMsg<< allowedValues[i] << " ";
        }
        return errorMsg.str();
    }
    virtual int     getType() { return (int)_IntType; }
    // Additional functions
    void setAllowedValues(vector<int> allowedValues) { this->allowedValues = allowedValues; }
    void addAllowedValue (int allowedValue) { this->allowedValues.push_back(allowedValue); }
private:
    int         defaultValue;
    int         value;
    vector<int> allowedValues;
};

// Unsigned integer type
class UintType : public AnyType
{
public:
    UintType(unsigned int value = uiDef) { this->value = value;}

    virtual unsigned int uiValue    () { return this->value; }
    virtual bool         setValue   (string value)
    {
        if(value[0] == '-')
            return false;
        bool result = fromString<unsigned int>(this->value, value, std::dec) && (this->value >= 0);
        this->value = result ? this->value : this->defaultValue;
        return result;
    }
    virtual string       errorSetVal() { return "Value must be 'unsigned int' type."; }
    virtual int          getType() { return (int)_UintType; }
private:
    unsigned int defaultValue;
    unsigned int value;
};

// String type
class StringType : public AnyType
{
public:
    StringType(string value = sDef) { this->value  = value; }

    virtual string sValue   () { return this->value; }
    virtual bool   setValue (string value)
    {
        bool result = true;
        if(allowedValues.size() > 0) {
            vector<string>::iterator iter = find(allowedValues.begin(), allowedValues.end(), value);
            result = result && (iter != allowedValues.end());
        }
        this->value = result ? value : defaultValue;
        return result;
    }
    virtual string  errorSetVal() 
    { 
        stringstream errorMsg;
        errorMsg << "Value must be 'string' type.";
        if(allowedValues.size() > 0) {
            errorMsg << endl << "Allowed values : ";
            for(unsigned int i=0; i<allowedValues.size(); i++)
                errorMsg<< allowedValues[i] << " ";
        }
        return errorMsg.str();
    }
    virtual int   getType() { return (int)_StringType; }
    // Additional functions
    void setAllowedValues(vector<string> allowedValues) { this->allowedValues = allowedValues; }
    void addAllowedValue (string allowedValue) { this->allowedValues.push_back(allowedValue); }
private:
    string         defaultValue;
    string         value;
    vector<string> allowedValues;
};

class ConfigData
{
public:
    ConfigData()
    {
        valueNotFound = new AnyType();

        // Add antennas' flag values
        addFlagged("10101", "1",  10101);
        addFlagged("10102", "2",  10102);
        addFlagged("10201", "3",  10201);
        addFlagged("10202", "4",  10202);
        addFlagged("20101", "5",  20101);
        addFlagged("20102", "6",  20102);
        addFlagged("20201", "7",  20201);
        addFlagged("20202", "8",  20202);
        addFlagged("30101", "9",  30101);
        addFlagged("30102", "10", 30102);
        addFlagged("40101", "11", 40101);
        addFlagged("40102", "12", 40102);
        addFlagged("40201", "13", 40201);
        addFlagged("40202", "14", 40202);
        addFlagged("50101", "15", 50101);
        addFlagged("50102", "16", 50102);
        addFlagged("50201", "17", 50201);
        addFlagged("50202", "18", 50202);
        addFlagged("60101", "19", 60101);
        addFlagged("60102", "20", 60102);
        addFlagged("70101", "21", 70101);
        addFlagged("70102", "22", 70102);
        addFlagged("70201", "23", 70201);
        addFlagged("70202", "24", 70202);
        addFlagged("80101", "25", 80101);
        addFlagged("80102", "26", 80102);
        addFlagged("80201", "27", 80201);
        addFlagged("80202", "28", 80202);
        addFlagged("90101", "29", 90101);
        addFlagged("90102", "30", 90102);
    }

    ~ConfigData()
    {
        delete valueNotFound;
        for(map<string, AnyType*>::iterator iter = data.begin(); iter != data.end(); iter++)
            delete iter->second;
    }

    AnyType* operator[](string name)
    {
        toUpper(name);
        if(checkExistence(name))
            return data[name];
        cerr<<"WARNING: The value : " << name << " has not been found"<<endl;
        return valueNotFound;
    }

    void addType(string name, AnyType* type)
    {
        toUpper(name);
        bool typeAdded = false;
        if(checkName(name)) {
            trim(name);
            if(!checkExistence(name)) {
                this->data[name] = type;
                typeAdded = true;
            }
        }
        if(!typeAdded)
            delete type;
    }

    void addBool(string name, bool value = bDef)
    {
        toUpper(name);
        if(checkName(name)) {
            trim(name);
            if(!checkExistence(name))
                this->data[name] = new BoolType(value);
        }
    }
    void addDouble(string name, double value = dDef)
    {
        toUpper(name);
        if(checkName(name)) {
            trim(name);
            if(!checkExistence(name))
                this->data[name] = new DoubleType(value);
        }
    }
    void addInt(string name, int value = iDef)
    {
        toUpper(name);
        if(checkName(name)) {
            trim(name);
            if(!checkExistence(name))
                this->data[name] = new IntType(value);
        }
    }
    void addUint(string name, unsigned int value = uiDef)
    {
        toUpper(name);
        if(checkName(name)) {
            trim(name);
            if(!checkExistence(name))
                this->data[name] = new UintType(value);
        }
    }
    void addString(string name, string value = sDef)
    {
        toUpper(name);
        if(checkName(name)) {
            trim(name);
            if(!checkExistence(name))
                this->data[name] = new StringType(value);
        }
    }

    void readConfigurationFile(string filename)
    {
        ifstream file(filename.c_str());
        if(file.is_open()) {
            while(!file.eof()) {
                string line;
                getline(file, line);
                removeTabs(line);
                trim(line);

                if(line[0] != commentIndicator) {
                    if(line.find_first_of("=") != string::npos) {
                        string varName          = line.substr(0, line.find_first_of("="));
                        string value            = line.substr(line.find_first_of("=") + 1, line.length());
                        string originalVarName  = varName;

                        trim(varName);
                        trim(value);

                        toUpper(varName);

                        // Special case - flagged antennas
                        if(varName.compare("FLAGGED") == 0) {
                            if(checkFlagged(value) != -1) {
                                flagged.push_back(checkFlagged(value));
                            } else {
                              cerr<<endl<<"Error processing file \""<<filename<<"\"."<<endl;
                              cerr<<varName<<": Antenna '"<<value<<"' is unknown!"<<endl;
                              cerr<<"Program will continue skipping the problem."<<endl;
                            }
                        } else {
                            if(checkExistence(varName)) {
                                if(!data[varName]->setValue(value)) {
                                    cerr<<endl<<"Error processing file \""<<filename<<"\"."<<endl;
                                    cerr<<varName<<"\""<<" : "<<data[varName]->errorSetVal()<<endl;
                                    cerr<<"Program will continue skipping the problem."<<endl;
                                }
                            } else {
                                cerr<<endl<<"Error processing file \""<<filename<<"\"."<<endl;
                                cerr<<varName<<" was not declared in call_pipeline.cc"<< endl;
                                cerr<<"Program will continue skipping the problem."<<endl;
                            }
                        }
                    }
                }
            }
            // Another special case - path variable
            if(checkExistence("PATH")) {
                string currentPathValue = data["PATH"]->sValue();
                if((currentPathValue.length() > 1) && (currentPathValue[currentPathValue.length() - 1] != '/'))
                    currentPathValue += "/";
                data["PATH"]->setValue(currentPathValue);
            }
        } else {
            cerr << "Failed to open file \"" << filename <<"\".\n";
            cerr << "Program will continue with default configuration." << endl;
        }
    }
    string showConfigData()
    {
      stringstream output(stringstream::in | stringstream::out);
      for(map<string, AnyType*>::iterator iter = data.begin(); iter != data.end(); iter++) {
         output << iter->first << " = ";
         switch(iter->second->getType()) {
            case (int)_BoolType :
               output << iter->second->bValue();
               break;
            case (int)_DoubleType :
               output << iter->second->dValue();
               break;
            case (int)_IntType :
               output << iter->second->iValue();
               break;
            case (int)_UintType :
               output << iter->second->uiValue();
               break;
            case (int)_StringType :
               output << iter->second->sValue();
               break;
         }
         output<<endl;
      }
      output<<"Flagged antennas:"<<endl;
      for(unsigned int i=0; i<flagged.size(); i++) {
	output<<flagged[i]<<" \t";
      }
      output<<endl;
      return output.str();
    }
    vector<int>  getFlagged() { return flagged; }
private:
    void addFlagged(string name1, string name2, int value)
    {
        flagged1[name1] = value;
        flagged2[name2] = value;
    }
    bool checkExistence(string name)
    {
        toUpper(name);
        map<string, AnyType*>::iterator iter = data.find(name);
        if(iter != data.end())
            return true;
        return false;
    }
    bool checkName(string name)
    {
        // Names should have form: letter(letter|digit)*
        // if(!isalpha(name[0]))
        //    return false;
        // for(unsigned int i=1; i<name.length(); i++)
        //    if(!isalnum(name[i]))
        //        return false;
        // Temporarily leave no name restrictions
        return true;
    }
    int checkFlagged(string name)
    {
        map<string, int>::iterator iter1 = flagged1.find(name);
        if(iter1 != flagged1.end())
            return flagged1[name];
        map<string, int>::iterator iter2 = flagged2.find(name);
        if(iter2 != flagged2.end())
            return flagged2[name];
        return -1;
    }
    void removeTabs(string& input)
    {
        for(unsigned int i=0; i<input.length(); i++)
            if(input[i] == '\t')
                input[i] = ' ';
    }
    void toUpper(string& input)
    {
        // Since the standard method is not supported by g++ compiler, the naive approach is being used
        for(unsigned int i=0; i<input.length(); i++)
           if(isalpha(input[i]))
            input[i] = toupper(input[i]);
    }
    void trim(string& input)
    {
        input = input.erase(input.find_last_not_of(" ") + 1);
        input = input.erase(0, input.find_first_not_of(" "));
    }

    map<string, int>   flagged1;
    map<string, int>   flagged2;
    vector<int>        flagged;

    map<string, AnyType*> data;
    AnyType*              valueNotFound;
    const static char     commentIndicator = '#';
};
// ---------------------------------------------
// NOTE: classes can be written even more general and therefore allow to perform serializaion and 
// deserialization totally independent of ROOT library

// NOTE: for the sake of proper architecture design those classes should be put in seperate files (which 
// is rather useless in current design


// ------------- Global variables ----------------

// Instance of configuration class (for read in of config file)
ConfigData config;

// Set default configuration values for the pipeline
bool both_pol = false;		      // Should both polarizations be processed?

// Event parameters for calling the pipeline
string eventfilelistname("");			         // Name of the ASCII event list
string kascadeRootFile("");			         // Name of root file with Kascade reconstruction and event names
string eventname("");
double azimuth=0, elevation=0, radiusOfCurvature=0, core_x=0, core_y=0;   // basic input parameters (e.g. from Kascade)

// Variables of KASCADE root file
float_t Az = 0, Ze = 0, Xc = 0, Yc = 0;			// KASCADE direction and core
float_t Azg = 0, Zeg = 0, Xcg = 0, Ycg = 0;		// Grande direction and core
float_t Size = 0, Sizeg = 0;				// Electron numbers (KASCADE + Grande)
float_t Nmu = 0, Lmuo = 0, Sizmg = 0;			// Muon number, trucated muon number (KASCADE), Muon number (Grande)
double_t geomag = 0;					// geomagnetic angle
double_t lgE = 0, lg_errE = 0;				// estimated energy (KASCADE)
double_t lgEg = 0, lg_errEg = 0;			// estimated energy (Grande)
double_t lnMass = 0, ln_errMass = 0;			// estimated mass A (KASCADE)
double_t lnMassg = 0, ln_errMassg = 0;			// estimated mass A (Grande)

char reconstruction = 'A';	// A = KASCADE reconstruction taken, G = Grande reconstruction taken


// ------------- Functions ----------------

/*!
  \brief reads text file which contains configuration information

  \param filenname     -- 

  \return nothing (If an error occurs, the program will continue with the default configuration.)
*/

void readConfigFile (const string &filename)
{
   // define default configuration values
   config.addString("caltablepath", caltable_lopes);
   config.addString("path", "");
   config.addBool("preferGrande", false);		// per default prefer KASCADE reconstruction as input
   config.addBool("generatePlots", true);         	// the plot prefix will be the name of the event file
   config.addBool("eventDisplayPlot", false);           // Plot an event display (amplitudes + arrival times)
   config.addBool("generateSpectra", false);        	// by default no spectra are plotted
   config.addBool("singlePlots", false);        	// by default there are no single plots for each antenna
   config.addBool("PlotRawData", false);	     	// by default there the raw data are not plotted
   config.addBool("CalculateMaxima", false);	      	// by default the maxima are not calculated
   config.addBool("listCalcMaxima", false);        	// print calculated maxima in more user friendly way
   config.addBool("printShowerCoordinates", false);   	// print the distance between antenna and shower core
   config.addBool("RotatePos", true); 	      		// should be true if coordinates are given in KASKADE frame
   config.addBool("verbose", true);
   config.addBool("ignoreDistance", true);          	// distance value of the eventlist will be ignored
   config.addBool("simplexFit", true);
   config.addBool("doGainCal", true);		      	// calibration of the electrical fieldstrength
   config.addBool("doDispersionCal", true);	      	// application of the CalTable PhaseCal values	
   config.addBool("doDelayCal", true);              	// correction for the general delay of each antenna
   config.addBool("doRFImitigation", true);	      	// supresses narrow band noise ("RFI)
   config.addBool("doFlagNotActiveAnts", true);     	// flags antennas marked as "not active" in the CalTables
   config.addBool("doAutoFlagging", true);	      	// flags antennas due to bad signal (does not affect flagging by the phase)
   config.addDouble("plotStart", -2.05e-6);	      	// in seconds
   config.addDouble("plotStop", -1.60e-6);	      	// in seconds
   config.addDouble("spectrumStart", 40e6);            	// for plotting", in Hz
   config.addDouble("spectrumStop", 80e6);	      	// for plotting", in Hz
   config.addDouble("freqStart", 40e6);                	// for analysis", in Hz
   config.addDouble("freqStop", 80e6);	                // for analysis", in Hz
   config.addDouble("upsamplingRate", 0.);	      	// Upsampling Rate for new upsampling
   config.addUint("upsamplingExponent", 0);  		// by default no upsampling will be done
   config.addUint("minBeamformingAntennas", 4);  	// minimal numbers of antennas required for beam forming
   config.addUint("summaryColumns", 0);      		// be default no summary of all plots
   config.addDouble("ccWindowWidth", 0.045e-6);	      	// width of window for CC-beam
   config.addString("rootFileName", "");               	// Name of root file for output
   config.addBool("writeBadEvents", false);         	// also bad events are written into the root file ("if possible)
   config.addBool("calibrationMode", false);	      	// Calibration mode is off by default
   config.addBool("lateralDistribution", false);    	// the lateral distribution will not be generated
   config.addBool("lateralOutputFile", false);      	// no file for the lateral distribution will be created
   config.addDouble("lateralSNRcut", 1.0);            	// SNR cut for removing points from lateral distribution
   config.addDouble("lateralTimeCut", 15e-9);         	// Allowed time window +/- arround CC-beam-center for found peaks
   config.addBool("calculateMeanValues", false);   	// calculate some mean values of all processed events
   config.addBool("lateralTimeDistribution", false);   // the lateral time distribution will not be generated

   IntType* _doTVcal = new IntType(-1);                // 1: yes, 0: no, -1: use default	
   _doTVcal->addAllowedValue(1);
   _doTVcal->addAllowedValue(0);
   _doTVcal->addAllowedValue(-1);
   config.addType("doTVcal", _doTVcal);

   StringType* _polarization = new StringType("ANY");  // polarization: ANY (=don't care), EW, NS or BOTH
   _polarization->addAllowedValue("ANY");
   _polarization->addAllowedValue("EW");
   _polarization->addAllowedValue("NS");
   _polarization->addAllowedValue("BOTH");
   config.addType("polarization", _polarization);

   StringType* _rootFileMode = new StringType("RECREATE");      // Mode, how to access root file
   _rootFileMode->addAllowedValue("RECREATE");
   _rootFileMode->addAllowedValue("UPDATE");
   config.addType("rootFileMode", _rootFileMode);

   config.readConfigurationFile(filename);

   cout<<"=== Configuration parameters ==="<<endl;
   cout<<config.showConfigData();
   cout<<"================================"<<endl;

   if(config["polarization"]->sValue() == "BOTH")
     both_pol = true;
   else
     both_pol = false;
}


/*!
  \brief reads the next event from an eventlist file

  \param eventfilelistname     -- name of eventlist

  \return true if an event could be read in, false in case of EOF or error
*/

bool getEventFromEventlist (const string &eventfilelistname)
{
  static bool fileOpen = false;		// will be set to true, once an eventlist was openend
  static ifstream eventfilelist;

  try {
    // reset pipeline parameters before readin
    eventname ="";
    azimuth=0, elevation=0, radiusOfCurvature=0, core_x=0, core_y=0;

    // if this function is called for the first time, then try to open the file
    if (!fileOpen) {
      // open event file list
      eventfilelist.open (eventfilelistname.c_str(), ifstream::in);

      // check if file could be opened
      if (!(eventfilelist.is_open())) {
        cerr << "Failed to open file \"" << eventfilelistname <<"\"." << endl;
        return false;
      }

      cout << "Opened eventlist: " << eventfilelistname << endl;

      // store the file is open and read in the header
      fileOpen = true;
      // look for the end of the header (after a line containing only and at least three '-' or '=')
      string temp_read;
      bool header_found = false;
      while ((header_found == false) && (eventfilelist.good())) {
        eventfilelist >> temp_read;
        if ((temp_read.find("---") != string::npos) || (temp_read.find("===") != string::npos))
        header_found = true;
      }
      //  if no header was found, assume that the file begins with an event and reopen it.
      if (header_found == false) {
        eventfilelist.close();  // close file
        cout << "\nWarning: No header found in file \"" << eventfilelistname <<"\".\n" ;
        cout << "Program will continue normally.\n" << endl;
        eventfilelist.open (eventfilelistname.c_str(), ifstream::in); // reopen file to start at the beginning
      }
    }

    // check if file is still good
    if (!(eventfilelist.good())) {
      cout << "\nFile \"" << eventfilelistname 
           << "\"is no longer good. Do not worry if this message appears after processing the last event."
           << endl;
      // close file and return to main()
      eventfilelist.close();
      fileOpen = false;
      return false;
    }

    // if the file is open, then try to get the next event
    bool read_in_error = false;
    if (fileOpen) {
      // read in event name
      if (eventfilelist.good()) eventfilelist >> eventname;
        else read_in_error = true;

      // check if end of file occured:
      // eventname should contain "", if file is terminated by a new line
      if (eventname == "") {
        cout << "\nFile \"" << eventfilelistname 
             << "\"seems to contain no more events."
             << endl;
        // close file and return to main()
        eventfilelist.close();
        fileOpen = false;
        return false;
      }

      // in calibration mode the eventfile list contains only the event name
      // otherwise read azimuth, elevation, radiusOfCurvature and shower core
      if ( !config["calibrationMode"]->bValue() ) {
        if (eventfilelist.good()) eventfilelist >> azimuth;
          else read_in_error = true;
        if (eventfilelist.good()) eventfilelist >> elevation;
          else read_in_error = true;
        if (eventfilelist.good()) eventfilelist >> radiusOfCurvature;
          else read_in_error = true;
        if (eventfilelist.good()) eventfilelist >> core_x;
          else read_in_error = true;
        if (eventfilelist.good()) eventfilelist >> core_y;
          else read_in_error = true;
      }
    }

    // return false if error occured and print error message
    if (read_in_error) {
      eventfilelist.close();  // close file
      fileOpen = false;

      cerr << "\nError processing file \"" << eventfilelistname <<"\".\n" ;
      cerr << "Use the following file format: \n\n";
      cerr << "some lines of description (any text)\n";
      cerr << "==================================\n";
      if (config["calibrationMode"]->bValue()) {
        cerr << "eventfile1\n";
        cerr << "eventfile2\n";
      } else {
        cerr << "eventfile1 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n";
        cerr << "eventfile2 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]]\n";
      }
      cerr << "... \n" << endl;
      return false;  // go back to main
    } else {
      // if no error occured the pipeline parameters should contain the new values now
      return true;
    }
  } catch (AipsError x) {
    cerr << "call_pipeline:getFileFromEventlist: " << x.getMesg() << endl;
  }

  // normally should not get here, if so return false
  return false;
}


/*!
  \brief reads the next event from an root file containing the KASCADE reconstruction and the LOPES event name

  \param kascadeRootFile     -- input file

  \return true if an event could be read in, false in case of EOF or error
*/

bool getEventFromKASCADE (const string &kascadeRootFile)
{
  static bool fileOpen = false;		// will be set to true, once an eventlist was openend
  static int eventNumber = 0;
  static TFile *inputFile = NULL;
  static TTree *inputTree  = NULL;

  // for input form root file
  static char Eventname[64];

  try {
    // reset pipeline parameters before readin
    eventname ="";
    azimuth=0, elevation=0, radiusOfCurvature=0, core_x=0, core_y=0;

    // if this function is called for the first time, then try to open the file
    if (!fileOpen) {
      // open root file with the KASCADE results
      inputFile = new TFile(kascadeRootFile.c_str());
      if(!inputFile || inputFile->IsZombie()) {
        cerr << "Failed to open file \"" << kascadeRootFile <<"\"." << endl;
        return false;
      }

      inputTree = (TTree*)inputFile->Get("k;1");
      if (!inputTree || (inputTree->GetEntries() == 0) ) {
        cerr << "Failed to get tree of file \"" << kascadeRootFile <<"\" or tree is empty." << endl;
        return false;
      }

      // set the read in variables in the tree
      inputTree->SetBranchAddress("Az",&Az);
      inputTree->SetBranchAddress("Ze",&Ze);
      inputTree->SetBranchAddress("Xc",&Xc);
      inputTree->SetBranchAddress("Yc",&Yc);
      inputTree->SetBranchAddress("Azg",&Az);
      inputTree->SetBranchAddress("Zeg",&Ze);
      inputTree->SetBranchAddress("Xcg",&Xc);
      inputTree->SetBranchAddress("Ycg",&Yc);
      inputTree->SetBranchAddress("Size",&Size);
      inputTree->SetBranchAddress("Sizeg",&Sizeg);
      inputTree->SetBranchAddress("Nmu",&Nmu);
      inputTree->SetBranchAddress("Lmuo",&Lmuo);
      inputTree->SetBranchAddress("Sizmg",&Sizmg);
      inputTree->SetBranchAddress("geomag",&geomag);
      inputTree->SetBranchAddress("lgE",&lgE);
      inputTree->SetBranchAddress("lg_errE",&lg_errE);
      inputTree->SetBranchAddress("lgEg",&lgEg);
      inputTree->SetBranchAddress("lg_errEg",&lg_errEg);
      inputTree->SetBranchAddress("lnMass",&lnMass);
      inputTree->SetBranchAddress("ln_errMass",&ln_errMass);
      inputTree->SetBranchAddress("lnMassg",&lnMassg);
      inputTree->SetBranchAddress("ln_errMassg",&ln_errMassg);
      inputTree->SetBranchAddress("Eventname",&Eventname);

      // as there is no radius of curvature in the file, set ignoreDistance to true
      if (!config["ignoreDistance"]->bValue()) {
        config["ignoreDistance"]->setValue("TRUE");
        cout << "\nWARNING: OVERWRITING CONFIGURATION: ignoreDistance was set to 'true'!\n" << endl;
      }

      cout << "Opened file for readin: " << kascadeRootFile << endl;
      fileOpen = true;
    }

    // check if events are remaining
    if ( inputFile->IsZombie() || (eventNumber >= inputTree->GetEntries()) ) {
      cout << "\nFile \"" << kascadeRootFile
           << "\" contains no more events. Do not worry if this message appears after processing the last event."
           << endl;
      fileOpen = false;
      return false;
    }

    // if file is open the get the next event
    if (fileOpen) {
      cout << "\nReading event number " << eventNumber+1 << " from file \"" << kascadeRootFile << "\" ";
      inputTree->GetEntry(eventNumber);
      eventNumber++;

      // choose if Kascade or Grande date should be used:
      bool grande = false;

      // if the Grande reconstruction should be prefered, then take it, if either the
      // core is outside of 90 m radius, or if the core lies inside the Grande array
      // if Kascade shall be prefered, then switch to Grande only if it is inside the 
      // Grande Array and not inside the 90 m radius
      if (config["preferGrande"]->bValue()) {
        if ( sqrt(Xc*Xc+Yc*Yc)>90 )
          grande = true;
        if ( (Xcg<-50) && (Ycg<-30) )
          grande = true;
      } else {
        if ( (sqrt(Xc*Xc+Yc*Yc)>90) && ((Xcg<-50) && (Xcg>-420) && (Ycg<-30) && (Ycg>-550)) )
          grande = true;
      }

      // set variables (Grande or KASCADE reconstruction)
      eventname = string(Eventname);
      if (grande) {
        cout << "using the Grande reconstruction as input..." << endl;
        azimuth = static_cast<double>(Azg);
        elevation = static_cast<double>(90.-Zeg);
        core_x = static_cast<double>(Xcg);
        core_y = static_cast<double>(Ycg);
        reconstruction = 'G';
      } else {
        cout << "using the KASCADE reconstruction as input..." << endl;
        azimuth = static_cast<double>(Az);
        elevation = static_cast<double>(90.-Ze);
        core_x = static_cast<double>(Xc);
        core_y = static_cast<double>(Yc);
        reconstruction = 'A';
      }
      return true;
    }

  } catch (AipsError x) {
    cerr << "call_pipeline:getEventFromKASCADE: " << x.getMesg() << endl;
  }

  // normally should not get here, if so return false
  return false;
}


/*!
  \brief reads the next event from the input list

  \return true if an event could be read in, false in case of EOF or error
*/

bool getNextEvent()
{
  // check if an eventlist was supplied as input format
  if (eventfilelistname != "")
    return getEventFromEventlist(eventfilelistname);
  if (kascadeRootFile != "")
    return getEventFromKASCADE(kascadeRootFile);

  cerr << "\ncall_pipeline:getNextEvent: No input list found!" << endl;
  return false; 		// should not come to this point
}



// -----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
  Record results;					 // results of the pipeline

  // variables for reconstruction information (output of pipeline)
  unsigned int gt = 0;
  char eventfilename[64] ;			         // Name of event file (first 64 characters)
  double CCheight, CCheight_NS;                          // CCheight will be used for EW polarization or ANY polarization
  double CCwidth, CCwidth_NS;
  double CCheight_error, CCheight_error_NS;
  bool CCconverged, CCconvergedNS;                       // is true if the Gaussian fit to the CCbeam converged
  double Xheight, Xheight_NS;                            // CCheight will be used for EW polarization or ANY polarization
  double Xheight_error, Xheight_error_NS;
  bool Xconverged, XconvergedNS;                         // is true if the Gaussian fit to the CCbeam converged
  double AzL, ElL, AzL_NS, ElL_NS;                       // Azimuth and Elevation
  double distanceResult = 0, distanceResultNS = 0;       // distance = radius of curvature
  double R_0 = 0, sigR_0 = 0, R_0_NS = 0, sigR_0_NS = 0;             // R_0 from lateral distribution exponential fit
  double eps = 0, sigeps = 0, eps_NS = 0, sigeps_NS = 0;             // Epsilon from lateral distribution exponential fit
  double chi2NDF = 0, chi2NDF_NS = 0;                                // Chi^2/NDF of lateral distribution exponential fit
  double kPow = 0, sigkPow = 0, kPow_NS = 0, sigkPow_NS = 0;         // k from lateral distribution power law fit
  double epsPow = 0, sigepsPow = 0, epsPow_NS = 0, sigepsPow_NS = 0; // Epsilon from lateral distribution power law fit
  double chi2NDFPow = 0, chi2NDFPow_NS = 0;                          // Chi^2/NDF of lateral distribution power law fit
  map <int,PulseProperties> rawPulsesMap;                // pulse properties of pules in raw data traces
  map <int,PulseProperties> calibPulsesMap;              // pulse properties of pules in calibrated data traces
  bool goodEW = false, goodNS = false;                // true if reconstruction worked
  double rmsCCbeam, rmsCCbeam_NS;                        // rms values of the beams in remote region
  double rmsXbeam, rmsXbeam_NS;
  double rmsPbeam, rmsPbeam_NS;
  int CutCloseToCore, CutCloseToCore_NS;              // # of cut antennas in lateral distribution fit
  int CutSmallSignal, CutSmallSignal_NS;              // # of cut antennas in lateral distribution fit
  int CutBadTiming, CutBadTiming_NS;                  // # of cut antennas in lateral distribution fit
  int CutSNR, CutSNR_NS;                              // # of cut antennas in lateral distribution fit
  double latMeanDist, latMeanDist_NS;                 // mean distance of the antennas in the lateral distribution
  double latMeanDistCC, latMeanDistCC_NS;             // mean distance of the antennas used for the CC beam
  PulseProperties* rawPulses[MAX_NUM_ANTENNAS];       // use array of pointers to store pulse properties in root tree
  PulseProperties* calibPulses[MAX_NUM_ANTENNAS];     // use array of pointers to store pulse properties in root tree
  PulseProperties* meanRawPulses[MAX_NUM_ANTENNAS];   // mean pulse properties of all events
  PulseProperties* meanCalPulses[MAX_NUM_ANTENNAS];   // mean pulse properties of all events
  unsigned int meanRawCounter[MAX_NUM_ANTENNAS], meanCalCounter[MAX_NUM_ANTENNAS];  // Counters for mean calculation
  unsigned int meanResCounter[MAX_NUM_ANTENNAS];  // Counter for mean calculation of lateral distr. residuals

  try {
    // allocate space for arrays with pulse properties
    for (int i=0; i < MAX_NUM_ANTENNAS; ++i) {
      rawPulses[i] = new PulseProperties();
      calibPulses[i] = new PulseProperties();
      meanRawPulses[i] = new PulseProperties();
      meanCalPulses[i] = new PulseProperties();
      meanRawCounter[i] = 0;
      meanCalCounter[i] = 0;
      meanResCounter[i] = 0;
    }


    cout << "\nStarting Program \"call_pipline\".\n\n" << endl;

    // check arguments of call_pipeline
    int i = 1;		//counter for arguments
    while (i < argc) {
      // get option and argument
      string option(argv[i]);
      i++;
      // look for keywords which require no argument
      if ( (option == "--help") || (option == "-help")
           || (option == "--h") || (option == "-h")) {
        cout << "call_pipeline runs the CR-Tools pipeline for LOPES events.\n"
             << "Possible options and arguments:\n"
             << "option        argument\n"
             << "--help                           Displays this help message\n"
             << "--in, -i      eventfilelist      A test file which contains a list of events to process (required)\n"
             << "--config, -c  configfile         A file containing configuration parameters how to process the events (optional)\n"
             << "\n\nUse the following file format for the eventlist: \n\n"
             << "some lines of text\n"
             << "===================================\n"
             << "eventfile1 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n"
             << "eventfile2 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n"
             << "... \n"
             << "\n\nUse the following file format for the config file\n"
             << "# comments by '#'\n\n"
             << "# some comments if you like\n"
             << "caltablepath = $LOFARSOFT/data/lopes/LOPES-CalTable\n"
             << "path = /home/schroeder/lopes/events\n"
             << "preferGrande = false\n"
             << "RotatePos = true\n"
             << "generatePlots = true\n"
             << "SinglePlots = true\n"
             << "eventDisplayPlot = false\n"
             << "PlotRawData = false\n"
             << "GenerateSpectra = true\n"
             << "CalculateMaxima = false\n"
             << "listCalcMaxima=false\n"
             << "printShowerCoordinates=false\n"
             << "verbose = true\n"
             << "ignoredistance = true\n"
             << "simplexFit = true\n"
             << "doTVcal = default\n"
             << "doGainCal = true\n"
             << "doDispersionCal = true\n"
             << "doDelayCal = true\n"
             << "doRFImitigation = true\n"
             << "doFlagNotActiveAnts = true\n"
             << "doAutoFlagging = true\n"
             << "polarization = ANY\n"
             << "plotStart = -2.05e-6\n"
             << "plotStop = -1.60e-6\n"
             << "spectrumStart = 40e6\n"
             << "spectrumEnd = 80e6\n"
             << "upsamplingRate = 320e6\n"
             << "upsamplingExponent = 0\n"
             << "minBeamformingAntennas = 4\n"
             << "summaryColumns = 3\n"
             << "ccWindowWidth = 0.045e-6\n"
             << "flagged = 10101\n"
             << "flagged = 10102\n"
             << "rootfilename = output.root\n"
             << "rootfilemode = recreate\n"
             << "writebadevents = false\n"
             << "calibration = false\n"
             << "lateralDistribution = false\n"
             << "lateralOutputFile = false\n"
             << "lateralSNRcut = 1.0\n"
             << "lateralTimeCut = 25e-9\n"
             << "calculateMeanValues = false\n"
             << "lateralTimeDistribution = false\n"
             << "... \n"
             << endl;
        return 0;	// exit here
      }

      // now look for options which require an argument and get the argument
      if (i >= argc) {
        cerr << "ERROR: No argument given for option or unknown option: \"" << option << "\"\n";
        cerr << "Use --help for more information." << endl;
        return 1;			// Exit the program if argument is missing
      }
      string argument(argv[i]);
      i++;

      // look for keywords which require an option
      if ( (option == "--in") || (option == "-in")
           || (option == "--i") || (option == "-i")) {
        eventfilelistname = argument;
        continue;
      }

      // look for keywords which require an option
      if ( (option == "--k") || (option == "-k")) {
        kascadeRootFile = argument;
        continue;
      }

      if ( (option == "--config") || (option == "-config")
           || (option == "--c") || (option == "-c")) {
        cout << "Reading config data from file: " << argument << endl;
        readConfigFile(argument);
        continue;
      }

      // if loop comes here, the option was not found
      cerr << "ERROR: Invalid option: \"" << option << "\"\n";
      cerr << "Use --help for more information." << endl;
      return 1;			// Exit the program if argument is missing
    }

    // Check if options are set correctly
    if ((eventfilelistname == "") && (kascadeRootFile == "") ) {
      cerr << "ERROR: Please set an input file with the --in or --k option!\n";
      cerr << "Use --help for more information." << endl;
      return 1;
    }
    if (!(eventfilelistname == "") && !(kascadeRootFile == "") ) {
      cerr << "ERROR: Please set only one input file with the --in and --k option!\n";
      cerr << "Use --help for more information." << endl;
      return 1;
    }

    // prepare output in root file
    TFile *rootfile=NULL;

    if (config["rootFileName"]->sValue() != "") {
      // open root file and create tree structure
      rootfile = new TFile(config["rootFileName"]->sValue().c_str(),config["rootFileMode"]->sValue().c_str(),"Resulst of CR-Tools pipeline");
      // check if file is open
      if (rootfile->IsZombie()) {
        cerr << "\nError: Could not create file: " << config["rootFileName"]->sValue() << "\n" << endl;
        return 1;		// exit program
      }
    }

    // create tree and tree structure (depends on chosen polarization)
    TTree *roottree = NULL;
    if (config["rootFileMode"]->sValue() == "RECREATE");
      roottree = new TTree("T","LOPES");
    if (config["rootFileMode"]->sValue() == "UPDATE")
      roottree = (TTree*)rootfile->Get("T;1");
    roottree->Branch("Gt",&gt,"Gt/i");	// GT as unsigned int
    roottree->Branch("Eventname",&eventfilename,"Eventname/C");

    // the following branches are not used in the calibration mode
    if ( !config["calibrationMode"]->bValue() ) {
      if (kascadeRootFile == "") {
        roottree->Branch("Xc",&core_x,"Xc/D");
        roottree->Branch("Yc",&core_y,"Yc/D");
        roottree->Branch("AzIn",&azimuth,"AzIn/D");
        roottree->Branch("ElIn",&elevation,"ElIn/D");
        roottree->Branch("DistanceIn",&radiusOfCurvature,"DistanceIn/D");
      } else {
        roottree->Branch("Xc",&Xc,"Xc/F");
        roottree->Branch("Xcg",&Xcg,"Xcg/F");
        roottree->Branch("Yc",&Yc,"Yc/F");
        roottree->Branch("Ycg",&Ycg,"Ycg/F");
        roottree->Branch("Az",&Az,"Az/F");
        roottree->Branch("Azg",&Azg,"Azg/F");
        roottree->Branch("Ze",&Ze,"Ze/F");
        roottree->Branch("Zeg",&Zeg,"Zeg/F");
        roottree->Branch("Size",&Size,"Size/F");
        roottree->Branch("Sizeg",&Sizeg,"Sizeg/F");
        roottree->Branch("Nmu",&Nmu,"Nmu/F");
        roottree->Branch("Lmuo",&Lmuo,"Lmuo/F");
        roottree->Branch("Sizmg",&Sizmg,"Sizmg/F");
        roottree->Branch("geomag",&geomag,"geomag/D");
        roottree->Branch("lgE",&lgE,"lgE/D");
        roottree->Branch("lg_errE",&lg_errE,"lg_errE/D");
        roottree->Branch("lgEg",&lgEg,"lgEg/D");
        roottree->Branch("lg_errEg",&lg_errEg,"lg_errEg/D");
        roottree->Branch("lnMass",&lnMass,"lnMass/D");
        roottree->Branch("ln_errMass",&ln_errMass,"ln_errMass/D");
        roottree->Branch("lnMassg",&lnMassg,"lnMassg/D");
        roottree->Branch("ln_errMassg",&ln_errMassg,"ln_errMassg/D");
        roottree->Branch("reconstruction",&reconstruction,"reconstruction/B");
      }

      // one result, if polarization = ANY
      if (config["polarization"]->sValue() == "ANY") {
        roottree->Branch("AzL",&azimuth,"AzL/D");
        roottree->Branch("ElL",&elevation,"ElL/D");
        roottree->Branch("Distance",&distanceResult,"Distance/D");	// radius of curvature
        roottree->Branch("CCheight",&CCheight,"CCheight/D");
        roottree->Branch("CCwidth",&CCwidth,"CCwidth/D");
        roottree->Branch("CCheight_error",&CCheight_error,"CCheight_error/D");
        roottree->Branch("CCconverged",&CCconverged,"CCconverged/B");
        roottree->Branch("Xheight",&Xheight,"Xheight/D");
        roottree->Branch("Xheight_error",&Xheight_error,"Xheight_error/D");
        roottree->Branch("Xconverged",&Xconverged,"Xconverged/B");
        roottree->Branch("goodReconstructed",&goodEW,"goodReconstructed/B");
        roottree->Branch("rmsCCbeam",&rmsCCbeam,"rmsCCbeam/D");
        roottree->Branch("rmsXbeam",&rmsXbeam,"rmsXbeam/D");
        roottree->Branch("rmsPbeam",&rmsPbeam,"rmsPbeam/D");
        roottree->Branch("latMeanDistCC",&latMeanDistCC,"latMeanDistCC/D");
        if (config["lateralDistribution"]->bValue()) {
          roottree->Branch("R_0",&R_0,"R_0/D");
          roottree->Branch("sigR_0",&sigR_0,"sigR_0/D");
          roottree->Branch("eps",&eps,"eps/D");
          roottree->Branch("sigeps",&sigeps,"sigeps/D");
          roottree->Branch("chi2NDF",&chi2NDF,"chi2NDF/D");
          roottree->Branch("kPow",&kPow,"kPow/D");
          roottree->Branch("sigkPow",&sigkPow,"sigkPow/D");
          roottree->Branch("epsPow",&epsPow,"epsPow/D");
          roottree->Branch("sigepsPow",&sigepsPow,"sigepsPow/D");
          roottree->Branch("chi2NDFPow",&chi2NDFPow,"chi2NDFPow/D");
          roottree->Branch("CutCloseToCore",&CutCloseToCore,"CutCloseToCore/I");
          roottree->Branch("CutSmallSignal",&CutSmallSignal,"CutSmallSignal/I");
          roottree->Branch("CutBadTiming",&CutBadTiming,"CutBadTiming/I");
          roottree->Branch("CutSNR",&CutSNR,"CutSNR/I");
          roottree->Branch("latMeanDist",&latMeanDist,"latMeanDist/D");
        }
      }
      if ( (config["polarization"]->sValue() == "EW") || (config["polarization"]->sValue() == "BOTH")) {
        roottree->Branch("AzL_EW",&AzL,"AzL_EW/D");
        roottree->Branch("ElL_EW",&ElL,"ElL_EW/D");
        roottree->Branch("Distance_EW",&distanceResult,"Distance_EW/D");	// radius of curvature
        roottree->Branch("CCheight_EW",&CCheight,"CCheight_EW/D");
        roottree->Branch("CCwidth_EW",&CCwidth,"CCwidth_EW/D");
        roottree->Branch("CCheight_error_EW",&CCheight_error,"CCheight_error_EW/D");
        roottree->Branch("CCconverged_EW",&CCconverged,"CCconverged_EW/B");
        roottree->Branch("Xheight_EW",&Xheight,"Xheight_EW/D");
        roottree->Branch("Xheight_error_EW",&Xheight_error,"Xheight_error_EW/D");
        roottree->Branch("Xconverged_EW",&Xconverged,"Xconverged_EW/B");
        roottree->Branch("goodReconstructed_EW",&goodEW,"goodReconstructed_EW/B");
        roottree->Branch("rmsCCbeam_EW",&rmsCCbeam,"rmsCCbeam_EW/D");
        roottree->Branch("rmsXbeam_EW",&rmsXbeam,"rmsXbeam_EW/D");
        roottree->Branch("rmsPbeam_EW",&rmsPbeam,"rmsPbeam_EW/D");
        roottree->Branch("latMeanDistCC_EW",&latMeanDistCC,"latMeanDistCC_EW/D");
        if (config["lateralDistribution"]->bValue()) {
          roottree->Branch("R_0_EW",&R_0,"R_0_EW/D");
          roottree->Branch("sigR_0_EW",&sigR_0,"sigR_0_EW/D");
          roottree->Branch("eps_EW",&eps,"eps_EW/D");
          roottree->Branch("sigeps_EW",&sigeps,"sigeps_EW/D");
          roottree->Branch("chi2NDF_EW",&chi2NDF,"chi2NDF_EW/D");
          roottree->Branch("kPow_EW",&kPow,"kPow_EW/D");
          roottree->Branch("sigkPow_EW",&sigkPow,"sigkPow_EW/D");
          roottree->Branch("epsPow_EW",&epsPow,"epsPow_EW/D");
          roottree->Branch("sigepsPow_EW",&sigepsPow,"sigepsPow_EW/D");
          roottree->Branch("chi2NDFPow_EW",&chi2NDFPow,"chi2NDFPow_EW/D");
          roottree->Branch("CutCloseToCore_EW",&CutCloseToCore,"CutCloseToCore_EW/I");
          roottree->Branch("CutSmallSignal_EW",&CutSmallSignal,"CutSmallSignal_EW/I");
          roottree->Branch("CutBadTiming_EW",&CutBadTiming,"CutBadTiming_EW/I");
          roottree->Branch("CutSNR_EW",&CutSNR,"CutSNR_EW/I");
          roottree->Branch("latMeanDist_EW",&latMeanDist,"latMeanDist_EW/D");
        }
      }
      if ( (config["polarization"]->sValue() == "NS") || (config["polarization"]->sValue() == "BOTH")) {
        roottree->Branch("AzL_NS",&AzL_NS,"AzL_NS/D");
        roottree->Branch("ElL_NS",&ElL_NS,"ElL_NS/D");
        roottree->Branch("Distance_NS",&distanceResultNS,"Distance_NS/D");	// radius of curvature
        roottree->Branch("CCheight_NS",&CCheight_NS,"CCheight_NS/D");
        roottree->Branch("CCwidth_NS",&CCwidth_NS,"CCwidth_NS/D");
        roottree->Branch("CCheight_error_NS",&CCheight_error_NS,"CCheight_error_NS/D");
        roottree->Branch("CCconverged_NS",&CCconverged,"CCconverged_NS/B");
        roottree->Branch("Xheight_NS",&Xheight_NS,"Xheight_NS/D");
        roottree->Branch("Xheight_error_NS",&Xheight_error_NS,"Xheight_error_NS/D");
        roottree->Branch("Xconverged_NS",&Xconverged,"Xconverged_NS/B");
        roottree->Branch("goodReconstructed_NS",&goodNS,"goodReconstructed_NS/B");
        roottree->Branch("rmsCCbeam_NS",&rmsCCbeam_NS,"rmsCCbeam_NS/D");
        roottree->Branch("rmsXbeam_NS",&rmsXbeam_NS,"rmsXbeam_NS/D");
        roottree->Branch("rmsPbeam_NS",&rmsPbeam_NS,"rmsPbeam_NS/D");
        roottree->Branch("latMeanDistCC_NS",&latMeanDistCC_NS,"latMeanDistCC_NS/D");
        if (config["lateralDistribution"]->bValue()) {
          roottree->Branch("R_0_NS",&R_0_NS,"R_0_NS/D");
          roottree->Branch("sigR_0_NS",&sigR_0_NS,"sigR_0_NS/D");
          roottree->Branch("eps_NS",&eps_NS,"eps_NS/D");
          roottree->Branch("sigeps_NS",&sigeps_NS,"sigeps_NS/D");
          roottree->Branch("chi2NDF_NS",&chi2NDF_NS,"chi2NDF_NS/D");
          roottree->Branch("kPow_NS",&kPow_NS,"kPow_NS/D");
          roottree->Branch("sigkPow_NS",&sigkPow_NS,"sigkPow_NS/D");
          roottree->Branch("epsPow_NS",&epsPow_NS,"epsPow_NS/D");
          roottree->Branch("sigepsPow_NS",&sigepsPow_NS,"sigepsPow_NS/D");
          roottree->Branch("chi2NDFPow_NS",&chi2NDFPow_NS,"chi2NDFPow_NS/D");
          roottree->Branch("CutCloseToCore_NS",&CutCloseToCore_NS,"CutCloseToCore_NS/I");
          roottree->Branch("CutSmallSignal_NS",&CutSmallSignal_NS,"CutSmallSignal_NS/I");
          roottree->Branch("CutBadTiming_NS",&CutBadTiming_NS,"CutBadTiming_NS/I");
          roottree->Branch("CutSNR_NS",&CutSNR_NS,"CutSNR_NS/I");
          roottree->Branch("latMeanDist_NS",&latMeanDist_NS,"latMeanDist_NS/D");
        }
      }
    } //if

    // Process events from event file list
    while ( getNextEvent() ) {
      // print information and process the event
      if (config["calibrationMode"]->bValue()) {
        cout << "\nProcessing calibration event \"" << eventname << "\".\n" << endl;
      } else {
        cout << "\nProcessing event \"" << eventname << "\"\nwith azimuth " << azimuth << " °, elevation " << elevation
                  << " °, distance (radius of curvature) " << radiusOfCurvature << " m, core position X " << core_x
                  << " m and core position Y " << core_y << " m." << endl;
      }

      // Check if file exists
      ifstream ftest( string(config["path"]->sValue()+eventname).c_str());
      if(!ftest.is_open()) {
        cerr << "Unable to open "<<config["path"]->sValue()+eventname<<endl;
        continue;
      }
      ftest.close();

      // Create a plotprefix using the eventname
      string plotprefix = eventname;

      // delete the file ending
      if (plotprefix.find(".event") != string::npos)
        plotprefix.erase(plotprefix.find_last_of('.'));	
      // deletes the file path, if it exists	
      if (plotprefix.find("/") != string::npos)
       plotprefix.erase(0,plotprefix.find_last_of('/')+1);

      // set reconstruction flags to bad
      // if they are not set to true during reconstruction the event is not written into the root file
      goodEW = false;
      goodNS = false;

      // Generate cstring with a length of 64 characters to write to the root file, using the eventname
      string eventname_ =  eventname;
      // deletes the file path, if it exists    
      if (eventname_.find("/") != string::npos)
        eventname_.erase(0,plotprefix.find_last_of('/')+1);
      // resize to 64 characters if longer
      eventname_.resize(64, char(0));
      strcpy (eventfilename, eventname_.c_str());

      // reset all resuls to 0
      gt = 0;
      CCheight = 0, CCheight_NS = 0;
      CCwidth = 0, CCwidth_NS = 0;
      CCheight_error = 0, CCheight_error_NS = 0;
      CCconverged = 0, CCconvergedNS = 0;
      Xheight = 0, Xheight_NS = 0;
      Xheight_error = 0, Xheight_error_NS = 0;
      Xconverged = 0, XconvergedNS = 0;
      AzL = 0, ElL = 0, AzL_NS = 0, ElL_NS = 0;
      distanceResult = 0, distanceResultNS = 0;
      R_0 = 0, sigR_0 = 0, R_0_NS = 0, sigR_0_NS = 0;
      eps = 0, sigeps = 0, eps_NS = 0, sigeps_NS = 0;
      chi2NDF = 0, chi2NDF_NS = 0;
      kPow = 0, sigkPow = 0, kPow_NS = 0, sigkPow_NS = 0;
      epsPow = 0, sigepsPow = 0, epsPow_NS = 0, sigepsPow_NS = 0;
      chi2NDFPow = 0, chi2NDFPow_NS = 0;
      rmsCCbeam = 0, rmsXbeam = 0, rmsPbeam = 0;
      rmsCCbeam_NS = 0, rmsXbeam_NS = 0, rmsPbeam_NS = 0;
      CutCloseToCore = 0, CutCloseToCore_NS = 0;
      CutSmallSignal = 0, CutSmallSignal_NS = 0;
      CutBadTiming = 0, CutBadTiming_NS = 0;
      CutSNR = 0, CutSNR_NS = 0;
      latMeanDist = 0, latMeanDist_NS = 0;
      latMeanDistCC = 0, latMeanDistCC_NS = 0;

      // Set observatory record to LOPES
      Record obsrec;
      obsrec.define("LOPES",config["caltablepath"]->sValue());

      // check for the polarizations, if both polarizations are required
      // then the pipeline has to be called twice (once for EW and once for NS);
      // in this case an additional plotprefix is used
      string polPlotPrefix = "";

      // Structure containg necessary structures to create event display plot [added: mfranc]
      Vector <int>    antIDs;                          // vector containing antennas IDs
      Matrix <double> antPos;                          // matrix containing antennas positions		 
      map<int, PulseProperties>::iterator itBeg;       // iterator.begin() of pulsesMap
      map<int, PulseProperties>::iterator itEnd;       // iterator.end() if pulsesMap

      if ( config["calibrationMode"]->bValue() ) {
        // initialize the pipeline
        analyseLOPESevent2 eventPipeline;
        eventPipeline.initPipeline(obsrec);

        // set parameters of pipeline
        eventPipeline.setPlotInterval(config["plotStart"]->dValue(),config["plotStop"]->dValue());
        eventPipeline.setSpectrumInterval(config["spectrumStart"]->dValue(),config["spectrumStop"]->dValue());
        eventPipeline.setFreqInterval(config["freqStart"]->dValue(),config["freqStop"]->dValue());
        eventPipeline.setUpsamplingExponent(config["upsamplingExponent"]->uiValue());

        // call the pipeline with an extra delay = 0.
        results = eventPipeline.CalibrationPipeline (config["path"]->sValue()+eventname,
                                                     plotprefix,
                                                     config["generatePlots"]->bValue(),
                                                     config["generateSpectra"]->bValue(),
                                                     static_cast< Vector<int> >(config.getFlagged()),
                                                     config["verbose"]->bValue(),
                                                     config["doGainCal"]->bValue(),
                                                     config["doDispersionCal"]->bValue(),
                                                     false,		// never correct delays in calibration mode
                                                     config["doRFImitigation"]->bValue(),
                                                     config["singlePlots"]->bValue(),
                                                     config["PlotRawData"]->bValue(),
                                                     config["CalculateMaxima"]->bValue());

        // make a postscript with a summary of all plots
        // if summaryColumns = 0 the method does not create a summary.
        eventPipeline.summaryPlot(plotprefix+"-summary",config["summaryColumns"]->uiValue());

        // get the pulse properties
        rawPulsesMap = eventPipeline.getRawPulseProperties();
        calibPulsesMap = eventPipeline.getCalibPulseProperties();

        // adding results to variables (needed to fill them into the root tree)
        goodEW = results.asBool("goodReconstructed");
        gt = results.asuInt("Date");

        // getting necessary data to plot [added: mfranc]
        if(config["eventDisplayPlot"]->bValue()) {
          antPos = eventPipeline.getAntennaPositions();
          antIDs = eventPipeline.getAntennaIDs();
        }
      } else {
        if ( (config["polarization"]->sValue() == "ANY") || (config["polarization"]->sValue() == "EW") || both_pol) {
          if (both_pol) {
            cout << "\nPipeline is started for East-West Polarization.\n" << endl;
            polPlotPrefix = "-EW";
            config["polarization"]->setValue("EW");	// do EW here
          }

          // initialize the pipeline
          analyseLOPESevent2 eventPipeline;
          eventPipeline.initPipeline(obsrec);

          // set parameters of pipeline
          eventPipeline.setMinBeamformingAntennasCut(config["minBeamformingAntennas"]->uiValue());
          eventPipeline.setPlotInterval(config["plotStart"]->dValue(),config["plotStop"]->dValue());
          eventPipeline.setSpectrumInterval(config["spectrumStart"]->dValue(),config["spectrumStop"]->dValue());
          eventPipeline.setFreqInterval(config["freqStart"]->dValue(),config["freqStop"]->dValue());
          eventPipeline.setCCWindowWidth(config["ccWindowWidth"]->dValue());
          eventPipeline.setUpsamplingExponent(config["upsamplingExponent"]->uiValue());
          eventPipeline.setLateralSNRcut(config["lateralSNRcut"]->dValue());
          eventPipeline.setLateralTimeCut(config["lateralTimeCut"]->dValue());

          // call the pipeline with an extra delay = 0.
          results = eventPipeline.RunPipeline (config["path"]->sValue()+eventname,
                                               azimuth,
                                               elevation,
                                               radiusOfCurvature,
                                               core_x,
                                               core_y,
                                               config["RotatePos"]->bValue(),
                                               plotprefix+polPlotPrefix,
                                               config["generatePlots"]->bValue(),
                                               config["generateSpectra"]->bValue(),
                                               static_cast< Vector<int> >(config.getFlagged()),
                                               config["verbose"]->bValue(),
                                               config["simplexFit"]->bValue(),
                                               0.,
                                               config["doTVcal"]->iValue(),
                                               config["doGainCal"]->bValue(),
                                               config["doDispersionCal"]->bValue(),
                                               config["doDelayCal"]->bValue(),
                                               config["doRFImitigation"]->bValue(),
                                               config["doFlagNotActiveAnts"]->bValue(),
                                               config["doAutoFlagging"]->bValue(),
                                               config["upsamplingRate"]->dValue(),
                                               config["polarization"]->sValue(),
                                               config["singlePlots"]->bValue(),
                                               config["PlotRawData"]->bValue(),
                                               config["CalculateMaxima"]->bValue(),
                                               config["listCalcMaxima"]->bValue(),
                                               config["printShowerCoordinates"]->bValue(),
                                               config["ignoreDistance"]->bValue());

          // make a postscript with a summary of all plots
          // if summaryColumns = 0 the method does not create a summary.
          eventPipeline.summaryPlot(plotprefix+polPlotPrefix+"-summary",config["summaryColumns"]->uiValue());

          // create a special file for the lateral distribution output
          if (config["lateralOutputFile"]->bValue())
            eventPipeline.createLateralOutput("lateral"+polPlotPrefix+"-",results, core_x, core_y);

          // generate the lateral distribution and get resutls
          if (config["lateralDistribution"]->bValue()) {
            eventPipeline.fitLateralDistribution("lateral"+polPlotPrefix+"-",results, core_x, core_y);
            R_0 = results.asDouble("R_0");
            sigR_0 = results.asDouble("sigR_0");
            eps = results.asDouble("eps");
            sigeps = results.asDouble("sigeps");
            chi2NDF = results.asDouble("chi2NDF");
            kPow = results.asDouble("kPow");
            sigkPow = results.asDouble("sigkPow");
            epsPow = results.asDouble("epsPow");
            sigepsPow = results.asDouble("sigepsPow");
            chi2NDFPow = results.asDouble("chi2NDFPow");
            CutCloseToCore = results.asInt("CutCloseToCore");
            CutSmallSignal = results.asInt("CutSmallSignal");
            CutBadTiming = results.asInt("CutBadTiming");
            CutSNR = results.asInt("CutSNR");
            latMeanDist = results.asDouble("latMeanDist");
          }

          // plot lateral distribution of arrival times, if requested
          if (config["lateralTimeDistribution"]->bValue())
            eventPipeline.lateralTimeDistribution("lateralTime"+polPlotPrefix+"-",results, core_x, core_y);

          // get the pulse properties
          rawPulsesMap = eventPipeline.getRawPulseProperties();
          calibPulsesMap = eventPipeline.getCalibPulseProperties();

          // adding results to variables (needed to fill them into the root tree)
          goodEW = results.asBool("goodReconstructed");
          AzL = results.asDouble("Azimuth");
          ElL = results.asDouble("Elevation");
          distanceResult = results.asDouble("Distance");
          CCheight = results.asDouble("CCheight");
          CCwidth = results.asDouble("CCwidth");
          CCheight_error = results.asDouble("CCheight_error");
          CCconverged = results.asBool("CCconverged");
          Xheight = results.asDouble("Xheight");
          Xheight_error = results.asDouble("Xheight_error");
          Xconverged = results.asBool("Xconverged");
          rmsCCbeam = results.asDouble("rmsCCbeam");
          rmsXbeam = results.asDouble("rmsXbeam");
          rmsPbeam = results.asDouble("rmsPbeam");
          latMeanDistCC = results.asDouble("meandist");
          gt = results.asuInt("Date");

          // getting necessary data to plot [added: mfranc]
          if(config["eventDisplayPlot"]->bValue()) {
            antPos = eventPipeline.getAntennaPositions();
            antIDs = eventPipeline.getAntennaIDs();
          }
        }

        if ( (config["polarization"]->sValue() == "NS") || both_pol) {
          if (both_pol) {
            cout << "\nPipeline is started for North-South Polarization.\n" << endl;
            polPlotPrefix = "-NS";
            config["polarization"]->setValue("NS");	// do NS here
          }

          // initialize the pipeline
          analyseLOPESevent2 eventPipeline;
          eventPipeline.initPipeline(obsrec);

          // set parameters of pipeline
          eventPipeline.setMinBeamformingAntennasCut(config["minBeamformingAntennas"]->uiValue());
          eventPipeline.setPlotInterval(config["plotStart"]->dValue(),config["plotStop"]->dValue());
          eventPipeline.setSpectrumInterval(config["spectrumStart"]->dValue(),config["spectrumStop"]->dValue());
          eventPipeline.setFreqInterval(config["freqStart"]->dValue(),config["freqStop"]->dValue());
          eventPipeline.setCCWindowWidth(config["ccWindowWidth"]->dValue());
          eventPipeline.setUpsamplingExponent(config["upsamplingExponent"]->uiValue());
          eventPipeline.setLateralSNRcut(config["lateralSNRcut"]->dValue());
          eventPipeline.setLateralTimeCut(config["lateralTimeCut"]->dValue());

          // call the pipeline with an extra delay = 0.
          results = eventPipeline.RunPipeline (config["path"]->sValue()+eventname,
                                               azimuth,
                                               elevation,
                                               radiusOfCurvature,
                                               core_x,
                                               core_y,
                                               config["RotatePos"]->bValue(),
                                               plotprefix+polPlotPrefix,
                                               config["generatePlots"]->bValue(),
                                               config["generateSpectra"]->bValue(),
                                               static_cast< Vector<int> >(config.getFlagged()),
                                               config["verbose"]->bValue(),
                                               config["simplexFit"]->bValue(),
                                               0.,
                                               config["doTVcal"]->iValue(),
                                               config["doGainCal"]->bValue(),
                                               config["doDispersionCal"]->bValue(),
                                               config["doDelayCal"]->bValue(),
                                               config["doRFImitigation"]->bValue(),
                                               config["doFlagNotActiveAnts"]->bValue(),
                                               config["doAutoFlagging"]->bValue(),
                                               config["upsamplingRate"]->dValue(),
                                               config["polarization"]->sValue(),
                                               config["singlePlots"]->bValue(),
                                               config["PlotRawData"]->bValue(),
                                               config["CalculateMaxima"]->bValue(),
                                               config["listCalcMaxima"]->bValue(),
                                               config["printShowerCoordinates"]->bValue(),
                                               config["ignoreDistance"]->bValue());

          /* make a postscript with a summary of all plots
           if summaryColumns = 0 the method does not create a summary. */
          eventPipeline.summaryPlot(plotprefix+polPlotPrefix+"-summary",config["summaryColumns"]->uiValue());

          // create a special file for the lateral distribution output
          if (config["lateralOutputFile"]->bValue())
            eventPipeline.createLateralOutput("lateral"+polPlotPrefix+"-",results, core_x, core_y);

          // generate the lateral distribution
          if (config["lateralDistribution"]->bValue()) {
            eventPipeline.fitLateralDistribution("lateral"+polPlotPrefix+"-",results, core_x, core_y);
            R_0_NS = results.asDouble("R_0");
            sigR_0_NS = results.asDouble("sigR_0");
            eps_NS = results.asDouble("eps");
            sigeps_NS = results.asDouble("sigeps");
            chi2NDF_NS = results.asDouble("chi2NDF");
            kPow_NS = results.asDouble("kPow");
            sigkPow_NS = results.asDouble("sigkPow");
            epsPow_NS = results.asDouble("epsPow");
            sigepsPow_NS = results.asDouble("sigepsPow");
            chi2NDFPow_NS = results.asDouble("chi2NDFPow");
            CutCloseToCore_NS = results.asInt("CutCloseToCore");
            CutSmallSignal_NS = results.asInt("CutSmallSignal");
            CutBadTiming_NS = results.asInt("CutBadTiming");
            CutSNR_NS = results.asInt("CutSNR");
            latMeanDist_NS = results.asDouble("meandist");
          }

          // plot lateral distribution of arrival times, if requested
          if (config["lateralTimeDistribution"]->bValue())
            eventPipeline.lateralTimeDistribution("lateralTime"+polPlotPrefix+"-",results, core_x, core_y);

          // get the pulse properties and insert them into allready existing EW map
          // (which will be empty, if polarization = EW, but still existing
          map <int,PulseProperties> newPulses(eventPipeline.getRawPulseProperties());
          rawPulsesMap.insert(newPulses.begin(), newPulses.end()) ;
          newPulses = eventPipeline.getCalibPulseProperties();
          calibPulsesMap.insert(newPulses.begin(), newPulses.end()) ;

          // adding results to variables (needed to fill them into the root tree)
          goodNS = results.asBool("goodReconstructed");
          AzL_NS = results.asDouble("Azimuth");
          ElL_NS = results.asDouble("Elevation");
          distanceResultNS = results.asDouble("Distance");
          CCheight_NS = results.asDouble("CCheight");
          CCwidth_NS = results.asDouble("CCwidth");
          CCheight_error_NS = results.asDouble("CCheight_error");
          CCconvergedNS = results.asBool("CCconverged");
          Xheight_NS = results.asDouble("Xheight");
          Xheight_error_NS = results.asDouble("Xheight_error");
          XconvergedNS = results.asBool("Xconverged");
          rmsCCbeam_NS = results.asDouble("rmsCCbeam");
          rmsXbeam_NS = results.asDouble("rmsXbeam");
          rmsPbeam_NS = results.asDouble("rmsPbeam");
          latMeanDistCC_NS = results.asDouble("latMeanDistCC");
          gt = results.asuInt("Date");

          // getting necessary data to plot [added: mfranc]
          if(config["eventDisplayPlot"]->bValue()) {
            antPos = eventPipeline.getAntennaPositions();
            antIDs = eventPipeline.getAntennaIDs();		
          }
        }
      }  // if...else (config["calibrationMode"]->bValue())


      // Create event display plot of all antennas [added: mfranc]
      if(config["eventDisplayPlot"]->bValue()) {
        if(calibPulsesMap.size() != 0) {
          itBeg = calibPulsesMap.begin();	
          itEnd = calibPulsesMap.end();
          cout << "Event display plot of all antennas will be created using calibrated pulses information." << endl;

          AntennasDisplay ad;
          vector<double> allAntennasPosX;
          vector<double> allAntennasPosY;
          vector<int>    antIDsSTL;
          antIDs.tovector(antIDsSTL);

          for (unsigned int i=0 ; i < antIDs.size(); i++) {
            allAntennasPosX.push_back(antPos.row(i)(0));
            allAntennasPosY.push_back(antPos.row(i)(1));
          }
          ad.setAllAntennas(allAntennasPosY, allAntennasPosX); // Attention: the position are reversed !!!

          ad.setCore(core_x, core_y, azimuth, elevation);	// TODO: Check core and direction (probably wrong)

          for(map<int, PulseProperties>::iterator it = itBeg; it != itEnd; ++it) {
            int                   id       = it->second.antennaID;
            vector<int>::iterator loc      = std::find(antIDsSTL.begin(), antIDsSTL.end(), id);

            if(loc != antIDsSTL.end()) {
              double xPos  = antPos.row(loc - antIDsSTL.begin())(0);
              double yPos  = antPos.row(loc - antIDsSTL.begin())(1);
              double magn  = it->second.envelopeMaximum;
              double time  = it->second.geomDelay + it->second.envelopeTime;	// TODO: check time information
              int    polar = it->second.polarization.compare("NS") ? (int)ad.NS : (int)ad.EW;
              ad.addNewAntenna(yPos, xPos, magn, time, polar); // Attention: the position are reversed !!!
            }
          }
          size_t found = eventname.rfind(".event");
          string modifiedEventname = (found != string::npos) ? eventname.erase(found, eventname.length()) : eventname;
          ad.createPlot(modifiedEventname+ "_" + "map", "Event: " + modifiedEventname);
        } else {
          cerr << "ERROR: Event display plot cannot be created because no calibrated pulse information has been found." << endl;
        }
      }

      // process pulse properties
      // delete arrays
      for (int i=0; i < MAX_NUM_ANTENNAS; i++) {
        *rawPulses[i] = PulseProperties();
        *calibPulses[i] = PulseProperties();
      }

      // fill information in array for raw pulses
      for ( map<int,PulseProperties>::iterator it=rawPulsesMap.begin() ; it != rawPulsesMap.end(); ++it) {
        // check if antenna number lies in valid range
        if ( (it->second.antenna < 1) || (it->second.antenna >= MAX_NUM_ANTENNAS) ) {
          cerr << "\nWARNING: Antenna number in rawPulsesMap is out of range!" << endl;
        } else {
          *rawPulses[it->second.antenna-1] = it->second;
          // create branch name
          stringstream antNumber(""); 
          antNumber << it->second.antenna;
          string branchname = "Ant_" + antNumber.str() + "_raw.";
          // check if branch allready exists and if not, create it
          if (! roottree->GetBranchStatus(branchname.c_str()))
            roottree->Branch(branchname.c_str(),"PulseProperties",&rawPulses[it->second.antenna-1]);
        }
      }


      // fill information in array for calibrated pulses
      for ( map<int,PulseProperties>::iterator it=calibPulsesMap.begin() ; it != calibPulsesMap.end(); ++it) {
        // check if antenna number lies in valid range
        if ( (it->second.antenna < 1) || (it->second.antenna >= MAX_NUM_ANTENNAS) ) {
          cerr << "\nWARNING: Antenna number in calibPulsesMap is out of range!" << endl;
        } else {
          int antenna = it->second.antenna;
          *calibPulses[antenna-1] = it->second;

          // calculate mean: if no mean exists so far, replace it by the value itself
          if (meanCalCounter[antenna-1] == 0) {
            ++meanCalCounter[antenna-1];
            meanCalPulses[antenna-1]->dist = it->second.dist;
          } else {
            ++meanCalCounter[antenna-1];
            meanCalPulses[antenna-1]->dist = 
              ((meanCalCounter[antenna-1] - 1) * meanCalPulses[antenna-1]->dist + it->second.dist) / meanCalCounter[antenna-1];
          }
          // calcutlate mean of lateral deviations (different counter, because of different antenna cut criteria
          if (!(it->second.lateralCut) && (config["lateralDistribution"]->bValue())) {
            if (meanResCounter[antenna-1] == 0) {
              ++meanResCounter[antenna-1];
              meanCalPulses[antenna-1]->lateralExpHeight = it->second.lateralExpHeight;
              meanCalPulses[antenna-1]->lateralPowHeight = it->second.lateralPowHeight;
              meanCalPulses[antenna-1]->lateralExpDeviation = it->second.lateralExpDeviation;
              meanCalPulses[antenna-1]->lateralPowDeviation = it->second.lateralPowDeviation;
            } else {
              ++meanResCounter[antenna-1];
              meanCalPulses[antenna-1]->lateralExpHeight = 
                ((meanResCounter[antenna-1] - 1) * meanCalPulses[antenna-1]->lateralExpHeight + it->second.lateralExpHeight)
                / meanResCounter[antenna-1];
              meanCalPulses[antenna-1]->lateralPowHeight = 
                ((meanResCounter[antenna-1] - 1) * meanCalPulses[antenna-1]->lateralPowHeight + it->second.lateralPowHeight)
                / meanResCounter[antenna-1];
              meanCalPulses[antenna-1]->lateralExpDeviation = 
                ((meanResCounter[antenna-1] - 1) * meanCalPulses[antenna-1]->lateralExpDeviation + it->second.lateralExpDeviation)
                / meanResCounter[antenna-1];
              meanCalPulses[antenna-1]->lateralPowDeviation = 
                ((meanResCounter[antenna-1] - 1) * meanCalPulses[antenna-1]->lateralPowDeviation + it->second.lateralPowDeviation)
                / meanResCounter[antenna-1];
            }
          }

          // create branch names
          stringstream antNumber(""); 
          antNumber << antenna;
          string branchname = "Ant_" + antNumber.str() + "_cal.";
          // check if branch allready exists and if not, create it
          if (! roottree->GetBranchStatus(branchname.c_str())) {
            roottree->Branch(branchname.c_str(),"PulseProperties",&calibPulses[antenna-1]);
          string branchname = "Ant_" + antNumber.str() + "_cal_mean.";
          if ((config["calculateMeanValues"]->bValue()) && (! roottree->GetBranchStatus(branchname.c_str())))
            roottree->Branch(branchname.c_str(),"PulseProperties",&meanCalPulses[antenna-1]);
          }
        }
      }

      // write information of last event to root file
      if (config["rootFileName"]->sValue() != "") {
        // check if event was reconstructed or if also bad events shall be written to the root file
        if (goodEW || goodNS || config["writeBadEvents"]->bValue()) {
          cout << "Adding results to root tree and saving root file \"" << config["rootFileName"]->sValue() << "\"\n" << endl;
          roottree->Fill();
          rootfile->Write("",TObject::kOverwrite);
        } else {
          cout << "WARNING: Event is not written into root file because it is marked as badly reconstructed." << endl;
        }
      }


    }

    // print mean values of lateral distribution
    if ((config["calculateMeanValues"]->bValue()) && (config["lateralDistribution"]->bValue())) {
      cout << "\n\nMean values of lateral distribution of all events:\n"
           << "antenna  distance  exp. residual  pow. residual\n";
      for (unsigned int i=0; i < MAX_NUM_ANTENNAS; ++i)
        cout << i+1 << " \t    "
             << meanCalPulses[i]->dist << "\t     "
             << meanCalPulses[i]->lateralExpDeviation << "\t     "
             << meanCalPulses[i]->lateralPowDeviation << "\n";
    }

    // close root file
    if (config["rootFileName"]->sValue() != "") {
      cout << "\nClosing root file: " << config["rootFileName"]->sValue() << endl;
      rootfile->Close();
    }

    // free allocated memory
    for (int i=0; i < MAX_NUM_ANTENNAS; i++) {
      delete rawPulses[i];
      delete calibPulses[i];
      delete meanCalPulses[i];
      delete meanRawPulses[i];
    }

    cout << "\nPipeline finished successfully.\n" << endl;

  } catch (AipsError x) {
    cerr << "call_pipeline: " << x.getMesg() << endl;
  }

  return 0;
}

