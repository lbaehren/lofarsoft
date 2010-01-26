/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>

#include <Pulsar/Telescope.h>
#include <Pulsar/FITSHdrExtension.h>
#include <Pulsar/ObsExtension.h>
#include <Pulsar/ITRFExtension.h>
#include <Pulsar/Backend.h>
#include <Pulsar/Receiver.h>
#include <Pulsar/WidebandCorrelator.h>
#include <Pulsar/FluxCalibratorExtension.h>
#include <Pulsar/ProcHistory.h>
#include <Pulsar/Parameters.h>
#include <Pulsar/Predictor.h>
#include <Pulsar/Pointing.h>
#include <TextInterface.h>
#include <Pulsar/Passband.h>
#include <Pulsar/PolnCalibratorExtension.h>
#include <Pulsar/DigitiserStatistics.h>
#include <Pulsar/DigitiserCounts.h>
#include <Pulsar/FITSSUBHdrExtension.h>
#include <Pulsar/CalInfoExtension.h>
#include <Pulsar/TapeInfo.h>

#include <dirutil.h>
#include <strutil.h>
#include <tostring.h>
#include <Angle.h>
#include <table_stream.h>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

#include <unistd.h>

#include <utc.h>
#include <FITSUTC.h>

#include "tempo++.h"

/**
 * For the sake of cleanliness, readability and common sense, I decided to concede that the text interfaces
 * don't provide the facilities requested of the old vap. So if you are going to try to clean up this code
 * be forewarned that my attempt to clean the code resulted in far more code to deal with all the exceptions
 * that arise, as well as a great deal of lost time. KISS
 * 
 * David Smith nopeer@gmail.com
 **/



using namespace std;
using namespace Pulsar;




////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
////////////////////////////////////////////////////////////////////////////////////////////


bool ephemmode = false;
bool polycmode = false;
bool verbose = false;
bool show_extensions = false;
bool hide_headers = false;
vector< string > commands;
vector< vector< string > > results;
vector< string > current_row;
bool new_new_vap = false;
string meta_filename = "";

bool neat_table = false;
table_stream ts(&cout);

bool full_paths = false;


////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS FOR RETREIVING OBSERVATION PARAMETERS
////////////////////////////////////////////////////////////////////////////////////////////



string get_name( Reference::To< Archive > archive )
{
  return archive->get_source();
}

string get_stime( Reference::To< Archive > archive )
{
  return string("TODO");
}

string get_etime( Reference::To< Archive > archive )
{
  return string("TODO" );
}

string get_length( Reference::To< Archive > archive )
{
  Reference::To<FITSHdrExtension> hdr_ext = archive->get<FITSHdrExtension>();

  if ( hdr_ext )
  {
    const string obs_mode = hdr_ext->get_obs_mode();

    if ( obs_mode == "SEARCH" || obs_mode == "SRCH" )
    {
      Reference::To<FITSSUBHdrExtension> sub_hdr_ext =
        archive->get<FITSSUBHdrExtension>();

      if ( sub_hdr_ext )
      {
        const double tsamp = sub_hdr_ext->get_tsamp();
        const int nrows = sub_hdr_ext->get_nrows();
        const int nsblk = sub_hdr_ext->get_nsblk();
        return tostring( tsamp * nrows * nsblk, 3, ios::fixed );
      }
    }
  }

  return tostring( archive->integration_length(), 3, ios::fixed );
}

string get_nbin_obs( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
  {
    ProcHistory::row first;
    first = ( *(ext->rows.begin()) );
    result = tostring( first.nbin );
  }

  return result;
}

string get_nchan_obs( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
  {
    ProcHistory::row first;
    first = ( *(ext->rows.begin()) );
    result = tostring( first.nchan );
  }

  return result;
}

string get_npol_obs( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
  {
    ProcHistory::row first;
    first = ( *(ext->rows.begin()) );
    result = tostring( first.npol );
  }

  return result;
}

string get_nsub_obs( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
  {
    ProcHistory::row first;
    first = ( *(ext->rows.begin()) );
    if( first.nsub == 0 )
      result = "*";
    else
      result = tostring( first.nsub );
  }

  return result;
}

string get_dm( Reference::To< Archive > archive )
{
  return tostring( archive->get_dispersion_measure(), 6, ios::fixed );
}

string get_rm( Reference::To< Archive > archive )
{
  return tostring( archive->get_rotation_measure(), 3, ios::fixed );
}

string get_state( Reference::To< Archive > archive )
{
  return tostring( archive->get_state() );
}


string get_scale( Reference::To< Archive > archive )
{
  return tostring( archive->get_scale() );
}


string get_type( Reference::To< Archive > archive )
{
  return tostring( archive->get_type() );
}


string get_dmc( Reference::To< Archive > archive )
{
  return tostring( archive->get_dedispersed() );
}


string get_rmc( Reference::To< Archive > archive )
{
  return tostring( archive->get_faraday_corrected() );
}


string get_polc( Reference::To< Archive > archive )
{
  return tostring( archive->get_poln_calibrated() );
}


string get_freq( Reference::To< Archive > archive )
{
  return tostring( archive->get_centre_frequency(), 3, ios::fixed );
}


string get_profile_centre_frequency( Reference::To< Archive > archive )
{
  double freq = archive->get_Profile(0, 0, 0)->get_centre_frequency();
  return tostring( freq, 3, ios::fixed );
}


string get_freq_pa( Reference::To< Archive > archive )
{
  string result = "UNDEF";

  if (archive->get_faraday_corrected())
    result = get_freq(archive);
  else if (archive->get_nchan() == 1)
    result = get_profile_centre_frequency(archive);

  return result;
}


