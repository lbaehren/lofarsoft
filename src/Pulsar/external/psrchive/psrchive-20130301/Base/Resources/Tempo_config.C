/***************************************************************************
 *
 *   Copyright (C) 2007-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// #define _DEBUG

#include "Pulsar/GeneratorInterpreter.h"
#include "Pulsar/Predictor.h"
#include "Pulsar/Config.h"

#ifdef HAVE_TEMPO2
#include "T2Generator.h"
#endif
#include "Predict.h"

using namespace std;

namespace Tempo
{
  int config ()
  {
    DEBUG("Tempo::config");
    return 0;
  }
}

/* ***********************************************************************

   Predictor::policy configuration

   *********************************************************************** */

Pulsar::Option<Pulsar::Predictor::Policy>
policy_config_wrapper 
(
 Pulsar::Predictor::get_policy(), 
 "Predictor::policy", Pulsar::Predictor::Ephemeris,

 "Policy for generating new predictors",

 "The type of predictor constructed when generating a new \n"
 "pulse phase prediction model (in Archive::update_model). \n"
 "There are currently three policies for choosing the type of \n"
 "the new predictor: \n"
 "\n"
 " 'input'   - same as the currently installed predictor \n"
 " 'ephem'   - based on the input parameter file \n"
 " 'default' - determined by Predictor::default"
);

/* ***********************************************************************

   Predictor::default_type configuration

   *********************************************************************** */

Pulsar::Option<Pulsar::Generator*> default_type_config_wrapper
(
 Pulsar::Generator::Interpreter::get_option(),
 
 new Pulsar::Generator::Interpreter,

 "Predictor::default", "polyco",

 "Default predictor type",

 "The default phase predictor model type: 'polyco' or 'tempo2'"
);

/* ***********************************************************************

   Tempo::Predict::minimum_nspan configuration

   *********************************************************************** */

Pulsar::Option<unsigned>
minimum_nspan_config_wrapper
(
 Tempo::Predict::get_minimum_nspan(),
 "Tempo::minimum_nspan", 0,

 "Minimum value of 'nspan' [minutes]",

 "When generating a polyco with 'tempo -z', the following error can occur\n"
 "\n"
 "  STOP  Nspan too small statement executed\n"
 "\n"
 "This parameter, if set, puts a lower limit on the value of 'nspan' used."
);


/* ***********************************************************************

   Tempo::Predict::maximum_rms configuration

   *********************************************************************** */

Pulsar::Option<double>
maximum_rms_config_wrapper
(
 Tempo::Predict::get_maximum_rms(),
 "Tempo::maximum_rms", 1e-5,    // default is 10 microturns

 "Maximum value of fit rms residual [turns]",

 "When generating a polyco with 'tempo -z', the rms between the best-fit \n"
 "polynomial and the model is reported.  If the maximum_rms is not zero, \n"
 "a warning will be issued if the rms reported by tempo exceeds this value."
);

#ifdef HAVE_TEMPO2

/* ***********************************************************************

   Tempo2::Generator::parameter_filename configuration

   *********************************************************************** */

Pulsar::Option<std::string>
parameter_filename_wrapper
(
 Tempo2::Generator::get_keyword_filename (),
 "Tempo2::keywords", Pulsar::Config::get_runtime() + "/tempo2.key",

 "Name of file containing tempo2-specific keywords",

 "When choosing a Generator with which to create a Predictor (and if tempo2 \n"
 "support is available) a list of keywords will be used to determine if a \n"
 "tempo2 predictor should be generated.  The list of keywords is parsed \n"
 "this file."
);

#endif
