/***************************************************************************
 *
 *   Copyright (C) 2004 by wvanstra Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "epnio.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* modify this macro as necessary for your compiler */
#define F772C(x) x##_

/* *************************************************************************
 *
 * declaration of symbols defined in rwepn.f 
 *
 * ************************************************************************* */

void F772C(rwepn) (char* filename, int* rw, int* recno, int* padout, int len);
int F772C(nepnrec) (char* filename, int len);

extern epn_header_line1 F772C(epn1);
extern epn_header_line2 F772C(epn2);
extern epn_header_line3 F772C(epn3);
extern epn_header_line4 F772C(epn4);
extern epn_header_line5 F772C(epn5);
extern epn_block_subheader_line1 F772C(epns1);
extern epn_block_subheader_line2 F772C(epns2);
extern epn_data_block F772C(dblk);

/* *************************************************************************
 *
 * some utilities
 *
 * ************************************************************************* */

void f2cstr (const char* f_str, char* c_str, unsigned length)
{
  /* fprintf (stderr, "f2cstr in '%.*s'\n", length, f_str); */

  unsigned i = length-1;
  
  strncpy (c_str, f_str, length);
  
  while (f_str[i] == ' ') {
    c_str[i] = '\0';
    if (i == 0)
      break;
    i--;
  }

  /* fprintf (stderr, "f2cstr out '%s'\n", c_str); */
}

void c2fstr (char* f_str, const char* c_str, unsigned length)
{
  unsigned i = strlen(c_str);
  
  strncpy (f_str, c_str, length);
  for (; i < length; i++)
    f_str[i] = ' ';
}

/* *************************************************************************
 *
 * the C wrapper to rwepn
 *
 * ************************************************************************* */

int crwepn ( const char* filename, int readwri, int recno, int padout,
             epn_header_line1* line1,
             epn_header_line2* line2,
             epn_header_line3* line3,
             epn_header_line4* line4,
             epn_header_line5* line5,
             epn_block_subheader_line1* sub_line1,
             epn_block_subheader_line2* sub_line2,
             epn_data_block* data )
{
  /* for dealing with filename string */
  unsigned length = 0;
  char* f77_filename = 0;

  /* for counting blocks */
  unsigned iblock = 0, nblock = 0;

  /* for transposing data */
  unsigned ibin = 0, nbin = 0;
  float* ptr = 0;

  if (!filename)
    return -1;

  /* space-terminate the filename */
  length = strlen (filename);
  f77_filename = strdup (filename);
  f77_filename[length] = ' ';

  if (readwri > 0) {

    F772C(epn1) = *line1;
    F772C(epn2) = *line2;
    F772C(epn3) = *line3;
    F772C(epn4) = *line4;
    F772C(epn5) = *line5;
    F772C(epns1) = *sub_line1;
    F772C(epns2) = *sub_line2;

    /* fix up the strings */

    /* don't fix the version string, as it uses all eight characters
     * c2fstr (F772C(epn1).version, line1->version, 8); */

    c2fstr (F772C(epn1).history, line1->history, 68);

    c2fstr (F772C(epn2).jname, line2->jname, 12);
    c2fstr (F772C(epn2).cname, line2->cname, 12);
    c2fstr (F772C(epn2).catref, line2->catref, 6);
    c2fstr (F772C(epn2).bibref, line2->bibref, 8);

    c2fstr (F772C(epn3).telname, line3->telname, 8);

    nblock = line5->npol * line5->nfreq;
    for (iblock = 0; iblock < nblock; iblock++) {
      c2fstr (F772C(epns1).idfield[iblock], sub_line1->idfield[iblock], 8);
      c2fstr (F772C(epns1).f0u[iblock], sub_line1->f0u[iblock], 8);
      c2fstr (F772C(epns1).dfu[iblock], sub_line1->dfu[iblock], 8);
    }

    /* transpose the data */
    ptr = &(F772C(dblk).rawdata[0][0]);
    nbin = line5->nbin;

    fprintf (stderr, "w nblock=%d nbin=%d\n", nblock, nbin);

    for (ibin = 0; ibin < nbin; ibin++) {
      for (iblock = 0; iblock < nblock; iblock++) {
	*ptr = data->rawdata[iblock][ibin];
	ptr ++;
      }
      for (; iblock < EPN_MAXBLK; iblock++) {
	*ptr = 0.0;
	ptr ++;
      }
    }

  }

  F772C(rwepn) (f77_filename, &readwri, &recno, &padout, length);

  free (f77_filename);

  if (recno == -999)
    return -1;

  if (readwri > 0)
    return 0;

  *line1 = F772C(epn1);
  *line2 = F772C(epn2);
  *line3 = F772C(epn3);
  *line4 = F772C(epn4);
  *line5 = F772C(epn5);
  *sub_line1 = F772C(epns1);
  *sub_line2 = F772C(epns2);

  /* fix up the strings */

  /* don't fix the version string, as it uses all eight characters
   * f2cstr (F772C(epn1).version, line1->version, 8); */

  f2cstr (F772C(epn1).history, line1->history, 68);
  
  f2cstr (F772C(epn2).jname, line2->jname, 12);
  f2cstr (F772C(epn2).cname, line2->cname, 12);
  f2cstr (F772C(epn2).catref, line2->catref, 6);
  f2cstr (F772C(epn2).bibref, line2->bibref, 8);
  
  f2cstr (F772C(epn3).telname, line3->telname, 8);
  
  nblock = line5->npol * line5->nfreq;
  for (iblock = 0; iblock < nblock; iblock++) {
    f2cstr (F772C(epns1).idfield[iblock], sub_line1->idfield[iblock], 8);
    f2cstr (F772C(epns1).f0u[iblock], sub_line1->f0u[iblock], 8);
    f2cstr (F772C(epns1).dfu[iblock], sub_line1->dfu[iblock], 8);
  }

  /* transpose the data */
  ptr = &(F772C(dblk).rawdata[0][0]);
  nbin = line5->nbin;

  /* fprintf (stderr, "r nblock=%d nbin=%d\n", nblock, nbin); */

  for (ibin = 0; ibin < nbin; ibin++) {
    for (iblock = 0; iblock < nblock; iblock++) {
      /* fprintf (stderr, "%d %d %f\n", iblock, ibin, *ptr); */
      data->rawdata[iblock][ibin] = *ptr;
      ptr ++;
    }
    ptr += EPN_MAXBLK - nblock;
  }

  /* fprintf (stderr, "crwepn return\n"); */

  return 0;
}

void epn_dump (const epn_header_line1* line1,
               const epn_header_line2* line2,
               const epn_header_line3* line3,
               const epn_header_line4* line4,
               const epn_header_line5* line5)
{
  fprintf (stderr, "line 5: npol=%d\n", line5->npol);
  fprintf (stderr, "line 5: nfreq=%d\n", line5->nfreq);
  fprintf (stderr, "line 5: nbin=%d\n", line5->nbin);
  fprintf (stderr, "line 5: tbin=%lf\n", line5->tbin);
  fprintf (stderr, "line 5: nint=%d\n", line5->nint);
  fprintf (stderr, "line 5: ncal=%d\n", line5->ncal);
}

int cnepnrec (const char* filename)
{
  /* for dealing with filename string */
  unsigned i, length = 0;
  char f77_filename [80];

  if (!filename)
    return -1;

  /* space-terminate the filename */
  length = strlen (filename);
  strcpy (f77_filename, filename);
  for (i=length; i<80; i++)
    f77_filename[i] = ' ';

  return F772C(nepnrec) (f77_filename, length);
}