string get_freq_phs( Reference::To< Archive > archive )
{
  string result = "UNDEF";

  if (archive->get_dedispersed())
    result = get_freq(archive);
  else if (archive->get_nchan() == 1)
    result = get_profile_centre_frequency(archive);

  return result;
}


string get_bw( Reference::To< Archive > archive )
{
  return tostring( archive->get_bandwidth(), 3, ios::fixed );
}


string get_intmjd( Reference::To< Archive > archive )
{
  string intmjd = "*";

  try 
  {
    MJD t = archive->start_time();
    intmjd = tostring<int>( t.intday() );
  }
  catch( Error e ) {}

  return intmjd;
}


string get_fracmjd( Reference::To< Archive > archive )
{
  string fracmjd;

  try
  {
    MJD t = archive->start_time();
    fracmjd = tostring( t.fracday(), 14 );
  }
  catch( Error e ) {}

  return fracmjd;
}

string get_mjd( Reference::To< Archive > archive )
{
  string mjd;

  try
  {
    MJD epoch;

    if (archive->get_type() == Signal::Calibrator)
    {
      CalibratorExtension* extension = archive->get<CalibratorExtension>();
      if (extension)
	epoch = extension->get_epoch();
    }
    else
      epoch = archive->start_time();

    mjd = tostring( epoch, 19 );
  } 
  catch( Error e ) {}

  return mjd;
}

string get_parang( Reference::To< Archive > archive )
{
  ostringstream result;

  int nsubs = archive->get_nsubint();

  if( nsubs != 0 )
  {
    Reference::To< Integration > integration = archive->get_Integration( nsubs / 2 );
    if( integration )
    {
      Reference::To< Pointing > ext = integration->get<Pointing>();

      if( ext )
      {
        result << ext->get_parallactic_angle().getDegrees();
      }
    }
  }

  return result.str();
}


string get_tsub( Reference::To< Archive > archive )
{
  string result;

  int nsubs = archive->get_nsubint();
  if( nsubs != 0 )
  {
    Reference::To< Integration > first_int = archive->get_first_Integration();
    if( first_int )
      result = tostring( first_int->get_duration(), 6 );
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// OBSERVER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_observer( Reference::To<Archive> archive )
{
  Reference::To< ObsExtension > ext = archive->get<ObsExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_observer();
}



string get_projid( Reference::To<Archive> archive )
{
  Reference::To<ObsExtension> ext = archive->get<ObsExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_project_ID();
}


////////////////////////////////////////////////////////////////////////////////////////////
// RECEIVER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


string get_rcvr( Reference::To<Archive> archive )
{
  Reference::To<Receiver> ext = archive->get<Receiver>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_name();
}

string get_nrcpt( Reference::To<Archive> archive )
{
  Reference::To<Receiver> ext = archive->get<Receiver>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_nrcvr() );
}


string get_fac( Reference::To<Archive> archive )
{
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    return "UNDEF";
  else
    return tostring( recv->get_basis_corrected() );
}

string get_fa_req( Reference::To<Archive> archive )
{
  string result = "UNDEF";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( recv )
  {
    Angle a = recv->get_tracking_angle();
    result = tostring<Angle>( a );
  }

  return result;
}

string get_fda( Reference::To<Archive> archive )
{
  const Pulsar::Pointing* ext = 0;

  if( archive->get_nsubint() )
    ext = archive->get_Integration(0)->get<Pulsar::Pointing>();

  if( ext )
    return tostring( ext->get_feed_angle().getDegrees() );
  else
    return "UNDEF";
}


string get_basis( Reference::To<Archive> archive )
{
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    return "UNDEF";
  else
    return tostring( recv->get_basis() );
}

string get_fd_hand( Reference::To<Archive> archive )
{
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    return "UNDEF";
  else
    return tostring( recv->get_hand() );
}

string get_fd_mode( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( recv )
  {
    Receiver::Tracking mode;
    mode = recv->get_tracking_mode();
    switch( mode )
    {
    case Receiver::Feed:
      result = "FA";
      break;
    case Receiver::Celestial:
      result = "CPA";
      break;
    case Receiver::Galactic:
      result = "GPA";
      break;
    };
  }

  return result;
}

string get_fd_xyph( Reference::To<Archive> archive )
{
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    return "UNDEF";
  else
    return tostring( recv->get_reference_source_phase() );
}

string get_oa( Reference::To<Archive> archive )
{
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    return "UNDEF";
  else
    return tostring( recv->get_orientation() );
}

string get_fd_sang( Reference::To<Archive> archive )
{
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    return "UNDEF";
  else
    return tostring( recv->get_field_orientation() );
}

string get_xoffset( Reference::To<Archive> archive )
{
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    return "UNDEF";
  else
    return tostring( recv->get_X_offset() );
}

string get_yo( Reference::To<Archive> archive )
{
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    return "UNDEF";
  else
    return tostring( recv->get_Y_offset() );
}

string get_co( Reference::To<Archive> archive )
{
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    return "UNDEF";
  else
    return tostring( recv->get_calibrator_offset() );
}


////////////////////////////////////////////////////////////////////////////////////////////
// FILE AND TELESCOPE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_ant_x( Reference::To< Archive > archive )
{
  Reference::To<ITRFExtension> ext = archive->get<ITRFExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->ant_x, 3, ios::fixed );
}

string get_ant_y( Reference::To< Archive > archive )
{
  Reference::To<ITRFExtension> ext = archive->get<ITRFExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->ant_y, 3, ios::fixed );
}

string get_ant_z( Reference::To< Archive > archive )
{
  Reference::To<ITRFExtension> ext = archive->get<ITRFExtension>();
  
  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->ant_z, 3, ios::fixed );
}

