/***************************************************************************
 *
 *   Copyright (C) 2006-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotLoop.h"
#include "Pulsar/MultiData.h"

#include "Pulsar/Plot.h"
#include "Pulsar/Archive.h"

#include <cpgplot.h>

#define VERBOSE(x) if (Plot::verbose) cerr << x << endl

// #define VERBOSE(x) cerr << x << endl

using namespace std;

//! Default constructor
Pulsar::PlotLoop::PlotLoop ()
{
  preprocess = true;
  overlay = false;
}

//! Set the Plot to be executed
void Pulsar::PlotLoop::add_Plot (Plot* p)
{
  plots.push_back (p);
}

void Pulsar::PlotLoop::configure (const std::vector<std::string>& options)
{
  if (overlay)
    for (unsigned i=0; i<plots.size(); i++)
      plots[i] = MultiData::factory (plots[i]);

  for (unsigned iopt=0; iopt < options.size(); iopt++)
    for (unsigned iplot=0; iplot < plots.size(); iplot++)
      plots[iplot]->configure( options[iopt] );
}

void Pulsar::PlotLoop::finalize ()
{
  for (unsigned i=0; i<plots.size(); i++)
    plots[i]->finalize ();
}

//! Set the Archive to be plotted
void Pulsar::PlotLoop::set_Archive (Archive* a)
{
  archives.resize( plots.size() );
  for (unsigned iplot=0; iplot < plots.size(); iplot++)
  {
    archives[iplot] = a;
    if (preprocess)
    {
      archives[iplot] = a->clone();
      plots[iplot]->preprocess (archives[iplot]);
    }
  }
}

//! Set the overlay flag
void Pulsar::PlotLoop::set_overlay (bool flag)
{
  overlay = flag;
}

bool Pulsar::PlotLoop::get_overlay () const
{
  return overlay;
}

void Pulsar::PlotLoop::set_preprocess (bool flag)
{
  preprocess = flag;
}

bool Pulsar::PlotLoop::get_preprocess () const
{
  return preprocess;
}

//! Add an index over which to loop
void Pulsar::PlotLoop::add_index (TextIndex* index)
{
  index_stack.push( index );
}

// execute the plot for each index in the stack
void Pulsar::PlotLoop::plot ()
{
  plot (index_stack);
}

void Pulsar::PlotLoop::plot( std::stack< Reference::To<TextIndex> >& indeces )
{
  if (indeces.empty())
  {
    VERBOSE("Pulsar::PlotLoop::plot plotting");

    for (unsigned i=0; i<plots.size(); i++)
    {
      if (!overlay)
        cpgpage ();

      plots[i]->plot (archives[i]);
    }
    return;
  }

  Reference::To<TextIndex> index = indeces.top();
  indeces.pop();

  vector<string> current (plots.size());

  unsigned loop_size = 0;
  for (unsigned iplot=0; iplot < plots.size(); iplot++)
  {
    index->set_container( archives[iplot]->get_interface() );

    if (iplot == 0)
      loop_size = index->size();
    else if ( loop_size != index->size() )
      throw Error (InvalidState, "Pulsar::PlotLoop::plot",
                   "loop size for plot[0]=%u != that of plot[%u]=%u",
                   loop_size, iplot, index->size());
  }
  
  Reference::To<TextInterface::Parser> interface;

  for (unsigned i=0; i<loop_size; i++)
  {
    for (unsigned iplot=0; iplot < plots.size(); iplot++) try
    {
      index->set_container( archives[iplot]->get_interface() );

      string index_command = index->get_index(i);

      VERBOSE("Pulsar::PlotLoop::plot " << index_command);

      if (!overlay)
      {
	PlotLabel* label = plots[iplot]->get_attributes()->get_label_above();
	current[iplot] = label->get_centre();
	if (current[iplot] != "unset")
	  label->set_centre( current[iplot] + " " + index_command );
      }

      //
      // by not calling Plot::configure, we avoid MultiData::configure,
      // which will add each of these temporary options to its common list
      //
      interface = plots[iplot]->get_interface();
      interface->process( index_command );
  
    }
    catch (Error& error)
    {
      cerr << "Pulsar::PlotLoop::plot error configuring "
	   << index->get_index(i) << " of "
           << "\n\t" << archives[iplot]->get_filename()
           << "\n\t" << error.get_message() << endl;
    }

    try
    {
      plot (indeces);
    }
    catch (Error& error)
    {
      cerr << "Pulsar::PlotLoop::plot error plotting " 
           << "\n\t" << error.get_message() << endl;
    }

    if (!overlay)
      for (unsigned iplot=0; iplot < plots.size(); iplot++)
      {
	PlotLabel* label = plots[iplot]->get_attributes()->get_label_above();
	label->set_centre( current[iplot] );
      }
  }

  indeces.push( index );
}

