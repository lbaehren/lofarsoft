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

#include <Analysis/CompletePipeline.h>

namespace CR { // Namespace CR -- begin

  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  CompletePipeline::CompletePipeline():
    plotStart_p(-2.05e-6), plotStop_p(-1.55e-6),
    plotlist(),
    lastUpsamplingExponent(-1),
    lastTimeUpsamplingExponent(-1),
    upFieldStrength(),
    upsampledAntennas(),
    upTimeValues()
  {;}
  
  CompletePipeline::CompletePipeline (CompletePipeline const &other):
    plotStart_p(-2.05e-6), plotStop_p(-1.55e-6),
    plotlist(),
    lastUpsamplingExponent(-1),
    lastTimeUpsamplingExponent(-1),
    upFieldStrength(),
    upsampledAntennas(),
    upTimeValues()
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  CompletePipeline::~CompletePipeline ()
  {
    destroy();
  }
  
  void CompletePipeline::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  CompletePipeline& CompletePipeline::operator= (CompletePipeline const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void CompletePipeline::copy (CompletePipeline const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void CompletePipeline::summary (std::ostream &os)
  {;}
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Matrix<Double> CompletePipeline::getUpsampledFieldstrength (DataReader *dr,
							      const int& upsampling_exp,
							      Vector<Bool> antennaSelection)
  {
    try 
    {
      // check if upsampling shoud be done at all (if not, return not upsampled data)
      if (upsampling_exp < 1) return GetTimeSeries(dr).copy(); 

      // Get the antenna selection from the DataReader if no selction was chosen 	
      if (antennaSelection.nelements() == 0) {
	antennaSelection = GetAntennaMask(dr);
      }

      // check if upsampling is done for the first time
      if (lastUpsamplingExponent == -1)
      {
        //create vector for flags showing which antenna is allready upsampled
        upsampledAntennas.assign(GetAntennaMask(dr).nelements(),false);	// at the moment there are no upsampled antennas
      } 

      // consistency check: number of elements in antennaSelction and upsampledAntennas must be equal
      if ( upsampledAntennas.size() != antennaSelection.nelements() )
      {
        std::cerr << "CompletePipeline:getUpsampledFieldstrength: Number of elements in \"antennaSelection\" is inconsistent.\n" 
		<< std::endl;
        return upFieldStrength.copy();
      }

      // check if there are allready upsampled values and create todo-list
      vector<bool> upsamplingToDo;
      upsamplingToDo.assign(upsampledAntennas.size(),true);		// form Vector of correct size
      // maximum to do is the whole antenna selction
      for (int i=0; i < upsamplingToDo.size(); i++) upsamplingToDo[i] = antennaSelection(i);

      // if the last upsampling exponent is the same es the current, there might be somthing done allready
      // if not, clear the upsampled flags
      if (lastUpsamplingExponent != upsampling_exp) upsampledAntennas.assign(upsampledAntennas.size(),false);
       else for (int i=0; i < upsamplingToDo.size(); i++)
         if (upsampledAntennas[i]) upsamplingToDo[i] = false;

      // Get the (not yet upsampled) fieldstrength of all antennas
      Matrix<Double> fieldstrength = GetTimeSeries(dr);  // don't pass antennaSelection to get full number of columns in the Matrix
      // create upsampling factor by upsampling exponent
      unsigned int upsampled = pow(2,upsampling_exp);

      // get length of trace
      int tracelength = fieldstrength.column(0).size();
      // allocate memory for original and upsampled traces
      float originalTrace[tracelength];
      float upsampledTrace[tracelength * upsampled];

      // resize Matrix for upsampled traces if the last upsampling exponent is different from the new one
      if (lastUpsamplingExponent != upsampling_exp)
      {
        upFieldStrength.resize(tracelength * upsampled, antennaSelection.nelements(), false);	// no need to copy old values
        // set all traces initially to zero (not neccessary but makes the detection of errors easier)
        upFieldStrength.set(0);	
      } 

      // do upsampling for each antenna in the todo-list
      std::cout << "Upsampling the calibrated data by a factor of " << upsampled << " ...\nAntenna:";
      for (int i = 0; i < antennaSelection.nelements(); i++) {
        if (upsamplingToDo[i]){
          std::cout << " " << i+1;
 	  // copy the trace into the array
	  for (int j = 0; j < tracelength; j++) 
          {
            originalTrace[j] = fieldstrength.column(i)(j);
          }

	  // do upsampling by factor #upsampled (--> NoZeros = upsampled -1)

          // calcutlate Offset:
          float before = originalTrace[0];

          ZeroPaddingFFT(tracelength, originalTrace, upsampled-1, upsampledTrace);
 
	  double offset = before - originalTrace[0];
	
  	  // copy upsampled trace into Matrix with antenna traces and subtract offset
          // remark: as there is no offset in the original data, this step should be avoided
          // as soon as upsampling without pedestal correction is available
	  for (int j = 0; j < tracelength*upsampled; j++) upFieldStrength.column(i)(j) = upsampledTrace[j] + offset;

          // remark: tried to fasten data transfer from the array to the Matrix but did not work because arrays are
          // of typ float but not double.

          // set flag, that data for this antenna are upsampled
          upsampledAntennas[i] = true;
        } 
      } 
      std::cout << " ... done\n";
      // set last upsampling exponent to current value
      lastUpsamplingExponent = upsampling_exp;

      // Return upsampled traces 
      return upFieldStrength.copy();
    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:getUpsampledFieldstrength: " << x.getMesg() << std::endl;
      }; 
  }

 
  Matrix<Double> CompletePipeline::getUpsampledFX (DataReader *dr,
						   const int& upsampling_exp,
						   Vector<Bool> antennaSelection,
						   const bool& offsetSubstraction)
  {
    try 
    {
      // Get the (not yet upsampled) raw data of all antennas
      Matrix<Double> rawData = dr->fx(); 

      // check if upsampling shoud be done at all (if not, return not upsampled data)
      if (upsampling_exp < 1) return rawData.copy(); 

      // Get the antenna selection from the DataReader if no selction was chosen 	
      if (antennaSelection.nelements() == 0) {
	antennaSelection = GetAntennaMask(dr);
      }

      // consistency check: number of elements in antennaSelction and rawData must be equal
      if ( rawData.ncolumn() != antennaSelection.nelements() )
      {
        std::cerr << "CompletePipeline:getUpsampledFX: Number of elements in \"antennaSelection\" is inconsistent.\n" 
		<< std::endl;
        return rawData.copy();
      }

      // create upsampling factor by upsampling exponent
      unsigned int upsampled = pow(2,upsampling_exp);

      // get length of trace
      int tracelength = rawData.column(0).size();

      // allocate memory for original and upsampled traces
      float originalTrace[tracelength];
      float upsampledTrace[tracelength * upsampled];

      // Create Matrix for usampled values
      Matrix<Double> upData(tracelength * upsampled, antennaSelection.nelements(), 0);

      // do upsampling for each antenna in the antennaSelection
      for (int i = 0; i < antennaSelection.nelements(); i++) if (antennaSelection(i))
      {
        std::cout << "Upsampling the raw data of antenna " << i+1 << " by a factor of " << upsampled << " ...\n";
	// copy the trace into the array
	for (int j = 0; j < tracelength; j++) 
        {
          originalTrace[j] = rawData.column(i)(j);
        }

	// do upsampling by factor #upsampled (--> NoZeros = upsampled -1)

        // calcutlate Offset:
        float before = originalTrace[0];

        ZeroPaddingFFT(tracelength, originalTrace, upsampled-1, upsampledTrace);

	double offset = before - originalTrace[0];
	
	// copy upsampled trace into Matrix with antenna traces and subtract offset
        // if no offset correction is wanted
        // remember: ZeroPaddingFFT removes the offset automatically
        if (offsetSubstraction)
	  for (int j = 0; j < tracelength*upsampled; j++) upData.column(i)(j) = upsampledTrace[j];
        else
	  for (int j = 0; j < tracelength*upsampled; j++) upData.column(i)(j) = upsampledTrace[j] + offset;

        // remark: tried to fasten data transfer from the array to the Matrix but did not work because arrays are
        // of typ float but not double.
      } 

      // Return upsampled traces 
      return upData.copy();

    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:getUpsampledFX: " << x.getMesg() << std::endl;
      }; 
  }



  Vector<Double> CompletePipeline::getUpsampledTimeAxis (DataReader *dr,
							 const int& upsampling_exp)
  {
    try 
    {
      // Check if upsampling shoud be done at all (if not, return not upsampled time axis)
      if (upsampling_exp < 1) return static_cast< Vector<Double> >(dr->timeValues());

      // Check if upampling should be done be the same factor as last time
      // in this case, return the old values
      if (lastTimeUpsamplingExponent == upsampling_exp)
        return upTimeValues.copy();

      // Get the not interpolated time axis
      Vector<Double> timeaxis = static_cast< Vector<Double> >(dr->timeValues()); 

      // create upsampling factor by upsampling exponent
      unsigned int upsampled = pow(2,upsampling_exp);

      // Get time between samples
      double sampleTime = 1/(dr->sampleFrequency() * upsampled);

      // get the length of the x-axis 
      long int time_length = timeaxis.size();

      // Resize the time Vector for the new upsampled xaxis
      upTimeValues.resize(time_length*upsampled, false);

      // copy old values to the right place and fill space inbetween 
      for (int i = time_length-1; i >= 0; i--)
      {
        // move existing time value to the right place
        upTimeValues(i*upsampled) = timeaxis(i);
        // create new values
        for (int j = 1; j < upsampled; j++) 
          upTimeValues(i*upsampled+j) = upTimeValues(i*upsampled) + j*sampleTime;
      }

      // set last upsampling exponent to current value
      lastTimeUpsamplingExponent = upsampling_exp;

      // Return upsampled xaxis
      return upTimeValues.copy();
    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:getUpsampledTimeAxis: " << x.getMesg() << std::endl;
      }; 
  }


  Slice CompletePipeline::calculatePlotRange (const Vector<Double>& xaxis) const
  {
    try
    {
      // check if plotStart is <= plotStop
      if (plotStop_p < plotStart_p)
      {
        std::cerr << "CompletePipeline:calculatePlotRange: Error: plotStop_p is greater than plotStart_p!" << std::endl;
        return Slice(0,0);
      }
      int startsample = ntrue(xaxis<plotStart_p);            //number of elements smaller then starting value of plot range
      int stopsample = ntrue(xaxis<plotStop_p);              //number of elements smaller then end of plot range
      Slice plotRange(startsample,(stopsample-startsample)); // create Slice with plotRange
      return plotRange;
    } catch (AipsError x) 
    {
        std::cerr << "CompletePipeline:calculatePlotRange: " << x.getMesg() << std::endl;
    }; 
  }


  void CompletePipeline::plotCCbeam(const string& filename,
				    DataReader *dr,
				    Vector<Bool> antennaSelection,
				    const double& ccBeamOffset,
				    const double& pBeamOffset)
  {
    try 
    {
      SimplePlot plotter;    			// define plotter
      Vector<Double> xaxis, ccbeam, pbeam;	// values for plot
      double xmax,xmin,ymin,ymax;		// Plotrange
 
      // add the ".ps" to the filename
      string plotfilename = filename + ".ps";
    
      std::cout <<"Plotting the CC-beam and the Power-beam to file: " << plotfilename << std::endl;

      // Get the time axis
      xaxis = (Vector<Double>)(dr->timeValues()); 

      // Define plotrange
      Slice plotRange = calculatePlotRange(xaxis); 

      // Get the CC-beam and the power-beam
      ccbeam = GetCCBeam(dr, antennaSelection).copy();
      pbeam = GetPBeam(dr, antennaSelection).copy();

      // smooth the data and substract Offset
      StatisticsFilter<Double> mf(3,FilterType::MEAN);
      ccbeam = mf.filter(ccbeam) - ccBeamOffset;
      pbeam = mf.filter(pbeam) - pBeamOffset;

      // conversion to micro
      xaxis *= 1e6;
      ccbeam *= 1e6;
      pbeam *= 1e6;
    
      // define Plotrange
      xmin = min(xaxis(plotRange));
      xmax = max(xaxis(plotRange));
      ymin = min(min(ccbeam(plotRange)), min(pbeam(plotRange)))*1.05;
      ymax = max(max(ccbeam(plotRange)), max(pbeam(plotRange)))*1.05;

      // Initialize the plots giving xmin, xmax, ymin and ymax
      plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);

      // Create the Plot
      plotter.AddLabels("Time t [#gmsec]", "CC-Beam [#gmV/m/MHz]","CC-Beam and Power");
    
      // Add CC-beam
      plotter.PlotLine(xaxis(plotRange),ccbeam(plotRange),4,1);
      // Add Power-beam
      plotter.PlotLine(xaxis(plotRange),pbeam(plotRange),5,1);

      // Add filename to list of created plots
      plotlist.push_back(plotfilename);

    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:plotCCBeam: " << x.getMesg() << std::endl;
      }; 
  }


  void CompletePipeline::plotXbeam(const string& filename,
				   DataReader *dr,
				   Vector<Bool> antennaSelection,
				   const double& xBeamOffset,
				   const double& pBeamOffset)
  {
    try 
    {
      SimplePlot plotter;    			// define plotter
      Vector<Double> xaxis, xbeam, pbeam;	// values for plot
      double xmax,xmin,ymin,ymax;		// Plotrange
 
      // add the ".ps" to the filename
      string plotfilename = filename + ".ps";
    
      std::cout <<"Plotting the X-beam and the Power-beam to file: " << plotfilename << std::endl;

      // Get the time axis
      xaxis = (Vector<Double>)(dr->timeValues()); 

      // Define plotrange
      Slice plotRange = calculatePlotRange(xaxis); 

      // Get the X-beam and the power-beam
      xbeam = GetXBeam(dr, antennaSelection).copy();
      pbeam = GetPBeam(dr, antennaSelection).copy();

      // smooth the data
      StatisticsFilter<Double> mf(3,FilterType::MEAN);
      xbeam = mf.filter(xbeam) - xBeamOffset;
      pbeam = mf.filter(pbeam) - pBeamOffset;

      // conversion to micro
      xaxis *= 1e6;
      xbeam *= 1e6;
      pbeam *= 1e6;
    
      // define Plotrange
      xmin = min(xaxis(plotRange));
      xmax = max(xaxis(plotRange));
      ymin = min(min(xbeam(plotRange)), min(pbeam(plotRange)))*1.05;
      ymax = max(max(xbeam(plotRange)), max(pbeam(plotRange)))*1.05;

      // Initialize the plots giving xmin, xmax, ymin and ymax
      plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);

      // Create the Plot
      plotter.AddLabels("Time t [#gmsec]", "X-Beam [#gmV/m/MHz]","X-Beam and Power");
    
      // Add X-beam
      plotter.PlotLine(xaxis(plotRange),xbeam(plotRange),4,1);
      // Add Power-beam
      plotter.PlotLine(xaxis(plotRange),pbeam(plotRange),5,1);

      // Add filename to list of created plots
      plotlist.push_back(plotfilename);

    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:plotXBeam: " << x.getMesg() << std::endl;
      }; 
  }


