/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include "ephio.h"

int main(int argc, char** argv)
{
   char strs[EPH_NUM_KEYS][EPH_STR_LEN];
   double doubles[EPH_NUM_KEYS], errors[EPH_NUM_KEYS];
   int ints[EPH_NUM_KEYS], statuses[EPH_NUM_KEYS];

   char* defaultin = "t.eph";
   char* defaultout = "out.eph";

   char* infname = defaultin;
   char* outfname = defaultout;

   if (argc > 1)
     infname = argv[1];
   if (argc > 2)
     outfname = argv[2];

   fprintf (stderr, "ephtest: Loading from '%s'\n", infname);
   eph_rd(infname, statuses, strs, doubles, ints, errors);

   fprintf (stderr, "ephtest: Writing to '%s'\n", outfname);
   eph_wr(outfname, statuses, strs, doubles, ints, errors);

   eph_wr_lun(6, statuses, strs, doubles, ints, errors);
   
   return 0;
}
