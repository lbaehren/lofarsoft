/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/*
 *
 * epnio.h - defines a C interface to rwepn
 *
 * The structs defined in this header file must be a binary match to
 * the common blocks defined in epnhdr.inc.  This includes:
 *
 * #define EPN_MAXBLK mmm // where: parameter(maxblk=mmm) in epnhdr.inc
 *
 * #define EPN_MAXBIN nnn // where: parameter(maxbin=nnn) in epnhdr.inc
 *
 * Willem van Straten - July 2004
 *
 */

#ifndef __EPN_IO_H
#define __EPN_IO_H

#ifdef __cplusplus
extern "C" {
#endif

  /* First line of main header */
  typedef struct {
    char version[8];
    char history[68];
    int  counter;
  } epn_header_line1;

  /* Second line of main header */
  typedef struct {
    char jname[12];
    char cname[12];
    double pbar;
    double dm;
    double rm;
    char catref[6];
    char bibref[8];
  } epn_header_line2;

  /* Third line of main header */
  typedef struct {
    int rah;
    int ram;
    int ded;
    int dem;
    float ras;
    float des;
    char telname[8];
    double epoch;
    double opos;
    char paflag;
    char timflag;
  } epn_header_line3;

  /* Fourth line of main header */
  typedef struct {
    double xtel, ytel, ztel;
  } epn_header_line4;

  /* Fifth line of main header */
  typedef struct {
    int cdy, cdm, cdd;
    int scanno, subscan;
    int npol, nfreq, nbin;
    double tbin;
    int nint, ncal, lcal;
    float fcal;
    double tres;
    char fluxflag;
  } epn_header_line5;

  /* Maximum blocks writing to sub-header... */
  #define EPN_MAXBLK 16

  /* First line of block sub-header */
  typedef struct {
    char idfield[EPN_MAXBLK][8];
    int nband[EPN_MAXBLK];
    int navg[EPN_MAXBLK];
    double f0[EPN_MAXBLK];
    char f0u[EPN_MAXBLK][8];
    double df[EPN_MAXBLK];
    char dfu[EPN_MAXBLK][8];
    double tstart[EPN_MAXBLK];
  } epn_block_subheader_line1;

  /* Second line of block sub-header (required to scale/descale data) */
  typedef struct {
    double offset[EPN_MAXBLK];
    double scale[EPN_MAXBLK];
    double rms[EPN_MAXBLK];
    double papp[EPN_MAXBLK];
  } epn_block_subheader_line2;

  /* Maximum number of phase bins... */
  #define EPN_MAXBIN 4096

  /* Data block */
  typedef struct {
    float rawdata[EPN_MAXBLK][EPN_MAXBIN];
  } epn_data_block;

  /* C wrapper of the Fortran rwepn function */
  int crwepn ( const char* filename, int readwri, int recno, int padout,
               epn_header_line1* line1,
               epn_header_line2* line2, 
               epn_header_line3* line3, 
               epn_header_line4* line4, 
               epn_header_line5* line5, 
               epn_block_subheader_line1* sub_line1,
               epn_block_subheader_line2* sub_line2,
               epn_data_block* data );

  /* C wrapper of the Fortran nepnrec function */
  int cnepnrec (const char* filename);

  void epn_dump (const epn_header_line1* line1,
                 const epn_header_line2* line2,
                 const epn_header_line3* line3,
                 const epn_header_line4* line4,
                 const epn_header_line5* line5);

#ifdef __cplusplus
}
#endif

#endif