  // Plot the timeshifted (= after beamforming) filedstrength of all antennas
  void CompletePipeline::plotAllAntennas(const string& filename,
					 DataReader *dr,
					 Vector<Bool> antennaSelection,
					 const bool& seperated,
					 const int& upsampling_exp,
					 const bool& rawData)
  {
    try 
    {
      SimplePlot plotter;    			// define plotter
      Vector<Double> xaxis;			// xaxis
      double xmax,xmin,ymin=0,ymax=0;		// Plotrange
      Matrix<Double> yValues;			// y-values
      Matrix<Double> upYvalues;			// upsampled y-values
      int color = 3;				// starting color

      // Get the antenna selection from the DataReader if no selction was chosen 	
      if (antennaSelection.nelements() == 0) {
	antennaSelection = GetAntennaMask(dr);
      }

      // Get the (not upsampled) time axis
      xaxis = static_cast< Vector<Double> >(dr->timeValues());
      
      // Get the yValues of all antennas (raw data or fieldstrength)
      if (rawData)
      {
        // get not upsampled data
        yValues = dr->fx().copy();
        // Upsampled yValues (ADC offset will not be substracted)
        upYvalues = getUpsampledFX(dr,upsampling_exp, antennaSelection, false);
      }
      else
      {
        yValues = GetTimeSeries(dr).copy();  // don't use the antennaSelection to get the full number of columns in the Matrix

        // Upsampled yValues
        upYvalues = getUpsampledFieldstrength(dr,upsampling_exp, antennaSelection);
      }
    
      // Upsampled x-axis
      Vector<Double> upxaxis = getUpsampledTimeAxis(dr,upsampling_exp);

      // check length of time axis and yValues traces for consistency
      if (upxaxis.size() != upYvalues.column(0).size())
        std::cerr << "CompletePipeline:plotAllAntennas: WARNING: Length of time axis differs from length of the antenna traces!\n"
           << std::endl;


      // Define plotrange for not upsampled and upsampled data
      Slice plotRange = calculatePlotRange(xaxis);
      Slice upplotRange = calculatePlotRange(upxaxis);

      // conversion to micro (no conversion for height of raw data)
      xaxis *= 1e6;
      if (!rawData) yValues *= 1e6;
      upxaxis *= 1e6;
      if (!rawData) upYvalues *= 1e6;  

      // define Plotrange
      xmin = min(xaxis(plotRange));
      xmax = max(xaxis(plotRange));

      // find the minimal and maximal y values for the plot
      // do it with the upsampled data only as they are as least as heigh as the original ones
      for (int i = 0; i < antennaSelection.nelements(); i++)
        if (antennaSelection(i))		// consider only selected antennas
        {
          if ( ymin > min(upYvalues.column(i)(upplotRange)) ) {
	    ymin = min(upYvalues.column(i)(upplotRange));
	  }
          if ( ymax < max(upYvalues.column(i)(upplotRange)) ) {
	    ymax = max(upYvalues.column(i)(upplotRange));
	  }
        }

      // multiply ymin and ymax by 105% to have some space at the bottom and the top of the plot
      ymin *= 1.05;
      ymax *= 1.05;

      // Make the plots (either all antennas together or seperated)
      if (seperated)
      {
        string plotfilename;
        string label;

        // Create empty vector for not existing error bars 
        Vector<Double> empty;

        // Get the AntennaIDs for labeling
        Vector<Int> AntennaIDs;
        dr->header().get("AntennaIDs",AntennaIDs);

        // Create the plots for each individual antenna looping through antennas
        if (rawData)
            std::cout <<"Plotting the raw data FX\nAntenna ... ";
        else
	    std::cout <<"Plotting the fieldstrength\n Antenna ..." ;


        for (int i = 0; i < antennaSelection.nelements(); i++){
          // consider only selected antennas
          if (antennaSelection(i)){
            // create filename and label
            stringstream antennanumber, antennaid;
            antennanumber << (i+1);
            antennaid << AntennaIDs(i);

            //plotfilename = filename + "-" + antennanumber.str() + ".ps";
            plotfilename = filename + "-" + antennanumber.str() + ".ps";
	    //            label = "Antenna " + antennanumber.str() + " (ID = " + antennaid.str() + ")";
            //  alternative label "GT - Ant.Nr"
            label = "GT " + filename + " - Antenna " + antennanumber.str();

            // Initialize the plot giving xmin, xmax, ymin and ymax
            plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);

            // Add labels
            if (rawData)
              plotter.AddLabels("Time t [#gmsec]", "Counts",label);
            else
              plotter.AddLabels("Time t [#gmsec]", "Fieldstrength #ge#d0#u [#gmV/m/MHz]",label);
  
            // Plot (upsampled) trace and original data points.
            plotter.PlotLine(upxaxis(upplotRange),upYvalues.column(i)(upplotRange),color,1);
            plotter.PlotSymbols(xaxis(plotRange),yValues.column(i)(plotRange),empty, empty, color, 2, 5);

            // Add filename to list of created plots
            plotlist.push_back(plotfilename);
  	    std::cout << " " << (i+1);
            color++;					// another color for the next antenna
            if (color >= 13) color = 3;			// there are only 16 colors available, 
							// use only ten as there are 3x10 antenna
          }
	}
        std::cout << std::endl;
      // if (seperated) else
      }else {
        // add the ".ps" to the filename
        string plotfilename = filename + ".ps";

        if (rawData)
    	  std::cout <<"Plotting the raw data FX of all antennas to file: "
	            << plotfilename << std::endl;
        else
    	  std::cout <<"Plotting the fieldstrength of all antennas to file: "
	            << plotfilename << std::endl;

        // Initialize the plot giving xmin, xmax, ymin and ymax
        plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);
        // Add labels 
        string label;
        stringstream antennanum;
        antennanum << (antennaSelection.nelements()-1);
        label = "GT " + filename + " - " + antennanum.str() + " Antennas";

        if (rawData)
          plotter.AddLabels("Time t [#gmsec]", "Counts",label);
        else
          plotter.AddLabels("Time t [#gmsec]", "Fieldstrength #ge#d0#u [#gmV/m/MHz]",label);

        // Create the plots looping through antennas
        for (int i = 0; i < antennaSelection.nelements(); i++)
        if (antennaSelection(i))			// consider only selected antennas
        {
          // Plot (upsampled) trace
          plotter.PlotLine(upxaxis(upplotRange),upYvalues.column(i)(upplotRange),color,1);

          color++;					// another color for the next antenna
          if (color >= 13) color = 3;			// there are only 16 colors available, 
							// use only ten as there are 3x10 antennas
        }

        // Add filename to list of created plots
        plotlist.push_back(plotfilename);
      }

    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:plotAllAntennas: " << x.getMesg() << std::endl;
      }; 
  }

  
  void CompletePipeline::calculateMaxima(DataReader *dr,
					 Vector<Bool> antennaSelection,
					 const int& upsampling_exp,
					 const bool& rawData)
  {
    try 
    {
      Vector<Double> timeValues;		// time values
      Vector<Double> timeRange;			// time values
      Matrix<Double> yValues;			// y-values
      Vector<Double> trace;			// trace currently processed
      vector<double> maxima;			// Stores the calculated maxima
      vector<double> minima;			// Stores the calculated minima
      vector<double> maxima_time;		// Stores the calculated time of the maxima
      vector<double> minima_time;		// Stores the calculated time of the minima

      if (rawData)
        std::cout << "\nLooking for maxima in the raw data FX: \n";
      else
        std::cout << "\nLooking for maxima in the calibrated fieldstrength: \n";
  
      // Get the antenna selection from the DataReader if no selction was chosen 	
      if (antennaSelection.nelements() == 0) {
	antennaSelection = GetAntennaMask(dr);
      }

 
      // Get the (upsampled) time axis
      timeValues = getUpsampledTimeAxis(dr,upsampling_exp);

      // Get the yValues of all selected antennas (raw data or fieldstrength)
      if (rawData)
        yValues = getUpsampledFX(dr,upsampling_exp, antennaSelection, true);  // true means: offset will be substracted
      else
        yValues = getUpsampledFieldstrength(dr,upsampling_exp, antennaSelection);

      // check length of time axis and yValues traces for consistency
      if (timeValues.size() != yValues.column(0).size())
        std::cerr << "CompletePipeline:calculateMaxima: WARNING: Length of time axis differs from length of the antenna traces!\n"
           << std::endl;


      // Define the time range considered (the same as the plot range)
      Slice range = calculatePlotRange(timeValues);

      // cut time values
      timeRange = timeValues(range);

      // find the maximal y values for all selected antennas
      for (int i = 0; i < antennaSelection.nelements(); i++) if (antennaSelection(i))
      {
        // Start with height 0 and search for heigher and lower values
        double maximum = 0;
	double minimum = 0;
        int maxtimevalue = 0;
        int mintimevalue = 0;

        // get current trace
        trace = yValues.column(i)(range);

        // loop through the values and search for the heighest and lowest one
        for(int j = 0; j < timeRange.nelements(); j++)
	{
          if ( maximum < trace(j)) 
          {
            maxtimevalue = j;
            maximum = trace(j);
          } 
          if ( minimum > trace(j)) 
          {
            mintimevalue = j;
            minimum = trace(j);
          } 
	}  

        // store the calculated values for later calculation of the mean
	// multiply by 1e6 for conversion to micro
        maxima.push_back(maximum*1e6);
        maxima_time.push_back(timeRange(maxtimevalue)*1e6);
        minima.push_back(minimum*1e6);
        minima_time.push_back(timeRange(mintimevalue)*1e6);


        // print the calculated values
        std::cout << "Antenna " << i+1 << ": \t maximum[micro] = " << maximum*1e6 
                  << "\t at time[micro s] = " << timeRange(maxtimevalue)*1e6 << "\n";
        std::cout << "Antenna " << i+1 << ": \t minimum[micro] = " << minimum*1e6 
                  << "\t at time[micro s] = " << timeRange(mintimevalue)*1e6 << "\n";
      }

      // calculate the averages and the range if there is more than one value
      if (maxima.size() > 1)
      {
        std::cout << "Summary for the maxima:\n" 
                  << "Amplitude range [micro]:   " << min(static_cast< Vector<Double> >(maxima)) 
                  << " to " << max(static_cast< Vector<Double> >(maxima)) << "\n"
                  << "Amplitude average [micro]: " << mean(static_cast< Vector<Double> >(maxima)) << "\n"
                  << "Time range [micro s]:      " << min(static_cast< Vector<Double> >(maxima_time))
                  << " to " << max(static_cast< Vector<Double> >(maxima_time)) << "\n"
                  << "Time average [micro s]:    " << mean(static_cast< Vector<Double> >(maxima_time)) << std::endl;
        std::cout << "Summary for the minima:\n" 
                  << "Amplitude range [micro]:   " << min(static_cast< Vector<Double> >(minima)) 
                  << " to " << max(static_cast< Vector<Double> >(minima)) << "\n"
                  << "Amplitude average [micro]: " << mean(static_cast< Vector<Double> >(minima)) << "\n"
                  << "Time range [micro s]:      " << min(static_cast< Vector<Double> >(minima_time))
                  << " to " << max(static_cast< Vector<Double> >(minima_time)) << "\n"
                  << "Time average [micro s]:    " << mean(static_cast< Vector<Double> >(minima_time)) << std::endl;
      }

    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:caclulateMaxima: " << x.getMesg() << std::endl;
      }; 
  }



  void CompletePipeline::listCalcMaxima(DataReader *dr,
					 Vector<Bool> antennaSelection,
					 const int& upsampling_exp,
					 Vector<Double> pbeam_offset,
					 const bool& rawData)
  {
    try 
    {
      Vector<Double> timeValues;		// time values
      Vector<Double> timeRange;			// time values
      Matrix<Double> yValues;			// y-values
      Vector<Double> trace;			// trace currently processed
      vector<double> maxima;			// Stores the calculated maxima
      vector<double> minima;			// Stores the calculated minima
      vector<double> maxima_time;		// Stores the calculated time of the maxima
      vector<double> minima_time;		// Stores the calculated time of the minima

      // Get the antenna selection from the DataReader if no selction was chosen 	
      if (antennaSelection.nelements() == 0) {
	antennaSelection = GetAntennaMask(dr);
      }

 
      // Get the (upsampled) time axis
      timeValues = getUpsampledTimeAxis(dr,upsampling_exp);

      // Get the yValues of all selected antennas (raw data or fieldstrength)
      if (rawData)
        yValues = getUpsampledFX(dr,upsampling_exp, antennaSelection, true);  // true means: offset will be substracted
      else
        yValues = getUpsampledFieldstrength(dr,upsampling_exp, antennaSelection);

      // check length of time axis and yValues traces for consistency
      if (timeValues.size() != yValues.column(0).size())
        std::cerr << "CompletePipeline:calculateMaxima: WARNING: Length of time axis differs from length of the antenna traces!\n"
           << std::endl;


      // Define the time range considered (the same as the plot range)
      Slice range = calculatePlotRange(timeValues);

      // cut time values
      timeRange = timeValues(range);

      // find the maximal y values for all selected antennas
      for (int i = 0; i < antennaSelection.nelements(); i++) if (antennaSelection(i))
      {
        // Start with height 0 and search for heigher and lower values
        double maximum = 0;
	double minimum = 0;
        double extrema =0;
        int maxtimevalue = 0;
        int mintimevalue = 0;

        // get current trace
        trace = yValues.column(i)(range);

        // loop through the values and search for the heighest and lowest one
        for(int j = 0; j < timeRange.nelements(); j++)
	{
          if ( maximum < trace(j)) 
          {
            maxtimevalue = j;
            maximum = trace(j);
          } 
          if ( minimum > trace(j)) 
          {
            mintimevalue = j;
            minimum = trace(j);
          } 

          if ( extrema < maximum ) 
             extrema = maximum;
          if (extrema < abs(minimum))
             extrema = abs(minimum);
            
	}  

        // store the calculated values for later calculation of the mean
	// multiply by 1e6 for conversion to micro
        maxima.push_back(maximum*1e6);
        maxima_time.push_back(timeRange(maxtimevalue)*1e6);
        minima.push_back(minimum*1e6);
        minima_time.push_back(timeRange(mintimevalue)*1e6);


        // print the calculated values
        std::cout <<  i+1 <<" " <<extrema*1e6 << "\n";
      }// end for loop over selected antennas
      std::cout << " ... done."<< std::endl;
    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:caclulateMaxima: " << x.getMesg() << std::endl;
      }; 
  }



} // Namespace CR -- end
