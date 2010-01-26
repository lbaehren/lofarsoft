/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/CalibratorExtension.h"
#include "FITSError.h"
#include "fitsutil.h"

#include <memory>
#include <fitsio.h>

namespace Pulsar {

  void load (fitsfile* fptr, CalibratorExtension* ext);

  void unload (fitsfile* fptr, const CalibratorExtension* ext);

  void load_Estimates (fitsfile* fptr, std::vector< Estimate<double> >& data,
		       char* column_name);

  void unload_Estimates (fitsfile* fptr, 
			 const std::vector< Estimate<double> >& data,
			 char* column_name,
			 const std::vector<unsigned>* dims = 0);
}