string get_telescop( Reference::To< Archive > archive )
{
  return archive->get_telescope();
}

string get_date( Reference::To< Archive > archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_creation_date();
}


string get_asite( Reference::To< Archive > archive )
{
  ostringstream s;
  s << Tempo::code(archive->get_telescope());

  return s.str();
}

string get_file( Reference::To<Archive > archive )
{
  string result = "";

  Reference::To<TapeInfo> ext = archive->get<TapeInfo>();
  if( ext )
  {
    int file_num = ext->get_file_number();

    if( file_num != -1 )
      result = tostring<int>( file_num );
  }

  return result;
}

string get_tlabel( Reference::To<Archive > archive )
{
  string result = "";

  Reference::To<TapeInfo> ext = archive->get<TapeInfo>();
  if( ext )
    result = ext->get_tape_label();

  return result;
}



////////////////////////////////////////////////////////////////////////////////////////////
// BACKEND FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


string get_backend( Reference::To< Archive > archive )
{
  Reference::To<Backend> ext = archive->get<Backend>();
  if( !ext )
    ext = archive->get<WidebandCorrelator>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_name();
}

string get_be_dcc( Reference::To< Archive > archive )
{
  Reference::To<Backend> ext = archive->get<Backend>();
  if( !ext )
    ext = archive->get<WidebandCorrelator>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_downconversion_corrected() );
}

string get_be_phase( Reference::To< Archive > archive )
{
  Reference::To<Backend> ext = archive->get<Backend>();
  if( !ext )
    ext = archive->get<WidebandCorrelator>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_argument() );
}

string get_beconfig( Reference::To< Archive > archive )
{
  Reference::To<WidebandCorrelator> ext = archive->get<WidebandCorrelator>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_config();
}



string get_be_delay( Reference::To< Archive > archive )
{
  Reference::To<Backend> ext = archive->get<Backend>();
  if( !ext )
    ext = archive->get<WidebandCorrelator>();
  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_delay(), 14 );
}



string get_period( Reference::To<Archive> archive )
{
  return tostring( archive->get_Integration(0)->get_folding_period(), 14 );
}


string get_tcycle( Reference::To< Archive > archive )
{
  string result;
  Reference::To<WidebandCorrelator> ext = archive->get<WidebandCorrelator>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_tcycle() );

  return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
// PSRFITS SPECIFIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////




string get_obs_mode( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_obs_mode();
}


string get_hdrver( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_hdrver();
}

string get_stt_date( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_stt_date();
}

string get_stt_time( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_stt_time();
}


string get_stt_lst( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();
  if (!ext)
    return "UNDEF";
  else 
    return tostring( ext->get_stt_lst(), 10 );
}

string get_coord_md( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_coordmode();
}

string get_equinox( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_equinox();
}

string get_trk_mode( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_trk_mode();

  return result;
}

string get_bpa( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_bpa() );

  return result;
}

string get_bmaj( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_bmaj(), 5, ios::fixed );
}

string get_bmin( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_bmin(), 5, ios::fixed );
}

string get_stt_imjd( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_stt_imjd() );
}

string get_stt_smjd( Reference::To<Archive> archive )
{
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_stt_smjd() );
}

string get_stt_offs( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_stt_offs(), 9 );

  return result;
}

string get_ra( Reference::To<Archive> archive )
{
  return archive->get_coordinates().ra().getHMS();
}

string get_dec( Reference::To<Archive> archive )
{
  return archive->get_coordinates().dec().getDMS();
}

string get_stt_crd1( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_stt_crd1();

  return result;
}

string get_stt_crd2( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_stt_crd2();

  return result;
}

string get_stp_crd1( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_stp_crd1();

  return result;
}

string get_stp_crd2( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_stp_crd2();

  return result;
}





////////////////////////////////////////////////////////////////////////////////////////////
// FLUXCAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_nchan_fluxcal( Reference::To<Archive> archive )
{
  Reference::To<FluxCalibratorExtension> ext = archive->get<FluxCalibratorExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_nchan() );
}

string get_nrcvr_fluxcal( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FluxCalibratorExtension> ext = archive->get<FluxCalibratorExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring<double>( ext->get_nreceptor() );

  return result;
}

string get_epoch_fluxcal( Reference::To<Archive> archive )
{
  Reference::To<FluxCalibratorExtension> ext = archive->get<FluxCalibratorExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_epoch(), 6, ios::fixed );
}




////////////////////////////////////////////////////////////////////////////////////////////
// HISTORY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_nbin_prd( Reference::To<Archive> archive )
{
  string result = "UNDEF";
  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
    return tostring( ext->get_last_nbin_prd() );

  return result;
}

string get_tbin( Reference::To<Archive> archive )
{
  string result = "UNDEF";
  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
    return tostring( ext->get_last_tbin() );

  return result;
}

string get_chbw( Reference::To<Archive> archive )
{
  string result = "UNDEF";
  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
    return tostring( ext->get_last_chan_bw() );

  return result;
}

string get_nbin( Reference::To< Archive > archive )
{
  return tostring( archive->get_nbin() );
}

string get_nchan( Reference::To< Archive > archive )
{
  return tostring( archive->get_nchan() );
}

string get_npol( Reference::To< Archive > archive )
{
  return tostring( archive->get_npol() );
}

string get_nsub( Reference::To< Archive > archive )
{
  string result;

  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if ( ext ) {
    return tostring( ext->get_nrows() );
  }

  if ( result.empty() ) {
    return tostring( archive->get_nsubint() );
  } else {
    return result;
  }
}



