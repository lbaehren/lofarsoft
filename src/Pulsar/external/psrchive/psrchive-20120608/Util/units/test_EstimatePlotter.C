/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "EstimatePlotter.h"

#include <stdio.h>
#include <cpgplot.h>

using namespace std;

void plot_and_label (EstimatePlotter& plotter,
		     unsigned iplot, unsigned ipol, float spacing)
{
  char stokes_label[64] = "S'\\b\\dn";
  int  position = strlen (stokes_label) - 1;
  char* stokes_index = "0123";

  plotter.plot (iplot);
  cpgbox ("bcst",0,0,"bcvnst",spacing,2);

  cerr << "stokes=" << stokes_index[ipol] << endl;
  cerr << "label=" << stokes_label[0] << endl;
  
  stokes_label[position] = stokes_index[ipol];
  cerr << "poo" << endl;
  cerr << "stokes label=" << stokes_label << endl;
  
  cpgmtxt("L",3.5,.5,.5,stokes_label);

  // possible circumflex, \\(0756,0832,2247)
  cpgsch(1.05);
  cpgmtxt("L",3.5,.5,.5,"\\u\\(2247)\\d \\(2197)");
  cpgsch(1.0);
}


int main (int argc, char** argv)
{
  if (argc < 2) {
    cerr << "run \"test_plot filename\"" << endl;
    return -1;
  }

  FILE* fptr = fopen (argv[1], "r");
  if (!fptr) {
    cerr << "Could not fopen(" << argv[1] << ")";
    perror ("");
    return -1;
  }

  std::vector< Estimate<float> > stokes[4];
  std::vector< float > para;

  float p;
  float I,Q,U,V;
  float i,q,u,v;

  while (fscanf (fptr, "%f %f %f %f %f %f %f %f %f", 
		 &p, &I,&i, &Q,&q, &U,&u, &V,&v) == 9) {

    para.push_back(p);
    stokes[0].push_back (Estimate<float>(I,i));
    stokes[1].push_back (Estimate<float>(Q,q));
    stokes[2].push_back (Estimate<float>(U,u));
    stokes[3].push_back (Estimate<float>(V,v));

  }

  fclose (fptr);

  cerr << "Scanned " << para.size() << " lines from " << argv[1] << endl;

  cpgbeg (0, "?", 0, 0);
  cpgsvp (.25,.75,.15,.95);

  // the plotting class
  EstimatePlotter plotter;

  float x1, x2, y1, y2;

  // query the current size of the viewport in normalized device coordinates
  cpgqvp (0, &x1, &x2, &y1, &y2);
  cpgslw (2);

  float Ispace = 0.18;
  float space = 0.02;

  float Iscale = 0.05;
  float Sscale = 0.2;

  float xborder  = 0.03;
  float Iyborder = 0.2;
  float Syborder = 0.08;

  cpgsvp (x1, x2, y1, y1+(y2-y1)*Ispace);

  plotter.add_plot (para, stokes[0]);
  plotter.set_border (xborder, Iyborder);
  plot_and_label (plotter, 0, 0, Iscale);

  plotter.clear ();
  plotter.set_border (xborder, Syborder);

  cpgsvp (x1, x2, y1+(y2-y1)*(Ispace+space), y2);

  for (unsigned ipol=1; ipol<4; ipol++)
    plotter.add_plot (para, stokes[ipol]);

  plotter.separate_viewports();

  for (unsigned iplot=0; iplot<3; iplot++)
    plot_and_label (plotter, iplot, iplot+1, Sscale);
  
  cpgsci (1);
  cpgsls (1);
  cpgsvp (x1, x2, y1, y2);
  cpgbox ("bcnst",0,0,"",0,0);
  cpgmtxt("B",3.0,.5,.5,"Parallactic Angle (degrees)");

  cpgend ();

  return 0;

}
