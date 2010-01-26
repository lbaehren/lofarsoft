//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __format_it_h_
#define __format_it_h_

#include <string>
#include <vector>

// See format.C to see how to use these functions

// Formats a file into a set of lines
std::vector<std::string>
format_file (std::string filename, unsigned extra_spaces,
             std::vector<unsigned>* rjustify_columns=0);

// Formats a set of lines
void format_it (std::vector<std::string>& lines, unsigned extra_spaces,
	        std::vector<unsigned>* rjustify_columns=0);

#endif
