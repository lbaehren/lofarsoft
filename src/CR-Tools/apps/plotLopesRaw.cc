#include<sstream>

#include <Analysis/analyseLOPESevent2.h>
#include <Display/SimplePlot.h>

using CR::analyseLOPESevent2;
using CR::LopesEventIn;

/*!
  \file plotLopesRaw.cc

  \ingroup CR_Applications

  \brief Generates plots of the uncalibrated traces of all antennas"
 
  \author Moses Ender
 
  \date 2009/16/02

  <h3>Motivation</h3>

  I think it is important to have an easy to use tool to have a look on
  the raw data.

  <h3>Usage</h3>

  \verbatim
  ./plotLopesRaw first.event [ second.event ...]
  \endverbatim
  
  At least one event is required but you can also provide several events at once.
*/

int main(int argc, char* argv[]) 
{
    if(argc<2)
    {
	cout<<" plotLopesRaw"<< endl
	    <<" generates plots of the raw data"<<endl
	    <<" Usage: ./plotLopesRaw first.event [ next.event ... ]"<<endl<<endl
	    <<" Error: at least one LOPES-event needed..."<<endl;	
	exit(1);
    }
    try {

    int c=1;
    while(c<argc)
    {
	string plotname = argv[c];
        // delete the file ending
        if (plotname.find(".event") != string::npos)
	{
	    plotname.erase(plotname.find_last_of('.'));
            // deletes the file path, if it exists    
            if (plotname.find("/") != string::npos)
		plotname.erase(0,plotname.find_last_of('/')+1);
	}
	else
	{
	    cout<<"Error: "<<argv[c]<<" doesn't seem to be a Lopes event!\n";
	    c++;
	    continue;
	}
	plotname.append("-raw.ps");

	CR::LopesEventIn event(argv[c]);
	cout<< "Generating plot "<<plotname<<endl;

	CR::SimplePlot plotter;        			// define plotter
        Vector<Double> xaxis;      			// values for plot
        double xmax,xmin,ymin=0,ymax=0;               	// Plotrange
	Matrix<Double> yValues;                   	// y-values


        xaxis = static_cast< Vector<Double> >(event.timeValues());
 	xaxis *= 1e6;

	Vector<Bool> antennaSelection = event.antennaSelection();
        antennaSelection.unique();

        yValues = event.voltage().copy();
	yValues *= 1e6;


        // define Plotrange
        xmin = min(xaxis);
        xmax = max(xaxis);

        // find the minimal and maximal y values for the plot
        for (unsigned int i = 0; i < antennaSelection.nelements(); i++)
          if (antennaSelection(i))                // consider only selected antennas
          {
            if ( ymin > min(yValues.column(i)) ) {
              ymin = min(yValues.column(i));
            }
            if ( ymax < max(yValues.column(i)) ) {
              ymax = max(yValues.column(i));
            }
          }

        // multiply ymin and ymax by 105% to have some space at the bottom and the top of the plot
        ymin *= 1.05;
        ymax *= 1.05;


        // Initialize the plot giving xmin, xmax, ymin and ymax
        plotter.InitPlot(plotname, xmin, xmax, ymin, ymax);
        // Add labels 
 	stringstream antennanumber;
        antennanumber << ntrue(antennaSelection);
        string label = string(argv[c]) + " - " + antennanumber.str() + " Antennas";

        plotter.AddLabels("Time t [#gmsec]", "Fieldstrength #ge#d0#u [#gmV/m/MHz]",label);

	int color=3;
        // Create the plots looping through antennas
        for (unsigned int i = 0; i < antennaSelection.nelements(); i++)
        if (antennaSelection(i))                        // consider only selected antennas
        {
          // Plot trace
          plotter.PlotLine(xaxis,yValues.column(i),color,1);

          color++;                                      // another color for the next antenna
          if (color >= 13) color = 3;                   // there are only 16 colors available, 
                                                        // use only ten as there are 3x10 antennas
        }

    c++;
    } // big while loop

    } catch (AipsError x) {
    cerr << "plotLopesRaw: " << x.getMesg() << endl;
  }

  return 0;
}
