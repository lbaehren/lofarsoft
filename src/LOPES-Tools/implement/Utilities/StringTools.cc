/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id: StringTools.cc,v 1.3 2006/10/31 18:24:08 bahren Exp $ */

#include <Utilities/StringTools.h>

namespace LOPES {

  // ---------------------------------------------------------------- string2char
  
  char* string2char (const String& mystring)
  {
    int x (0);
    char* retChar;
    const char* tmpChar = mystring.c_str();
    
    retChar = new char [strlen(tmpChar)+1];
    
    /* change the size of string2 to the size of string1 */
    realloc(retChar, strlen(tmpChar)+1);
    
    do {
      retChar[x] = tmpChar[x];
    } while (tmpChar[x++] != '\0');
    
    return retChar;
  }
  
  // ---------------------------------------------------------------- string2char

  char* string2char (const String& mystring,
		     const uint & length)
  {
    unsigned int nofChars (mystring.length());
    char* mychar;
    
    // check the requested number of of characters
    if (length > nofChars) {
      cerr << "Requested character array is longer as provided string." << endl;
    } else {
      nofChars = length;
    }
    
    nofChars += 1;
    
    // resize the array of characters
    mychar = new char [nofChars];
    
    // copy the string character by character
    for (unsigned int i(0); i<nofChars; i++) {
      mychar[i] = mystring[i];
    }
    
    // display conversion result
    cout << nofChars << "\t[" << mystring << "]  ->  [" << mychar << "]"
	 << endl;
    
    return mychar;
  }

  // -------------------------------------------------------------- replaceString
  
  String replaceString (const String str,
			const String out,
			const String in)
  {
    unsigned int lstr = str.length();
    unsigned int lout = out.length();
    unsigned int loc=0;
    String repl = str;
    
    for (unsigned int i=0; i<lstr; i++) {
      loc = str.find(out,i);
      if (i == loc) {
	repl.replace(i, lout, in);
      }
    }
    
    return repl;
  }
  
  // ---------------------------------------------------------------- nofSubstrings
  
  int nofSubstrings (std::string const &str,
		     std::string const &sep)
  {
    uint nofChars = str.length();
    uint loc=0;
    int num = 1;
    
    for (unsigned int i=0; i<nofChars; i++) {
      loc = str.find(sep,i);
      if (i == loc) {
	++num;
      }
    }
    
    return num;
  }
  
  // ---------------------------------------------------------------- getSubstrings
  
  std::vector<std::string> getSubstrings (std::string const &str,
					  std::string const &sep) 
  {
    string tmp;
    uint nofChars = str.length();
    uint start=0,loc=0;
    int num (nofSubstrings(str,sep));
    std::vector<std::string> substrings;
    
    substrings.resize(num);
    
    num = start = 0;
    for (unsigned int i=0; i<nofChars; i++) {
      loc = str.find(sep,i);
      if (i == loc) {
	tmp.assign(str,start,loc-start);
	substrings[num] = tmp;
	// update counters
	++num;
	start = loc+1;
      }
    }
    tmp.assign(str,start,loc-nofChars);
    substrings[num] = tmp;
    
    return substrings;
  }
  
  // ---------------------------------------------------------------- nofSubstrings
  
  int nofSubstrings (const String str,
		     const String sep)
  {
    unsigned int nofChars = str.length();
    unsigned int loc=0;
    int num = 1;
    
    for (unsigned int i=0; i<nofChars; i++) {
      loc = str.find(sep,i);
      if (i == loc) {
	++num;
      }
    }
    
    return num;
  }
  
  // -------------------------------------------------------------- getSubstrings

  Vector<String> getSubstrings (const String str,
				const String sep) 
  {
    String tmp;
    unsigned int nofChars = str.length();
    unsigned int start=0,loc=0;
    int num = nofSubstrings(str,sep);
    Vector<String> substrings(num);
    
    num = start = 0;
    for (uint i=0; i<nofChars; i++) {
      loc = str.find(sep,i);
      if (i == loc) {
	tmp.assign(str,start,loc-start);
	substrings(num) = tmp;
	// update counters
	++num;
	start = loc+1;
      }
    }
    tmp.assign(str,start,loc-nofChars);
    substrings(num) = tmp;
    
    return substrings;
  }
  
  // --------------------------------------------------------------- fileFromPath

  String fileFromPath (const String pathname)
  {
    String filename;
    unsigned int nofChars = pathname.length();
    unsigned int loc=0,start=0;
    
    for (unsigned int i=0; i<nofChars; i++) {
      loc = pathname.find( "/", i);
      if (i == loc) start = i;
    }
    
    filename.assign(pathname,start+1,nofChars-start-1);
    
    return filename;
  }
  
  // ---------------------------------------------------------------- dirFromPath

  String dirFromPath (const String pathname)
  {
    String filename = fileFromPath (pathname);
    unsigned int lpathname = pathname.length();
    unsigned int lfilename = filename.length();
    String dirname;
    
    dirname.assign(pathname,0,lpathname-lfilename-1);
    
    return dirname;
  }
  
  // ------------------------------------------------------------ dateFromISO8601

  String dateFromISO8601 (const String iso)
  {
    unsigned int loc = iso.find( "T", 0);
    string date;
    
    date.assign(iso,0,loc);  
    
    return date;
  }
  
  // ------------------------------------------------------------ timeFromISO8601

  String timeFromISO8601 (const String iso)
  {
    unsigned int nofChars = iso.length();
    unsigned int loc = iso.find( "T", 0);
    String date = dateFromISO8601 (iso);
    string time;
    
    time.assign(iso,loc+1,nofChars-loc-2);
    
    return time;
  }
  
}
