//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Config.h,v $
   $Revision: 1.13 $
   $Date: 2009/11/29 12:13:49 $
   $Author: straten $ */

#ifndef __Pulsar_Config_h
#define __Pulsar_Config_h

// #define _DEBUG

#include "Configuration.h"
#include "TextInterface.h"
#include "CommandParser.h"

namespace Pulsar 
{
  //! Stores PSRCHIVE configuration parameters
  class Config : public Configuration 
  {
  public:

    //! Default constructor
    Config ();

    //! Set the configuration filename
    void set_filename (const std::string&);

    //! Return the name of the installation directory
    static std::string get_home ();
    
    //! Return the name of the runtime directory
    static std::string get_runtime ();

    //! Return the configuration key/value pairs
    static Config* get_configuration ();

    class Interface;

    //! Return the text interface to the configuration parameters
    static Interface* get_interface ();

    //! Ensure that configuration options are linked
    /*! This method is defined in More/Config_ensure_linage.C so that
      every configuration option to be defined can be included. */
    static void ensure_linkage ();

  protected:

    //! The global configuration file
    static Config* config;
    
    //! The global configuration interpreter
    static Interface* interface;

    //! Load the global configuration files
    void load ();

    std::string filename;
    bool databases_loaded;
  };


  //! Configuration option
  template<typename T>
  class Option : public Configuration::Parameter<T>
  {

  public:

    //! Construct a new Configuration::Parameter
    Option (const std::string& name, const T& default_value,
	    const std::string& description = "none",
	    const std::string& detailed_description = "none" );

    //! Construct a new Configuration::Parameter with an associated parser
    Option (CommandParser* parser,
	    const std::string& name,
	    const std::string& default_value,
	    const std::string& description = "none",
	    const std::string& detailed_description = "none" );

    //! Wrap an existing Configuration::Parameter
    Option (Configuration::Parameter<T>& parameter,
	    const std::string& name, const T& default_value,
	    const std::string& description = "none",
	    const std::string& detailed_description = "none" );

    //! Wrap an existing Configuration::Parameter with an associated parser
    Option (Configuration::Parameter<T>& parameter,
	    CommandParser* parser,
	    const std::string& name,
	    const std::string& default_value,
	    const std::string& description = "none",
	    const std::string& detailed_description = "none" );

    //! Set equal to T operator
    T& operator = (const T& t)
    { Configuration::Parameter<T>::set_value(t); return *this; }

    std::string description;
    std::string detailed_description;
  };

  template<typename T>
  std::ostream& operator << (std::ostream& ostr, const Option<T>& o)
  { return ostr << o.get_value(); }

  template<typename T>
  std::istream& operator >> (std::istream& istr, Option<T>& option)
  { T val; istr >> val; option.set_value (val); return istr; }

  class Config::Interface : public TextInterface::Parser
  {
  public:

    Interface () { alphabetical = true; }

    template<typename T>
    class Element
    {
    public:
      typedef TextInterface::Attribute< Configuration::Parameter<T> > Type;
    };

    template<typename T>
    typename Element<T>::Type* add (Option<T>* option)
    {
      TextInterface::Allocator< Configuration::Parameter<T>,T > gen;
      
      typename Element<T>::Type* getset;

      getset = gen (option->get_key(),
		    &Configuration::Parameter<T>::get_value,
		    &Configuration::Parameter<T>::set_value);

      getset->instance = option;
      getset->set_description (option->description);
      getset->set_detailed_description (option->detailed_description);

      add_value( getset );
      return getset;
    }

    template<typename T, typename Parser>
    void add (Option<T>* option, Parser* parser)
    {
      add_value( TextInterface::new_Interpreter( option->get_key(),
						 option->description,
						 option->detailed_description,
						 parser,
						 &CommandParser::empty,
						 &CommandParser::parse ) );
    }
  };
}


template<typename T>
Pulsar::Option<T>::Option (const std::string& _name,
			   const T& _default,
			   const std::string& _description,
			   const std::string& _detailed_description)
  : Configuration::Parameter<T> (_name, Config::get_configuration(), _default)
{
  DEBUG("Pulsar::Option<T> name=" << _name \
	<< " description=" << _description << " default=" << _default);

  description = _description;
  detailed_description = _detailed_description;

  DEBUG("Pulsar::Option<T> add to interface");

  Config::get_interface()->add (this);

  DEBUG("Pulsar::Option<T> return");
}

template<typename T>
Pulsar::Option<T>::Option (CommandParser* parser,
			   const std::string& _name,
			   const std::string& _default,
			   const std::string& _description,
			   const std::string& _detailed_description )
  : Configuration::Parameter<T> (_name, Config::get_configuration(),
				 parser, _default)
{
  DEBUG("Pulsar::Option<T> parser around Configuration::parameter");

  description = _description;
  detailed_description = _detailed_description;

  DEBUG("Pulsar::Option<T> parser add to interface");

  Config::get_interface()->add (this, parser);

  DEBUG("Pulsar::Option<T> parser return");
}

template<typename T>
Pulsar::Option<T>::Option (Configuration::Parameter<T>& parameter,
			   const std::string& _name, const T& _default,
			   const std::string& _description,
			   const std::string& _detailed_description )
  : Configuration::Parameter<T> (_name, Config::get_configuration(), _default)
{
  DEBUG("Pulsar::Option<T> wrap Configuration::parameter");

  description = _description;
  detailed_description = _detailed_description;
  parameter.set_key (_name);

  parameter.set_loader( this->loader );
  this->loader = 0;

  DEBUG("Pulsar::Option<T> wrap add to interface");

  Config::get_interface()->add(this)->instance = &parameter;

  DEBUG("Pulsar::Option<T> wrap return");
}

template<typename T>
Pulsar::Option<T>::Option (Configuration::Parameter<T>& parameter,
			   CommandParser* parser,
			   const std::string& _name,
			   const std::string& _default,
			   const std::string& _description,
			   const std::string& _detailed_description )
  : Configuration::Parameter<T> (_name, Config::get_configuration(),
				 parser, _default)
{
  DEBUG("Pulsar::Option<T> wrap parser around Configuration::parameter");

  description = _description;
  detailed_description = _detailed_description;
  parameter.set_key (_name);

  DEBUG("Pulsar::Option<T> wrap parser that=" << &parameter \
	<< " loader=" << this->loader);

  parameter.set_loader( this->loader );
  this->loader = 0;

  DEBUG("Pulsar::Option<T> wrap parser add to interface");

  Config::get_interface()->add (this, parser);

  DEBUG("Pulsar::Option<T> wrap parser return");
}



#endif

