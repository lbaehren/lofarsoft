/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotLabel.h"
#include "Pulsar/Statistics.h"

#include "escape.h"
#include "substitute.h"
#include "evaluate.h"

#include <cpgplot.h>

#if 0
#define DEBUG(x) cerr << x << endl
#else
#define DEBUG(X)
#endif

using namespace std;

string Pulsar::PlotLabel::unset = "unset";

Pulsar::PlotLabel::PlotLabel ()
{
  left = centre = right = unset;
  spacing = 1.2;
  offset = 0.5;
}

Pulsar::PlotLabel::~PlotLabel ()
{
}

void Pulsar::PlotLabel::plot (const Archive* data)
{
  plot (data, get_left(),   0.0);
  plot (data, get_centre(), 0.5);
  plot (data, get_right(),  1.0);
}

void Pulsar::PlotLabel::plot (const Archive* archive, 
                              const string& label, float side) try
{
  if (label == PlotLabel::unset)
    return;

  string elabel = escape (label);
  vector<string> labels;
  separate (elabel, labels, "\n");

  TextInterface::Parser* parser 
    = standard_interface( const_cast<Archive*>(archive) );
  parser->set_indentation ("");

  for (unsigned i=0; i < labels.size(); i++)
  {
    DEBUG( "Pulsar::PlotLabel::plot label[" <<i<< "]=" << labels[i] );

    labels[i] = substitute( labels[i], parser );

    DEBUG( "Pulsar::PlotLabel::plot subst label=" << labels[i] );

    labels[i] = evaluate( labels[i] );

    DEBUG( "Pulsar::PlotLabel::plot eval label=" << labels[i] );

    row (labels[i], i, labels.size(), side);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::PlotLabel::plot";
}
unsigned Pulsar::PlotLabel::get_nrows (const string& label) const
{
  string elabel = escape (label);
  vector<string> labels;
  separate (elabel, labels, "\n");
  return labels.size();
}

//! Get the margin in the specified units
float Pulsar::PlotLabel::get_margin (pgplot::Units units) const
{
  float xl, yl;
  // the margin is defined by the width of a dash
  cpglen (units, "-", &xl, &yl);
  return 1.5 * xl;
}

//! Get the displacement of label[irow] in the specified units
float Pulsar::PlotLabel::get_displacement (int row, pgplot::Units units) const
{
  float xl, yl;
  // the row displacement is defined by the height of a zero
  cpgqcs (units, &xl, &yl);

  return yl * get_displacement(row);
}

//! Get the displacement of row character heights from the viewport edge
float Pulsar::PlotLabel::get_displacement (int row) const
{
  if (spacing > 0)
    return offset + row * spacing;
  else
    return (row+1) * spacing - offset;
}

void Pulsar::PlotLabel::row (const string& label,
			     unsigned irow, unsigned nrow, float side)
{
  /*
    COORD: the location of the character string along the specified
    edge of the viewport, as a fraction of the length of the edge.
  */

  float coord = 0.5;

  if (side < 0.5)
    coord = side + get_margin (pgplot::Viewport);
  else if (side > 0.5)
    coord = side - get_margin (pgplot::Viewport);

  // rows always go from top to bottom
  if (spacing > 0)
    irow = nrow - irow -1;

  cpgmtxt ("T", get_displacement(irow), coord, side, label.c_str());
}