////////////////////////////////////////////////////////////////////////////////////////////
// BANDPASS FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_npol_bp( Reference::To<Archive> archive )
{
  string result;
  Reference::To<Passband> ext = archive->get<Passband>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_npol() );

  return result;
}

string get_nch_bp( Reference::To<Archive> archive )
{
  string result;
  Reference::To<Passband> ext = archive->get<Passband>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_nchan() );

  return result;
}




////////////////////////////////////////////////////////////////////////////////////////////
// FEED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_npar_feed( Reference::To<Archive> archive )
{
  string result;
  Reference::To<PolnCalibratorExtension> ext = archive->get<PolnCalibratorExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_nparam() );

  return result;
}

string get_nchan_feed( Reference::To<Archive> archive )
{
  string result;
  Reference::To<PolnCalibratorExtension> ext = archive->get<PolnCalibratorExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_nchan() );

  return result;
}

string get_MJD_feed( Reference::To<Archive> archive )
{
  string result;
  Reference::To<PolnCalibratorExtension> ext = archive->get<PolnCalibratorExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_epoch(), 6, ios::fixed );
}



////////////////////////////////////////////////////////////////////////////////////////////
// DIGITISER STATISTICS FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_dig_atten( Reference::To<Archive> archive )
{
  ostringstream result;
  Reference::To<DigitiserStatistics> ext = archive->get<DigitiserStatistics>();

  if( !ext )
    result << "UNDEF";
  else
  {
    vector<float>::iterator it;
    for( it = ext->rows[0].atten.begin(); it != ext->rows[0].atten.end(); it ++ )
    {
      if( it != ext->rows[0].atten.begin() )
        result << ",";
      result << (*it);
    }
  }

  return result.str();
}

string get_ndigstat( Reference::To<Archive> archive )
{
  string result;
  Reference::To<DigitiserStatistics> ext = archive->get<DigitiserStatistics>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_ndigr() );

  return result;
}

string get_npar_digstat( Reference::To<Archive> archive )
{
  string result;
  Reference::To<DigitiserStatistics> ext = archive->get<DigitiserStatistics>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_npar() );

  return result;
}

string get_ncycsub( Reference::To<Archive> archive )
{
  string result;
  Reference::To<DigitiserStatistics> ext = archive->get<DigitiserStatistics>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_ncycsub() );

  return result;
}

string get_levmode_digstat( Reference::To<Archive> archive )
{
  string result;
  Reference::To<DigitiserStatistics> ext = archive->get<DigitiserStatistics>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_diglev() );

  return result;
}




////////////////////////////////////////////////////////////////////////////////////////////
// DIGITISER COUNTS FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_dig_mode( Reference::To<Archive> archive )
{
  string result;
  Reference::To<DigitiserCounts> ext = archive->get<DigitiserCounts>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_dig_mode() );

  return result;
}

string get_dyn_levt( Reference::To<Archive> archive )
{
  string result = "UNDEF";
  Reference::To<DigitiserCounts> ext = archive->get<DigitiserCounts>();

  if( ext )
  {
    return tostring<float>( ext->get_dyn_levt() );
  }

  return result;
}

string get_nlev_digcnts( Reference::To<Archive> archive )
{
  string result;
  Reference::To<DigitiserCounts> ext = archive->get<DigitiserCounts>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_nlev() );

  return result;
}

string get_npthist( Reference::To<Archive> archive )
{
  string result;
  Reference::To<DigitiserCounts> ext = archive->get<DigitiserCounts>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_npthist() );

  return result;
}

string get_levmode_digcnts( Reference::To<Archive> archive )
{
  string result;
  Reference::To<DigitiserCounts> ext = archive->get<DigitiserCounts>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_diglev() );

  return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
// SUBINT FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_subint_type( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_int_type();

  return result;
}

string get_subint_unit( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return ext->get_int_unit();

  return result;
}

string get_tsamp( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_tsamp() );

  return result;
}


string get_nbits( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_nbits() );

  return result;
}


string get_nch_strt( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_nch_strt() );

  return result;
}


string get_nsblk( Reference::To<Archive> archive )
{
  string result;
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->get_nsblk() );

  return result;
}




////////////////////////////////////////////////////////////////////////////////////////////
// CALIBRATION FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_cal_mode( Reference::To<Archive> archive )
{
  Reference::To<CalInfoExtension> ext = archive->get<CalInfoExtension>();
  if( !ext )
    return "UNDEF";
  else
    return ext->cal_mode;
}


string get_cal_freq( Reference::To<Archive> archive )
{
  Reference::To<CalInfoExtension> ext = archive->get<CalInfoExtension>();
  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->cal_frequency, 6 );
}


string get_cal_dcyc( Reference::To<Archive> archive )
{
  Reference::To<CalInfoExtension> ext = archive->get<CalInfoExtension>();
  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->cal_dutycycle, 3 );
}


string get_cal_phs( Reference::To<Archive> archive )
{
  Reference::To<CalInfoExtension> ext = archive->get<CalInfoExtension>();
  if( !ext )
    return "UNDEF";
  else
    return tostring( ext->cal_phase, 3 );
}


// NOTE - who would have thought is_numeric would be so complicated, need to find a better one, this comes nowhere
//        near what we require for vap.

bool is_numeric( string src )
{
  for( unsigned i =0; i < src.size(); i ++ )
  {
    char next_char = src[i];
    if( next_char != '.' &&
        next_char != 'e' &&
        next_char != '-' &&
        next_char != '+' &&
        (next_char < char('0') || next_char > char('9') ) )
      return false;
  }

  return true;
}







/**
 * PrintBasicHlp - message output when we get -h on command line.
 **/

