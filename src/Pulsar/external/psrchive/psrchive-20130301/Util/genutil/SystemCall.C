/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "SystemCall.h"
#include "Error.h"
#include "fsleep.h"

#include <stdlib.h>
#include <string.h>

using namespace std;

void SystemCall::run (const string& command)
{
  string errstr;

  if (!system(NULL))
    throw Error (InvalidState, "SystemCall::run",
                 "shell not available; insufficient resources");
 
  for (unsigned i=0; i < retries; i++)
  {    
    int err = system (command.c_str());

    if (!err)
      return;

    // else an error occured
    if (err < 0)
      errstr = strerror (err);

    else if (WIFSIGNALED(err))
      errstr = "\n\t" "terminated by signal "
	+ string(strsignal(WTERMSIG(err)));

    else if (WIFEXITED(err))
      errstr = "\n\t" "returned error code "
	+ tostring(WEXITSTATUS(err));

    else
      errstr = "\n\t" "failed for an unknown reason";

    fsleep (5e-4);
    retries --; 
  }

  // the above loop finished without a successful return
  throw Error (FailedCall, "SystemCall::run", "system (\"" + command + "\")"
	       " failed: " + errstr);
}
