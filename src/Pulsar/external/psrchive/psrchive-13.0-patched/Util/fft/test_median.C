/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "median_smooth.h"

#include <iostream>

using namespace std;
using namespace fft;

template <typename T>
void test ()
{
  vector<T> data (100);
  median_smooth (data, 5);
  median_filter (data, 5, T(4));
}

int main (int argc, char** argv)
{
  test<double> ();
  test<unsigned> ();
}
