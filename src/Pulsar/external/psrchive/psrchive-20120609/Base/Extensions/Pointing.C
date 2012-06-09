/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/Pointing.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Telescope.h"
#include "Horizon.h"
#include "strutil.h"



using namespace std;




Pulsar::Pointing::Pointing () : Extension ("Pointing")
{
}

Pulsar::Pointing::Pointing (const Pointing& extension)
  : Extension ("Pointing")
{
  operator = (extension);
}

const Pulsar::Pointing&
Pulsar::Pointing::operator= (const Pointing& pointing)
{
  local_sidereal_time = pointing.local_sidereal_time;
  right_ascension     = pointing.right_ascension;
  declination         = pointing.declination;
  galactic_longitude  = pointing.galactic_longitude;
  galactic_latitude   = pointing.galactic_latitude;
  feed_angle          = pointing.feed_angle;
  position_angle      = pointing.position_angle;
  parallactic_angle   = pointing.parallactic_angle;
  telescope_azimuth   = pointing.telescope_azimuth;
  telescope_zenith    = pointing.telescope_zenith;

  return *this;
}

const Pulsar::Pointing&
Pulsar::Pointing::operator += (const Pointing& pointing)
{
  local_sidereal_time += pointing.local_sidereal_time;
  right_ascension     += pointing.right_ascension;
  declination         += pointing.declination;
  galactic_longitude  += pointing.galactic_longitude;
  galactic_latitude   += pointing.galactic_latitude;
  feed_angle          += pointing.feed_angle;
  position_angle      += pointing.position_angle;
  parallactic_angle   += pointing.parallactic_angle;
  telescope_azimuth   += pointing.telescope_azimuth;
  telescope_zenith    += pointing.telescope_zenith;

  return *this;
}

Pulsar::Pointing::~Pointing ()
{
}


static const double seconds_per_day = 24.0 * 60.0 * 60.0;

void Pulsar::Pointing::set_local_sidereal_time (double seconds)
{
  local_sidereal_time = Estimate<double> (seconds*M_PI/seconds_per_day, 1.0);
}

double Pulsar::Pointing::get_local_sidereal_time () const
{
  return local_sidereal_time.get_Estimate().val * seconds_per_day / M_PI;
}

static void setmean (MeanRadian<double>& value, const Angle& angle)
{
  value = Estimate<double> (angle.getRadians(), 1.0);
}

static Angle getmean (const MeanRadian<double>& value)
{
  return value.get_Estimate().val;
}

void Pulsar::Pointing::set_right_ascension (const Angle& angle)
{
  setmean (right_ascension, angle);
}

Angle Pulsar::Pointing::get_right_ascension () const
{
  return getmean (right_ascension);
}


void Pulsar::Pointing::set_declination (const Angle& angle)
{
  setmean (declination, angle);
}

Angle Pulsar::Pointing::get_declination () const
{
  return getmean (declination);
}


void Pulsar::Pointing::set_galactic_longitude (const Angle& angle)
{
  setmean (galactic_longitude, angle);
}

Angle Pulsar::Pointing::get_galactic_longitude () const
{
  return getmean (galactic_longitude);
}


void Pulsar::Pointing::set_galactic_latitude (const Angle& angle)
{
  setmean (galactic_latitude, angle);
}

Angle Pulsar::Pointing::get_galactic_latitude () const
{
  return getmean (galactic_latitude);
}


void Pulsar::Pointing::set_feed_angle (const Angle& angle)
{
  setmean (feed_angle, angle);
}

Angle Pulsar::Pointing::get_feed_angle () const
{
  return getmean (feed_angle);
}


void Pulsar::Pointing::set_position_angle (const Angle& angle)
{
  setmean (position_angle, angle);
}

Angle Pulsar::Pointing::get_position_angle () const
{
  return getmean (position_angle);
}


void Pulsar::Pointing::set_parallactic_angle (const Angle& angle)
{
  setmean (parallactic_angle, angle);
}

Angle Pulsar::Pointing::get_parallactic_angle () const
{
  return getmean (parallactic_angle);
}


