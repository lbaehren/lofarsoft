/***************************************************************************
 *
 *   Copyright (C) 2002-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/ArchiveInterface.h"
#include "Pulsar/ArchiveMatch.h"

#include "Pulsar/Agent.h"
#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Receiver.h"

#include "Pulsar/Predictor.h"
#include "Pulsar/Parameters.h"

#include "Types.h"
#include "Error.h"
#include "typeutil.h"
#include "strutil.h"

using namespace std;

void Pulsar::Archive::init ()
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::init" << endl;

  expert_interface = new Expert (this);
  copy_subints = 0;
}

//! Provide access to the expert interface
Pulsar::Archive::Expert* Pulsar::Archive::expert ()
{
  return expert_interface;
}

//! Provide access to the expert interface
const Pulsar::Archive::Expert* Pulsar::Archive::expert () const
{
  return expert_interface;
}

//! Return a text interface that can be used to access this instance
TextInterface::Parser* Pulsar::Archive::get_interface ()
{
  if (!text_interface)
    text_interface = new Interface (this);

  return text_interface;
}

Pulsar::Archive::Archive ()
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::null constructor" << endl;

  init();
}

//! The Archive copy constructor must never be called, call Archive::copy
Pulsar::Archive::Archive (const Archive& archive)
{
  throw Error (Undefined, "Pulsar::Archive copy constructor",
               "sub-classes must define copy constructor");
}


Pulsar::Archive::~Archive ()
{
  if (verbose == 3)
    cerr << "Pulsar::Archive dtor this=" << this << endl;
}

string Pulsar::Archive::get_revision (const char* revision)
{
  char buf1 [64];
  char buf2 [64];
  sscanf (revision, "%s %s", buf1, buf2);
  return buf2;
}

//! operator =
Pulsar::Archive&
Pulsar::Archive::operator = (const Archive& a)
{
  copy (&a);
  return *this;
}

//! Return a null-constructed instance of the derived class
Pulsar::Archive* Pulsar::Archive::new_Archive (const string& class_name)
{
  if (Agent::registry.size() == 0)
    throw Error (InvalidState, "Pulsar::Archive::new_Archive",
                 "no Agents loaded");

  for (unsigned agent=0; agent<Agent::registry.size(); agent++)
    if (lowercase(Agent::registry[agent]->get_name()) == lowercase(class_name))
      return Agent::registry[agent]->new_Archive();

  throw Error (InvalidParam, "Pulsar::Archive::new_Archive",
               "no Agent named '" + class_name + "'");
}

void Pulsar::Archive::agent_report ()
{
  Agent::report ();
}

void Pulsar::Archive::agent_list ()
{
  cout << Agent::get_list () << endl;
}

//! Return the number of extensions available
unsigned Pulsar::Archive::get_nextension () const
{
  clean_dangling (extension);
  return extension.size ();
}

Pulsar::Archive::Extension::Extension (const char* _name)
{
  extension_name = _name;
}

Pulsar::Archive::Extension::~Extension ()
{}

string Pulsar::Archive::Extension::get_extension_name () const
{
  return extension_name;
}

string Pulsar::Archive::Extension::get_short_name () const
{
  return extension_name;
}

/*! Derived classes need only define this method, as the non-const version
  implemented by the Archive base class simply calls this method. */
const Pulsar::Archive::Extension*
Pulsar::Archive::get_extension (unsigned iext) const
{
  if ( iext >= extension.size() )
    throw Error (InvalidRange, "Pulsar::Archive::get_extension",
                 "index=%d >= nextension=%d", iext, extension.size());

  if ( !extension[iext] )
    return 0;

  return extension[iext];
}

/*! Simply calls get_extension const */
Pulsar::Archive::Extension*
Pulsar::Archive::get_extension (unsigned iext)
{
  if ( iext >= extension.size() )
    throw Error (InvalidRange, "Pulsar::Archive::get_extension",
                 "index=%d >= nextension=%d", iext, extension.size());

  if ( !extension[iext] )
    return 0;

  return extension[iext];
}

/*! Derived classes need only define this method, as the non-const version
  implemented by the Archive base class simply calls this method. */
