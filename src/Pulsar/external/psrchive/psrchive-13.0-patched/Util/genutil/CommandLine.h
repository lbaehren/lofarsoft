//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/CommandLine.h,v $
   $Revision: 1.9 $
   $Date: 2009/09/07 18:16:17 $
   $Author: straten $ */

#ifndef __CommandLine_h
#define __CommandLine_h

#include "Reference.h"
#include "Functor.h"
#include "tostring.h"

#include <string>
#include <vector>

#include <getopt.h>

//! Enable a single template that works for both func(T) and func(const T&)
template<typename T>
struct arg_traits { typedef T unref; };

template<typename T>
struct arg_traits<const T&> { typedef T unref; };

namespace CommandLine {

  typedef std::pair<std::string,std::string> Help;

  //! A single item in a command line menu
  class Item : public Reference::Able
  {
  public:

    virtual ~Item () { }

    //! Return true if code matches
    virtual bool matches (int code) const = 0;

    //! Handle the argument
    virtual void handle (const std::string& arg) = 0;

    //! Return two columns of help text
    virtual Help get_help () const = 0;
  };

  class Menu;

  //! A single command line argument
  class Argument : public Item
  {

  protected:

    //! Single-character name of value
    std::string short_name;

    //! Long name of value
    std::string long_name;

    //! Type of value
    std::string type;

    //! Brief description of value
    std::string help;

    //! Detailed description of value
    std::string long_help;

    //! The has_arg attribute used by getopt_long
    int has_arg;

    //! Code assigned to this Argument by Menu class
    int val;

    friend class Menu;

  public:

    Argument () { val = 0; has_arg = no_argument; }

    void set_short_name (char c) { short_name = std::string(1,c); }
    void set_long_name (const std::string& s) { long_name = s; }
    void set_type (const std::string& s) { type = s; }
    void set_help (const std::string& s) { help = s; }
    void set_long_help (const std::string& s) { long_help = s; }
    void set_has_arg (int h) { has_arg = h; }

    //! Return true if key matches
    bool matches (int c) const { return val == c; }

    Help get_help () const;
  
  };

  //! A command line value
  template<typename T>
  class Value : public Argument
  {
  protected:

    //! Reference to the value to be set
    T& value;

  public:

    //! Default constructor
    Value (T& _value)
      : value (_value) { has_arg = required_argument; }

    //! Handle the argument
    void handle (const std::string& arg) { value = fromstring<T>(arg); }
  };

  //! A command line value that may be specified multiple times
  template<typename T>
  class Value< std::vector<T> > : public Argument
  {
  protected:

    //! Reference to the value to be set
    std::vector<T>& values;

  public:

    //! Default constructor
    Value (std::vector<T>& _values)
      : values (_values) { has_arg = required_argument; }

    //! Handle the argument
    void handle (const std::string& arg) 
    {
      values.push_back( fromstring<T>(arg) );
    }

  };

  //! A command line toggle
  template<>
  class Value<bool> : public Argument
  {
  protected:

    //! Reference to the value to be set
    bool& value;

  public:

    //! Default constructor
    Value (bool& _value) : value (_value) { }

    //! Handle the argument
    void handle (const std::string& arg) { value = !value; }
  };

  //! A command line corresponding to a class attribute
  template<class C, typename M, typename P>
  class Attribute : public Argument
  {
  protected:

    //! Pointer to the instance
    C* instance;

    //! Pointer to the method (unary member function)
    M method;

    //! Pointer to the parser (unary)
    P parse;

  public:

    //! Default constructor
    Attribute (C* _instance, M _method, P _parse)
      : instance (_instance), method (_method), parse (_parse)
    { has_arg = required_argument; }

    //! Handle the argument
    void handle (const std::string& arg)
    { (instance->*method) (parse(arg)); }
  };

  //! A command line Action
  class Action : public Argument
  {
  protected:

    //! The action to be taken
    Functor < void() > action;

  public:

    //! Default constructor
    template<class C, typename M>
    Action (C* instance, M method)
    { action = Functor< void() > (instance, method); }

    //! Handle the argument
    void handle (const std::string& arg) { action(); }
  };

  class Heading : public Item
  {

  public:

    //! Default constructor
    Heading (const std::string& _text) { text = _text; }

    //! Always eturn false
    bool matches (int) const { return false; }

    //! Handle the argument
    void handle (const std::string& arg) { }

    //! Return two columns of help text
    Help get_help () const { return Help(text,""); }