void Pulsar::Pointing::set_telescope_azimuth (const Angle& angle)
{
  setmean (telescope_azimuth, angle);
}

Angle Pulsar::Pointing::get_telescope_azimuth () const
{
  return getmean (telescope_azimuth);
}


void Pulsar::Pointing::set_telescope_zenith (const Angle& angle)
{
  setmean (telescope_zenith, angle);
}

Angle Pulsar::Pointing::get_telescope_zenith () const
{
  return getmean (telescope_zenith);
}


void Pulsar::Pointing::update (const Integration* subint)
{
  const Archive* archive = get_parent (subint);

  if (!archive)
    throw Error (InvalidState, "Pulsar::Pointing::update",
	         "Integration has no parent Archive");
  update(subint, archive);
}


/*! Based on the epoch of the Integration, uses slalib to re-calculate
  the following Pointing attributes: local_sidereal_time, parallactic_angle, 
  telescope_azimuth, and telescope_zenith. */
void Pulsar::Pointing::update (const Integration* subint, 
    const Archive *archive)
{

  const Telescope* telescope = archive->get<Telescope>();

  if (!telescope)
    throw Error (InvalidState, "Pulsar::Pointing::update",
	         "parent Archive has no telescope Extension");

  sky_coord coord( get_right_ascension(), get_declination() );

  if (Integration::verbose)
    cerr << 
      "Pulsar::Pointing::update before:\n"
      "  lst=" << get_local_sidereal_time()/3600.0 << " hours\n"
      "   az=" << get_telescope_azimuth().getDegrees() << " deg\n"
      "  zen=" << get_telescope_zenith().getDegrees() << " deg\n"
      " para=" << get_parallactic_angle().getDegrees() << " deg\n"
      " posa=" << get_position_angle().getDegrees() << " deg\n"
      "Pulsar::Pointing::update using:\n"
      " r.a.=" << get_right_ascension().getDegrees() << " deg\n"
      " dec.=" << get_declination().getDegrees() << " deg\n"
      " pos.=" << coord << "\n"
      "Pulsar::Pointing::update parent:\n"
      " r.a.=" << archive->get_coordinates().ra().getDegrees() << " deg\n"
      " dec.=" << archive->get_coordinates().dec().getDegrees() << " deg"
	 << endl;


  Horizon horizon;

  horizon.set_source_coordinates( coord );
  horizon.set_observatory_latitude( telescope->get_latitude().getRadians() );
  horizon.set_observatory_longitude( telescope->get_longitude().getRadians() );
  horizon.set_epoch( subint->get_epoch() );

  double rad2sec = 3600.0*12.0/M_PI;
  set_local_sidereal_time( horizon.get_local_sidereal_time()*rad2sec );
  set_telescope_azimuth( horizon.get_azimuth() );
  set_telescope_zenith( horizon.get_zenith() );
  set_parallactic_angle( horizon.get_parallactic_angle() );

  set_position_angle( get_feed_angle() + get_parallactic_angle() );

  if (Integration::verbose)
    cerr << "Pulsar::Pointing::update after:\n"  
         "  lst=" << get_local_sidereal_time()/3600.0 << " hours\n"
         "   az=" << get_telescope_azimuth().getDegrees() << " deg\n"
         "  zen=" << get_telescope_zenith().getDegrees() << " deg\n"
         " para=" << get_parallactic_angle().getDegrees() << " deg\n"
         " posa=" << get_position_angle().getDegrees() << " deg" << endl;

}


/*! Calculates the mean of the Pointing attributes. */
void Pulsar::Pointing::integrate (const Integration* subint)
{ 
  const Pointing* useful = subint->get<Pointing>();

  if (!useful) {
    if (Integration::verbose)
      cerr << "Pulsar::Pointing::integrate subint has no Pointing" << endl;
    return;
  }

  if (Integration::verbose)
    cerr << "Pulsar::Pointing::integrate other Pointing" << endl;

  operator += (*useful);
}


//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* Pulsar::Pointing::get_interface()
{
  return new Interface( this );
}

