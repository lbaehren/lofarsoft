/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/BinaryPhaseOrder.h"
#include "Pulsar/BinLngAscOrder.h"
#include "Pulsar/BinLngPeriOrder.h"
#include "Pulsar/ProcHistory.h"

#include "FITSError.h"
#include "psrfitsio.h"

using namespace std;

void Pulsar::FITSArchive::load_integration_state (fitsfile* fptr)
{
  int status = 0;

  // Move to the SUBINT Binary Table
  
  fits_movnam_hdu (fptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_integration_state no SUBINT HDU" << endl;
    return;
  }

  if (!search_mode)
  {
    // Get the number of rows (ie. the number of sub-ints)
    
    long numrows = 0;
    fits_get_num_rows (fptr, &numrows, &status);

    if (verbose > 2)
      cerr << "FITSArchive::load_integration_state nsubint="<< numrows << endl;

    set_nsubint(numrows);
  }

  string tempstr;
  string dfault = "";
  psrfits_read_key (fptr, "INT_TYPE", &tempstr, dfault, verbose > 2);

  if (verbose > 2)
    cerr << "FITSArchive::load_integration_state INT_TYPE=" << tempstr << endl;

  if (tempstr != "TIME" && tempstr != "")
  {
    if (tempstr == "BINPHSPERI")
      add_extension(new PeriastronOrder());
    else if (tempstr == "BINPHSASC")
      add_extension(new BinaryPhaseOrder());
    else if (tempstr == "BINLNGPERI")
      add_extension(new BinLngPeriOrder());
    else if (tempstr == "BINLNGASC")
      add_extension(new BinLngAscOrder());
    else
      throw Error(InvalidParam, "FITSArchive::load_integration_state",
		  "unknown INT_TYPE=" + tempstr);
    
    get<Pulsar::IntegrationOrder>()->resize(get_nsubint());
  }
 
  Reference::To<ProcHistory> hist = get<ProcHistory>();

  if (psrfits_version < 3)
  {
    if (!hist)
    {
      if (verbose > 2)
        cerr << "FITSArchive::load_integration_state version=" 
             << psrfits_version << " with no HISTORY" << endl;
      load_state (fptr);
    }
    else if (hist->rows.size() == 1)
    {
      if (verbose > 2)
        cerr << "FITSArchive::load_integration_state version=" 
             << psrfits_version << " with one HISTORY row" << endl;

      // if the proc history contained only 1 row, set nsub in proc history so
      // that for files pre 2.13 the observed nsub is correct.

      hist->rows[0].nsub = get_nsubint();
    }
  }
  else
  {
    if (verbose > 2)
        cerr << "FITSArchive::load_integration_state version=" 
             << psrfits_version << " load SUBINT state" << endl;
    load_state (fptr);
  }

  interpret_scale ();
  interpret_pol_type ();
}

void Pulsar::FITSArchive::interpret_scale ()
{
  if (state_scale.empty())
  {
    Reference::To<ProcHistory> hist = get<ProcHistory>();
    if (!hist)
    {
      if (verbose > 1)
	cerr << "FITSArchive::interpret_scale assuming undefined SCALE=FluxDen"
	     << endl;
      set_scale( Signal::FluxDensity );
      return;
    }

    state_scale = hist->get_last().scale;
    if (verbose > 2)
      cerr << "FITSArchive::interpret_scale using HISTORY SCALE=" 
	   << state_scale << endl;
  }
  else if (verbose > 2)
    cerr << "FITSArchive::interpret_scale SCALE=" << state_scale << endl;

  if (state_scale == "FluxDen")
    set_scale( Signal::FluxDensity );

  else if (state_scale == "RefFlux")
    set_scale( Signal::ReferenceFluxDensity );

  else if (state_scale == "Jansky")
    set_scale( Signal::Jansky );

  else if (verbose > 1)
    cerr << "FITSArchive::interpret_scale WARNING unrecognized SCALE='"
	 << state_scale << "'" << endl;
}

