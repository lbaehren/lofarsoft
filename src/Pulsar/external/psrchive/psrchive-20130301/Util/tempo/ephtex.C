/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "psrephem.h"

#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
using Legacy::psrephem;

int main (int argc, char** argv)
{
  bool aastex = false;
  bool nature = false;

  int c;
  float efac = 0.0;

  while ((c = getopt(argc, argv, "hae:nv")) != -1) {
    switch (c)  {
    case 'h':
      cerr <<
	"ephtex - produce a LaTeX formatted table of pulsar parameters\n"
	"\n"
	"USAGE:  ephtex [-hanv] [-e fac] filename\n"
	"\n"
	" -h     this help\n"
	" -a     aastex deluxe table\n"
	" -e fac multiply errors by fac\n"
	" -n     Nature suitable\n"
	"        see http://www.nature.com/nature/submit/gta/index.html#4.6\n"
	" -v     verbose on stderr\n"
	   << endl;
      return 0;

    case 'a':
      aastex = true;
      break;

    case 'e':
      efac = atof (optarg);
      break;
    case 'n':
      nature = true;
      break;

    case 'v':
      psrephem::verbose = true;
      break;
    }
  }

  if (optind == argc)
    cerr << "ephtex: provide a TEMPO parameter file as the last argument"
	 << endl;
  
  vector<psrephem> ephs;
  for (int iarg=optind; iarg<argc; iarg++) {
    psrephem eph (argv[iarg]);
    if (efac > 0)
      eph.efac (efac);
    ephs.push_back (eph);
  }

  if (aastex) {
    cout <<
      "\\begin{deluxetable}{lr}\n"
      "\\tablecaption{Derived Parameters for " << ephs[0].tex_name() << "}\n"
      "\\tablecolumns{1}\n"
      "\\tablewidth{0pt}\n"
      "\\startdata\n"
	 << endl;
  }
  else {
    cout <<
      "\\begin{tabular}{l";
    for (unsigned ie=0; ie<ephs.size(); ie++)
      cout << "r";
    cout << "}\n";

    if (!nature)
      cout << " \\hline \\hline" << endl;
  }

  cout << psrephem::tex(ephs) << endl;

  if (aastex) {
    cout <<
      "\\enddata\n"
      "\\end{deluxetable}\n"
	 << endl;
  }
  else {
    cout <<
      "\\end{tabular}\n"
	 << endl;
  }
}

