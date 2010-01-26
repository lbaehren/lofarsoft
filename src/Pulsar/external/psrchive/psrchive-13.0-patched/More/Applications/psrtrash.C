/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/PlotOptions.h"

#include "Pulsar/PlotFactory.h"
#include "Pulsar/FramedPlot.h"
#include "Pulsar/Archive.h"

#include <cpgplot.h>

using namespace std;
using namespace Pulsar;

//
//! An application to interactively trash files
//
class trash : public Pulsar::Application
{
public:

  //! Default constructor
  trash ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Process any remaining archives
  void finalize ();

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&) {}

  unsigned plotted;
  string plot_name;

  // Available plots
  PlotFactory factory;

  // Plot classes to be used
  Reference::To<Plot> plot;

  // Plot options
  Pulsar::PlotOptions plot_options;
  Pulsar::StandardOptions standard_options;

  // List of archives plotted
  vector<string> files;

  void select ();
};


trash::trash ()
  : Application ("trash", "quickly visually inspect and trash files")
{
  plot_options.set_x_npanel (5);
  plot_options.set_y_npanel (4);
  plot_options.set_device ("/xs");

  // by default, pscrunch, tscrunch, and fscrunch
  standard_options.add_default_job ("pTFC");

  // by default, plot the total intensity
  plot_name = "flux";

  add( &plot_options );
  add( &standard_options );

  plotted = 0;
}

void trash::process (Pulsar::Archive* archive)
{
  cpgpage ();
  cpgask (0);

  if (!plot)
  {
    plot = factory.construct( plot_name );
    
    FramedPlot* framed = dynamic_cast<FramedPlot*> (plot.get());

    if (framed)
    {
      // make the plot larger
      framed -> get_frame() -> set_viewport (0.05, 0.95, 0.05, 0.95);
      
      // remove the above frame labels
      framed -> get_frame() -> get_label_above()->set_all ("");
    }
  }

  plot->preprocess (archive);
  plot->plot (archive);

  if (files.size() == 0)
    files.resize( plot_options.get_x_npanel() * plot_options.get_y_npanel() );

  files[plotted] = archive->get_filename();

  plotted ++;

  if (plotted == files.size())
  {
    select ();
    plotted = 0;
  }
}

void trash::finalize ()
{
  if (plotted)
  {
    select ();
    plotted = 0;
  }
}

void trash::select ()
{
  char ans='j';
  float x=0.5, y=0.5;

  unsigned xpanel = plot_options.get_x_npanel();
  unsigned ypanel = plot_options.get_y_npanel();

  while (ans != ' ')
  {
    cpgpanl (1,1);
    cpgsvp (0,1, 0,1);
    // left-to-right top-to-bottom
    cpgswin (0,1, 1,0);
    cpgcurs (&x,&y,&ans);

    if (ans == 'A')
    {
      if (x < 0) x = 0;
      if (y < 0) y = 0;

      unsigned xip = unsigned (x);
      unsigned yip = unsigned (y);

      if (xip >= xpanel) xip = xpanel - 1;
      if (yip >= ypanel) yip = ypanel - 1;

      unsigned index = yip * xpanel + xip;

      if (index >= plotted)
	continue;

      cout << "trash: " << files[index] << endl;

      // draw a big red x
      cpgpanl (xip+1, yip+1);
      cpgsci (2);
      cpgmove (0,0);
      cpgdraw (1,1);
      cpgmove (0,1);
      cpgdraw (1,0);
    }
    else if (ans == 'a' || ans == 'e')
    {
      cerr << "psrtrash: trashing all plotted" << endl;
      for (unsigned i=0; i<plotted; i++)
        cout << "trash: " << files[i] << endl;

      break;
    }
  }

  cpgpanl (xpanel,ypanel);
}

int main (int argc, char** argv)
{
  trash program;
  return program.main (argc, argv);
}

