/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Pulsar/Config.h"

#include <iostream>
using namespace std;

Pulsar::Option<double> 
double_test
(
 "double_test", 0.1,

 "Test of double",
 
 "not much more to say about this"
);

Pulsar::Option<std::string>
string_test
(
 "string_test", "hi there",

 "Test of string",

 "does it work?"
);

Configuration::Parameter<std::string> unconfigured_string;
Pulsar::Option<std::string>
string_wrap
(
 unconfigured_string,

 "string_wrap", "new value",

 "Test of string wrapper",

 "Wraps an existing configuration parameter with Pulsar::Config"
);

int main ()
{
  double copy = double_test;

  double_test = 4.5;

  cerr << "test=" << double_test << endl;

  Pulsar::Config::Interface* interface = Pulsar::Config::get_interface();

  // find the maximum string length of the name and description
  for (unsigned i=0; i<interface->get_nvalue(); i++) {

    string name = interface->get_name (i);

    TextInterface::Value* value = interface->find (name);

    cout << string(75, '#') << "\n"
      "#\n"
      "# " << value->get_name() << " - " << value->get_description() 
           << " = " << value->get_value() << endl;

  }

  return 0;
}
