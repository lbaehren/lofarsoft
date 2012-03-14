/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/BasicArchive.h"
#include "Pulsar/Agent.h"
#include "Plugin.h"

#include <stdlib.h>

using namespace std;

// the plugins loader
static Registry::Plugin plugins;

void Pulsar::Archive::Agent::plugin_load ()
{
  Pulsar::BasicArchive::ensure_linkage();

  if (verbose == 3) {
    cerr << "Pulsar::Archive::Agent::plugin_load" << endl;
    Registry::Plugin::verbose = true;
  }

  if (plugin_path.length() != 0)  {
    if (verbose > 1)
      cerr << "Pulsar::Archive::Agent::plugin_load plugin_path="
           << plugin_path << endl;

    plugins.load (plugin_path);
  }

  if (plugins.ok.size() != 0) {
    if (verbose > 1)
      cerr << "Pulsar::Archive::Agent::plugin_load ok="
           << plugins.ok.size() << endl;
    return;
  }

  char* env = getenv ("PSRCHIVE_PLUGINS");

  if (env) {

    if (verbose > 1)
      cerr << "Pulsar::Archive::Agent::plugin_load"
              " PSRCHIVE_PLUGINS=" << env <<endl;

    plugins.load (env);

  }
  else {

    string path = get_plugin_path ("PSRHOME");

    if (verbose > 1)
      cerr << "Pulsar::Archive::Agent::plugin_load from " << path << endl;

    plugins.load (path);

  }

}

void Pulsar::Archive::Agent::plugin_report ()
{
  if (plugins.ok.size() == 0)
    cerr << "Archive::Agent::report No successfully loaded plugins." << endl;
  else
    cerr << "Archive::Agent::report Successfully loaded plugins:" << endl;
  
  unsigned ip = 0;
  
  for (ip=0; ip < plugins.ok.size(); ip++)
    cerr << " " << plugins.ok[ip] << endl;

  cerr << endl;

  if (plugins.fail.size() == 0)
    return;

  cerr << "Archive::Agent::report Failed plugins:" << endl;

  for (unsigned ip=0; ip < plugins.fail.size(); ip++)
    cerr << "##############################################################\n"
	 << plugins.fail[ip] << endl;

  cerr << endl;
}

/*! constructs the plugin directory name from environment variables */
string Pulsar::Archive::Agent::get_plugin_path (const char* shell_variable)
{
  char* env = getenv (shell_variable);

  string path;

  if (!env)
    return path;

  path = env;
  path += "/lib";

  env = getenv ("LOGIN_ARCH");
  if (env) {
    path += "/";
    path += env;
  }

  path += "/Pulsar";

  return path;
}