void PrintBasicHlp( void )
{
  cout <<
  "A program for getting Pulsar::Archive attributes\n"
  "Usage:\n"
  "     vap -c PAR [-c PAR ...] [-E] [-p] filenames\n"
  "Where:\n"
  "\n"
  "PAR is a string containing one or more parameter names, separated by\n"
  "    commas.\n"
  "    If any whitespace is required, then the string containing it must\n"
  "    be enclosed in quotation marks. Multiple parameters may also be\n"
  "    specified by using multiple -c options.\n"
  "\n"
  "    vap -c name,freq\n"
  "\n"
  "    will print the source name and centre frequency.\n"
  "    Note that parameter names are case insensitive.\n"
  "    For a full list of parameter names, type \"vap -H\"\n"
  "\n"
  "-n  suppress outputting headers when doing -c\n"
  "\n"
  "-E  is used to print the most recent ephemeris in an archive\n"
  "\n"
  "-p  is used to print the set of polynomial coefficients\n"
  "\n"
  "-M  extract list of files from metafile\n"
  "\n"
  "-R  show relative path names\n"
  "\n"
  "-s  show the extensions present in an archive\n"
  "\n"
  "-f  format the output into a table with aligned columns and headers\n"
  "    (overrides -n)\n"
  << endl;
}


/**
 * PrintExtdHelp - Show all the parameteres available via the text finder.
 **/

void PrintExtdHlp( void )
{
  cout << 
    "\n"
    "BACKEND PARAMETERS \n"
    "backend                         Name of the backend instrument \n"
    "beconfig                        Backend Config file \n"
    "be_dcc                          Downconversion conjugation corrected \n"
    "be_delay                        Proportional delay from digitiser input \n"
    "be_phase                        Phase convention of backend \n"
    "period                          Folding period \n"
    "tcycle                          Correlator cycle time \n"
    " \n"

    "BANDPASS PARAMETERS \n"
    "nch_bp                          Number of channels in original bandpass \n"
    "npol_bp                         Number of polarizations in bandpass \n"
    " \n"

    "OBSERVATION PARAMETERS \n"
    "bw                              Observation Bandwidth (MHz) \n"
    "dm                              Dispersion measure \n"
    "dmc                             Dispersion corrected (boolean) \n"
    "freq_phs                        Pulse phase frequency \n"
    "freq_pa                         Position angle frequency \n"
    //"freq_obs                        Observed frequency\n"
    "length                          The full duration of the observation (s) \n"
    "name                            Name of the source \n"
    "nbin_obs                        Observed number of pulse phase bins \n"
    "nchan_obs                       Observed number of frequency channels \n"
    "npol_obs                        Observed number of polarizations \n"
    "nsub_obs                        Observed number of Sub-Integrations \n"
    "obs_mode                        Observation Mode (PSR, CAL, SEARCH) \n"
    "polc                            Polarization calibrated (boolean) \n"
    "rm                              Rotation measure (rad/m^2) \n"
    "rmc                             Faraday Rotation corrected (boolean) \n"
    "scale                           Units of profile amplitudes \n"
    "state                           State of profile amplitudes \n"
    "tsub                            The duration of the first subint (s) \n"
    "type                            Observation type (Pulsar, PolnCal, etc.) \n"
    " \n"

    "DIGITISER COUNTS PARAMETERS \n"
    "dig_mode                        Digitiser mode \n"
    "dyn_levt                        Timescale for dynamic leveling \n"
    "levmode_digcnts                 Digitiser level-setting mode (AUTO, FIX) \n"
    "nlev_digcnts                    Number of digitiser levels \n"
    "npthist                         Number of points in histogram (I) \n"
    " \n"

    "DIGITISER STATISTICS PARAMETERS \n"
    "dig_atten                       Digitiser attenuator settings \n"
    "levmode_digstat                 Digitiser level-setting mode (AUTO, FIX) \n"
    "ncycsub                         Number of correlator cycles per subint \n"
    "ndigstat                        Number of digitised channels (I) \n"
    "npar_digstat                    Number of digitiser parameters \n"
    " \n"

    "FEED COUPLING PARAMETERS \n"
    "MJD_feed                        [MJD] Epoch of calibration obs \n"
    "nchan_feed                      Nr of channels in Feed coupling data \n"
    "npar_feed                       Number of coupling parameters \n"
    " \n"

    "FLUXCAL \n"
    "epoch_fluxcal                   [MJD] Epoch of calibration obs \n"
    "nchan_fluxcal                   Nr of frequency channels (I) \n"
    "nrcvr_fluxcal                   Number of receiver channels (I) \n"
    " \n"

    "HISTORY \n"
    "chbw                            Channel bandwidth \n"
    "freq                            Centre frequency (MHz) \n"
    "nchan                           Number of frequency channels \n"
    "nbin                            Number of pulse phase bins \n"
    "nbin_prd                        Nr of bins per period \n"
    "npol                            Number of polarizations \n"
    "nsub                            Number of sub-integrations \n"
    "tbin                            Time per bin or sample \n"
    " \n"

    "OBSERVER PARAMETERS \n"
    "observer                        Observer name(s) \n"
    "projid                          Project name \n"
    " \n"

    "COORDINATES & TIMES \n"
    "coord_md                        The coordinate mode (J2000, GAL, ECLIP, etc ). \n"
    "dec                             Declination (-dd:mm:ss.sss) \n"
    "equinox                         Equinox of coords (2000.0) \n"
    "fracmjd                         MJD faction of day \n"
    "intmjd                          MJD day \n"
    "mjd                             MJD \n"
    "parang                          Parallactic angle at archive mid point \n"
    "ra                              Right ascension (hh:mm:ss.ssss) \n"
    "stp_crd1                        Stop coord 1 (hh:mm:ss.sss or ddd.ddd) \n"
    "stp_crd2                        Stop coord 2 (-dd:mm:ss.sss or -dd.ddd) \n"
    "stt_crd1                        Start coord 1 (hh:mm:ss.sss or ddd.ddd) \n"
    "stt_crd2                        Start coord 2 (-dd:mm:ss.sss or -dd.ddd) \n"
    "stt_date                        Start UT date (YYYY-MM-DD) \n"
    "stt_imjd                        Start MJD (UTC days) (J - long integer) \n"
    "stt_lst                         Start LST (hh:mm:ss) \n"
    "stt_offs                        [s] Start time offset (D) \n"
    "stt_smjd                        [s] Start time (sec past UTC 00h) (J) \n"
    "stt_time                        Start UT (hh:mm:ss) \n"
    "trk_mode                        Track mode ( TRACK, SCANGC, SCANLAT ) \n"
    " \n"

    "FEED & RECEIVER PARAMETERS \n"
    "basis                           Basis of receptors \n"
    "fac                             Feed angle corrected \n"
    "fa_req                          Feed angle requested \n"
    "fda                             Feed angle recorded in first subint \n"
    "fd_hand                         Hand of receptor basis \n"
    "fd_mode                         Feed track mode (FA,CPA,GPA) \n"
    "fd_sang                         Feed symmetry angle (rcvr:ra) \n"
    "fd_xyph                         Reference source phase (rcvr:rph) \n"
    "nrcpt                           Number of receptors \n"
    "rcvr                            Name of receiver \n"
    "xoffset                         Offset of feed X-axis wrt platform zero \n"
    " \n"

    "SUBINT PARAMETERS \n"
    "nbits                           Nr of bits/datum (SEARCH mode 'X' data, else 1) \n"
    "nch_strt                        Start channel/sub-band number (0 to NCHAN-1) \n"
    "nsblk                           Samples/row (SEARCH mode, else 1) \n"
    "subint_type                     Time axis (TIME, BINPHSPERI, BINLNGASC, etc) \n"
    "subint_unit                     Unit of time axis (SEC, PHS (0-1), DEG) \n"
    "tsamp                           [s] Sample interval for SEARCH-mode data \n"
    " \n"

    "FILE & TELESCOPE PARAMETERS \n"
    "ant_x                           ITRF X coordinate. \n"
    "ant_y                           ITRF Y coordinate. \n"
    "ant_z                           ITRF Z coordinate. \n"
    "bmaj                            [deg] beam major axis \n"
    "bmin                            [deg] beam minor axis \n"
    "bpa                             [deg] beam position angle \n"
    "date                            File creation date \n"
    "file                            The file number (FB data only) \n"
    "hdrver                          Header Version \n"
    "asite                           Telescope tempo code \n"
    "telescop                        Telescope name \n"
    "tlabel                          Tape label (FB data only) \n"
    " \n"

    "CALIBRATION PARAMETERS \n"
    "cal_dcyc                        Cal duty cycle (E) \n"
    "cal_freq                        [Hz] Cal modulation frequency (E) \n"
    "cal_mode                        Cal mode (OFF, SYNC, EXT1, EXT2) \n"
    "cal_phs                         Cal phase (wrt start time) (E) \n"
    " \n"

    "Values listed as: \n"
    " - UNDEF   are not valid for the current file \n"
    " - *       are defined, but not present in the particular file \n"
    " - INVALID are unrecognised \n"
    "\n"
    "For example, nch_fluxcal will return UNDEF for all files that \n"
    "do not contain a FluxCalibratorExtension. \n"
       << endl;
}


