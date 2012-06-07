/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/WidebandCorrelator.h"
#include "Pulsar/FITSHdrExtension.h"

#include "psrfitsio.h"

#include "RegularExpression.h"
#include "templates.h"
#include "strutil.h"

using namespace std;

void Pulsar::FITSArchive::load_WidebandCorrelator (fitsfile* fptr)
{
  if (verbose == 3)
    cerr << "FITSArchive::load_WidebandCorrelator" << endl;
  
  Reference::To<WidebandCorrelator> ext = new WidebandCorrelator;

  {
    string temp;
    string dfault;

    psrfits_read_key (fptr, "BACKEND", &temp, dfault, verbose == 3);
    ext->set_name(temp);
    
    if (verbose == 3)
      cerr << "FITSArchive::load_WidebandCorrelator BACKEND=" << temp << endl;
    
    psrfits_read_key (fptr, "BECONFIG", &temp, dfault, verbose == 3);
    ext->configfile = temp;

    if (verbose == 3)
      cerr << "FITSArchive::load_WidebandCorrelator BECONFIG=" << temp << endl;
    
  }

  {
    int temp;
    psrfits_read_key (fptr, "BE_PHASE", &temp, 0, verbose == 3);
    ext->set_argument( (Signal::Argument) temp );

    if (verbose == 3)
      cerr << "FITSArchive::load_WidebandCorrelator BE_PHASE=" << temp << endl;
    
    psrfits_read_key (fptr, "BE_DCC",  &temp, 0, verbose == 3);
    ext->set_downconversion_corrected( temp );

    if (verbose == 3)
      cerr << "FITSArchive::load_WidebandCorrelator BE_DCC=" << temp << endl;
    
  }

  if (verbose == 3)
    cerr << "FITSArchive::load_WidebandCorrelator reading cycle time" << endl;
  
  psrfits_read_key (fptr, "TCYCLE", &(ext->tcycle), 0.0, verbose == 3);
 
  if (verbose == 3)
    cerr << "FITSArchive::load_WidebandCorrelator reading NRCVR" << endl;
  
  psrfits_read_key (fptr, "NRCVR", &(ext->nrcvr), 0, verbose == 3);
  
  if( verbose == 3   )
    cerr << "FITSArchive::load_WidebandCorrelator reading BE_DELAY" << endl;
  
  double dtmp = 0.0;
  psrfits_read_key( fptr, "BE_DELAY", &dtmp, 0.0, verbose == 3 );
  ext->set_delay( dtmp );

  add_extension (ext);

  if (ext->get_name() != "WBCORR")
    return;

  // check for corrections

  FITSHdrExtension* hdr_ext = get<FITSHdrExtension>();
  if (!hdr_ext)
    throw Error (InvalidParam, "FITSArchive::load_WidebandCorrelator",
		 "no FITSHdrExtension extension");

  int major=-1, minor=-1;

  if (sscanf (hdr_ext->hdrver.c_str(), "%d.%d", &major, &minor) != 2)
    throw Error (InvalidParam, "FITSArchive::load_WidebandCorrelator",
		 "could not parse header version from " + hdr_ext->hdrver);
  
  if (major == 1 && minor < 10)  {
    
    /*
      Prior to header version 1.10, the WBCORR backend at Parkes did
      not properly set the reference_epoch.
    */
    
    correct_P236_reference_epoch = true;
    if (verbose > 1)
      cerr << "Pulsar::FITSArchive::load_WidebandCorrelator\n"
	"  correcting reference epoch of P236 data with version " 
	   << hdr_ext->hdrver << endl;
    
  }
  
  if (major == 1 && minor < 14 && minor > 9)  {
    
    /*
      Prior to header version 1.14, the WBCORR backend at Parkes
      produced cross products that were out by a scale factor of
      two. This little check applies the correction factor if it detects
      an archive that was affected by this instrumentation bug.
    */
    
    scale_cross_products = true;
    if (verbose == 3)
      cerr << "Pulsar::FITSArchive::load_WidebandCorrelator "
	"doubling cross products of WBCORR data with version " 
	   << hdr_ext->hdrver << endl;
    
  }
    
  static char* conv_config = getenv ("WBCCONVCFG");
  static vector<string> conv_configs;

  if (conv_config)  {
    
    if (verbose > 1)
      cerr << "Pulsar::FITSArchive::load_WidebandCorrelator\n"
	"  loading " << conv_config << endl;
    
    stringfload (&conv_configs, conv_config);
    
    conv_config = 0; // load it only once
    
    if (verbose > 2 && conv_configs.size()) {
      cerr << "WBCORR conv configurations:" << endl;
      for (unsigned i=0; i < conv_configs.size(); i++)
	cerr << conv_configs[i] << endl;
    }
    
  }

  static RegularExpression conv_grep ("wb.*_c");

  bool in_grep = conv_grep.get_match(ext->configfile);
  bool in_list = found (ext->configfile, conv_configs);

  bool conventional = in_grep || in_list;

  if (ext->get_argument() == (Signal::Argument) 0) {
    
    if (conventional)
      ext->set_argument( Signal::Conventional );
    else
      ext->set_argument( Signal::Conjugate );

    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_WidebandCorrelator\n"
	"  set BE_PHASE=" << ext->get_argument()
	   << " for WBCORR config=" << ext->configfile << endl;

  }

}

