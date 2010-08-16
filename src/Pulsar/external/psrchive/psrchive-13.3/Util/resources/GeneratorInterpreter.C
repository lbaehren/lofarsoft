/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// #define _DEBUG

#include "Pulsar/GeneratorInterpreter.h"

#ifdef HAVE_TEMPO2
#include "T2Generator.h"
#endif
#include "Predict.h"

using namespace std;

Pulsar::Generator::Interpreter::Interpreter ()
{
  DEBUG("Pulsar::Generator::Interpreter");

  add_command 
    ( &Generator::Interpreter::polyco,
      "polyco", "use tempo polyco by default",
      "usage: polyco \n" );

#ifdef HAVE_TEMPO2
  add_command 
    ( &Generator::Interpreter::tempo2,
      "tempo2", "use tempo2 predictor by default",
      "usage: tempo2 \n" );
#endif
}


string Pulsar::Generator::Interpreter::polyco (const string& args) try
{
  DEBUG("Pulsar::Generator::Interpreter::polyco");

  get_default_generator().set_value( new Tempo::Predict );
  current = "polyco";
  return "";
}
catch (Error& error)
{
  return error.get_message();
}

#ifdef HAVE_TEMPO2

string Pulsar::Generator::Interpreter::tempo2 (const string& args) try
{ 
  DEBUG("Pulsar::Generator::Interpreter::tempo2");

  get_default_generator().set_value( new Tempo2::Generator );
  current = "tempo2";
  return "";
}
catch (Error& error) {
  return error.get_message();
}

#endif
    
string Pulsar::Generator::Interpreter::empty ()
{
  DEBUG("Pulsar::Generator::Interpreter::empty");

  // ensure that the Configuration::Parameter is loaded
  get_default_generator().get_value();

  return current;
}

Configuration::Parameter<Pulsar::Generator*>&
Pulsar::Generator::Interpreter::get_option ()
{
  DEBUG("Pulsar::Generator::Interpreter::get_option");
  return get_default_generator();
}