/**
* Process the command line options, return the index into argv of the first non option.
**/

void ProcArgs( int argc, char *argv[] )
{
  int gotc;
  while ((gotc = getopt (argc, argv, "fnc:sEphHvVtTXM:Rq")) != -1)
    switch (gotc)
    {

    case 'E':
      ephemmode = true;
      break;

    case 'p':
      polycmode = true;
      break;

    case 'c':
      separate (optarg, commands, " ,");
      break;
    case 'h':
      PrintBasicHlp();
      break;

    case 'H':
      PrintExtdHlp();
      break;

    case 'v':
      verbose = true;
      Pulsar::Archive::set_verbosity(2);
      break;

    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(3);
      break;

    case 'n':
      hide_headers = true;
      break;

    case 's':
      show_extensions = true;
      break;

    case 'M':
      meta_filename = optarg;
      break;

    case 'R':
      full_paths = true;
      break;

    case 'f':
      neat_table = true;
      break;

    case 'q':
      break;

    default:
      cerr << "Unknown command line option" << endl;
      return;
    };

  // -f overrides -n
  if (neat_table) hide_headers = false;

}




string FetchValue( Reference::To< Archive > archive, string command )
{
  try
  {
    if( command == "name" ) return get_name( archive );
    else if( command == "nbin" ) return get_nbin( archive );
    else if( command == "nchan" ) return get_nchan( archive );
    else if( command == "npol" ) return get_npol( archive );
    else if( command == "nsub" ) return get_nsub( archive );
    else if( command == "stime" ) return get_stime( archive );
    else if( command == "etime" ) return get_etime( archive );
    else if( command == "length" ) return get_length( archive );
    else if( command == "nbin_obs" ) return get_nbin_obs( archive );
    else if( command == "nchan_obs" ) return get_nchan_obs( archive );
    else if( command == "npol_obs" ) return get_npol_obs( archive );
    else if( command == "nsub_obs" ) return get_nsub_obs( archive );
    else if( command == "dm" ) return get_dm( archive );
    else if( command == "rm" ) return get_rm( archive );
    else if( command == "state" ) return get_state( archive );
    else if( command == "scale" ) return get_scale( archive );
    else if( command == "type" ) return get_type( archive );
    else if( command == "dmc" ) return get_dmc( archive );
    else if( command == "rmc" ) return get_rmc( archive );
    else if( command == "polc" ) return get_polc( archive );
    else if( command == "freq" ) return get_freq( archive );
    else if( command == "freq_pa" ) return get_freq_pa( archive );
    else if( command == "freq_phs" ) return get_freq_phs( archive );
    else if( command == "bw" ) return get_bw( archive );
    else if( command == "intmjd" ) return get_intmjd( archive );
    else if( command == "fracmjd" ) return get_fracmjd( archive );
    else if( command == "mjd" ) return get_mjd( archive );
    else if( command == "parang" ) return get_parang( archive );
    else if( command == "tsub" ) return get_tsub( archive );
    else if( command == "observer" ) return get_observer( archive );
    else if( command == "projid" ) return get_projid( archive );
    else if( command == "fac" ) return get_fac( archive );
    else if( command == "rcvr" ) return get_rcvr( archive );
    else if( command == "nrcpt" ) return get_nrcpt( archive );
    else if( command == "basis" ) return get_basis( archive );
    else if( command == "fd_hand" ) return get_fd_hand( archive );
    else if( command == "fd_xyph" ) return get_fd_xyph( archive );
    else if( command == "oa" ) return get_oa( archive );
    else if( command == "fd_sang" ) return get_fd_sang( archive );
    else if( command == "xoffset" ) return get_xoffset( archive );
    else if( command == "yo" ) return get_yo( archive );
    else if( command == "co" ) return get_co( archive );
    else if( command == "ant_x" ) return get_ant_x( archive );
    else if( command == "ant_y" ) return get_ant_y( archive );
    else if( command == "ant_z" ) return get_ant_z( archive );
    else if( command == "telescop" ) return get_telescop( archive );
    else if( command == "asite" ) return get_asite( archive );
    else if( command == "backend" ) return get_backend( archive );
    else if( command == "be_dcc" ) return get_be_dcc( archive );
    else if( command == "be_phase" ) return get_be_phase( archive );
    else if( command == "beconfig" ) return get_beconfig( archive );
    else if( command == "tcycle" ) return get_tcycle( archive );
    else if( command == "obs_mode" ) return get_obs_mode( archive );
    else if( command == "hdrver" ) return get_hdrver( archive );
    else if( command == "stt_date" ) return get_stt_date( archive );
    else if( command == "stt_time" ) return get_stt_time( archive );
    else if( command == "stt_lst" ) return get_stt_lst( archive );
    else if( command == "coord_md" ) return get_coord_md( archive );
    else if( command == "equinox" ) return get_equinox( archive );
    else if( command == "trk_mode" ) return get_trk_mode( archive );
    else if( command == "bpa" ) return get_bpa( archive );
    else if( command == "bmaj" ) return get_bmaj( archive );
    else if( command == "bmin" ) return get_bmin( archive );
    else if( command == "stt_imjd" ) return get_stt_imjd( archive );
    else if( command == "stt_smjd" ) return get_stt_smjd( archive );
    else if( command == "stt_offs" ) return get_stt_offs( archive );
    else if( command == "ra" ) return get_ra( archive );
    else if( command == "dec" ) return get_dec( archive );
    else if( command == "stt_crd1" ) return get_stt_crd1( archive );
    else if( command == "stt_crd2" ) return get_stt_crd2( archive );
    else if( command == "stp_crd1" ) return get_stp_crd1( archive );
    else if( command == "stp_crd2" ) return get_stp_crd2( archive );
    else if( command == "nbin_prd" ) return get_nbin_prd( archive );
    else if( command == "tbin" ) return get_tbin( archive );
    else if( command == "chbw" ) return get_chbw( archive );
    else if( command == "npol_bp" ) return get_npol_bp( archive );
    else if( command == "nch_bp" ) return get_nch_bp( archive );
    else if( command == "npar_feed" ) return get_npar_feed( archive );
    else if( command == "nchan_feed" ) return get_nchan_feed( archive );
    else if( command == "mjd_feed" ) return get_MJD_feed( archive );
    else if( command == "ndigstat" ) return get_ndigstat( archive );
    else if( command == "npar_digstat" ) return get_npar_digstat( archive );
    else if( command == "ncycsub" ) return get_ncycsub( archive );
    else if( command == "levmode_digstat" ) return get_levmode_digstat( archive );
    else if( command == "dig_mode" ) return get_dig_mode( archive );
    else if( command == "nlev_digcnts" ) return get_nlev_digcnts( archive );
    else if( command == "npthist" ) return get_npthist( archive );
    else if( command == "levmode_digcnts" ) return get_levmode_digcnts( archive );
    else if( command == "subint_type" ) return get_subint_type( archive );
    else if( command == "subint_unit" ) return get_subint_unit( archive );
    else if( command == "tsamp" ) return get_tsamp( archive );
    else if( command == "nbits" ) return get_nbits( archive );
    else if( command == "nch_strt" ) return get_nch_strt( archive );
    else if( command == "nsblk" ) return get_nsblk( archive );
    else if( command == "date" ) return get_date( archive );
    else if( command == "cal_mode" ) return get_cal_mode( archive );
    else if( command == "cal_freq" ) return get_cal_freq( archive );
    else if( command == "cal_dcyc" ) return get_cal_dcyc( archive );
    else if( command == "cal_phs" ) return get_cal_phs( archive );
    else if( command == "file" ) return get_file( archive );
    else if( command == "tlabel" ) return get_tlabel ( archive );
    else if( command == "fd_mode" ) return get_fd_mode ( archive );
    else if( command == "fa_req" || command == "ta" )
      return get_fa_req ( archive );
    else if( command == "fda" ) return get_fda ( archive );
    else if( command == "dyn_levt" ) return get_dyn_levt ( archive );
    else if( command == "dig_atten" ) return get_dig_atten ( archive );
    else if( command == "be_delay" ) return get_be_delay( archive );
    else if( command == "period" ) return get_period( archive );
    else if( command == "epoch_fluxcal" ) return get_epoch_fluxcal( archive );
    else if( command == "nchan_fluxcal" ) return get_nchan_fluxcal( archive );
    else if( command == "nrcvr_fluxcal" ) return get_nrcvr_fluxcal( archive );

    else return "INVALID";
  }
  catch( Error e )
  {
    return "*error*";
  }


}


