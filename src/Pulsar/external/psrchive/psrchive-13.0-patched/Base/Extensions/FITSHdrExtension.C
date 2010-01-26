/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/Pulsar.h"

#include "strutil.h"

using namespace std;

#define unset "UNSET"

//! Default constructor
Pulsar::FITSHdrExtension::FITSHdrExtension ()
  : Extension ("FITSHdrExtension")
{
  hdrver = unset;
  major_version = minor_version = 0;

  creation_date = unset;
  coordmode = unset;
  equinox  = unset;
  trk_mode = unset;
  stt_date = unset;
  stt_time = unset;

  stt_lst = 0.0;
  obsfreq = 0.0;

  obs_mode = unset;
  ra = dec = unset;
  stt_crd1 = stt_crd2 = stp_crd1 = stp_crd2 = unset;

  bpa = bmaj = bmin = 0.0;
}

//! Copy constructor
Pulsar::FITSHdrExtension::FITSHdrExtension (const FITSHdrExtension& extension)
  : Extension ("FITSHdrExtension")
{
  operator = (extension);
}

TextInterface::Parser* Pulsar::FITSHdrExtension::get_interface()
{
  return new Interface( this );
}

void Pulsar::FITSHdrExtension::set_coordmode (const string& mode)
{
  if (mode == "EQUAT")
    coordmode = "J2000";

  else if (mode == "Gal")
    coordmode = "GAL";

  else if (mode == "J2000" ||
           mode == "GAL" ||
           mode == "ECLIP" ||
           mode == "AZEL" ||
           mode == "HADEC")
    coordmode = mode;

  else
    coordmode = unset;
}

void Pulsar::FITSHdrExtension::set_date_str (const string& date)
{
  creation_date = date;
}

void Pulsar::FITSHdrExtension::get_coord_string (const sky_coord& coordinates,
    string& coord1,
    string& coord2) const
{
  AnglePair newcoord;

  if (coordmode == "J2000" || coordmode == unset)
  {
    newcoord = coordinates.getRaDec();

    coord1 = newcoord.angle1.getHMS();
    coord2 = newcoord.angle2.getDMS();

    return;
  }

  if (coordmode == "GAL")
  {
    newcoord = coordinates.getGalactic();

    coord1 = tostring (newcoord.angle1.getDegrees());
    coord2 = tostring (newcoord.angle2.getDegrees());

    return;
  }

  warning << "FITSHdrExtension::get_coord_string WARNING COORD_MD="
	  << coordmode << " not implemented" << endl;
}

void Pulsar::FITSHdrExtension::set_obsfreq( double set_obsfreq )
{
  obsfreq = set_obsfreq;
}

double Pulsar::FITSHdrExtension::get_obsfreq () const
{
  return obsfreq;
}

std::string Pulsar::FITSHdrExtension::get_hdrver () const
{
  return hdrver;
}

std::string Pulsar::FITSHdrExtension::get_creation_date () const
{
  return creation_date;
}

std::string Pulsar::FITSHdrExtension::get_coordmode () const
{
  return coordmode;
}

std::string Pulsar::FITSHdrExtension::get_equinox () const
{
  return equinox;
}

std::string Pulsar::FITSHdrExtension::get_trk_mode () const
{
  return trk_mode;
}

void Pulsar::FITSHdrExtension::set_stt_imjd (int imjd)
{
  start_time = MJD (imjd, start_time.get_secs(), start_time.get_fracsec());
}

void Pulsar::FITSHdrExtension::set_stt_smjd (int smjd)
{
  start_time = MJD (start_time.intday(), smjd, start_time.get_fracsec());
}

void Pulsar::FITSHdrExtension::set_stt_offs (double offs)
{
  start_time = MJD (start_time.intday(), start_time.get_secs(), offs);
}

void Pulsar::FITSHdrExtension::set_start_time (const MJD& mjd)
{
  start_time = mjd;
}

// void Pulsar::FITSHdrExtension::set_end_coordinates( sky_coord _end_coord )
// {
//   end_coord = _end_coord;
// }


// sky_coord Pulsar::FITSHdrExtension::get_end_coordinates () const
// {
//   return end_coord;
// }
// 
// 
// string Pulsar::FITSHdrExtension::get_stp_crd1 () const
// {
//   sky_coord coord = get_end_coordinates();
// 
//   int hours, mins;
//   double seconds;
//   coord.ra().getHMS( hours, mins, seconds );
// 
//   stringstream result;
//   result << hours << ":" << mins << ":" << seconds;
// 
//   return result.str();
// }
// 
// 
// string Pulsar::FITSHdrExtension::get_stp_crd2 () const
// {
//   sky_coord coord = get_end_coordinates();
// 
//   int hours, mins;
//   double seconds;
//   coord.ra().getDMS( hours, mins, seconds );
// 
//   stringstream result;
//   result << hours << ":" << mins << ":" << seconds;
// 
//   return result.str();
// }



// string Pulsar::FITSHdrExtension::get_stt_crd1 () const 
// {
//   return stt_crd1;
// }
// 
// 
// string Pulsar::FitsHdrExtension::get_stt_crd2 () const
// {
//   return stt_crd2;
// }
// 
// 
// string Pulsar::FITSHdrExtension::get_ra_HMS () const
// {
//   sky_coord coords;
// 
//   if( get_coordmode() == "GAL" )
//   {
//     coords = coordinates.getGalactic();
//   }
//   else if( get_coord_mode() == "J2000" )
//   {
//     coords = coordinates;
//   }
// 
//   return coords.ra().getHMS();
// }
// 
// 
// string Pulsar::FITSHdrExtension::get_dec_DMS () const
// {
//   sky_coord coords;
// 
//   if( get_coordmode() == "GAL" )
//   {
//     coords = coordinates.getGalactic();
//   }
//   else if( get_coord_mode() == "j2000" )
//   {
//     coords = coordinates;
//   }
// 
//   return coords.dec().getDMS();
// }






