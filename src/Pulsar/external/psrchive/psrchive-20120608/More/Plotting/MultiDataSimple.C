/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MultiDataSimple.h"
#include "Pulsar/Archive.h"

using namespace std;

//! Construct with plot to be managed
Pulsar::MultiDataSimple::MultiDataSimple (SimplePlot* simple)
{
  manage (simple);
}

//! Manage a plot
void Pulsar::MultiDataSimple::manage (SimplePlot* simple)
{
  managed_plot = simple;
  simple_plot = simple;
}

//! prepare the Simple Plot and push it onto the plots vector
void Pulsar::MultiDataSimple::plot (const Archive* data)
{
  simple_plot->get_frame()->init (data);
  simple_plot->prepare (data);

  plots.push_back (simple_plot);
  data_sets.push_back (data);

  Plot* like = simple_plot->get_constructor()->construct();
  SimplePlot* new_simple = dynamic_cast<SimplePlot*> (like);

  if (!new_simple)
    throw Error (InvalidState, "Pulsar::MultiDataSimple::plot",
		 "Plot::Constructor::construct does not return a SimplePlot");

  // configure new_simple using the saved options
  for (unsigned i=0; i<options.size(); i++)
  {
    if (verbose)
      cerr << "Pulsar::MultiDataSimple::plot apply "
	"'" << options[i] << "'" << endl;
    new_simple->configure( options[i] );
  }

  // then replace simple_plot with new_simple
  manage (new_simple);
}

void Pulsar::MultiDataSimple::finalize ()
{
  if (verbose)
    cerr << "Pulsar::MultiDataSimple::finalize" << endl;

  if (!data_sets.size())
    return;

  const Archive* main_data = data_sets[0];
  SimplePlot* main_plot = plots[0];

  PlotFrame* main_frame = main_plot->get_frame();

  for (unsigned i=1; i < plots.size(); i++)
  {
    PlotFrame* frame = plots[i]->get_frame();
    main_frame->get_x_scale()->include( frame->get_x_scale() );
    main_frame->get_y_scale()->include( frame->get_y_scale() );
  }

  main_frame->focus( main_data );

  for (unsigned i=0; i < plots.size(); i++)
    plots[i]->draw( data_sets[i] );

  main_frame->draw_axes( main_data );

  main_frame->label_axes( main_plot->get_xlabel(main_data),
			  main_plot->get_ylabel(main_data) );

  main_frame->decorate( main_data );

  plots.resize(0);
  data_sets.resize(0);
}
