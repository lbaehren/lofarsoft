/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/timer_supp.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __TIMER_SUPP_H
#define __TIMER_SUPP_H

/* As timer.bandb is not used by any of the current software, the space 
   occupied by this structure is being converted into useful storage.
   As bandb.npol should be set to -1 in all previous files, this value
   will be used to distinguish between pre-supp and post-supp archives.

   Willem van Straten
   23 July 04 
*/

struct supp {

  /* 
   * Future versions may use these attributes.
   */

  double unused1;
  double unused2;
  double unused3;

  double unused4;
  double unused5;
  double unused6;

  float unused7;

  int   unused8;
  int   unused9;
  int   unused10;

  float unused11;
  float unused12;
  int   unused13;

  int   unused14;

  /*
   * Version -2 attributes
   */

  /* Receiver Extension class parameters; stored in degrees */
  float X_offset;
  float Y_offset;
  float calibrator_offset;

  /* 
   * Supplemental version number (note that it is always -ve) 
   */
  int version;

};

#endif
