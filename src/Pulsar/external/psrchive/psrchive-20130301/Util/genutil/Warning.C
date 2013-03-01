/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Warning.h"

using namespace std;

bool Warning::verbose = false;

Warning::Warning (std::ostream& o) : output(o)
{
  context = new ThreadContext;
  first = true;
  all = false;
  summary = true;
}

Warning::~Warning ()
{
  if (summary)
  {
    std::map<std::string,unsigned>::iterator msg;
    for (msg = messages.begin(); msg != messages.end(); msg++)
      if (msg->second > 1)
	output << prefix << msg->first 
	       << " [repeated " << msg->second << " times]" << endl;
  }
  delete context;
}

Warning& operator<< (Warning& warning, const char* t)
{
  ThreadContext::Lock (warning.context);

  if (Warning::verbose)
    cerr << "Warning::operator << const char*" << endl;

  warning.message += t;
  warning.buffer << t;

  return warning;
}

Warning& operator<< (Warning& warning, manipulator m)
{
  ThreadContext::Lock (warning.context);

  if (Warning::verbose)
    cerr << "Warning::operator << manipulator" << endl;

  warning.insertion(m);
  return warning;
}

void Warning::insertion (manipulator m)
{
  // has the buffered message already been printed?
  std::map<std::string,unsigned>::iterator msg = messages.find(message);

  bool new_message = msg == messages.end();

  if (new_message)
    messages[message] = 1;
  else
    msg->second ++;

  if ( all || (first && new_message) )
    // print the buffer to the ostream, with the prefix and manipulator
    output << prefix << buffer.str() << m;

  // empty the buffers
  buffer.str("");
  message = "";
}

