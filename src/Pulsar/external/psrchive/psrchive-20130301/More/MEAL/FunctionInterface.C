/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/FunctionInterface.h"

using namespace std;

MEAL::Function::Interface::Interface( Function *c )
{
  if (c)
    set_instance (c);

  add( &Function::get_nparam, "nparam", "Number of parameters" );

  {
    VGenerator<string> generator;

    add_value(generator( "name", string("Parameter name"),
			  &Function::get_param_name,
			  &Function::get_nparam ));

    add_value(generator( "help", string("Parameter description"),
			  &Function::get_param_description,
			  &Function::get_nparam ));
  }

  {
    VGenerator<double> generator;

    add_value(generator( "val", string("Parameter value"),
			  &Function::get_param,
			  &Function::set_param,
			  &Function::get_nparam ));

    add_value(generator( "var", string("Parameter variance"),
			  &Function::get_variance,
			  &Function::set_variance,
			  &Function::get_nparam ));
  }

  {
    VGenerator<bool> generator;

    add_value(generator( "fit", string("Fit flag"),
			  &Function::get_infit,
			  &Function::set_infit,
			  &Function::get_nparam ));
  }

#if 0
  add( &Function::get_,
       &Function::set_,
       "", "" );
#endif

}

