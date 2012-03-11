//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProcHistory.h"

using namespace std;

// //////////////////////////////////////////////////
// ProcHistory methods
//

//Default constructor
Pulsar::ProcHistory::ProcHistory ()
  : Extension ("ProcHistory")
{
  command_str  = "UNKNOWN";
  the_cal_mthd = "NONE";
  the_sc_mthd  = "NONE";
  the_cal_file = "NONE";
  the_rfi_mthd = "NONE";
}

//! Get the number of rows
unsigned Pulsar::ProcHistory::get_nrow () const
{
  return rows.size ();
}

//! Get the ith row
Pulsar::ProcHistory::row* Pulsar::ProcHistory::get_row (unsigned irow)
{
  check_irow (irow);
  return &rows[irow];
}

const Pulsar::ProcHistory::row* 
Pulsar::ProcHistory::get_row (unsigned irow) const
{
  check_irow (irow);
  return &rows[irow];
}

void Pulsar::ProcHistory::check_irow (unsigned irow) const
{
  if (irow >= rows.size())
    throw Error (InvalidParam, "Pulsar::ProcHistory::check_irow",
		 "irow=%u >= nrow=%u", irow, rows.size());
}

Pulsar::ProcHistory::row& Pulsar::ProcHistory::get_last ()
{
  if (rows.size() == 0)
    throw Error (InvalidState, "Pulsar::ProcHistory::get_last",
		 "history is empty");

  return rows.back();
}

void Pulsar::ProcHistory::set_command_str (const string& str)
{
  if (str.length() > 80)
  {
    cerr << "ProcHistory::set_command_str WARNING truncated to 80 chars" 
	 << endl;
    command_str = str.substr(0, 80);
  }
  else
    command_str = str;
}

string Pulsar::ProcHistory::get_command_str()
{
  return command_str;
}

void Pulsar::ProcHistory::set_cal_mthd (const string& str)
{
  the_cal_mthd = str;
}

string Pulsar::ProcHistory::get_cal_mthd()
{
  return the_cal_mthd;
}

void Pulsar::ProcHistory::set_sc_mthd (const string& str)
{
  the_sc_mthd = str;
}

string Pulsar::ProcHistory::get_sc_mthd()
{
  return the_sc_mthd;
}

void Pulsar::ProcHistory::set_cal_file (const string& str)
{
  the_cal_file = str;
}

void Pulsar::ProcHistory::set_rfi_mthd (const string& str)
{
  the_rfi_mthd = str;
}

string Pulsar::ProcHistory::get_rfi_mthd()
{
  return the_rfi_mthd;
}

void Pulsar::ProcHistory::add_blank_row ()
{
  rows.push_back(row());
  rows.back().index = rows.size();
}

// //////////////////////////////////////////////////
// ProcHistory::row methods
//

Pulsar::ProcHistory::row::row ()
{
  date_pro = "UNSET";
  proc_cmd = "UNSET";
  pol_type = "UNSET";
  sc_mthd  = "NONE";
  cal_mthd = "NONE";
  cal_file = "NONE";
  rfi_mthd = "NONE";
  dds_mthd = "UNSET";

  aux_rm_model = "NONE";
  aux_dm_model = "NONE";

  nsub     = 0;
  npol     = 0;
  nbin     = 0;
  nbin_prd = 0;
  tbin     = 0.0;
  ctr_freq = 0.0;
  nchan    = 0;
  chan_bw  = 0.0;
  dispersion_measure = 0.0;
  rotation_measure = 0.0;
  pr_corr  = 0;
  fd_corr  = 0;
  be_corr  = 0;
  rm_corr  = 0;
  dedisp   = 0;

  aux_rm_corr = 0;
  aux_dm_corr = 0;

  scale    = Signal::FluxDensity;
}


TextInterface::Parser* Pulsar::ProcHistory::get_interface() 
{
  return new Interface( this );
}





