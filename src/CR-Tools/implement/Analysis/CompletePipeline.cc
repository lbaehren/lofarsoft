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
    plotStart_p(-2.05e-6), plotStop_p(-1.55e-6)
  {;}
  
  CompletePipeline::CompletePipeline (CompletePipeline const &other):
    plotStart_p(-2.05e-6), plotStop_p(-1.55e-6)
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
      ccbeam = GetCCBeam(dr, antennaSelection);
      pbeam = GetPBeam(dr, antennaSelection);


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
      plotter.AddLabels("Time [microseconds]", "CC-Beam [microV/m/MHz]","");
    
      // Add CC-beam
      plotter.PlotLine(xaxis(plotRange),ccbeam(plotRange),4,1);
      // Add Power-beam
      plotter.PlotLine(xaxis(plotRange),pbeam(plotRange),5,1);

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
      xbeam = GetXBeam(dr, antennaSelection);
      pbeam = GetPBeam(dr, antennaSelection);


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
      plotter.AddLabels("Time [microseconds]", "X-Beam [microV/m/MHz]","");
    
      // Add X-beam
      plotter.PlotLine(xaxis(plotRange),xbeam(plotRange),4,1);
      // Add Power-beam
      plotter.PlotLine(xaxis(plotRange),pbeam(plotRange),5,1);

    } catch (AipsError x) 
      {
        std::cerr << "CompletePipeline:plotXBeam: " << x.getMesg() << std::endl;
      }; 
  }


  // Plot the time shifed (= after beamforming) filedstrength of all antennas
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

      // Get the time axis
      xaxis = static_cast< Vector<Double> >(dr->timeValues()); 

      // Get the fieldstrength of all antennas
      fieldstrength = GetTimeSeries(dr);

      // Upsampled FieldStrength
      Matrix<Double> upfieldstrength = fieldstrength;
      // Upsampled x-axis
      Vector<Double> upxaxis = xaxis;

      // do upsampling of all the antenna traces
      if (upsampling_exp >= 0) 
      {
        // create upsampling factor by upsampling exponent
        unsigned int upsampled = pow(2,upsampling_exp);

        // get length of trace
        int tracelength = fieldstrength.column(0).size();
        // allocate memory for original and upsampled traces
        float originalTrace[tracelength];
        float upsampledTrace[tracelength * upsampled];

        // resize matrix with traces without copying the old values
        upfieldstrength.resize(tracelength * upsampled, antennaSelection.nelements(), false);

        for (int i = 0; i < antennaSelection.nelements(); i++)
        {
          std::cout << "\nUpsampling the data of antenna " << i+1 << " by a factor of " << upsampled << " ...\n";
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
	  // correct data for offset
          for (int j = 0; j < tracelength; j++) fieldstrength.column(i)(j) -= offset;
          std::cout << "Corrected Offset: " << offset << "\n";

	  // copy upsampled trace int Matrix with antenna traces
	  for (int j = 0; j < tracelength*upsampled; j++) upfieldstrength.column(i)(j) = upsampledTrace[j];

          
        }

        // stretch x-axis:

        // calculate time between two samples
        double sampleTime = 1/(dr->sampleFrequency() * upsampled);  
        std::cout << "SampleTime = " << sampleTime << " s\n\n.";

        // get the length of the x-axis and check if consistent
        int x_length = xaxis.size();
        if (x_length != tracelength)
          std::cerr << "CompletePipeline:plotAllAntennas: WARNING: Length of time axis differs from length of the antenna traces!\n"
           << std::endl;

        // resize x-axis and copy values
        upxaxis.resize(x_length*upsampled, false);

        // copy old values to the right place and fill space inbetween 
        for (int i = x_length-1; i >= 0; i--)
        {
          // move existing time value to the right place
          upxaxis(i*upsampled) = xaxis(i);
          // create new values
          for (int j = 1; j < upsampled; j++) 
            upxaxis(i*upsampled+j) = upxaxis(i*upsampled) + j*sampleTime;
        }
      }	

      // Define plotrange
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

//again for the not upsampled values (should not be avoidable)
      for (int i = 0; i < antennaSelection.nelements(); i++)
        if (antennaSelection(i))		// consider only selected antennas
        {
          if ( ymin > min(fieldstrength.column(i)(plotRange)) ) {
	    ymin = min(fieldstrength.column(i)(plotRange));
	  }
          if ( ymax < max(fieldstrength.column(i)(plotRange)) ) {
	    ymax = max(fieldstrength.column(i)(plotRange));
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
        plotter.AddLabels("Time [microseconds]", "fieldstrength [microV/m/MHz]","");

        // Create the plots looping through antennas
        for (int i = 0; i < antennaSelection.nelements(); i++)
        if (antennaSelection(i))			// consider only selected antennas
        {
          plotter.PlotLine(xaxis(plotRange),fieldstrength.column(i)(plotRange),color,1);
          color++;					// another color for the next antenna
        }
      } else	// seperated == true
      {
        string plotfilename;
        string label;

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

          Vector<Double> empty;
          if (upsampling_exp >= 0)
          {
            plotter.PlotLine(upxaxis(upplotRange),upfieldstrength.column(i)(upplotRange),color,1);
          } else
          {
            plotter.PlotLine(xaxis(plotRange),fieldstrength.column(i)(plotRange),color,1);
          }
          plotter.PlotSymbols(xaxis(plotRange),fieldstrength.column(i)(plotRange),empty, empty, color, 2, 5);
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
