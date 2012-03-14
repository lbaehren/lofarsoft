/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "CommandLine.h"
#include "tostring.h"

#include <iostream>

using namespace std;

class Test : public Reference::Able
{
public:

  Test () { x = 0.0; flag = false; }

  void parseOptions (int argc, char** argv);

  void action () { cerr << "action" << endl; }

  void parse (const std::string& arg) { cerr << "parse arg=" << arg << endl; }

  void set (double x) { cerr << "set x=" << x << endl; }

  string text;
  double x;
  vector<int> indeces;
  bool flag;
};

int main(int argc, char** argv)
{
  Test test;
  test.parseOptions (argc,argv);

  cerr << "test_CommandLine:\n"
    " -s " << test.text << "\n"
    " -x " << test.x << "\n"
    " -i " << test.indeces.size() << "\n"
    " -f " << test.flag << endl;
}

void Test::parseOptions (int argc, char** argv)
{
  CommandLine::Menu menu;
  CommandLine::Argument* arg;

  menu.set_help_header ("test_CommandLine - simple test");
  menu.set_version ("test_CommandLine version 1.0");

  arg = menu.add (flag, 'f');
  arg->set_help ("flag is toggled on every use");

  arg = menu.add (text, 's');
  arg->set_help ("string of text");

  arg = menu.add (this, &Test::parse, 'p', "arg");
  arg->set_help ("prints the argument");

  arg = menu.add (this, &Test::action, "action");
  arg->set_help ("does something");

  arg = menu.add (this, &Test::set, "set", "arg");
  arg->set_help ("sets something");

  menu.add (x, 'x');

  arg = menu.add (indeces, 'i');
  arg->set_long_name ("index");
  arg->set_help ("multiple indeces may be specified");
  arg->set_long_help
    ("This option can be used to specify the indeces of an array. \n"
     "Multiple indeces are specified by using the option more than once. \n"
     "e.g. \n"
     "\n"
     "  test_CommandLine -i 3 -i 55\n");

  //
  // Parse the command line.
  //
  menu.parse (argc, argv);
}
