/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <stdio.h>
#include <cpgplot.h>
#include <math.h>

void draw_band (int i, double lst0, double lst1)
{
  double y0 = i;
  double y1 = i+1;

  if (lst1 < lst0)
    {
      cpgrect (lst0, 24, y0, y1);
      cpgrect (0, lst1, y0, y1);
    }
  else
    cpgrect (lst0, lst1, y0, y1);
}

void label (int i, double lst, const char* txt)
{
  cpgptxt (lst, i+0.25, 0, 0.0, txt);
}
 
int main (int argc, char** argv)
{
  if (argc == 1)
  {
    fprintf (stderr, "Please specify input filename\n");
    return -1;
  }

  FILE* fptr = fopen (argv[1],"r");
  if (!fptr)
  {
    fprintf (stderr, "Could not open '%s' for reading\n", argv[1]);
    return -1;
  }

  // open the plot device
  cpgopen ("psrlst.eps/cps");
  cpgsvp (0.05,0.8, 0.15,0.85);

  float heat_l[] = {0.0, 0.2, 0.4, 0.6, 1.0};
  float heat_r[] = {0.0, 0.5, 1.0, 1.0, 1.0};
  float heat_g[] = {0.0, 0.0, 0.5, 1.0, 1.0};
  float heat_b[] = {0.0, 0.0, 0.0, 0.3, 1.0};
  float contrast = 1.0;
  float brightness = 0.5;

  cpgctab (heat_l, heat_r, heat_g, heat_b, 5, contrast, brightness);

  char text [64];
  unsigned hr0, min0, hr1, min1;
  int count = 0;

  while (fscanf (fptr, "%s %u:%u %u:%u", text, &hr0, &min0, &hr1, &min1) == 5)
    count ++;

  if (!count)
  {
    fprintf (stderr, "No sources loaded\n");
    return -1;
  }

  fprintf (stderr, "%u sources\n", count);

  rewind (fptr);

  cpgswin (0, 24, 0, count);

  float i = 0;
  cpgsls (2);

  for (i=1; i<24; i++)
  {
    cpgmove (i, 0);
    cpgdraw (i, count);
  }

  int current = 0;
  cpgsls (1);

  while (fscanf (fptr, "%s %u:%u %u:%u", text, &hr0, &min0, &hr1, &min1) == 5)
  {
    fprintf (stderr, "%u:%s %u:%u -> %u:%u\n",
	     current, text, hr0, min0, hr1, min1);

    double lst0 = (hr0 + min0/60.0);
    double lst1 = (hr1 + min1/60.0);

    fprintf (stderr, "t0=%lf t1=%lf\n", lst0, lst1);

    cpgsci (160-20*(current%2));

    draw_band (count-current-1, lst0, lst1);

    float xlabel = lst0+0.5;
    if (lst0 > 20)
      xlabel = 0.5;

    cpgsci (1);
    label (count-current-1, xlabel, text);

    current ++;
  }

  fclose (fptr);

  cpgbox ("bcn", 3.0,0, "bc", 0,0);

  cpgsclp (0);

  for (i=25.5; i<31; i+=1.5)
  {
    cpgmove (i, 0);
    cpgdraw (i, count);
  }

  cpgend ();

  return 0;
}
