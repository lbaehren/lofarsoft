/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "RegularExpression.h"

#include <sys/types.h>
#include <regex.h>

using namespace std;

//! Default constructor
RegularExpression::RegularExpression ()
{
  preg = 0;
}

//! Construct with regular expression
RegularExpression::RegularExpression (const string& regexp)
{
  preg = 0;
  set_regexp (regexp);
}

//! Destructor
RegularExpression::~RegularExpression ()
{
  destroy ();
}

void RegularExpression::destroy ()
{
  if (!preg)
    return;
  regex_t* the_preg = (regex_t*) preg;
  regfree (the_preg); delete the_preg; preg = 0;
}

//! Set the regular expression
void RegularExpression::set_regexp (const string& regexp)
{
  destroy ();

  if (regexp.empty())
    return;

  preg = new regex_t;

  int err = regcomp ((regex_t*)preg, regexp.c_str(), REG_NOSUB);

  if (err != 0) 
    throw error (err, "set_regexp", "error regcomp(" + regexp + ")");

}

//! Check if the supplied string matches the regular expression
bool RegularExpression::get_match (const string& test)
{
  if (!preg)
    throw Error (InvalidState, "RegularExpression::get_match",
		 "no regular expression specified");

  int match = regexec ((regex_t*)preg, test.c_str(), 0, NULL, 0);

  if (match == REG_NOMATCH)
    return false;

  if (match != 0)
    throw error (match, "get_match", "error regexec(" + test + ")");
  
  return true;
}

Error RegularExpression::error (int err, const string& func, const string& t)
{
  string this_msg = t;

  char errmsg [1024];
  regerror (err, (regex_t*)preg, errmsg, 1024);

  this_msg += ": ";
  this_msg += errmsg;

  return Error (FailedCall, "RegularExpression::" + func, this_msg);
}
