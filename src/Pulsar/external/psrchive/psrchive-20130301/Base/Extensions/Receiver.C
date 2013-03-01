/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Receiver.h"

#include "Pulsar/Receiver_Native.h"
#include "Pulsar/Receiver_Field.h"
#include "Pulsar/Receiver_Linear.h"

#include "TextInterface.h"
#include "Pauli.h"

using namespace std;

/*! If the current state is not of the specified StateType, a new state
  will be created, copied, and installed */
template<class StateType>
StateType* Pulsar::Receiver::get () const
{
  Receiver* thiz = const_cast<Receiver*>( this );
  StateType* st = dynamic_cast<StateType*>( thiz->state.get() );

  if (!st) {
    st = new StateType;
    st -> copy (state);
    thiz->state = st;
  }
  
  return st;  
}

//! Default constructor
Pulsar::Receiver::Receiver () : Extension ("Receiver")
{
  tracking_mode = Feed;
  name = "unknown";

  state = new Native;

  basis_corrected = false;
  projection_corrected = false;

  atten_a = 0.0;
  atten_b = 0.0;
  
  nrcvr = 0;
}

//! Copy constructor
Pulsar::Receiver::Receiver (const Receiver& ext) : Extension ("Receiver")
{
  operator = (ext);
}

//! Operator =
const Pulsar::Receiver&
Pulsar::Receiver::operator= (const Receiver& ext)
{
  tracking_mode = ext.tracking_mode;
  tracking_angle = ext.tracking_angle;

  name = ext.name;
  state = new Native;
  state->copy(ext.state);

  basis_corrected = ext.basis_corrected;
  projection_corrected = ext.projection_corrected;

  atten_a = ext.atten_a;
  atten_b = ext.atten_b;

  return *this;
}

//! Destructor
Pulsar::Receiver::~Receiver ()
{
}

string Pulsar::Receiver::get_tracking_mode_string() const
{
  switch (tracking_mode) {
  case Feed:
    return "FA";
  case Celestial:
    return "CPA";
  case Galactic:
    return "GPA";
  }
  return "unknown";
}

void Pulsar::Receiver::set_basis (Signal::Basis basis)
{
  get<Native>()->set_basis (basis);
}

/*! If this method is called, then any previous changes due to
 set_X_offset, set_Y_offset, or set_field_orientation will be
 reset. */
void Pulsar::Receiver::set_orientation (const Angle& angle)
{
  get<Native>()->set_orientation (angle);
}

/*! If this method is called, then any changes due to set_Y_offset
  will be reset. */
void Pulsar::Receiver::set_hand (Signal::Hand hand)
{
  get<Native>()->set_hand (hand);
}

//! Set the phase of the reference source
void Pulsar::Receiver::set_reference_source_phase (const Angle& angle)
{
  get<Native>()->set_reference_source_phase (angle);
}

void Pulsar::Receiver::set_field_orientation (const Angle& angle)
{
  get<Field>()->set_field_orientation (angle);
}

Angle Pulsar::Receiver::get_field_orientation () const
{
  return get<Field>()->get_field_orientation ();
}

/*! If this method is called, then any previous changes due to
  set_orientation or set_field_orientation will be reset. */
void Pulsar::Receiver::set_X_offset (const Angle& offset)
{
  get<Linear>()->set_X_offset (offset);
}

Angle Pulsar::Receiver::get_X_offset () const
{
  return get<Linear>()->get_X_offset ();
}


/*
  \param offset either 0 or +/- 180 degrees
*/
void Pulsar::Receiver::set_Y_offset (const Angle& offset)
{ 
  get<Linear>()->set_Y_offset (offset);
}

Angle Pulsar::Receiver::get_Y_offset () const
{
  return get<Linear>()->get_Y_offset ();
}

/*! In the linear basis, the noise diode must illuminate both receptors
    equally.  Therefore, there are only two valid orientations:

    \param offset either 0 or +/- 90 degrees
*/
void Pulsar::Receiver::set_calibrator_offset (const Angle& offset)
{
  get<Linear>()->set_calibrator_offset (offset);
}

Angle Pulsar::Receiver::get_calibrator_offset () const
{ 
  return get<Linear>()->get_calibrator_offset ();
}

void Pulsar::Receiver::set_basis_corrected (bool val)
{
  // cerr << "Pulsar::Receiver::set_basis_corrected " << val << endl;
  basis_corrected = val;
}

static string match_indent = "\n\t";

bool Pulsar::Receiver::match (const Receiver* receiver, string& reason) const
{
  if (!receiver)
  {
    reason += match_indent + "no Receiver";
    return false;
  }

  bool result = true;

  if (get_basis() != receiver->get_basis())
  {
    result = false;
    reason += match_indent + "basis mismatch: "
      + tostring( get_basis() ) + " != " 
      + tostring( receiver->get_basis() );
  }

  if (get_hand() != receiver->get_hand())
  {
    result = false;
    reason += match_indent + "hand mismatch: "
      + tostring( get_hand() ) + " != " 
      + tostring( receiver->get_hand() );
  }

  if (get_orientation() != receiver->get_orientation())
  {
    result = false;
    reason += match_indent + "orientation mismatch: "
      + tostring( get_orientation() ) + " != " 
      + tostring( receiver->get_orientation() );
  }
  return result;
}



Stokes<double> Pulsar::Receiver::get_reference_source () const
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::Receiver::get_reference_source phase="
	 << get_reference_source_phase().getDegrees() << " deg" << endl;

  /* The following line provides a basis-independent representation of a
   reference source that illuminates both receptors equally and in phase. */
  Quaternion<double,Hermitian> ideal (1,0,1,0);

  /* .. and the following is the reference source */
  double phi = get_reference_source_phase().getRadians();
  Quaternion<double,Hermitian> cal (1,0,cos(phi),sin(phi));

  if (Archive::verbose == 3)
    cerr << "Pulsar::Receiver::get_reference_source cal=" << cal << endl;

  return standard (cal);
}

// Text interface to a receiver object
class Pulsar::Receiver::Interface : public TextInterface::To<Receiver>
{
public:
  Interface( Receiver *s_instance = NULL )
  {
    if (s_instance)
      set_instance( s_instance );

    add( &Receiver::get_name,
	 &Receiver::set_name,
	 "name", "Receiver name" );
    
    add( &Receiver::get_basis,
	 &Receiver::set_basis,
	 "basis", "Basis of receptors" );
    
    add( &Receiver::get_hand,
	 &Receiver::set_hand,
	 "hand", "Hand of receptor basis" );
    
    add( &Receiver::get_field_orientation,
	 &Receiver::set_field_orientation,
	 "sa", "Symmetry angle of receptor basis" );

    add( &Receiver::get_reference_source_phase,
	 &Receiver::set_reference_source_phase,
	 "rph", "Reference source phase" );

    add( &Receiver::get_basis_corrected,
	 &Receiver::set_basis_corrected,
	 "fdc", "Receptor basis corrected" );

    add( &Receiver::get_projection_corrected,
	 &Receiver::set_projection_corrected,
	 "prc", "Receptor projection corrected" );

    add( &Receiver::get_tracking_angle,
	 &Receiver::set_tracking_angle,
	 "ta", "Tracking angle of feed" );
  }
  
  virtual std::string get_interface_name() const { return "ReceiverTI"; }
};

TextInterface::Parser* Pulsar::Receiver::get_interface()
{
  return new Receiver::Interface( this );
}