  protected:

    std::string text;
  };


  //! A command line menu
  class Menu : public Reference::Able
  {
  public:

    //! Construct with two default options: --help and --version
    Menu ();

    virtual ~Menu ();

    //! Set the help header (activates -h,--help)
    virtual void set_help_header (const std::string& s)
    { help_header=s; add_help (); }

    //! Get the help header
    std::string get_help_header () const
    { return help_header; }

    //! Set the help footer
    virtual void set_help_footer (const std::string& s) { help_footer=s; }

    //! Get the help footer
    std::string get_help_footer () const { return help_footer; }

    //! Add the --help command line option
    virtual void add_help ();

    //! Set the version information string (activates -i,--version)
    virtual void set_version (const std::string& s)
    { version_info = s; add_version (); }

    //! Add the --version command line option
    virtual void add_version ();

    //! Parse the command line
    virtual void parse (int argc, char* const *argv);

    //! Add an item to the menu
    virtual void add (Item*);

    //! Add a Value with only a single letter name
    template<typename T>
    Argument* add (T& value, char name, const char* type = 0)
    {
      return add_value (value, name, &Argument::set_short_name, type);
    }

    //! Add a Value with only a long string name
    template<typename T>
    Argument* add (T& value, const std::string& name, const char* type = 0)
    { 
      return add_value (value, name, &Argument::set_long_name, type);
    }

    //! Add an Attribute with only a single letter name
    template<class C, typename T>
    Argument* add (C* ptr, void (C::*method)(T), char name, 
		   const char* type = 0)
    {
      return add_attribute (ptr, method,
			    &fromstring< typename arg_traits<T>::unref >,
			    name, &Argument::set_short_name, type);
    }

    //! Add an Attribute with only a long string name
    template<class C, typename T>
    Argument* add (C* ptr, void (C::*method)(T), const std::string& name, 
		   const char* type = 0)
    {
      return add_attribute (ptr, method,
			    &fromstring< typename arg_traits<T>::unref >,
			    name, &Argument::set_long_name, type);
    }

    //! Add an Attribute with only a single letter name
    template<class C, typename M, typename P>
    Argument* add (C* ptr, M method, P parse, char name, const char* type)
    {
      return add_attribute (ptr, method, parse,
			    name, &Argument::set_short_name, type);
    }

    //! Add an Attribute with only a long string name
    template<class C, typename M, typename P>
    Argument* add (C* ptr, M method, P parse, const std::string& name, 
		   const char* type)
    {
      return add_attribute (ptr, method, parse,
			    name, &Argument::set_long_name, type);
    }

    //! Add an Action with only a single letter name
    template<class C>
    Argument* add (C* ptr, void (C::*method)(), char name)
    {
      return add_action (ptr, method, name, &Argument::set_short_name);
    }

    //! Add an Action with only a long string name
    template<class C>
    Argument* add (C* ptr, void (C::*method)(), const std::string& name)
    {
      return add_action (ptr, method, name, &Argument::set_long_name);
    }

    //! Add a Heading with the given text
    void add (const std::string&);

    //! Print help and exit
    void help (const std::string&);

    //! Print version and exit
    void version ();

    //! Optional Argument::val filter 
    /*! can be used to address backward compatibility issues */
    Functor< int(int) > filter;

  protected:

    std::vector< Reference::To<Item> > item;
 
    std::string version_info;
    Argument* version_item;

    std::string help_header;
    std::string help_footer;
    Argument* help_item;

    //! Add a Value with only a long string name
    template<typename T, typename N, typename S>
    Argument* add_value (T& value, N name, S set, const char* type)
    { 
      Argument* argument = new Value<T> (value);
      (argument->*set) (name);
      if (type)
	argument->set_type (type);
      item.push_back (argument);
      return argument;
    }

    template<class C, typename M, typename P, typename N, typename S>
    Argument* add_attribute (C* ptr, M method, P parse, N name, S set,
			     const char* type)
    {
      Argument* argument = new Attribute<C,M,P> (ptr, method, parse);
      (argument->*set) (name);
      if (type)
	argument->set_type (type);
      item.push_back (argument);
      return argument;
    }

    template<class C, typename M, typename N, typename S>
    Argument* add_action (C* ptr, M method, N name, S set)
    {
      Argument* argument = new Action (ptr, method);

      (argument->*set) (name);
      item.push_back (argument);
      return argument;
    }

  };


}

#endif
