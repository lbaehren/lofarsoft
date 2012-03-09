/***************************************************************************
 *
 *   Copyright (C) 2006-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotAttributes.h"
#include <cpgplot.h>

Pulsar::PlotAttributes::PlotAttributes ()
{
  character_height = 1.0;
  character_font = 1;
  line_width = 1;
  set_applied = "none";

  above = new PlotLabel;
  above->set_centre("$file");
}

Pulsar::PlotAttributes::~PlotAttributes ()
{
}

//! Set the character height
void Pulsar::PlotAttributes::set_character_height (float h)
{
  character_height = h; set_applied = "none";
}

//! Set the character font
void Pulsar::PlotAttributes::set_character_font (int font)
{
  character_font = font; set_applied = "none";
}

//! Set the line width
void Pulsar::PlotAttributes::set_line_width (int width)
{
  line_width = width; set_applied = "none";
}

void Pulsar::PlotAttributes::set_label_above (PlotLabel* label)
{
  above = label;
}

void Pulsar::PlotAttributes::publication_quality ()
{
  get_label_above()->set_all( PlotLabel::unset );

  set_character_height (1.5);
  set_character_font (2);
  set_line_width (3);
  set_applied = "pub";
}

void Pulsar::PlotAttributes::no_labels ()
{
  set_applied = "blank";
}

//! Apply a named set of commands (a shortcut)
void Pulsar::PlotAttributes::apply_set (const std::string& name)
{
  if (name == "pub")
    publication_quality ();
  else if (name == "blank")
    no_labels ();
  else
    throw Error (InvalidParam, "Pulsar::PlotAttributes::apply_set",
		 "unrecognized set name '" + name + "'");
}

void Pulsar::PlotAttributes::setup ()
{
  cpgsch (character_height);
  cpgscf (character_font);
  cpgslw (line_width);

  cpgsls (1);
  cpgsci (1);
}
