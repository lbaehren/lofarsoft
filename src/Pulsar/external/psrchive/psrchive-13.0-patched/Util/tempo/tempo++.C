/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "tempo++.h"
#include "Error.h"
#include "dirutil.h"
#include "strutil.h"
#include "fsleep.h"

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#else
#include <wait.h>
#endif

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
// static storage of the tempo working directory
// can be set with Tempo::set_directory
//
static string directory;

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

void Tempo::set_directory (const string& dir)
{
  directory = dir;
}

string Tempo::get_directory ()
{
  char* unknown = "unknown";

  if (!directory.length())
  {
    char* userid = getenv ("USER");
    if (!userid)
      userid = unknown;

    directory = string ("/tmp/tempo/") + userid;
  }

  if (makedir (directory.c_str()) < 0)
  {
    if (verbose)
      cerr << "Tempo::get_directory failure creating '" << directory 
	   << "'" << endl;

    char* home = getenv ("HOME");

    if (home)
      directory = home;
    else
      directory = ".";

    directory += "/tempo.tmp";

    if (makedir (directory.c_str()) < 0)
      throw Error (InvalidState, "Tempo::get_directory",
		   "cannot create a temporary working directory");
  }
  else
    chmod ("/tmp/tempo/", 0777);

  return directory;
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
    line = stringtok (&line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    // the first key loaded should be the name of the instance
    string key = stringtok (&line, " \t");

    if (verbose)
      cerr << "Tempo::get_configuration key=" << key << endl;

    if (key == parameter)
      return stringtok (&line, " \t");

  }

  throw Error (InvalidParam, "Tempo::get_configuration",
	   "configuration parameter '" + parameter + "' not found");

}

static int lock_fd = -1;
static bool have_lock = false;

static void open_lockfile ()
{
  string filename = Tempo::get_lockfile();
  const char* fname = filename.c_str();

  if (lock_fd < 0)
    lock_fd = open (fname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  if (lock_fd < 0)
    throw Error (FailedSys, "Tempo::open_lockfile", "failed open(%s)", fname);
}

// run tempo with the given arguments
void Tempo::lock ()
{
  if (have_lock)
    return;

  open_lockfile ();

  struct flock lock;
  lock.l_whence = SEEK_SET;
  lock.l_type   = F_WRLCK;
  lock.l_start  = 0;
  lock.l_len    = 0;

  if (fcntl (lock_fd, F_SETLKW, &lock) < 0)
    throw Error (FailedSys, "Tempo::lock", "failed fcntl(%s,F_SETLKW F_WRLCK)",
		 get_lockfile().c_str());

  have_lock = true;
}

void Tempo::unlock ()
{
  if (!have_lock)
    return;

  open_lockfile ();

  struct flock lock;
  lock.l_whence = SEEK_SET;
  lock.l_type   = F_UNLCK;
  lock.l_start  = 0;
  lock.l_len    = 0;

  if (fcntl (lock_fd, F_SETLK, &lock) < 0)
    throw Error (FailedSys, "Tempo::unlock", "failed fcntl(%s,F_SETLK F_UNLCK)",
		 get_lockfile().c_str());

  have_lock = false;
}

string Tempo::get_lockfile () 
{
  return get_directory() + "/.lock";
}

/* This command should be run only after a lock is obtained */
void Tempo::clean ()
{
  string clean_command = "rm -f " + get_directory() + "/*";
  system (clean_command.c_str());
}

// run tempo with the given arguments
void Tempo::tempo (const string& arguments, const string& input)
{
  if (!getenv("TEMPO"))
    throw Error (InvalidState, "Tempo::tempo",
                 "TEMPO environment variable not defined");

  char cwd[FILENAME_MAX];

  if (getcwd (cwd, FILENAME_MAX) == NULL)
    throw Error (FailedSys, "Tempo::tempo", "failed getcwd");
  
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

  int retries = 3;
  string errstr;

  if (!system(NULL))
    throw Error (InvalidState, "Tempo::tempo",
                 "shell not available; insufficient resources");

  while (retries)
  {    
    if (chdir (get_directory().c_str()) != 0)
      throw Error (FailedSys, "Tempo::tempo",
		   "failed chdir(" + get_directory() + ")");

    lock ();

    int err = system (runtempo.c_str());

    unlock ();

    if (chdir (cwd) != 0)
      throw Error (FailedSys, "Tempo::tempo", "failed chdir(%s)", cwd);

    if (!err)
      return;

    // else an error occured
    if (err < 0)
      errstr = strerror (err);
    else if (WIFSIGNALED(err))
      errstr = stringprintf ("\n\tTempo terminated by signal %s", 
			     strsignal(WTERMSIG(err)));
    else if (WIFEXITED(err))
      errstr = stringprintf ("\n\tTempo returned error code %i",
			     WEXITSTATUS(err));
    else
      errstr = "\n\tTempo failed for an unknown reason";

    fsleep (5e-4);
    retries --; 
  }

  // the above loop finished without a successful return
  throw Error (FailedCall, "Tempo::tempo", "system (\"" + runtempo + "\")"
	       " failed: " + errstr);
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