void Pulsar::FITSArchive::interpret_pol_type ()
{
  bool pol_type_undefined = false;

  if (state_pol_type.empty())
  {
    Reference::To<ProcHistory> hist = get<ProcHistory>();
    if (!hist)
    {
      if (verbose > 1)
	cerr << "FITSArchive::interpret_pol_type undefined POL_TYPE" << endl;
      pol_type_undefined = true;
    }
    else
    {
      state_pol_type = hist->get_last().pol_type;
      if (verbose > 2)
	cerr << "FITSArchive::interpret_pol_type using HISTORY POL_TYPE=" 
	     << state_pol_type << endl;
    }
  }
  else if (verbose > 2)
    cerr << "FITSArchive::interpret_pol_type POL_TYPE=" << state_pol_type
	 << endl;

  if (!pol_type_undefined)
  {
    if( state_pol_type == "XXYY" || 
	state_pol_type == "LLRR" ||
	state_pol_type == "AABB" )
    {
      set_state ( Signal::PPQQ );
      if (verbose > 2)
	cerr << "FITSArchive::interpret_pol_type Signal::PPQQ" << endl;
    }

    else if( state_pol_type == "STOKE" )
    {
      set_state ( Signal::Stokes );
      if (verbose > 2)
	cerr << "FITSArchive::interpret_pol_type Signal::Stokes" << endl;
    }

    else if( state_pol_type == "PSTOKES" )
    {
      set_state ( Signal::PseudoStokes );
      if (verbose > 2)
	cerr << "FITSArchive::interpret_pol_type Signal::PseudoStokes" << endl;
    }

    else if( state_pol_type == "XXYYCRCI" ||
	     state_pol_type == "LLRRCRCI" ||
	     state_pol_type == "AABBCRCI" )
    {
      set_state ( Signal::Coherence );
      if (verbose > 2)
	cerr << "FITSArchive::interpret_pol_type Signal::Coherence" << endl;
    }

    else if( state_pol_type == "INTEN" ||
	     state_pol_type == "AA+BB" )
    {
      set_state ( Signal::Intensity );
      if (verbose > 2)
	cerr << "FITSArchive::interpret_pol_type Signal::Intensity" << endl;
    }

    else if( state_pol_type == "INVAR" )
    {
      set_state ( Signal::Invariant );
      if (verbose > 2)
	cerr << "FITSArchive::interpret_pol_type Signal::Invariant" << endl;
    }
    
    else
    {
      if (verbose > 1)
	cerr << "FITSArchive::interpret_pol_type WARNING unknown POL_TYPE='"
	     << state_pol_type << "'" << endl;
      pol_type_undefined = true;
    }
  }


  if (pol_type_undefined)
  {
    if (verbose > 1)
      cerr << "FITSArchive::interpret_pol_type WARNING"
	" guessing state from NPOL=" << get_npol() << endl;

    if (npol == 4)
      set_state ( Signal::Stokes );
    else if (npol == 2)
      set_state ( Signal::PPQQ );
    else if (npol == 1)
      set_state ( Signal::Intensity );
    else
      throw Error (InvalidState, "Pulsar::FITSArchive::interpret_pol_type",
		   "Bad archive, no POL_TYPE and npol=%d", npol);
  }
}


/*!  
  \pre the current HDU is the SUBINT binary table
*/
void Pulsar::FITSArchive::load_state (fitsfile* fptr)
{
  string empty;

  psrfits_read_key (fptr, "SCALE", &state_scale, empty, verbose > 2);

  int npol = 0;
  psrfits_read_key (fptr, "NPOL", &npol);
  set_npol( npol );

  psrfits_read_key (fptr, "POL_TYPE", &state_pol_type, empty, verbose > 2);

  int nbin = 0;
  psrfits_read_key (fptr, "NBIN", &nbin);
  set_nbin( nbin );

  int nchan = 0;

  int undefined = -1;
  psrfits_read_key (fptr, "NCHAN", &nchan, undefined, verbose > 2);
  
  // support old format
  if (nchan == undefined)
    psrfits_read_key (fptr, "NCH_FILE", &nchan);

  set_nchan( nchan );

  double chan_bw = 0.0;
  double zero = 0.0;
  psrfits_read_key (fptr, "CHAN_BW", &chan_bw, zero, verbose > 2);

  if (chan_bw != zero)
    set_bandwidth (chan_bw * nchan);

  double dm = 0.0;
  psrfits_read_key (fptr, "DM", &dm, zero, verbose > 2);
  if (dm != zero)
    set_dispersion_measure (dm);

  double rm = 0.0;
  psrfits_read_key (fptr, "RM", &rm, zero, verbose > 2);
  if (rm != zero)
    set_rotation_measure (rm);

  int naux = 0;
  undefined = 0;
  psrfits_read_key (fptr, "NAUX", &naux, undefined, verbose > 2);

  naux_profile = naux;
}

