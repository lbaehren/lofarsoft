/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Telescopes.h"
#include "Pulsar/Telescope.h"
#include "tempo++.h"
#include "coord.h"

void Pulsar::Telescopes::set_telescope_info (Telescope *t, Archive *a) try
{
  switch ( Tempo::code( a->get_telescope() ) )
  {

  case '1':
    Telescopes::GBT(t);
    // Hack to pick correct focus type for GBT
    if (a->get_centre_frequency()<1200.0)
      t->set_focus(Telescope::PrimeFocus);
    break;

  case '3':
    Telescopes::Arecibo(t);
    break;

  case '4':
    Telescopes::MtPleasant26(t);
    break;

  case '7':
    Telescopes::Parkes(t);
    break;

  case 'a':
    Telescopes::GB140(t);
    break;

  case 'b':
    Telescopes::GB85_3(t);
    break;

  case 'f':
    Telescopes::Nancay(t);
    break;

  case 'g':
    Telescopes::Effelsberg(t);
    break;
    
  default: // Unknown code, throw error
    throw Error (InvalidParam, "Pulsar::Telescopes::set_telescope_info",
		 "Unrecognized telescope code (%s)",
		 a->get_telescope().c_str());
  }

  t->set_coordinates();
}
catch (Error& error)
{
  throw error += "Pulsar::Telescopes::set_telescope_info";
}

// Info for each telescope below.  Maybe the coordinate setting
// routine could be incorporated here, to have a centralized 
// location for all this.  Also would be good to not have to use
// the Tempo codes explicitly below.

void Pulsar::Telescopes::GBT(Telescope *t) 
{
  t->set_name("GBT");
  t->set_mount(Telescope::Horizon);
  t->set_primary(Telescope::Parabolic);
  t->set_focus(Telescope::Gregorian); // XXX only true for L-band and up
}

void Pulsar::Telescopes::Arecibo(Telescope *t)
{
  t->set_name("Arecibo");
  t->set_mount(Telescope::Horizon);
  t->set_primary(Telescope::Spherical);
  t->set_focus(Telescope::Gregorian); // What about CH receivers?
}

void Pulsar::Telescopes::GB140(Telescope *t)
{
  t->set_name("GB 140ft");
  t->set_mount(Telescope::Equatorial);
  t->set_primary(Telescope::Parabolic);
  t->set_focus(Telescope::PrimeFocus); 
}

void Pulsar::Telescopes::GB85_3(Telescope *t)
{
  t->set_name("GB 85-3");
  t->set_mount(Telescope::Equatorial);
  t->set_primary(Telescope::Parabolic);
  t->set_focus(Telescope::PrimeFocus); 
}

void Pulsar::Telescopes::Nancay(Telescope *t)
{
  t->set_name("Nancay");
  t->set_mount(Telescope::Horizon);
  t->set_primary(Telescope::Parabolic);
  t->set_focus(Telescope::Gregorian); 
}

void Pulsar::Telescopes::Effelsberg(Telescope *t)
{
  t->set_name("Effelsberg");
  t->set_mount(Telescope::Horizon);
  t->set_primary(Telescope::Parabolic);
  t->set_focus(Telescope::Gregorian); // XXX also varies by receiver
}

void Pulsar::Telescopes::MtPleasant26(Telescope *t)
{
  t->set_name ("Hobart");
  t->set_mount (Telescope::Meridian);
  t->set_primary (Telescope::Parabolic);
  t->set_focus (Telescope::PrimeFocus);
}

void Pulsar::Telescopes::Parkes(Telescope *t)
{
  t->set_name ("Parkes");
  t->set_mount (Telescope::Horizon);
  t->set_primary (Telescope::Parabolic);
  t->set_focus (Telescope::PrimeFocus);
}

