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
    lastTimeUpsamplingExponent(-1)
  {;}
  
  CompletePipeline::CompletePipeline (CompletePipeline const &other):
    plotStart_p(-2.05e-6), plotStop_p(-1.55e-6),
    plotlist(),
    lastUpsamplingExponent(-1),
    lastTimeUpsamplingExponent(-1)
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
							      int upsampling_exp,
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
      for (int i = 0; i < antennaSelection.nelements(); i++) if (upsamplingToDo[i])
      {
        std::cout << "Upsampling the data of antenna " << i+1 << " by a factor of " << upsampled << " ...\n";
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
	for (int j = 0; j < tracelength*upsampled; j++) upFieldStrength.column(i)(j) = upsampledTrace[j] - offset;

        // remark: tried to fasten data transfer from the array to the Matrix but did not work because arrays are
        // of typ float but not double.

        // set flag, that data for this antenna are upsampled
        upsampledAntennas[i] = true;
      } 

      // set last upsampling exponent to current value
      lastUpsamplingExponent = upsampling_exp;

      // Return upsampled traces 
      return upFieldStrength.copy();
    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:getUpsampledFieldstrength: " << x.getMesg() << std::endl;
      }; 
  }

  Vector<Double> CompletePipeline::getUpsampledTimeAxis (DataReader *dr,
							 int upsampling_exp)
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




  void CompletePipeline::plotCCbeam(const string filename,
				    DataReader *dr,
				    Vector<Bool> antennaSelection)
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
      int startsample = ntrue(xaxis<plotStart_p); //number of elements smaller then starting value of plot range
      int stopsample = ntrue(xaxis<plotStop_p);  //number of elements smaller then end of plot range
      Slice plotRange(startsample,(stopsample-startsample)); 

      // Get the CC-beam and the power-beam
      ccbeam = GetCCBeam(dr, antennaSelection).copy();
      pbeam = GetPBeam(dr, antennaSelection).copy();


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
      plotter.AddLabels("Time [microseconds]", "CC-Beam [microV/m/MHz]","CC-Beam and Power");
    
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


  void CompletePipeline::plotXbeam(const string filename,
				   DataReader *dr,
				   Vector<Bool> antennaSelection)
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
      int startsample = ntrue(xaxis<plotStart_p); //number of elements smaller then starting value of plot range
      int stopsample = ntrue(xaxis<plotStop_p);  //number of elements smaller then end of plot range
      Slice plotRange(startsample,(stopsample-startsample)); 

      // Get the X-beam and the power-beam
      xbeam = GetXBeam(dr, antennaSelection).copy();
      pbeam = GetPBeam(dr, antennaSelection).copy();


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
      plotter.AddLabels("Time [microseconds]", "X-Beam [microV/m/MHz]","X-Beam and Power");
    
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
  void CompletePipeline::plotAllAntennas(const string filename,
					 DataReader *dr,
					 Vector<Bool> antennaSelection,
					 bool seperated,
					 int upsampling_exp)
  {
    try 
    {
      SimplePlot plotter;    			// define plotter
      Vector<Double> xaxis;			// xaxis
      double xmax,xmin,ymin=0,ymax=0;		// Plotrange
      Matrix<Double> fieldstrength;		// y-values
      int color = 3;				// starting color

      // Get the antenna selection from the DataReader if no selction was chosen 	
      if (antennaSelection.nelements() == 0) {
	antennaSelection = GetAntennaMask(dr);
      }

      // Get the time (upsampled) time axis
      xaxis = static_cast< Vector<Double> >(dr->timeValues());

      // Get the fieldstrength of all antennas
      fieldstrength = GetTimeSeries(dr).copy();  // don't use the antennaSelection to get the full number of columns in the Matrix

      // Upsampled FieldStrength
      Matrix<Double> upfieldstrength = getUpsampledFieldstrength(dr,upsampling_exp, antennaSelection);
      // Upsampled x-axis
      Vector<Double> upxaxis = getUpsampledTimeAxis(dr,upsampling_exp);

      // check length of time axis and fieldstrength traces for consistency
      if (upxaxis.size() != upfieldstrength.column(0).size())
        std::cerr << "CompletePipeline:plotAllAntennas: WARNING: Length of time axis differs from length of the antenna traces!\n"
           << std::endl;


      // Define plotrange for not upsampled data
      int startsample = ntrue(xaxis<plotStart_p); //number of elements smaller then starting value of plot range
      int stopsample = ntrue(xaxis<plotStop_p);  //number of elements smaller then end of plot range
      Slice plotRange(startsample,(stopsample-startsample));

      // Define plotrange for upsampled data
      startsample = ntrue(upxaxis<plotStart_p); //number of elements smaller then starting value of plot range
      stopsample = ntrue(upxaxis<plotStop_p);  //number of elements smaller then end of plot range
      Slice upplotRange(startsample,(stopsample-startsample));

      // conversion to micro
      xaxis *= 1e6;
      fieldstrength *= 1e6;
      upxaxis *= 1e6;
      upfieldstrength *= 1e6;  

      // define Plotrange
      xmin = min(xaxis(plotRange));
      xmax = max(xaxis(plotRange));

      // find the minimal and maximal y values for the plot
      // do it with the upsampled data only as they are as least as heigh as the original ones
      for (int i = 0; i < antennaSelection.nelements(); i++)
        if (antennaSelection(i))		// consider only selected antennas
        {
          if ( ymin > min(upfieldstrength.column(i)(upplotRange)) ) {
	    ymin = min(upfieldstrength.column(i)(upplotRange));
	  }
          if ( ymax < max(upfieldstrength.column(i)(upplotRange)) ) {
	    ymax = max(upfieldstrength.column(i)(upplotRange));
	  }
        }

      // multiply ymin and ymax by 105% to have some space at the bottom and the top of the plot
      ymin *= 1.05;
      ymax *= 1.05;

      // Make the plots (either all antennas together or seperated)
      if (seperated == false)
      {
        // add the ".ps" to the filename
        string plotfilename = filename + ".ps";

    	std::cout <<"Plotting the fieldstrength of all antennas to file: "
		  << plotfilename
		  << std::endl;

        // Initialize the plot giving xmin, xmax, ymin and ymax
        plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);
        // Add labels
        plotter.AddLabels("Time [microseconds]", "fieldstrength [microV/m/MHz]","All selected Antennas");

        // Create the plots looping through antennas
        for (int i = 0; i < antennaSelection.nelements(); i++)
        if (antennaSelection(i))			// consider only selected antennas
        {
          // Plot (upsampled) trace
          plotter.PlotLine(upxaxis(upplotRange),upfieldstrength.column(i)(upplotRange),color,1);

          color++;					// another color for the next antenna
          if (color >= 13) color = 3;			// there are only 16 colors available, 
							// use only ten as there are 3x10 antennas
        }

        // Add filename to list of created plots
        plotlist.push_back(plotfilename);
      } else	// seperated == true
      {
        string plotfilename;
        string label;

        // Create empty vector for not existing error bars 
        Vector<Double> empty;

        // Get the AntennaIDs for labeling
        Vector<Int> AntennaIDs;
        dr->header().get("AntennaIDs",AntennaIDs);

        // Create the plots for each individual antenna looping through antennas


        for (int i = 0; i < antennaSelection.nelements(); i++)
        if (antennaSelection(i))			// consider only selected antennas
        {
	  // create filename and label
          stringstream antennanumber, antennaid;
          antennanumber << (i+1);
          antennaid << AntennaIDs(i);

          plotfilename = filename + "-" + antennanumber.str() + ".ps";
	  label = "Antenna " + antennanumber.str() + " (ID = " + antennaid.str() + ")";

    	  std::cout <<"Plotting the fieldstrength of antenna " << (i+1) << " (ID = " << AntennaIDs(i) 
			<< ") to file: " << plotfilename << std::endl;

          // Initialize the plot giving xmin, xmax, ymin and ymax
          plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);

          // Add labels
          plotter.AddLabels("Time [microseconds]", "fieldstrength [microV/m/MHz]",label);

          // Plot (upsampled) trace and original data points.
          plotter.PlotLine(upxaxis(upplotRange),upfieldstrength.column(i)(upplotRange),color,1);

	  // Known bug: Symbols are plotted with y = 0 if the same plot should be created for a second time
          // Even though in SimplePlot::PlotSymbols, plpoin(npoints, plxval, plyval, symbol); is called
          // exactly with the same values.
          plotter.PlotSymbols(xaxis(plotRange),fieldstrength.column(i)(plotRange),empty, empty, color, 2, 5);
          // Add filename to list of created plots
          plotlist.push_back(plotfilename);

          color++;					// another color for the next antenna
          if (color >= 13) color = 3;			// there are only 16 colors available, 
							// use only ten as there are 3x10 antennas
        }
      }


    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:plotAllAntennas: " << x.getMesg() << std::endl;
      }; 
  }


} // Namespace CR -- end