void Pulsar::Archive::add_extension (Extension* ext)
{

  if (dynamic_cast<Pulsar::IntegrationOrder*>(ext))

    // Test to see if the archive has already had its Integrations
    // re-ordered. At the moment the code only supports doing this
    // once, otherwise each IntegrationOrder class would have to know
    // how to convert from all of its fellows and this would get very
    // complicated... AWH 29/12/2003

    for (unsigned i = 0; i < extension.size(); i++)
      if (dynamic_cast<Pulsar::IntegrationOrder*>(extension[i].get()))
        throw Error(InvalidState, "Archive::add_extension",
                    "Stacking IntegrationOrder Extensions is not supported");

  // If we reach here, there are no IntegrationOrder conflicts.
  // Continue as normal... AWH 29/12/2003

  unsigned index = find (extension, ext);

  if (index < extension.size())
  {
    if (verbose == 3)
      cerr << "Pulsar::Archive::add_extension replacing" << endl;
    extension[index] = ext;
  }
  else
  {
    if (verbose == 3)
      cerr << "Pulsar::Archive::add_extension appending "
	   << ext->get_extension_name() << endl;
    extension.push_back(ext);
  }
}

/* Unless a Receiver Extension is present, this method assumes that the
   signal basis is linear. */
Signal::Basis Pulsar::Archive::get_basis () const
{
  const Receiver* receiver = get<Receiver>();
  if (receiver)
    return receiver->get_basis();
  else
    return Signal::Linear;
}

void Pulsar::Archive::refresh()
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::refresh" << endl;

  IntegrationManager::resize(0);

  load_header (__load_filename.c_str());
}

void Pulsar::Archive::update()
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::update" << endl;

  load_header (__load_filename.c_str());
}


//! Return a pointer to the Profile
/*!
  \param sub the index of the requested Integration
  \param pol the index of the requested polarization
  \param chan the index of the requested frequency channel
  \return pointer to Profile instance
*/
Pulsar::Profile*
Pulsar::Archive::get_Profile (unsigned sub, unsigned pol, unsigned chan)
{
  return get_Integration (sub) -> get_Profile (pol, chan);
}

const Pulsar::Profile*
Pulsar::Archive::get_Profile (unsigned sub, unsigned pol, unsigned chan) const
{
  return get_Integration (sub) -> get_Profile (pol, chan);
}

const Pulsar::Parameters* Pulsar::Archive::get_ephemeris () const
{
  if (!ephemeris)
    throw Error (InvalidState,
                 "Pulsar::Archive::get_ephemeris", "no ephemeris");

  return ephemeris;
}

const Pulsar::Predictor* Pulsar::Archive::get_model () const
{
  if (!model)
    throw Error (InvalidState,
                 "Pulsar::Archive::get_model", "no model");

  return model;
}

MJD Pulsar::Archive::start_time() const
{
  if (get_nsubint() < 1)
    throw Error (InvalidState, "Pulsar::Archive::start_time",
                 "no Integrations");

  return get_Integration(0) -> get_start_time();
}

MJD Pulsar::Archive::end_time() const
{
  if (get_nsubint() < 1)
    throw Error (InvalidState, "Pulsar::Archive::end_time",
                 "no Integrations");

  return get_Integration(get_nsubint()-1) -> get_end_time();
}

double Pulsar::Archive::integration_length() const
{
  double total = 0.0;

  for (unsigned i = 0; i < get_nsubint(); i++)
  {
    total = total + get_Integration(i) -> get_duration();
  }

  return total;

}

bool Pulsar::Archive::type_is_cal () const
{
  return
    get_type() == Signal::PolnCal ||
    get_type() == Signal::FluxCalOn ||
    get_type() == Signal::FluxCalOff;
}

void Pulsar::Archive::uniform_weight (float new_weight)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> uniform_weight (new_weight);
}

bool Pulsar::Archive::zero_phase_aligned () const
{
  if (!model)
    return false;

  unsigned nsubint = get_nsubint ();
  for (unsigned isub=0; isub < nsubint; isub++)
    if (!get_Integration(isub)->zero_phase_aligned)
    {
      if (verbose > 2)
        cerr << "Pulsar::Archive::zero_phase_aligned false on isub="
        << isub << endl;
      return false;
    }

  if (verbose > 2)
    cerr << "Pulsar::Archive::zero_phase_aligned true" << endl;
  return true;
}

