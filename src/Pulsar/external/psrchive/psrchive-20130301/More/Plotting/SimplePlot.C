/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SimplePlot.h"
#include "Pulsar/PlotAnnotation.h"

using namespace std;

void Pulsar::SimplePlot::plot (const Archive* data)
{
  if (verbose)
    cerr << "Pulsar::SimplePlot::plot init" << endl;
  get_frame()->init (data);

  if (verbose)
    cerr << "Pulsar::SimplePlot::plot prepare" << endl;
  prepare (data);

  if (verbose)
    cerr << "Pulsar::SimplePlot::plot focus" << endl;
  get_frame()->focus (data);

  draw (data);

  for (unsigned i=0; i<annotations.size(); i++) 
  {
    if (verbose)
      cerr << "Pulsar::SimplePlot::plot draw annotation " 
        << i << "/" << annotations.size() << endl;
    annotations[i]->draw(data);
  }

  if (verbose)
    cerr << "Pulsar::SimplePlot::plot draw axes" << endl;
  get_frame()->draw_axes (data);

  if (verbose)
    cerr << "Pulsar::SimplePlot::plot label axes" << endl;
  get_frame()->label_axes (get_xlabel (data), get_ylabel (data));

  if (verbose)
    cerr << "Pulsar::SimplePlot::plot decorate frame" << endl;
  get_frame()->decorate (data);
}

