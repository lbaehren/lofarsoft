/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MultiFrame.h"
#include "Pulsar/Plot.h"

#include <cpgplot.h>

using namespace std;

void Pulsar::MultiFrame::manage (const std::string& name, PlotFrame* frame)
{
  frames[name] = frame;
}

void Pulsar::MultiFrame::focus (const Archive* data)
{
  std::pair<float,float> xvp = get_x_edge()->get_viewport ();
  std::pair<float,float> yvp = get_y_edge()->get_viewport ();
  
  if (Plot::verbose)
    cerr << "Pulsar::MultiFrame::focus cpgsvp ("
         << xvp.first << ", " << xvp.second << ", "
         << yvp.first << ", " << yvp.second << ")" << endl;

  cpgsvp (xvp.first, xvp.second, yvp.first, yvp.second);
}

void Pulsar::MultiFrame::decorate (const Archive* data)
{
  cpgsls (1);
  cpgsci (1);

  get_label_above()->plot(data);
}


Pulsar::PlotFrame* Pulsar::MultiFrame::get_frame (const std::string& name)
{
  std::map< std::string, Reference::To<PlotFrame> >::iterator frame;
  frame = frames.find (name);

  if (frame == frames.end())
    throw Error (InvalidParam, "Pulsar::MultiFrame::get_frame",
		 "no frame named '" + name + "'");

  return frame->second;
}

template<typename Iterator, typename UnaryMemberFunction, typename T>
void foreach (Iterator begin, Iterator end,
	       UnaryMemberFunction func, T value)
{
  for (Iterator i = begin; i != end; i++)
    (i->second->*func)(value);
}

//! Set the character height
void Pulsar::MultiFrame::set_character_height (float height)
{
  character_height = height;
  foreach( frames.begin(), frames.end(), 
	   &PlotFrame::set_character_height, character_height );
}

//! Set the character font
void Pulsar::MultiFrame::set_character_font (int font)
{
  character_font = font;
  foreach( frames.begin(), frames.end(), 
	   &PlotFrame::set_character_font, character_font );
}

//! Set the line width
void Pulsar::MultiFrame::set_line_width (int width)
{
  line_width = width;
  foreach( frames.begin(), frames.end(), 
	   &PlotFrame::set_line_width, line_width );
}

template<typename Iterator, typename GeneratorMemberFunction>
void foreach (Iterator begin, Iterator end,
	      GeneratorMemberFunction func)
{
  for (Iterator i = begin; i != end; i++)
    (i->second->*func)();
}

void Pulsar::MultiFrame::no_labels ()
{
  foreach( frames.begin(), frames.end(), 
	   &PlotFrame::no_labels );
}
