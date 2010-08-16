//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/units/stringtok.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:55 $
   $Author: straten $ */

#ifndef __STRINGTOK_H
#define __STRINGTOK_H

#include <string>

// ///////////////////////////////////////////////////////////
// returns the first sub-string of 'instr' delimited by
// characters in 'delimiters'.  the substring and any leading
// delimiter characters are removed from 'instr'
// ///////////////////////////////////////////////////////////
std::string stringtok (std::string& instr, const std::string & delimiters,
                  bool skip_leading_delimiters = true,
                  bool strip_leading_delimiters_from_remainder = true);

#endif

