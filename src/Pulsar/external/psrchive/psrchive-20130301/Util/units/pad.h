//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/units/pad.h,v $
   $Revision: 1.2 $
   $Date: 2006/10/06 21:13:55 $
   $Author: straten $ */

#ifndef __UTILS_UNITS_PAD_H
#define __UTILS_UNITS_PAD_H

#include <string>

// Pad text with spaces up to length, to the right or left of the original
std::string pad (unsigned length, const std::string& text, bool right = true);

#endif
