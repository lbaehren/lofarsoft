/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/CoherentDedispersion.h"
#include "psrfitsio.h"

using namespace std;

typedef Pulsar::CoherentDedispersion::InputChannel InputChannel;
typedef Pulsar::CoherentDedispersion::OutputChannel OutputChannel;

template<typename T, class Method>
void pack (std::vector<T>& data, InputChannel& ext, unsigned n, Method set)
{
  unsigned idata = 0;
  for (unsigned i=0; i<n; i++)
  {
    (ext.get_output(i).*set)(data[idata]);
    if (data.size() > 1)
      idata ++;
  }
}

template<typename T, class Method>
void read (fitsfile* fptr, const char* colname, InputChannel& ext,
	   unsigned ichan, Method set)
{
  vector<T> data;
  psrfits_read_col (fptr, colname, data, ichan+1);
  pack (data, ext, ext.get_nchan_output(), set);
}

void Pulsar::FITSArchive::load_CoherentDedispersion (fitsfile* fptr) try
{
  if (verbose > 2)
    cerr << "FITSArchive::load_CoherentDedispersion entered" << endl;
  
  // Move to the COHDDISP
  if (!psrfits_move_hdu (fptr, "COHDDISP", true))
    return;

  // Check for old/invalid COHDDISP HDUs
  long nrows = 0;
  int status = 0;
  fits_get_num_rows(fptr, &nrows, &status);
  if (status)
    throw FITSError (status, "FITSArchve::load_CoherentDedispersion", 
        "fits_get_num_rows");
  if (nrows==0) return;

  Reference::To<CoherentDedispersion> ext = new CoherentDedispersion;

  string unknown = "unknown";
  string text;

  // Get DOMAIN
  psrfits_read_key (fptr, "DOMAIN", &text, unknown, verbose > 2);
  if (text == "TIME")
    ext->set_domain( Signal::Time );
  else
    ext->set_domain( Signal::Frequency );

  // Get CHRPTYPE
  psrfits_read_key (fptr, "CHRPTYPE", &text, unknown, verbose > 2);
  ext->set_description( text );

  double value = 0.0;
  double zero = 0.0;

  try {
    psrfits_read_key (fptr, "DM", &value);
    ext->set_dispersion_measure( value );
  }
  catch (Error& error)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_CoherentDedispersion " << error.get_message()
           << " ... aborting" << endl;
    return;
  }

  psrfits_read_key (fptr, "DOPPLER", &value, zero, verbose > 2);
  ext->set_doppler_correction( value );

  int nbit = 0;
  int ieee = -32;

  psrfits_read_key (fptr, "DATANBIT", &nbit, ieee, verbose > 2);
  ext->set_nbit_data (nbit);

  psrfits_read_key (fptr, "CHRPNBIT", &nbit, ieee, verbose > 2);
  ext->set_nbit_chirp (nbit);

  int nchan_input;
  psrfits_read_key (fptr, "NCHAN", &nchan_input);
  ext->set_nchan_input (nchan_input);

  for (int ichan=0; ichan<nchan_input; ichan++)
  {
    InputChannel& input = ext->get_input (ichan);

    double value = 0;
    psrfits_read_col (fptr, "FREQ", &value, ichan+1);
    input.set_centre_frequency (value);

    psrfits_read_col (fptr, "BW", &value, ichan+1);
    input.set_bandwidth (value);

    unsigned nchan_output = 0;
    psrfits_read_col (fptr, "OUT_NCHAN", &nchan_output, ichan+1);
    input.set_nchan_output (nchan_output);

    read<double> (fptr, "OUT_FREQ", input, ichan,
		  &OutputChannel::set_centre_frequency);
    read<double> (fptr, "OUT_BW", input, ichan,
		  &OutputChannel::set_bandwidth);
    
    read<unsigned> (fptr, "NCHIRP", input, ichan,
		    &OutputChannel::set_nsamp);
    read<unsigned> (fptr, "NCYC_POS", input, ichan,
		    &OutputChannel::set_nsamp_overlap_pos);
    read<unsigned> (fptr, "NCYC_NEG", input, ichan,
		    &OutputChannel::set_nsamp_overlap_neg);
  }

  if (verbose > 2)
    cerr << "FITSArchive::load_CoherentDedispersion adding extension" << endl;

  add_extension (ext);
}
catch (Error& error)
{
  throw error += "Pulsar::FITSArchive::load_CoherentDedispersion";
}
