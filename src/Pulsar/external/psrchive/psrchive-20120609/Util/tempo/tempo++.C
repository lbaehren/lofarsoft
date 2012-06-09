/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tempo++.h"
#include "SystemCall.h"
#include "Error.h"
#include "dirutil.h"
#include "strutil.h"
#include "fsleep.h"

#include <fstream>
#include <stdio.h>

using namespace std;

// //////////////////////////////////////////////////////////////////////
// members of the Tempo namespace
//
bool   Tempo::verbose = false;
bool   Tempo::debug = false;
string Tempo::extension (".tpo");
MJD    Tempo::unspecified;

// base directory in which TEMPO will work
string Tempo::tmpdir = "/tmp/tempo/";

// file to which tempo ephemeris will be written
string Tempo::ephem_filename = "pulsar.eph";

// file to which tempo stderr will be redirected
string Tempo::stderr_filename = "stderr.txt";

// file to which tempo stdout will be redirected
string Tempo::stdout_filename = "stdout.txt";

Warning Tempo::warning;

// //////////////////////////////////////////////////////////////////////
// static storage of the tempo system call.
// can be set with Tempo::set_system 
//
static string system_call ("tempo");

// //////////////////////////////////////////////////////////////////////
// static storage of the tempo version number.
// is set on call to Tempo::set_system 
//
static float version = -1.0;

// //////////////////////////////////////////////////////////////////////
// get_system_version
//   Static function makes a system call to tempo using the current value
// of 'system_call' to query the version number
static void get_system_version ()
{
  string call = system_call + " -v";

  FILE* fptr = popen (call.c_str(),"r");
  if (!fptr) {
    fprintf (stderr, "Tempo::get_system_version failed '%s'\n", call.c_str());
    return;
  }
  if (fscanf (fptr," Tempo v %f", &version) < 1)
    fprintf (stderr, "Tempo::get_system_version failed to parse version\n");

  pclose (fptr);
}

float Tempo::get_version ()
{
  if (version < 0)
    get_system_version ();
  return version;
}

void Tempo::set_system (const string& sys_call)
{
  system_call = sys_call;
  get_system_version ();
}

string Tempo::get_system ()
{
  return system_call;
}

// get the specified tempo configuration parameter
std::string Tempo::get_configuration (const std::string& parameter)
{
  static char* tpodir = getenv("TEMPO");

  if (!tpodir)
    throw Error (InvalidState, "Tempo::get_configuration",
		 "TEMPO environment variable not defined");

  string filename = tpodir;
  filename += "/tempo.cfg";

  ifstream input (filename.c_str());
  if (!input)
    throw Error (FailedSys, "Tempo::get_configuration",
		 "ifstream (" + filename + ")");

  string line;

  while (!input.eof()) {

    getline (input, line);
    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    // the first key loaded should be the name of the instance
    string key = stringtok (line, " \t");

    if (verbose)
      cerr << "Tempo::get_configuration key=" << key << endl;

    if (key == parameter)
      return stringtok (line, " \t");

  }

  throw Error (InvalidParam, "Tempo::get_configuration",
	   "configuration parameter '" + parameter + "' not found");

}


// run tempo with the given arguments
void Tempo::tempo (const string& arguments, const string& input)
{
  if (!getenv("TEMPO"))
    throw Error (InvalidState, "Tempo::tempo",
                 "TEMPO environment variable not defined");

  string runtempo = get_system() + " " + arguments;

  if (!Tempo::verbose)
    runtempo += " > " + stdout_filename + " 2> " + stderr_filename;

  if (!input.empty())
  {
    string tmp_input = input;
    if (input[input.length()-1] != '\n')
      tmp_input += '\n';
    runtempo += " << EOD\n" + tmp_input + "EOD\n";
  }

  if (verbose)
    cerr << "Tempo::tempo system (" << runtempo << ")" << endl;

  SystemCall shell;
  shell.run( runtempo );
}

//! Convert a telescope name to a code
char Tempo::code (const string& name)
{
  const Observatory* obs = observatory (name);

  if (!obs)
    throw Error (InvalidParam, "Tempo::code",
		 "no observatory named " + name);

  return obs->get_code();
}