static string current_filename;

/**
* ProcessArchive - load an archive and process all the command line parameters using the text finder.
**/

template<typename OS>
void ProcessArchiveImplementation( OS& os, string filename )
{
  current_filename = filename;
  Reference::To< Archive > archive;
  try
  {
    archive = Archive::load( filename );
  }
  catch ( Error& e )
  {
    cerr << "failed to load archive " << filename;
    if( verbose )
    {
      cerr << e << endl;
    }
    else
    {
      cerr << "\n\t" << e.get_message() << endl;
    }
  }
  catch ( ... )
  {
    cerr << "failed to load archive " << filename << endl;
  }

  if( !archive )
    return;

  if( full_paths )
    os << filename;
  else
    os << basename( filename );

  vector< string >::iterator it;
  for( it = commands.begin(); it != commands.end(); it ++ )
  {
    if (!neat_table)
      os << "   ";

    string val = FetchValue( archive, lowercase((*it)) );
    if ( val == "" ) val = "*";
    os << val;
  }

  os << endl;
}

void ProcessArchive( string filename )
{
  if (neat_table)
    ProcessArchiveImplementation( ts, filename );
  else
    ProcessArchiveImplementation( cout, filename );
}


/**
* ExtractPolyco - Get the polyco data from the archive, this will be moved elsewhere
**/

