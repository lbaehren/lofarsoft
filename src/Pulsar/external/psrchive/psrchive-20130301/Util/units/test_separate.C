/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "separate.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

void test_separate( const string& input,
		    const vector<string>& expected,
		    const string& delimiters = string() )
{
  vector<string> result;

  if (delimiters.size())
    separate (input, result, delimiters);
  else
    separate (input, result);

  if (result.size() != expected.size())
  {
    cerr << "FAIL: separate(" << input << ") yields " << result.size()
	 << " elements - expected " << expected.size() << endl;
    exit (-1);
  }

  for (unsigned i=0; i < result.size(); i++)
    if (result[i] != expected[i])
    {
      cerr << "FAIL: separate result[" << i << "]=" << result[i]
	   << " != expected=" << expected[i] << endl;
      exit (-1);
    }
}

int main ()
{
  cerr << "testing string separation ... should finish immediately" << endl;

  {
    string input = "one=1, two,three, four=(4,5)";
    string delim = " ,";

    vector<string> expect (4);
    expect[0] = "one=1";
    expect[1] = "two";
    expect[2] = "three";
    expect[3] = "four=(4,5)";

    test_separate (input, expect, delim);
  }


  {
    string input = "one \"two, three and four\"";
    vector<string> expect (2);
    expect[0] = "one";
    expect[1] = "\"two, three and four\"";

    test_separate (input, expect);
  }

  {
    string input = "1,2 3,4";
    vector<string> expect (2);
    expect[0] = "1,2";
    expect[1] = "3,4";

    test_separate (input, expect);
  }

  cerr << "PASS: all tests passed" << endl;

  return 0;
}

