/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ProcHistory.h"

#include "Pulsar/Receiver.h"
#include "Pulsar/Backend.h"

#include "strutil.h"

string fits_scale_string (Signal::Scale scale, bool verbose)
{
  switch (scale)
  {
  case Signal::FluxDensity:
    return "FluxDen";
  case Signal::ReferenceFluxDensity:
    return "RefFlux";
  case Signal::Jansky:
    return "Jansky";
  default:
    if (verbose)
      cerr << "PSRFITS WARNING output SCALE unknown" << endl;
    return "UNKNOWN";
  }
}

string fits_state_string (Signal::State state, bool verbose)
{
  switch (state)
  {
  case Signal::PPQQ:
    return "AABB";
  case Signal::Stokes:
    return "STOKE";
  case Signal::PseudoStokes:
    return "PSTOKES";
  case Signal::Coherence:
    return "AABBCRCI";
  case Signal::Intensity:
    return "INTEN";
  case Signal::Invariant:
    return "INVAR";
  default:
    if (verbose)
      cerr << "PSRFITS WARNING output POL_TYPE unknown" << endl;
    return "UNKNOWN";
  }
}

void Pulsar::FITSArchive::update_history()
{
  ProcHistory* history = getadd<ProcHistory>();

  /*
    Add a new row only if the data have been loaded from a PSRFITS file,
    in which case the history should be updated, or there is no current
    row
  */

  if (history->get_nrow() == 0 || loaded_from_fits)
    history->add_blank_row();
  
  time_t timeval = time(0);
  string timestr = ctime (&timeval);

  ProcHistory::row& last = history->get_last();

  last.date_pro = remove_all(timestr,'\n');
  
  last.proc_cmd = history->get_command_str();

  last.scale = state_scale = fits_scale_string(get_scale(), verbose > 1);

  last.pol_type = state_pol_type = fits_state_string(get_state(), verbose > 1);

  last.nsub = get_nsubint();
  last.npol = get_npol();
  last.nbin = get_nbin();
  last.nbin_prd = get_nbin();

  if ( get_nsubint() )
    last.tbin = get_Integration(0)->get_folding_period() / get_nbin();
  else
    last.tbin = 0.0;

  last.ctr_freq = get_centre_frequency();
  last.nchan = get_nchan();
  last.chan_bw = get_bandwidth() / float(get_nchan());
  last.rm_corr = get_faraday_corrected();
  last.dedisp = get_dedispersed();

  Receiver* receiver = get<Receiver>();
  if (!receiver)
  {
    last.pr_corr = false;
    last.fd_corr = false;
  }
  else
  {
    last.pr_corr = receiver->get_projection_corrected();
    last.fd_corr = receiver->get_basis_corrected();
  }

  Backend* backend = get<Backend>();
  if (!backend)
    last.be_corr = false;
  else
    last.be_corr = backend->get_corrected();

  if (get_poln_calibrated())
  {
    if (history->get_cal_mthd() == "NONE")
      history->set_cal_mthd("Other");
    last.cal_mthd = history->get_cal_mthd();
  }
  else
    last.cal_mthd = "NONE";

  last.cal_file = history->get_cal_file();
  last.rfi_mthd = history->get_rfi_mthd();;
}