void ExtractPolyco( string filename )
{
  current_filename = filename;
  Reference::To< Archive > archive = Archive::load( filename );

  if( !archive )
    return;

  if( !full_paths )
    cout << basename(filename);
  else
    cout << filename;

  cout << " has polyco:" << endl << endl;

  archive->get_model()->unload( stdout );

  cout << endl;
}



/**
* ExtractEphemeris - Get the ephemeris, this will be moved elsewhere.
**/

void ExtractEphemeris( string filename )
{
  current_filename = filename;
  Reference::To< Archive > archive = Archive::load( filename );

  if( !archive )
    return;

  if( !archive->has_ephemeris() )
    cout << "vap: " << filename << " has no ephemeris" << endl;
  else
    archive->get_ephemeris()->unload( stdout );
}



/**
* ShowExtensions - a simple debugging routine for showing the extensions available on an archive.
**/

void ShowExtensions( string filename )
{
  current_filename = filename;
  Reference::To< Archive > archive = Archive::load( filename );

  cout << "Extensions for " << filename << endl;

  unsigned int num_extensions = archive->get_nextension();
  for( unsigned int i = 0; i < num_extensions; i ++ )
  {
    Reference::To< Archive::Extension > next_ext = archive->get_extension(i);
    cout << next_ext->get_extension_name() << endl;
  }
}



/**
 * ExpandMetafile - open the metafile and add all the filenames in it to the vector of filenames given.
 **/

void ExpandMetafile( string meta_filename, vector< string > &filenames )
{
  ifstream infile;
  infile.open( meta_filename.c_str() );
  if( infile.fail() )
  {
    cerr << "Failed to open metafile " << meta_filename << endl;
  }

  string next_filename;
  while( 1 )
  {
    infile >> next_filename;
    if( infile.fail() )
      break;
    filenames.push_back( next_filename );
  }
  infile.close();
}


template<typename OS>
void Header( OS& os )
{
  // first heading is always the filename
  os << "filename";

  // add the commands as headings for the table.
  vector< string >::iterator it;
  for( it = commands.begin(); it != commands.end(); it ++ )
  {
    if (!neat_table)
      os << "   ";
    os << (*it);
  }

  os << endl;
}

/**
* main -
**/

int main( int argc, char *argv[] ) try
{
  tostring_precision = 4;
  Angle::default_type = Angle::Degrees;

  // load files more quickly by ignoring profile data
  Pulsar::Profile::no_amps = true;

  ProcArgs( argc, argv );

  vector< string > filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  if( meta_filename != "" )
  {
    ExpandMetafile( meta_filename, filenames );
  }

  if (!hide_headers) {
    if( neat_table )
      Header( ts );
    else
      Header( cout );
  }

  if( filenames.size() )
  {
    if( polycmode )
      for_each( filenames.begin(), filenames.end(), ExtractPolyco );
    else if( ephemmode )
      for_each( filenames.begin(), filenames.end(), ExtractEphemeris );
    else if( show_extensions )
      for_each( filenames.begin(), filenames.end(), ShowExtensions );
    else
    {
      for_each( filenames.begin(), filenames.end(), ProcessArchive );
      if( neat_table )
        ts.flush();
    }
  }

  return 0;
}
 catch (Error& error)
   {
     cerr << "vap: error while handling " << current_filename << error << endl;
     return -1;
   }
