/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "LogFile.h"

#include <iostream>

using namespace std;

int main () try
{
  LogFile log;

  log.add ("2008-12-13-12:34:56", "this is the first log message");
  log.add ("2009-12-13-12:34:56", "one year later");
  log.add ("2010-01-13-12:34:56", "one month later");
  log.add ("2007-02-01-10:04:06", "before");

  // one second after the first message
  std::string message = log.get_message ("2007-02-01-10:04:07");

  if (message != "before")
  {
    cerr << "test_LogFile unexpected result = " << message << endl;
    return -1;
  }

  try
    {
      // one second before the first message
      log.get_message ("2007-02-01-10:04:06");
    }
  catch (Error& error)
    {
      // expected exception caught
    }

  cerr << "test_LogFile: all tests passed" << endl;

  return 0;
}
catch (Error& error)
  {
    cerr << error << endl;
    return -1;
  }
