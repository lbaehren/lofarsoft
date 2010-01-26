/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorExtensionInterface.h"

using namespace std;

Pulsar::PolnCalibratorExtension::Interface::Interface
( PolnCalibratorExtension *s_instance )
{
  if (s_instance)
    set_instance (s_instance);

  // read-only: requires resize
  add( &PolnCalibratorExtension::get_nparam,
       "nparam", "Number of calibration parameters" );

  import ( CalibratorExtension::Interface::Interface() );

  import( "eqn", Transformation::Interface(),
          (Transformation*(PolnCalibratorExtension::*)(unsigned))
	  &PolnCalibratorExtension::get_transformation,
          &PolnCalibratorExtension::get_nchan );
}

Pulsar::PolnCalibratorExtension::Transformation::Interface::Interface ()
{
  {
    VGenerator<string> generator;
    add_value(generator( "name", "Parameter name",
			 &Transformation::get_param_name,
			 &Transformation::set_param_name,
			 &Transformation::get_nparam ));

    add_value(generator( "desc", "Parameter description",
			 &Transformation::get_param_description,
			 &Transformation::set_param_description,
			 &Transformation::get_nparam ));
  }

  {
    VGenerator<double> generator;
    add_value(generator( "val", "Parameter value",
			 &Transformation::get_param,
			 &Transformation::set_param,
			 &Transformation::get_nparam ));

    add_value(generator( "var", "Parameter variance",
			 &Transformation::get_variance,
			 &Transformation::set_variance,
			 &Transformation::get_nparam ));
  }

   add( &Transformation::get_chisq,
	&Transformation::set_chisq,
	"chisq", "Chi squared" );

   add( &Transformation::get_nfree,
	&Transformation::set_nfree,
	"nfree", "Number of free parameters" );
}
