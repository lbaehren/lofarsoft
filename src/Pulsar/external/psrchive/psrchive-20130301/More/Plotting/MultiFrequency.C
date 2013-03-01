/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MultiFrequency.h"

Pulsar::MultiFrequency::MultiFrequency ()
{
  frames.set_shared_x_scale (new FrequencyScale);
}

//! Get the scale
Pulsar::FrequencyScale* Pulsar::MultiFrequency::get_scale ()
{
  FrequencyScale* scale = 0;
  scale = dynamic_cast<FrequencyScale*>( frames.get_shared_x_scale() );
  if (!scale)
    throw Error (InvalidState, "Pulsar::MultiFrequency::get_scale",
		 "x scale is not a FrequencyScale");
  
  return scale;
}

// Text interface to the MultiFrequency class
Pulsar::MultiFrequency::Interface::Interface (MultiFrequency* instance)
{
  if (instance)
    set_instance (instance);
  
  import( "x", FrequencyScale::Interface(), &MultiFrequency::get_scale );
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::MultiFrequency::get_interface ()
{
  return new Interface (this);
}
