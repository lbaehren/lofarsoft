/***************************************************************************
 *
 *   Copyright (C) 2007-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "evaluate.h"
#include "ThreadContext.h"

#include "templates.h"
#include "Error.h"
#include "pad.h"
#include "tostring.h"
#include "stringtok.h"

// evaluateExpression from Parsifal Software
#include "evaldefs.h"
#include "substitute.h"

#include <string>
#include <iostream>
#include <functional>

#include <math.h>
#include <ctype.h>

using namespace std;

#ifdef _DEBUG
#define DEBUG(x) cerr << x << endl;
#else
#define DEBUG(x)
#endif

double compute (const string& eval)
{
  static ThreadContext* context = 0;
  if (!context)
    context = new ThreadContext;

  ThreadContext::Lock lock (context);

  string result = "__result";
  string expression = result + " = (" + eval + ")";

  // Call parser function from Parsifal Software
  int errorFlag = evaluateExpression (const_cast<char*>(expression.c_str()));

  if (errorFlag)
    throw Error (InvalidParam, "compute",
		 "%s in expression\n\n  %s\n  %s",
		 errorRecord.message, eval.c_str(), 
		 pad (errorRecord.column-10, "^", false).c_str());
    
  for (int i = 0; i < nVariables; i++)
    if (variable[i].name == result)
      return variable[i].value;

  throw Error (InvalidParam, "compute", "internal error");
}

string evaluate1 (const string& eval, unsigned precision)
{
  double value = compute (eval);

  if (precision)
    return tostring (value, precision);
  else
    return tostring (value);
}

string get_command (string& text)
{
  const char* commands[] = { "min", "max", "med", "sum", "avg", "rms", 0 };

  for (const char** cmd = commands; *cmd != 0; cmd++)
  {
    DEBUG("cmd='" << *cmd << "'");

    string test = *cmd;

    string::size_type in = text.rfind (test);
    if (in == string::npos)
      continue;

    // ensure that the command occurs at the end of the text
    if (in + test.length() == text.length())
    {
      text = text.substr (0, in);
      return test;
    }
  }

  return string();
}

string evaluate2 (const string& command, string vals, unsigned precision)
{
  vector<double> values;
  while (vals.length())
  {
    string val = stringtok (vals, ",");
    values.push_back( fromstring<double>(val) );
  }

  if (!values.size())
    return string();

  double result = 0.0;

  if (command == "min")
    result = *std::min_element (values.begin(), values.end());

  else if (command == "max")
    result = *std::max_element (values.begin(), values.end());

  else if (command == "med")
  {
    unsigned mid = values.size() / 2;
    std::nth_element (values.begin(), values.begin()+mid, values.end());
    result = values[mid];
  }

  else if (command == "sum")
    result = sum (values);

  else if (command == "avg")
    result = mean (values);

  else if (command == "rms")
    result = sqrt( variance(values) );

  if (precision)
    return tostring (result, precision);
  else
    return tostring (result);
}

string evaluate (const string& text, char cstart, char cend) try
{
  string remain = text;
    
  string::size_type start;
  string::size_type end;

  while ( (end = remain.find(cend)) != string::npos &&
	  (start = remain.rfind(cstart, end)) != string::npos )
  {

    // find the end of the variable name
    string eval = remain.substr (start+1, end-start-1);

    DEBUG("eval='" << eval << "'");

    // string preceding the variable substitution
    string before = remain.substr (0, start);

    DEBUG("before='" << before << "'");

    // preceding text may be a command
    string command = get_command (before);

    DEBUG("command='" << command << "'");

    end ++;

    unsigned precision = 0;

    if (remain[end] == '%')
    {
      end ++;

      // make an Adaptable Predicate out of std C isdigit
      Functor< bool(char) > pred ( (int(*)(int)) isdigit );

      string::size_type pend = find_first_if (remain, std::not1(pred), end);
      string::size_type len = (pend != string::npos ) ? pend-end : pend;

      precision = fromstring<unsigned> (remain.substr (end, len));

      end = pend;
    }

    // string following the variable substitution
    string after = (end != string::npos) ? remain.substr (end) : "";

    DEBUG("after='" << after << "'");

    string subst;

    if (command.empty())
      subst = evaluate1 (eval, precision);
    else
      subst = evaluate2 (command, eval, precision);

    remain = before + subst + after;
  }

  return remain;
}
catch (std::exception& error)
{
  throw Error (InvalidParam, "evaluate", error.what());
}
