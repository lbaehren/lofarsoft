//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/interface_factory.h

#ifndef __interface_factory_h
#define __interface_factory_h

// #define _DEBUG 1

namespace TextInterface
{
  /*!

    T = base class
    C = container of pointers to derived classes

    The base class interface must include a method that returns a
    text-based interface to the properties of the class:

    TextInterface::Parser* get_interface ();

    The TextInterface::Parser::get_interface_name method will be used
    to select the appropriate derived class to be constructed.  The
    new derived class instance is created by calling the clone method.

  */

  template<typename T, typename C>
  T* factory (C& ptrs, std::string name_parse)
  {
    std::string name = stringtok (name_parse, ":");

#ifdef _DEBUG
    std::cerr << "TextInterface::factory name=" << name << std::endl;
#endif

    Reference::To<T> result;

    if (name == "help")
      std::cerr <<
	"\n\n"
	"Options:"
	"\n\n";
    
    for (typename C::iterator ptr=ptrs.begin(); ptr != ptrs.end(); ptr++)
    {
      Reference::To<TextInterface::Parser> interface = (*ptr)->get_interface();
      if (interface->get_interface_name() == name)
	{
	  result = (*ptr)->clone ();
	  break;
	}
      else if (name == "help")
	{
	  std::cerr << interface->get_interface_name() << std::endl
		    << interface->help (true, false, "   ") << std::endl;
	}
    }

    if (name == "help")
      {
	std::cerr << std::endl;
	exit (0);
      }
    
    if (!result)
      throw Error (InvalidState, std::string(),
		   "no instance named '" + name + "'");
    
#ifdef _DEBUG
    std::cerr << "TextInterface::factory options=" << name_parse << std::endl;
#endif

    while (*(name_parse.begin()) == '(' && 
	   *(name_parse.end()-1) == ')')
    {
#ifdef _DEBUG
      std::cerr << "TextInterface::factory removing () brackets" << std::endl;
#endif
      name_parse.erase (name_parse.begin());
      name_parse.erase (name_parse.end()-1);
    }

    Reference::To<TextInterface::Parser> interface = result->get_interface();
    
    std::vector<std::string> options;
    standard_separation (options, name_parse);
    for (unsigned i=0; i<options.size(); i++)
    {
#ifdef _DEBUG
      std::cerr << "TextInterface::factory option["<<i<<"]="
		<< options[i] << std::endl;
#endif
      
      interface->process (options[i]);
    }

    return result.release();
  }
  
}
  
#endif
