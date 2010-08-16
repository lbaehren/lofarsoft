//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/strutil.h,v $
   $Revision: 1.10 $
   $Date: 2009/03/12 09:17:08 $
   $Author: straten $ */

#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "pad.h"
#include "tostring.h"

//! Makes the string lowercase
std::string lowercase (std::string s);

//! Makes the string uppercase
std::string uppercase (std::string s);

//! Return true if every character in string is printable
bool printable (const std::string& s);

// returns the filename without its path
std::string basename (const std::string& filename);
// returns the path preceding a filename
std::string pathname (const std::string& filename);
//! a simple command for replacing the extension on a filename
std::string replace_extension (const std::string& filename, 
			       const std::string& extension);

//! a simple command for returning a user friendly time std::string
std::string time_string (double seconds);

std::vector<std::string>
stringdecimate(const std::string& wordstr, std::string delimiters);

//! removes all instances of c from input
std::string remove_all (std::string input, char c);

// ///////////////////////////////////////////////////////////
// reads up to 'nbytes' (or to eof) from 'istr', adding them 
// to 'str'.  returns number of bytes read or -1 on error
// ///////////////////////////////////////////////////////////
ssize_t 
stringload (std::string* str, std::istream &istr, std::streamsize nbytes=0);

// ///////////////////////////////////////////////////////////
// reads up to 'nbytes' (or to eof) from 'fptr', adding them 
// to 'str'.  returns number bytes read or -1 on error
// ///////////////////////////////////////////////////////////
ssize_t 
stringload (std::string* str, FILE* fptr, size_t nbytes=0);

//
// copy up to nbytes (or to eof) from 'from' to 'to'
//
void copy (FILE* from, FILE* to, ssize_t nbytes=-1);

// ///////////////////////////////////////////////////////////
// fills a vector of std::string with the first word from each line
// in the file.  A line is delimited by \n or commented by #.
// ///////////////////////////////////////////////////////////
int stringfload (std::vector<std::string>* lines, const std::string& filename);
int stringload (std::vector<std::string>* lines, FILE* fptr);

// ///////////////////////////////////////////////////////////
// fills a vector of std::string with the lines form file.  
// A line is delimited by \n or commented by #.
// ///////////////////////////////////////////////////////////
void loadlines (const std::string& filename, std::vector<std::string>& lines);

// ///////////////////////////////////////////////////////////
// returns the first sub-std::string of 'instr' delimited by
// characters in 'delimiters'.  the substd::string and any leading
// delimiter characters are removed from 'instr'
// ///////////////////////////////////////////////////////////
std::string stringtok (std::string * instr, const std::string & delimiters,
		       bool skip_leading_delimiters = true,
		       bool strip_leading_delimiters_from_remainder = true);

inline
std::string stringtok (std::string* instr, char delimiter,
		       bool skip_leading_delimiters = true,
		       bool strip_leading_delimiters_from_remainder = true)
{ return stringtok (instr, std::string(1, delimiter),
		    skip_leading_delimiters,
		    strip_leading_delimiters_from_remainder); }

// ///////////////////////////////////////////////////////////
// like sprintf, but returns a string
// ///////////////////////////////////////////////////////////
std::string stringprintf (const char *fmt ...);

// //////////////////////////////////////////////////////////
// string_split splits a string setting parameters before and after
// to the contents before and after the delimiter string
// before and after will be empty if the delimiter does not exist
// //////////////////////////////////////////////////////////

void string_split( std::string source, std::string &before, std::string &after, std::string delimiter );



#endif

