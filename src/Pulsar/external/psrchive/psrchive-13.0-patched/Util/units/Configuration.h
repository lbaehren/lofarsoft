//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Configuration.h,v $
   $Revision: 1.8 $
   $Date: 2009/11/29 12:13:50 $
   $Author: straten $ */

#ifndef __Configuration_h
#define __Configuration_h

#include "tostring.h"
#include "debug.h"
#include "Error.h"

#include <vector>
#include <iostream>
#include <sstream>

//! Stores keyword-value pairs from a configuration file
/*! The Configuration class enables convenient, distributed access to
  configuration parameters stored in a simple text file */
class Configuration
{
public:

  virtual ~Configuration () {}

  //! Construct from the specified file
  Configuration (const char* filename = 0);

  //! Load the configuration from the specified file
  void load (const std::string& filename) throw (Error);

  //! Keyword-value pair
  class Entry;

  //! A configurable parameter
  template<typename T> class Parameter;

  //! Get the value for the specified key
  template<typename T> T get (const std::string& key, T default_value) const;

  //! Find the entry with the specified key
  Entry* find (const std::string& key) const;

  //! Get the names of the configuration files in the order they were parsed
  const std::vector<std::string>& get_filenames () const { return filenames; }

  //! Get the number of times that the find method has been called
  unsigned get_find_count () const { return find_count; }

protected:

  //! Allow derived types to implement lazy evaluation of file loading
  virtual void load () {}

  std::vector<std::string> filenames;
  std::vector<Entry> entries;

private:

  mutable unsigned find_count;
};

class Configuration::Entry
{
public:
  Entry (std::string& k, std::string& v) { key=k; value=v; }
  std::string key;
  std::string value;
};


template<class T>
class Configuration::Parameter
{
public:

  Parameter ()
  {
    loader = 0;
  }

  Parameter (const T& val)
  {
    value = val;
    loader = 0;
  }

  Parameter (const std::string& _key, Configuration* config, T default_value)
  {
    key = _key;
    loader = new DirectLoader (default_value);
    loader->parameter = this;
    loader->configuration = config;
  }

  template<typename Parser>
  Parameter (const std::string& _key, Configuration* config,
	     Parser* parser, const std::string& default_value)
  {
    key = _key;
    loader = new ParseLoader<Parser> (parser, default_value);
    loader->parameter = this;
    loader->configuration = config;
  }

  ~Parameter ()
  {
    if (loader)
      delete loader;
  }

  //! Cast to T& operator
  operator T& ()
  {
    if (loader)
      loader->load();

    return value;
  }

  //! Get a copy of the value
  T get_value () const
  {
    if (loader)
      loader->load();

    return value;
  }

  //! Set the value
  void set_value (const T& val)
  {
    value = val;
    if (loader)
      loader->destroy();
  }

  //! Set equal to T operator
  T& operator = (const T& t) { set_value(t); return value; }

  //! Equality test operator
  bool operator == (const T& t) const { return get_value() == t; }

  //! Loads parameter when it is requested (lazy evaluation)
  class Loader;

  //! Parses configuration directly
  class DirectLoader;

  //! Uses an intermediate parser to load from configuration
  template<class P> class ParseLoader;

  std::string get_key () const { return key; }
  void set_key (const std::string& _key) { key = _key;}

  void set_loader (Loader* _load)
  {
    DEBUG("Configuration::Parameter::set_loader this=" << this \
	  << " key=" << key << " loader=" << _load);

    if (loader)
      loader->destroy();
    loader = _load;
    loader->parameter = this;
  }

protected:
  std::string key;
  T value;
  Loader* loader;
};

template<typename T>
class Configuration::Parameter<T>::Loader
{
public:

  virtual ~Loader() {}

  //! Load the parameter value
  virtual void load () = 0;

  void destroy ()
  {
    parameter->loader = 0;
    delete this;
  }

  Configuration* configuration;
  Parameter* parameter;
};

template<typename T>
class Configuration::Parameter<T>::DirectLoader : public Loader
{
public:

  DirectLoader (const T& _default) { default_value = _default; }

  //! Load the parameter value
  virtual void load ()
  {
    Parameter<T>* param = this->parameter;
    Configuration* config = this->configuration;

    param->value = config->get (param->key, default_value);
    DEBUG("Configuration::Parameter<T>::Loader::load key=" << param->key \
	  << " default=" << default_value << " value=" << param->value);

    this->destroy();
  }

protected:
  T default_value;
};

template<typename T>
template<typename P>
class Configuration::Parameter<T>::ParseLoader : public Loader
{
public:

  ParseLoader ()
  {
    parser = 0;
  }

  ParseLoader (P* _parser, const std::string& _default)
  {
    parser = _parser;
    default_value = _default;
  }

  //! Load the parameter value
  virtual void load ()
  {
    std::string value 
      = this->configuration->get (this->parameter->key, default_value);

    DEBUG("Configuration::Parameter<T>::ParseLoader::load" \
	  " key=" << this->parameter->key << \
	  " default=" << this->default_value << \
	  " value=" << value);

    parser->parse( value );

    /* the parser must call either set_value or the assignment operator,
       which will result in the desctruction of this loader */
  }

protected:
  P* parser;
  std::string default_value;
};

//! Get the value for the specified key
template<typename T> 
T Configuration::get (const std::string& key, T default_value) const
{
  Entry* entry = find (key);
  if (entry)
  {
    DEBUG("Configuration::get found entry->value=" << entry->value); 
    return fromstring<T> (entry->value);
  }

  DEBUG("Configuration::get default " << key << " = " << default_value);

  return default_value;
}

#endif

